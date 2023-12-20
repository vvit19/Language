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
static Node* GetArgs        (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetExpression  (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetAddSub      (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetMult        (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetUnary       (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetBrackets    (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetVariable    (Token* tokens, int* cur_token, ExitCodes* exit_code);
static Node* GetNumber      (Token* tokens, int* cur_token, ExitCodes* exit_code);

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

    free (tokens);

    return main_node;
}

static Node* GetActions (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CHECK_OP (END)) return nullptr;

    if (CHECK_OP (NEXT_OP))
    {
        *cur_token += 1;
    }

    if (CHECK_OP (CLOSE_BRACKET) || CHECK_OP (CLOSE_FUNC))
    {
        *cur_token += 1;
        return nullptr;
    }

    Node* left  = GetFunction (TOKENS_DATA);
    Node* right = GetActions  (TOKENS_DATA);

    return CreateOpNode (left, right, NEXT_OP);
}

static Node* GetFunction (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CHECK_OP (FUNC))
    {
        *cur_token += 1;

        Node* signature = GetSignature (TOKENS_DATA);
        SYNT_ASSERT (exit_code, CHECK_OP (OPEN_FUNC));
        *cur_token += 1;
        Node* actions   = GetActions   (TOKENS_DATA);

        return CreateOpNode (signature, actions, FUNC);
    }

    return GetIfElse (TOKENS_DATA);
}

static Node* GetSignature (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    SYNT_ASSERT (exit_code, CUR_TOKEN_TYPE == VAR_T);
    if (*exit_code == ERR) return nullptr;

    Node* name = CreateVarNode (nullptr, nullptr, CUR_TOKEN.value.var);
    name->type = FUNC_NAME;
    *cur_token += 1;

    Node* param = GetParam (TOKENS_DATA);
    name->left = param;

    return name;
}

static Node* GetParam (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CHECK_OP (OPEN_BRACKET) || CHECK_OP (NEXT_PARAM)) *cur_token += 1;

    if (CHECK_OP (CLOSE_BRACKET) || CHECK_OP (NEXT_OP))
    {
        *cur_token += 1;
        return nullptr;
    }

    Node* left  = GetExpression (TOKENS_DATA);
    Node* right = GetParam      (TOKENS_DATA);

    return CreateOpNode (left, right, PARAM);
}

static Node* GetIfElse (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CUR_TOKEN_TYPE == OP_T && CUR_TOKEN_OP == IF)
    {
        *cur_token += 1;
        SYNT_ASSERT (exit_code, CHECK_OP (OPEN_BRACKET))
        *cur_token += 1;

        Node* main_node = CreateOpNode (nullptr, nullptr, IF);

        Node* statement = GetExpression (TOKENS_DATA);

        SYNT_ASSERT (exit_code, CHECK_OP (CLOSE_BRACKET))
        *cur_token += 1;
        SYNT_ASSERT (exit_code, CHECK_OP (OPEN_FUNC));
        *cur_token += 1;

        Node* if_body   = GetActions (TOKENS_DATA);

        SYNT_ASSERT (exit_code, PREV_TOKEN.type == OP_T && PREV_TOKEN.value.op == CLOSE_FUNC)

        main_node->left = statement;

        if (CUR_TOKEN_TYPE == OP_T && CUR_TOKEN_OP == ELSE)
        {
            *cur_token += 1;
            SYNT_ASSERT (exit_code, CHECK_OP (OPEN_FUNC))
            *cur_token += 1;

            Node* else_body = GetActions (TOKENS_DATA);
            SYNT_ASSERT (exit_code, PREV_TOKEN.type == OP_T && PREV_TOKEN.value.op == CLOSE_FUNC)

            main_node->value.op = ELSE;
            main_node->right = CreateOpNode (if_body, else_body, IF);
        }
        else main_node->right = if_body;

        return main_node;
    }

    return GetWhile (TOKENS_DATA);
}

static Node* GetWhile (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CHECK_OP (WHILE))
    {
        *cur_token += 1;
        SYNT_ASSERT (exit_code, CHECK_OP (OPEN_BRACKET))
        *cur_token += 1;

        Node* statement = GetExpression (TOKENS_DATA);

        SYNT_ASSERT (exit_code, CHECK_OP (CLOSE_BRACKET))
        *cur_token += 1;
        SYNT_ASSERT (exit_code, CHECK_OP (OPEN_FUNC));
        *cur_token += 1;

        Node* body = GetActions (TOKENS_DATA);
        SYNT_ASSERT (exit_code, PREV_TOKEN.type == OP_T && PREV_TOKEN.value.op == CLOSE_FUNC)

        return CreateOpNode (statement, body, WHILE);
    }

    return GetRet (TOKENS_DATA);
}

static Node* GetRet (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CHECK_OP (RET))
    {
        *cur_token += 1;
        Node* param = GetParam (TOKENS_DATA);

        return CreateOpNode (param, nullptr, RET);
    }

    return GetCall (TOKENS_DATA);
}

static Node* GetCall (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CHECK_OP (CALL))
    {
        *cur_token += 1;
        Node* func_name = CreateVarNode (nullptr, nullptr, CUR_TOKEN.value.var);
        func_name->type = FUNC_NAME;
        *cur_token += 1;
        Node* params    = GetParam (TOKENS_DATA);

        return CreateOpNode (func_name, params, CALL);
    }

    return GetInOut (TOKENS_DATA);
}

static Node* GetInOut (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CHECK_OP (IN) || CHECK_OP (OUT))
    {
        KeyWords op = CUR_TOKEN_OP;
        *cur_token += 1;
        return CreateOpNode (GetParam (TOKENS_DATA), nullptr, op);
    }

    return GetArgs (TOKENS_DATA);
}

static Node* GetArgs (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    if (CHECK_OP (VAR) || CHECK_OP (GL_VAR))
    {
        KeyWords op = CUR_TOKEN_OP;
        *cur_token += 1;
        Node* var_node  = GetVariable (TOKENS_DATA);
        Node* main_node = GetCall (TOKENS_DATA);

        return CreateOpNode (var_node, main_node, op);
    }

    return GetExpression (TOKENS_DATA);

}

static Node* GetExpression (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    Node* main_node = GetAddSub (TOKENS_DATA);

    if ( CUR_TOKEN_TYPE == OP_T    &&
       ( CUR_TOKEN_OP   == EQ      ||
         CUR_TOKEN_OP   == ISEQ    ||
         CUR_TOKEN_OP   == NEQ     ||
         CUR_TOKEN_OP   == SMALLER ||
         CUR_TOKEN_OP   == SMALLEQ ||
         CUR_TOKEN_OP   == BIGGER  ||
         CUR_TOKEN_OP   == BIGEQ   ))
    {
        KeyWords op = CUR_TOKEN_OP;
        *cur_token += 1;
        Node* sub_node = GetCall (TOKENS_DATA);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}

static Node* GetAddSub (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    Node* main_node = GetMult (TOKENS_DATA);

    while (CHECK_OP (SUB) || CHECK_OP (ADD))
    {
        KeyWords op = CUR_TOKEN_OP;
        *cur_token += 1;

        Node* sub_node = GetMult (TOKENS_DATA);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}

static Node* GetMult (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    Node* main_node = GetUnary (TOKENS_DATA);

    while (CHECK_OP (MULT) || CHECK_OP (DIV) || CHECK_OP (SQRT))
    {
        KeyWords op = CUR_TOKEN_OP;
        *cur_token += 1;

        Node* sub_node = GetUnary (TOKENS_DATA);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}


static Node* GetUnary (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    ASSERT_TOKENS_DATA

    Node* main_node = GetBrackets (TOKENS_DATA);

    while (CUR_TOKEN_TYPE == OP_T &&
          (CUR_TOKEN_OP == SIN || CUR_TOKEN_OP == COS))
    {
        KeyWords op = CUR_TOKEN_OP;

        *cur_token += 1;
        SYNT_ASSERT (exit_code, CHECK_OP (OPEN_BRACKET));
        *cur_token += 1;

        main_node = GetAddSub (TOKENS_DATA);

        SYNT_ASSERT (exit_code, CHECK_OP (CLOSE_BRACKET));
        *cur_token += 1;

        main_node = CreateOpNode (main_node, nullptr, op);
    }

    return main_node;
}

static Node* GetBrackets (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    if (CHECK_OP (OPEN_BRACKET))
    {
        *cur_token += 1;
        Node* main_node = GetAddSub (TOKENS_DATA);

        SYNT_ASSERT (exit_code, CHECK_OP (CLOSE_BRACKET));
        *cur_token += 1;

        return main_node;
    }
    else return GetVariable (TOKENS_DATA);
}

static Node* GetVariable (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    Node* main_node = GetNumber (TOKENS_DATA);

    if (CUR_TOKEN_TYPE == VAR_T)
    {
        main_node = _VAR (CUR_TOKEN.value.var);
        *cur_token += 1;
    }

    return main_node;
}

static Node* GetNumber (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    Node* main_node = nullptr;

    if (CHECK_OP (SUB) && NEXT_TOKEN.type == NUM_T)
    {
        main_node = _NUM ((-1.0) * NEXT_TOKEN.value.num);
        *cur_token += 2;
    }
    if (CUR_TOKEN_TYPE == NUM_T)
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

    for (size_t cnt = 0; cnt < sizeof (LanguageSyntax) / sizeof (const char*); cnt++)
    {
        int op_length = strlen (LanguageSyntax[cnt]);
        if (strncmp (LanguageSyntax[cnt], &buffer[*i], op_length) == 0)
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

    for (int cnt = 0; isalpha (buffer[*i]) || buffer[*i] == '_' || isdigit (buffer[*i]); *i += 1, cnt++)
    {
        variable[cnt] = buffer[*i];
    }

    strcpy (tokens[cur_token].value.var, variable);
}
