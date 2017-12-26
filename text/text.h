#include <fox.h>
#pragma once
char* text_version();
void* text_invoke(map* v, char* name);
map* text_reflect();
int exec_cmdline(map* args);
void* user_invoke(map* params, char* name);
map* user_funcs();
