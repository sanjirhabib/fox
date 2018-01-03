#include <fox.h>
#pragma once
char* hello_version();
void* hello_invoke(map* v, char* name);
map* hello_reflect();
int exec_cmdline(map* args);
void* user_invoke(map* params, char* name);
map* user_funcs();
