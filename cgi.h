#include <fox.h>
#pragma once
void* px(void* str, int newline);
void xexit(int val);
void* fox_error(char* msg, int dump);
void* call_php(map* params, char* func);
char* cgi_version();
void* cgi_invoke(map* v, char* name);
map* cgi_reflect();
int exec_cmdline(map* args);
