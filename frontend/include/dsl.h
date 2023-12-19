#ifndef DSL_H
#define DSL_H

#define _NUM(val)    CreateNumNode (nullptr, nullptr, val)
#define _VAR(var)    CreateVarNode (nullptr, nullptr, var)

#define  _ADD(L, R)  CreateOpNode (L, R, ADD )
#define  _SUB(L, R)  CreateOpNode (L, R, SUB )
#define _MULT(L, R)  CreateOpNode (L, R, MULT)
#define  _DIV(L, R)  CreateOpNode (L, R, DIV )
#define  _SIN(L)     CreateOpNode (L, nullptr, SIN )
#define  _COS(L)     CreateOpNode (L, nullptr, COS )
#define  _POW(L, R)  CreateOpNode (L, R, POW )
#define  _LN(L)      CreateOpNode (L, nullptr, LN  )

#define CUR_TOKEN      tokens[*cur_token]
#define NEXT_TOKEN     tokens[*cur_token + 1]
#define PREV_TOKEN     tokens[*cur_token - 1]
#define CUR_TOKEN_OP   tokens[*cur_token].value.op
#define CUR_TOKEN_TYPE tokens[*cur_token].type
#define TOKENS_DATA    tokens, cur_token, exit_code
#define CUR_OP         node->value.op

#define ASSERT_TOKENS_DATA assert (tokens); assert (cur_token); assert (exit_code); \
                           if (*exit_code == ERR) return nullptr;

#define CHECK_OP(op)  (CUR_TOKEN_TYPE == OP_T && CUR_TOKEN_OP == op)

#endif
