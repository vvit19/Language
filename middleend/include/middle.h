#ifndef MIDDLE_H
#define MIDDLE_H

#include "lang_tree.h"

const char* const TREE_TXT    = "../frontend/tree.txt";

Node* GetTree  (char* buffer);
void  Optimize (Node** node);
void  ASTDump  (Node* main_node);

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

#define CUR_OP       node->value.op

#endif
