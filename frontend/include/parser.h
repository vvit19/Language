#ifndef PARSER_H
#define PARSER_H

#include "lang_tree.h"

struct Token
{
    Types type;
    Value value;
};

#define CUR_TOKEN  tokens[*cur_token]
#define NEXT_TOKEN tokens[*cur_token + 1]
#define CUR_OP     node->value.op

Node*  GetGrammar (char* buffer);

#endif
