#include <fox.h>
#pragma once
char* teststruct_version(); 
void* teststruct_invoke(map* v, char* name); 
map* teststruct_reflect(); 
int cmdline(); 
int exec_cmdline(map* args); 
void* user_invoke(map* params, char* name); 
map* user_funcs(); 
