#include <fox.h>
#pragma once
map* insert_tree(map* row, char* table, char* db); //tree.fox
map* update_tree(char* id, map* row, char* table, char* db); //tree.fox
char* tree_version(); 
void* tree_invoke(map* v, char* name); 
map* tree_reflect(); 
int exec_cmdline(map* args); 
void* user_invoke(map* params, char* name); 
map* user_funcs(); 
