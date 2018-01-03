#include <fox.h>
#pragma once
void usage(char* name);
char* xpath_version();
void* xpath_invoke(map* v, char* name);
map* xpath_reflect();
int exec_cmdline(map* args);
void* user_invoke(map* params, char* name);
map* user_funcs();
