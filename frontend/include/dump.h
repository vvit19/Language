#ifndef DUMP_H
#define DUMP_H

#include "parser.h"

const char* const OUTPUT_DOT        = "dump.dot";
const char* const OUTPUT_TREE       = "tree.png";

void GraphDump (Node* node);

#endif
