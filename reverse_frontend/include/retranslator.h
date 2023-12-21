#ifndef RETRANSL_H
#define RETRANSL_H

#include "lang_tree.h"

const char* const TREE_TXT  = "../middleend/tree.txt";
const char* const OUTPUT    = "output.txt";

Node* GetTree     (char* buffer);
void  GetLangCode (Node* main_node);

#endif
