#include "backend.h"

#define GLOBAL_VAR_POS(var) GetVarPosition (var, info->global_names_table, info->global_names_num)
#define VAR_POS(var) GetVarPosition (var, info->names_table, info->names_num)

#define NODES_TO_ASM(...) \
    NodeToAsm (file, node->left,  info); \
    NodeToAsm (file, node->right, info); \
    __VA_ARGS__;

const int MAX_NAMES = 50;

struct AsmInfo
{
    char**   names_table;
    char**   global_names_table;
    int      global_names_num;
    int      names_num;
    int      label_num;
    KeyWords prev_option;
};

static void NodeToAsm       (FILE* file, Node* node, AsmInfo* info);
static int  GetVarPosition  (const char* var, char** names_array, int n_names);
static void PushParams      (FILE* file, Node* node, AsmInfo* info);
static void CountGlobalVars (Node* node, int* num);
static void PushVar         (FILE* file, const char* var, AsmInfo* info);
static void PopVar          (FILE* file, const char* var, AsmInfo* info);

void GetAsmCode (const char* asm_file_name, Node* main_node)
{
    int global_names_num = 0;
    CountGlobalVars (main_node, &global_names_num);

    struct AsmInfo info     = {};
    info.names_table        = (char**) calloc (MAX_NAMES, sizeof (char*));
    info.global_names_table = (char**) calloc (global_names_num, sizeof (char*));
    info.global_names_num   = 0;
    info.names_num          = 0;
    info.label_num          = 0;
    info.prev_option        = FUNC;

    FILE* asm_file = fopen (asm_file_name, "w");

    fprintf (asm_file, "push %d \n"
                       "pop rax \n"
                       "call main \n"
                       "hlt \n\n", global_names_num);

    NodeToAsm (asm_file, main_node, &info);

    fclose (asm_file);

    free (info.global_names_table);
    free (info.names_table);
}

static void NodeToAsm (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    if (!node) return;

    if (node->type == NUM_T)
    {
        fprintf (file, "push %lg \n", node->value.num);
    }

    if (node->type == VAR_T)
    {
        PushVar (file, node->value.var, info);
    }

    if (node->type == FUNC_NAME)
    {
        fprintf (file, "%s: \n" "pop rcx \n", node->value.var);
        NodeToAsm (file, node->left,  info);
        NodeToAsm (file, node->right, info);
    }

    if (node->type == OP_T)
    {
        switch (node->value.op)
        {
        case NEXT_OP:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                break;
            }

        case FUNC:
            {
                free (info->names_table);

                info->names_table = (char**) calloc (MAX_NAMES, sizeof (char*));
                info->names_num = 0;
                info->prev_option = FUNC;

                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                break;
            }

        case VAR:
            {
                NodeToAsm (file, node->right, info);

                info->names_table[info->names_num] = node->left->value.var;
                fprintf (file, "pop [rax + %d] \n", info->names_num);

                info->names_num++;

                break;
            }

        case GL_VAR:
            {
                NodeToAsm (file, node->right, info);

                info->global_names_table[info->global_names_num] = node->left->value.var;
                fprintf (file, "pop [%d] \n", info->global_names_num);

                info->global_names_num++;

                break;
            }

        case CALL:
            {
                info->prev_option = CALL;

                fprintf (file, "push rcx \n");
                PushParams (file, node->right, info);
                fprintf (file, "push rax + %d \n" "pop rax \n", info->names_num);

                fprintf (file, "call %s \n" "pop rcx \n", node->left->value.var);
                fprintf (file, "push rax\n" "push %d \nsub\n" "pop rax \n", info->names_num);

                break;
            }

        case RET:
            {
                info->prev_option = RET;
                NodeToAsm (file, node->left, info);

                fprintf (file, "push rcx \n" "ret \n\n");

                break;
            }

        case IN:
            {
                info->prev_option = IN;
                NodeToAsm (file, node->left, info);

                break;
            }

        case OUT:
            {
                info->prev_option = OUT;
                NodeToAsm (file, node->left, info);

                break;
            }

        case IF:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                NODES_TO_ASM (fprintf (file, "jump label_%d \n" "label_%d: \n", info->label_num, info->label_num));

                info->label_num++;

                break;
            }

        case ELSE:
            {
                NodeToAsm (file, node->left, info);
                int else_label = info->label_num++;

                NodeToAsm (file, node->right->left, info);
                fprintf (file, "jump label_%d \n", info->label_num);
                int after_label = info->label_num++;

                fprintf (file, "label_%d: \n", else_label);
                NodeToAsm (file, node->right->right, info);
                fprintf (file, "jump label_%d \n" "label_%d: \n", after_label, after_label);

                break;
            }

        case WHILE:
            {
                NodeToAsm (file, node->left,  info);

                int end_label   = info->label_num++;
                int while_label = info->label_num;

                fprintf (file, "jump label_%d \nlabel_%d: \n", while_label, while_label);

                NodeToAsm (file, node->right, info);

                int temp = info->label_num;
                info->label_num = end_label;
                NodeToAsm (file, node->left, info);
                info->label_num = temp;

                fprintf (file, "jump label_%d \n" "label_%d: \n", while_label, end_label);

                break;
            }

        case EQ:
            {
                NodeToAsm (file, node->right, info);

                if (node->right->type == OP_T && node->right->value.op == CALL) fprintf (file, "push rbx \n");

                if (node->left && node->left->type == VAR_T)
                {
                    int position = GLOBAL_VAR_POS (node->left->value.var);
                    if (position >= 0) fprintf (file, "pop [%d] \n", position);
                    else fprintf (file, "pop [rax + %d] \n", VAR_POS (node->left->value.var));
                }


                break;
            }

        case ISEQ:
            {
                NODES_TO_ASM (fprintf (file, "jne label_%d \n", info->label_num));
                break;
            }

        case NEQ:
            {
                NODES_TO_ASM (fprintf (file, "je label_%d \n", info->label_num));
                break;
            }

        case BIGGER:
            {
                NODES_TO_ASM(fprintf (file, "jae label_%d \n", info->label_num));
                break;
            }

        case SMALLER:
            {
                NODES_TO_ASM(fprintf (file, "jbe label_%d \n", info->label_num));
                break;
            }

        case BIGEQ:
            {
                NODES_TO_ASM (fprintf (file, "ja label_%d \n", info->label_num));
                break;
            }

        case SMALLEQ:
            {
                NODES_TO_ASM (fprintf (file, "jb label_%d \n", info->label_num));
                break;
            }

        case ADD:
            {
                NODES_TO_ASM (fprintf (file, "add \n"));
                break;
            }

        case SUB:
            {
                NODES_TO_ASM (fprintf (file, "sub \n"));
                break;
            }

        case MULT:
            {
                NODES_TO_ASM (fprintf (file, "mult \n"));
                break;
            }

        case DIV:
            {
                NODES_TO_ASM (fprintf (file, "div \n"));
                break;
            }

        case SIN:
            {
                NODES_TO_ASM (fprintf (file, "sin \n"));
                break;
            }

        case COS:
            {
                NODES_TO_ASM (fprintf (file, "cos \n"));
                break;
            }

        case SQRT:
            {
                NODES_TO_ASM (fprintf (file, "sqrt \n"));
                break;
            }

        case PARAM:
            {
                int position = 0;

                switch (info->prev_option)
                {
                case IN:
                    {
                        fprintf (file, "in \n");
                        PopVar (file, node->left->value.var, info);

                        break;
                    }

                case OUT:
                    {
                        if (node->left->type == NUM_T)
                        {
                            fprintf (file, "push %lg \nout \n", node->left->value.num);
                            break;
                        }

                        PushVar (file, node->left->value.var, info);
                        fprintf (file, "out \n");

                        break;
                    }

                case FUNC:
                    {
                        info->names_table[info->names_num] = node->left->value.var;
                        fprintf (file, "pop [rax + %d] \n", info->names_num);

                        info->names_num++;
                        break;
                    }

                case RET:
                    {
                        NODES_TO_ASM (fprintf (file, "pop rbx \n"));

                        return;
                    }

                default:
                    NO_PROPER_CASE_FOUND;
                }

                NodeToAsm (file, node->right, info);
                break;
            }

        default:
            NO_PROPER_CASE_FOUND;
        }
    }
}

static int GetVarPosition (const char* var, char** names_array, int n_names)
{
    assert (var);
    assert (names_array);

    for (int i = 0; i < n_names; i++)
    {
        if (strcmp (var, names_array[i]) == 0) return i;
    }

    return -1;
}

static void PushParams (FILE* file, Node* node, AsmInfo* info)
{
    if (node->right) PushParams (file, node->right, info);

    NodeToAsm (file, node->left, info);
}

static void CountGlobalVars (Node* node, int* num)
{
    if (!node) return;

    CountGlobalVars (node->left,  num);
    CountGlobalVars (node->right, num);

    if (node->type == OP_T)
    {
        if (node->value.op == GL_VAR) *num += 1;
    }
}

static void PushVar (FILE* file, const char* var, AsmInfo* info)
{
    assert (file);
    assert (var);
    assert (info);

    int position = GLOBAL_VAR_POS (var);

    if (position >= 0) fprintf (file, "push [%d] \n", position);
    else fprintf (file, "push [rax + %d] \n", VAR_POS (var));
}

static void PopVar (FILE* file, const char* var, AsmInfo* info)
{
    assert (file);
    assert (var);
    assert (info);

    int position = GLOBAL_VAR_POS (var);

    if (position >= 0) fprintf (file, "pop [%d] \n", position);
    else fprintf (file, "pop [rax + %d] \n", VAR_POS (var));
}
