#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cctype>

typedef double elem_t;

const int MAX_VAR_LENGTH = 15;

enum Ways
{
    LEFT,
    RIGHT,
};

enum Types
{
    NO_TYPE,
    OP_T,
    NUM_T,
    SPEC_T,
    VAR_T
};

enum KeyWords
{
    FUNC,
    VAR,
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
    POW,
    LN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_FUNC,
    CLOSE_FUNC,
    NEXT_OP,
    END,
    PARAM,
    NEXT_PARAM
};

const char* const OperationsArray[] = { "func", "var", "call", "return", "input", "output", "if",
"else", "while", "==", "=", "!=", ">", "<", ">=", "<=", "+", "-", "*", "/", "sin", "cos", "^", "ln",
"(", ")", "{", "}", ";", "end", "param", "," };

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

struct Token
{
    Types type;
    Value value;
};

#define CUR_TOKEN  tokens[*cur_token]
#define NEXT_TOKEN tokens[*cur_token + 1]
#define CUR_OP     node->value.op

Node*  Diff                 (Node* node, void (*DumpFunction) (Node*, Node*, const char*, bool));
elem_t Eval                 (Node* node, elem_t var_value);
Node*  CreateNumNode        (Node* left, Node* right, elem_t num);
Node*  CreateVarNode        (Node* left, Node* right, const char* var);
Node*  CreateOpNode         (Node* left, Node* right, KeyWords op);
Node*  CopyNode             (Node* original_node);
void   TreeDtor             (Node* node);
Node*  GetGrammar           (char* buffer);
void   Optimize             (Node** node);

#endif
