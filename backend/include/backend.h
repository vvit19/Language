#ifndef BACKEND_H
#define BACKEND_H

#include "lang_tree.h"

const char* const TREE_TXT = "../middleend/tree.txt";

Node* GetTree  (char* buffer);
void  GetAsmCode (const char* asm_file_name, Node* main_node);

#endif
