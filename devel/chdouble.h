#include <fox.h>
#pragma once
char* chdouble_version();
void* chdouble_invoke(map* v, char* name);
map* chdouble_reflect();
int exec_cmdline(map* args);
void* user_invoke(map* params, char* name);
map* user_funcs();
