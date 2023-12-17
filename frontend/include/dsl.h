#ifndef DSL_H
#define DSL_H

#define NO_PROPER_CASE_FOUND \
    fprintf (stderr, "Default case reached in file: %s, function: %s, line: %d\n", \
                          __FILE__, __PRETTY_FUNCTION__, __LINE__)

#define dL Diff     (node->left, DumpFunction)
#define dR Diff     (node->right, DumpFunction)
#define cL CopyNode (node->left)
#define cR CopyNode (node->right)

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

#define CUR_TOKEN  tokens[*cur_token]
#define NEXT_TOKEN tokens[*cur_token + 1]
#define CUR_OP     node->value.op

#endif
