#include <fox.h>
#pragma once
char* ucs_str(int* in, int len);
int* str_ucs(char* in);
int is_letter(int code);
int* word_end(int* in);
int* word_start(int* in);
int* ucs_substr(int* in, int from, int upto);
int ucs_len(int* in);
map* ucs_words(int* uin);
map* str_words(char* in);
void go(char* in);
char* stem_bangla(char* in);
int stem_test();
char* text_version();
void* text_invoke(map* v, char* name);
map* text_reflect();
int exec_cmdline(map* args);
void* user_invoke(map* params, char* name);
map* user_funcs();
