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
static void PushOp          (FILE* file, Node* node, AsmInfo* info);
static int  GetVarPosition  (const char* var, char** names_array, int n_names);
static void PushParams      (FILE* file, Node* node, AsmInfo* info);
static void CountGlobalVars (Node* node, int* num);
static void PushVar         (FILE* file, const char* var, AsmInfo* info);
static void PopVar          (FILE* file, const char* var, AsmInfo* info);
static int  PrintCommand    (FILE* file, const char* command, ...);
static void PushFunction    (FILE* file, Node* node, AsmInfo* info);
static void PushNewVar      (FILE* file, Node* node, AsmInfo* info);
static void PushParam       (FILE* file, Node* node, AsmInfo* info);
static void PushFunction    (FILE* file, Node* node, AsmInfo* info);
static void PushEq          (FILE* file, Node* node, AsmInfo* info);
static void PushIf          (FILE* file, Node* node, AsmInfo* info);
static void PushElse        (FILE* file, Node* node, AsmInfo* info);
static void PushWhile       (FILE* file, Node* node, AsmInfo* info);
static void PushIn          (FILE* file, Node* node, AsmInfo* info);
static void PushOut         (FILE* file, Node* node, AsmInfo* info);
static void PushCall        (FILE* file, Node* node, AsmInfo* info);
static void PushRet         (FILE* file, Node* node, AsmInfo* info);
static void PushNextOp      (FILE* file, Node* node, AsmInfo* info);
static void PushFuncName    (FILE* file, Node* node, AsmInfo* info);

static int PrintCommand (FILE* file, const char* command, ...)
{
    assert (file);

    fprintf (file, "\t\t");

    va_list args = {};
    va_start (args, command);

    int n_symbols = vfprintf (file, command, args);

    va_end (args);

    return n_symbols;
}

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
    assert (info);

    if (!node) return;

    if (node->type == NUM_T)     PrintCommand (file, "push %lg \n", node->value.num);
    if (node->type == VAR_T)     PushVar (file, node->value.var, info);
    if (node->type == FUNC_NAME) PushFuncName (file, node, info);
    if (node->type == OP_T)      PushOp (file, node, info);
}

static void PushParam (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    switch (info->prev_option)
    {
    case IN:
        PrintCommand(file, "in \n");
        PopVar (file, node->left->value.var, info);
        break;

    case OUT:
        if (node->left->type == NUM_T)
        {
            PrintCommand(file, "push %lg \n", node->left->value.num);
            PrintCommand(file, "out \n");
            break;
        }
        PushVar (file, node->left->value.var, info);
        PrintCommand(file, "out \n");
        break;

    case FUNC:
        info->names_table[info->names_num] = node->left->value.var;
        PrintCommand(file, "pop [rax + %d] \n", info->names_num);

        info->names_num++;
        break;

    case RET:
        NODES_TO_ASM (PrintCommand(file, "pop rbx \n"));
        return;

    default:
        NO_PROPER_CASE_FOUND;
    }

    NodeToAsm (file, node->right, info);
}

static void PushOp (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    switch (node->value.op)
    {
        case NEXT_OP:          PushNextOp   (file, node, info); break;
        case FUNC:             PushFunction (file, node, info); break;
        case VAR: case GL_VAR: PushNewVar   (file, node, info); break;
        case CALL:             PushCall     (file, node, info); break;
        case RET:              PushRet      (file, node, info); break;
        case IN:               PushIn       (file, node, info); break;
        case OUT:              PushOut      (file, node, info); break;
        case IF:               PushIf       (file, node, info); break;
        case ELSE:             PushElse     (file, node, info); break;
        case WHILE:            PushWhile    (file, node, info); break;
        case EQ:               PushEq       (file, node, info); break;
        case ISEQ:             NODES_TO_ASM (PrintCommand (file, "jne label_%d \n", info->label_num)); break;
        case NEQ:              NODES_TO_ASM (PrintCommand (file, "je label_%d \n",  info->label_num)); break;
        case BIGGER:           NODES_TO_ASM (PrintCommand (file, "jae label_%d \n", info->label_num)); break;
        case SMALLER:          NODES_TO_ASM (PrintCommand (file, "jbe label_%d \n", info->label_num)); break;
        case BIGEQ:            NODES_TO_ASM (PrintCommand (file, "ja label_%d \n",  info->label_num)); break;
        case SMALLEQ:          NODES_TO_ASM (PrintCommand (file, "jb label_%d \n",  info->label_num)); break;
        case ADD:              NODES_TO_ASM (PrintCommand (file, "add \n"));  break;
        case SUB:              NODES_TO_ASM (PrintCommand (file, "sub \n"));  break;
        case MULT:             NODES_TO_ASM (PrintCommand (file, "mult \n")); break;
        case DIV:              NODES_TO_ASM (PrintCommand (file, "div \n"));  break;
        case SIN:              NODES_TO_ASM (PrintCommand (file, "sin \n"));  break;
        case COS:              NODES_TO_ASM (PrintCommand (file, "cos \n"));  break;
        case SQRT:             NODES_TO_ASM (PrintCommand (file, "sqrt \n")); break;
        case PARAM:            PushParam    (file, node, info);               break;
        default:               NO_PROPER_CASE_FOUND;
    }
}

static void PushFunction (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    free (info->names_table);

    info->names_table = (char**) calloc (MAX_NAMES, sizeof (char*));
    info->names_num = 0;
    info->prev_option = FUNC;

    NodeToAsm (file, node->left,  info);
    NodeToAsm (file, node->right, info);
}

static void PushEq (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    NodeToAsm (file, node->right, info);

    if (node->right->type == OP_T && node->right->value.op == CALL) PrintCommand (file, "push rbx \n");

    if (node->left && node->left->type == VAR_T)
    {
        PrintCommand (file, "; new value of %s \n", node->left->value.var);
        int position = GLOBAL_VAR_POS (node->left->value.var);
        if (position >= 0) PrintCommand (file, "pop [%d] \n", position);
        else PrintCommand (file, "pop [rax + %d] \n", VAR_POS (node->left->value.var));
    }
}

static void PushElse (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    NodeToAsm (file, node->left, info);
    int else_label = info->label_num++;

    NodeToAsm (file, node->right->left, info);
    PrintCommand (file, "; if: \n");
    PrintCommand (file, "jump label_%d \n", info->label_num);
    int after_label = info->label_num++;

    PrintCommand (file, "; else: \n");
    PrintCommand (file, "label_%d: \n", else_label);
    NodeToAsm (file, node->right->right, info);
    PrintCommand (file, "; end if/else \n");
    PrintCommand (file, "jump label_%d \n", after_label);
    fprintf (file, "label_%d: \n", after_label);
}

static void PushWhile (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    NodeToAsm (file, node->left,  info);

    int end_label   = info->label_num++;
    int while_label = info->label_num;

    PrintCommand (file, "; while \n");
    PrintCommand (file, "jump label_%d \n", while_label);
    fprintf (file, "label_%d: \n", while_label);

    NodeToAsm (file, node->right, info);

    int temp = info->label_num;
    info->label_num = end_label;
    NodeToAsm (file, node->left, info);
    info->label_num = temp;

    PrintCommand (file, "jump label_%d \n", while_label);
    fprintf (file, "label_%d: \n", end_label);
}

static void PushNewVar (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    NodeToAsm (file, node->right, info);

    PrintCommand (file, "; initialize %s \n", node->left->value.var);

    if (node->value.op == VAR)
    {
        info->names_table[info->names_num] = node->left->value.var;
        PrintCommand(file, "pop [rax + %d] \n", info->names_num);
        info->names_num++;
    }
    else
    {
        info->global_names_table[info->global_names_num] = node->left->value.var;
        PrintCommand(file, "pop [%d] \n", info->global_names_num);
        info->global_names_num++;
    }
}

static void PushIn (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    info->prev_option = IN;
    NodeToAsm (file, node->left, info);
}

static void PushOut (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    info->prev_option = OUT;
    NodeToAsm (file, node->left, info);
}

static void PushCall (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    info->prev_option = CALL;

    PrintCommand(file, "push rcx \n");

    PushParams (file, node->right, info);

    PrintCommand(file, "push rax + %d \n", info->names_num);
    PrintCommand(file, "pop rax \n");
    PrintCommand(file, "call %s \n", node->left->value.var);
    PrintCommand(file, "pop rcx \n");
    PrintCommand(file, "push rax\n", info->names_num);
    PrintCommand(file, "push %d\n", info->names_num);
    PrintCommand(file, "sub\n");
    PrintCommand(file, "pop rax \n");
}

static void PushRet (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    info->prev_option = RET;
    NodeToAsm (file, node->left, info);

    PrintCommand (file, "push rcx \n");
    PrintCommand (file, "ret ; function end\n\n");
}

static void PushNextOp (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    NodeToAsm (file, node->left,  info);
    NodeToAsm (file, node->right, info);
}

static void PushFuncName (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    fprintf (file, "%s: \n", node->value.var);
    PrintCommand (file, "pop rcx \n");
    NodeToAsm (file, node->left,  info);
    NodeToAsm (file, node->right, info);
}

static void PushIf (FILE* file, Node* node, AsmInfo* info)
{
    assert (file);
    assert (node);
    assert (info);

    PrintCommand (file, "; if: \n");
    NODES_TO_ASM (PrintCommand (file, "jump label_%d \n", info->label_num);
    fprintf (file, "label_%d: \n", info->label_num));

    info->label_num++;
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

    if (position >= 0) PrintCommand(file, "push [%d] \n", position);
    else PrintCommand(file, "push [rax + %d] \n", VAR_POS (var));
}

static void PopVar (FILE* file, const char* var, AsmInfo* info)
{
    assert (file);
    assert (var);
    assert (info);

    int position = GLOBAL_VAR_POS (var);

    if (position >= 0) PrintCommand(file, "pop [%d] \n", position);
    else PrintCommand(file, "pop [rax + %d] \n", VAR_POS (var));
}
