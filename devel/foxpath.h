#include <fox.h>
#pragma once
map* execute_xpath_expression(char* in, char* xpathExpr);
map* print_xpath_nodes(void* nodes1);
char* foxpath_version();
void* foxpath_invoke(map* v, char* name);
map* foxpath_reflect();
int exec_cmdline(map* args);
void* user_invoke(map* params, char* name);
map* user_funcs();
