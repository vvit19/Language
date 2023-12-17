#include "utils.h"
#include "../include/dsl.h"
#include "../include/parser.h"
#include "dump.h"

const int MAX_TOKENS = 1024;

static void  Lexer          (Token* tokens, char* buffer);
static void  AddVarToken    (Token* tokens, int cur_token, char* buffer, int* i);
static void  AddNumToken    (Token* tokens, int cur_token, char* buffer, int* i);
static void  AddOpToken     (Token* tokens, int cur_token, char* buffer, int* i);

static Node* GetActions     (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetFunction    (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetSignature   (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetParam       (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetIfElse      (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetWhile       (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetRet         (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetCall        (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetInOut       (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetArgs        (Token* tokens, int* cur_token, ExitCodes* exit_code); // foo (Args)
static Node* GetExpression  (Token* tokens, int* cur_token, ExitCodes* exit_code); // =, ==, >, <, ...
static Node* GetAddSub      (Token* tokens, int* cur_token, ExitCodes* exit_code); // +-
static Node* GetMult        (Token* tokens, int* cur_token, ExitCodes* exit_code); // */
static Node* GetPower       (Token* tokens, int* cur_token, ExitCodes* exit_code); // ^
static Node* GetUnary       (Token* tokens, int* cur_token, ExitCodes* exit_code); // unary: sin, cos, ln
static Node* GetBrackets    (Token* tokens, int* cur_token, ExitCodes* exit_code); // ()
static Node* GetVariable    (Token* tokens, int* cur_token, ExitCodes* exit_code); // variables
static Node* GetNumber      (Token* tokens, int* cur_token, ExitCodes* exit_code); // numbers

int main (int argc, const char** argv)
{
    char* buffer = GetFileContent (argv[1]);
    Node* node   = GetGrammar (buffer);
    GraphDump (node);
    TreeDtor (node);
    free (buffer);
}

Node* GetGrammar (char* buffer)
{
    Token* tokens = (Token*) calloc (MAX_TOKENS, sizeof (Token));

    Lexer (tokens, buffer);

    ExitCodes exit_code = NO_ERR;
    int cur_token = 0;
    Node* main_node = GetActions (tokens, &cur_token, &exit_code);

    SYNT_ASSERT (&exit_code, tokens[cur_token].type == OP_T && tokens[cur_token].value.op == END);

    if (exit_code == ERR)
    {
        TreeDtor (main_node);
        return nullptr;
    }

    return main_node;
}

static Node* GetActions (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == END) return nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == NEXT_OP)
    {
        *cur_token += 1;
    }

    if (CUR_TOKEN.type == OP_T && (CUR_TOKEN.value.op == CLOSE_BRACKET || CUR_TOKEN.value.op == CLOSE_FUNC))
    {
        *cur_token += 1;
        return nullptr;
    }

    Node* left  = GetFunction (tokens, cur_token, exit_code);
    Node* right = GetActions  (tokens, cur_token, exit_code);

    return CreateOpNode (left, right, NEXT_OP);
}

static Node* GetFunction (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == FUNC)
    {
        *cur_token += 1;

        Node* signature = GetSignature (tokens, cur_token, exit_code);
        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == OPEN_FUNC);
        *cur_token += 1;
        Node* actions   = GetActions   (tokens, cur_token, exit_code);

        return CreateOpNode (signature, actions, FUNC);
    }

    return GetIfElse (tokens, cur_token, exit_code);
}

static Node* GetSignature (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    SYNT_ASSERT (exit_code, CUR_TOKEN.type == VAR_T);
    if (*exit_code == ERR) return nullptr;

    Node* name = CreateVarNode (nullptr, nullptr, CUR_TOKEN.value.var);
    *cur_token += 1;

    Node* param = GetParam (tokens, cur_token, exit_code);
    name->left = param;

    return name;
}

static Node* GetParam (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && (CUR_TOKEN.value.op == OPEN_BRACKET || CUR_TOKEN.value.op == NEXT_PARAM)) *cur_token += 1;

    if (CUR_TOKEN.type == OP_T && (CUR_TOKEN.value.op == CLOSE_BRACKET || CUR_TOKEN.value.op == NEXT_OP))
    {
        *cur_token += 1;
        return nullptr;
    }

    Node* left  = GetExpression (tokens, cur_token, exit_code);
    Node* right = GetParam      (tokens, cur_token, exit_code);

    return CreateOpNode (left, right, PARAM);
}

static Node* GetIfElse (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == IF)
    {
        *cur_token += 1;
        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == OPEN_BRACKET)
        *cur_token += 1;

        Node* main_node = CreateOpNode (nullptr, nullptr, IF);

        Node* statement = GetExpression (tokens, cur_token, exit_code);

        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == CLOSE_BRACKET)
        *cur_token += 1;
        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == OPEN_FUNC);
        *cur_token += 1;

        Node* if_body   = GetActions (tokens, cur_token, exit_code);

        //synt assert prev token == }

        main_node->right = statement;

        if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == ELSE)
        {
            *cur_token += 1;
            SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == OPEN_FUNC)
            *cur_token += 1;

            Node* else_body = GetActions (tokens, cur_token, exit_code);

            main_node->left = CreateOpNode (else_body, if_body, ELSE);
        }
        else main_node->left = if_body;

        return main_node;
    }

    return GetWhile (tokens, cur_token, exit_code);
}

static Node* GetWhile (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == WHILE)
    {
        *cur_token += 1;
        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == OPEN_BRACKET)
        *cur_token += 1;

        Node* statement = GetExpression (tokens, cur_token, exit_code);

        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == CLOSE_BRACKET)
        *cur_token += 1;
        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == OPEN_FUNC);
        *cur_token += 1;

        Node* body = GetActions (tokens, cur_token, exit_code);

        return CreateOpNode (body, statement, WHILE);
    }

    return GetRet (tokens, cur_token, exit_code);
}

static Node* GetRet (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == RET)
    {
        *cur_token += 1;
        Node* param = GetParam (tokens, cur_token, exit_code);

        return CreateOpNode (param, nullptr, RET);
    }

    return GetCall (tokens, cur_token, exit_code);
}

static Node* GetCall (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == CALL)
    {
        *cur_token += 1;
        Node* func_name = CreateVarNode (nullptr, nullptr, CUR_TOKEN.value.var);
        puts (CUR_TOKEN.value.var);
        *cur_token += 1;
        Node* params    = GetParam (tokens, cur_token, exit_code);

        return CreateOpNode (func_name, params, CALL);
    }

    return GetInOut (tokens, cur_token, exit_code);
}

static Node* GetInOut (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && (CUR_TOKEN.value.op == IN || CUR_TOKEN.value.op == OUT))
    {
        KeyWords op = CUR_TOKEN.value.op;
        *cur_token += 1;
        return CreateOpNode (GetParam (tokens, cur_token, exit_code), nullptr, op);
    }

    return GetArgs (tokens, cur_token, exit_code);
}

static Node* GetArgs (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == VAR)
    {
        KeyWords op = CUR_TOKEN.value.op;
        *cur_token += 1;
        Node* var_node  = GetVariable (tokens, cur_token, exit_code);
        Node* main_node = GetCall (tokens, cur_token, exit_code);

        return CreateOpNode (var_node, main_node, op);
    }

    return GetExpression (tokens, cur_token, exit_code);

}

static Node* GetExpression (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetAddSub (tokens, cur_token, exit_code);

    if ( CUR_TOKEN.type     == OP_T    &&
       ( CUR_TOKEN.value.op == EQ      ||
         CUR_TOKEN.value.op == ISEQ    ||
         CUR_TOKEN.value.op == NEQ     ||
         CUR_TOKEN.value.op == SMALLER ||
         CUR_TOKEN.value.op == SMALLEQ ||
         CUR_TOKEN.value.op == BIGGER  ||
         CUR_TOKEN.value.op == BIGEQ   ))
    {
        KeyWords op = CUR_TOKEN.value.op;
        *cur_token += 1;
        Node* sub_node = GetAddSub (tokens, cur_token, exit_code);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}

static Node* GetAddSub (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetMult (tokens, cur_token, exit_code);

    while (CUR_TOKEN.type == OP_T &&
          (CUR_TOKEN.value.op == SUB || CUR_TOKEN.value.op == ADD))
    {
        KeyWords op = CUR_TOKEN.value.op;
        *cur_token += 1;

        Node* sub_node = GetMult (tokens, cur_token, exit_code);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}

static Node* GetMult (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetPower (tokens, cur_token, exit_code);

    while (CUR_TOKEN.type == OP_T &&
          (CUR_TOKEN.value.op == MULT || CUR_TOKEN.value.op == DIV))
    {
        KeyWords op = CUR_TOKEN.value.op;
        *cur_token += 1;

        Node* sub_node = GetPower (tokens, cur_token, exit_code);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}

static Node* GetPower (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetUnary (tokens, cur_token, exit_code);

    while (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == POW)
    {
        KeyWords op = CUR_TOKEN.value.op;
        *cur_token += 1;

        Node* sub_node = GetUnary (tokens, cur_token, exit_code);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}


static Node* GetUnary (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetBrackets (tokens, cur_token, exit_code);

    while (CUR_TOKEN.type == OP_T &&
          (CUR_TOKEN.value.op == SIN || CUR_TOKEN.value.op == COS || CUR_TOKEN.value.op == LN))
    {
        KeyWords op = CUR_TOKEN.value.op;

        *cur_token += 1;
        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == OPEN_BRACKET);
        *cur_token += 1;

        main_node = GetAddSub (tokens, cur_token, exit_code);

        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == CLOSE_BRACKET);
        *cur_token += 1;

        main_node = CreateOpNode (main_node, nullptr, op);
    }

    return main_node;
}

static Node* GetBrackets (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == OPEN_BRACKET)
    {
        *cur_token += 1;
        Node* main_node = GetAddSub (tokens, cur_token, exit_code);

        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == CLOSE_BRACKET);
        *cur_token += 1;

        return main_node;
    }
    else return GetVariable (tokens, cur_token, exit_code);
}

static Node* GetVariable (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    Node* main_node = GetNumber (tokens, cur_token, exit_code);

    if (CUR_TOKEN.type == VAR_T)
    {
        main_node = _VAR (CUR_TOKEN.value.var);
        puts (main_node->value.var);
        *cur_token += 1;
    }

    return main_node;
}

static Node* GetNumber (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    Node* main_node = nullptr;

    if (CUR_TOKEN.type == OP_T && CUR_TOKEN.value.op == SUB && NEXT_TOKEN.type == NUM_T)
    {
        main_node = _NUM ((-1.0) * NEXT_TOKEN.value.num);
        *cur_token += 2;
    }
    if (CUR_TOKEN.type == NUM_T)
    {
        main_node = _NUM (CUR_TOKEN.value.num);
        *cur_token += 1;
    }

    return main_node;
}

static void Lexer (Token* tokens, char* buffer)
{
    int tokens_cnt = 0;

    for (int i = 0; buffer[i] != '\0'; tokens_cnt++)
    {
        i = SkipSpaces (buffer, i);
        if (buffer[i] == '\0') break;

        tokens[tokens_cnt].type = NO_TYPE;

        if (isdigit (buffer[i]))
        {
            AddNumToken (tokens, tokens_cnt, buffer, &i);
        }

        else
        {
            AddOpToken (tokens, tokens_cnt, buffer, &i);

            if (tokens[tokens_cnt].type != NO_TYPE) continue;

            AddVarToken (tokens, tokens_cnt, buffer, &i);
        }
    }

    tokens[tokens_cnt].type     = OP_T;
    tokens[tokens_cnt].value.op = END;
}

static void AddNumToken (Token* tokens, int cur_token, char* buffer, int* i)
{
    assert (tokens);
    assert (buffer);
    assert (i);

    tokens[cur_token].type = NUM_T;

    char* end_ptr = &buffer[*i];
    tokens[cur_token].value.num = strtod (&buffer[*i], &end_ptr);

    while (&buffer[*i] != end_ptr) *i += 1;
}

static void AddOpToken (Token* tokens, int cur_token, char* buffer, int* i)
{
    assert (tokens);
    assert (buffer);
    assert (i);

    for (size_t cnt = 0; cnt < sizeof (OperationsArray) / sizeof (const char*); cnt++)
    {
        int op_length = strlen (OperationsArray[cnt]);
        if (strncmp (OperationsArray[cnt], &buffer[*i], op_length) == 0)
        {
            tokens[cur_token].type = OP_T;
            *i += op_length;
            tokens[cur_token].value.op = (KeyWords) cnt;

            break;
        }
    }
}

static void AddVarToken (Token* tokens, int cur_token, char* buffer, int* i)
{
    assert (tokens);
    assert (buffer);
    assert (i);

    tokens[cur_token].type = VAR_T;
    char variable[MAX_VAR_LENGTH] = "";

    for (int k = 0; isalpha (buffer[*i]) || buffer[*i] == '_' || isdigit (buffer[*i]); *i += 1, k++)
    {
        variable[k] = buffer[*i];
    }

    strcpy (tokens[cur_token].value.var, variable);
}

Node* CreateOpNode (Node* left, Node* right, KeyWords op)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left     = left;
    node->right    = right;

    node->type     = OP_T;
    node->value.op = op;

    return node;
}

Node* CreateVarNode (Node* left, Node* right, const char* var)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left     = left;
    node->right    = right;

    node->type     = VAR_T;
    strcpy (node->value.var, var);

    return node;
}

Node* CreateNumNode (Node* left, Node* right, elem_t num)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left      = left;
    node->right     = right;

    node->type      = NUM_T;
    node->value.num = num;

    return node;
}

void TreeDtor (Node* node)
{
    if (!node) return;

    if (node->left)  TreeDtor (node->left);
    if (node->right) TreeDtor (node->right);

    free (node);
    node = nullptr;
}
