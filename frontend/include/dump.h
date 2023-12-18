#ifndef DUMP_H
#define DUMP_H

#include "parser.h"

const char* const OUTPUT_DOT        = "dump.dot";
const char* const OUTPUT_PNG        = "dump.png";
const char* const OUTPUT_AST        = "tree.txt";

void  GraphDump (Node* node);
void  ASTDump   (Node* node);

#endif
