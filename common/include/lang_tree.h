#ifndef LANG_TREE_H
#define LANG_TREE_H

#include "utils.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cctype>

const int MAX_VAR_LENGTH = 25;

typedef double elem_t;

enum Types
{
    NO_TYPE,
    OP_T,
    NUM_T,
    VAR_T,
    FUNC_NAME,
};

enum KeyWords
{
    FUNC,
    VAR,
    GL_VAR,
    CALL,
    RET,
    IN,
    OUT,
    IF,
    ELSE,
    WHILE,
    ISEQ,
    EQ,
    NEQ,
    BIGGER,
    SMALLER,
    BIGEQ,
    SMALLEQ,
    ADD,
    SUB,
    MULT,
    DIV,
    SIN,
    COS,
    SQRT,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_FUNC,
    CLOSE_FUNC,
    NEXT_OP,
    END,
    PARAM,
    NEXT_PARAM
};

const char* const LanguageSyntax[] = { "func", "var", "gl_var", "call", "return", "in", "out", "if",
"else", "while", "==", "=", "!=", ">", "<", ">=", "<=", "+", "-", "*", "/", "sin", "cos", "sqrt",
"(", ")", "{", "}", ";", "end", "param", "," };

const char* const OperationsArray[] = { "FUNC", "VAR", "GL_VAR", "CALL", "RET", "IN", "OUT", "IF", "ELSE", "WHILE",
"ISEQ", "EQ", "NEQ", "BIGGER", "SMALLER", "BIGEQ", "SMALLEQ", "ADD", "SUB", "MULT", "DIV", "SIN", "COS", "SQRT",
"OB", "CB", "OF", "CF", ";", "END", "PARAM", "," };

union Value
{
    elem_t num;
    KeyWords op;
    char var[MAX_VAR_LENGTH];
};

struct Node
{
    Node* left;
    Node* right;

    Types type;
    Value value;
};

Node*  CreateNumNode        (Node* left, Node* right, elem_t num);
Node*  CreateVarNode        (Node* left, Node* right, const char* var);
Node*  CreateOpNode         (Node* left, Node* right, KeyWords op);
void   TreeDtor             (Node* node);

#endif
