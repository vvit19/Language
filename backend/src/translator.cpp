#include "backend.h"

const int MAX_NAMES = 50;

struct AsmInfo
{
    char**   names_table;
    int      names_num;
    int      label_num;
    KeyWords prev_option;
};

static void NodeToAsm      (FILE* file, Node* node, AsmInfo* info);
static int  GetVarPosition (const char* var, AsmInfo* info);

void GetAsmCode (const char* asm_file_name, Node* main_node)
{
    struct AsmInfo info = {};
    info.names_table    = (char**) calloc (MAX_NAMES, sizeof (char*));
    info.names_num      = 0;
    info.label_num      = 0;
    info.prev_option    = FUNC;

    FILE* asm_file = fopen (asm_file_name, "w");

    fprintf (asm_file, "call main\n"
                       "hlt\n\n");

    NodeToAsm (asm_file, main_node, &info);

    free (info.names_table);
    fclose (asm_file);
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
        fprintf (file, "push [rax + %d] \n", GetVarPosition (node->value.var, info));
    }

    if (node->type == FUNC_NAME)
    {
        fprintf (file, "%s: \n" "pop rcx \n", node->value.var); // rcx - берет адрес возврата, в конце кладет обратно
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
                info->label_num = 0;
                info->prev_option = FUNC;

                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                break;
            }

        // case FUNC_NAME:
        //     {
        //         fprintf (file, "%s: \n" "pop rcx \n", node->value.var); // rcx - берет адрес возврата, в конце кладет обратно
        //         NodeToAsm (file, node->left,  info);
        //         NodeToAsm (file, node->right, info);

        //         break;
        //     }

        case VAR:
            {
                NodeToAsm (file, node->right, info);

                info->names_table[info->names_num] = node->left->value.var;
                fprintf (file, "pop [rax + %d] \n", info->names_num);

                info->names_num++;

                break;
            }

        case CALL:
            {
                info->prev_option = CALL;

                fprintf (file, "push rcx \n");
                NodeToAsm (file, node->right, info); // push params
                fprintf (file, "push rax + %d \n" "pop rax \n", info->names_num); // увеличиваю количество занятых ячеек оперативы

                fprintf (file, "call %s \n" "pop rcx \n", node->left->value.var);
                fprintf (file, "push rax\n" "push %d \nsub\n" "pop rax \n", info->names_num); // возвращаю кол-во ячеек оперативы

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
                info->prev_option = IF;

                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "if_%d: \n", info->label_num);

                break;
            }

        case ELSE:
            {
                int cur_label = info->label_num++;

                NodeToAsm (file, node->left, info);

                fprintf (file, "jump else_%d\n\n"
                               "if_%d: \n", cur_label, cur_label);

                NodeToAsm (file, node->right->left, info);

                fprintf (file, "else_%d: \n", cur_label);
                NodeToAsm (file, node->right->right, info);

                break;
            }

        // case WHILE:
        //     {

        //     }

        case EQ:
            {
                NodeToAsm (file, node->right, info);

                if (node->right->type == OP_T && node->right->value.op == CALL) fprintf (file, "push rbx \n");

                break;
            }

        case ISEQ:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "jne if_%d \n", info->label_num);

                break;
            }

        case NEQ:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "je if_%d \n", info->label_num);
                break;
            }

        case BIGGER:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "jbe if_%d \n", info->label_num);
                break;
            }

        case SMALLER:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "jae if_%d \n", info->label_num);
                break;
            }

        case BIGEQ:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "jb if_%d \n", info->label_num);
                break;
            }

        case SMALLEQ:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "ja if_%d \n", info->label_num);
                break;
            }

        case ADD:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "add \n");
                break;
            }

        case SUB:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "sub \n");
                break;
            }

        case MULT:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "mult \n");
                break;
            }

        case DIV:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "div \n");
                break;
            }

        case SIN:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "sin \n");
                break;
            }

        case COS:
            {
                NodeToAsm (file, node->left,  info);
                NodeToAsm (file, node->right, info);

                fprintf (file, "cos \n");
                break;
            }

        case PARAM:
            {
                int position = 0;

                switch (info->prev_option)
                {
                case IN:
                    {
                        position = GetVarPosition (node->left->value.var, info);

                        fprintf (file, "in \n" "pop [rax + %d]\n", position);
                        break;
                    }

                case OUT:
                    {
                        position = GetVarPosition (node->left->value.var, info);

                        fprintf (file, "push [rax + %d]\n" "out\n", position);
                        break;
                    }

                case CALL:
                    {
                        position = GetVarPosition (node->left->value.var, info);

                        fprintf (file, "push [rax + %d]\n", position);
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
                        position = GetVarPosition (node->left->value.var, info);

                        NodeToAsm (file, node->left,  info);
                        NodeToAsm (file, node->right, info);

                        fprintf (file, "pop rbx \n");

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

static int  GetVarPosition (const char* var, AsmInfo* info)
{
    assert (var);
    assert (info);

    for (int i = 0; i < info->names_num; i++)
    {
        if (strcmp (var, info->names_table[i]) == 0) return i;
    }

    return -1;
}

