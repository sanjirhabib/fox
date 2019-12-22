#pragma once
#include <sys/wait.h>
#include <core.h>
#pragma fox module playground
#include <dir.h>
#ifdef PHP_MOD
#undef assert
#define assert(x) if(!(x)){ fox_error("Assert failed!",1); }
#endif
void cache_blast(char* type); //playground.fox
void* cache(void* key, char* type, void* out); //playground.fox
void gotoxy(int x, int y); //playground.fox
int floor_pow2(int i); //playground.fox
int utest(char* found, char* expect, char* in, char* title); //playground.fox
char* xlog(char* str); //playground.fox
char* hello(); //playground.fox
map* command_line(char* in, int argc, char** argv); //playground.fox
map* param_test(char* one, char* two); //playground.fox
int test_add(int a, int b); //playground.fox
map* test_map(map* out); //playground.fox
map* load_global(char* file); //playground.fox
int err_msg(char* msg, char** ptr); //playground.fox
char* tutorial(); //playground.fox
char* fork_exec(char* cmd, map* params); //playground.fox
int fib(int i); //playground.fox
int msleep(int msecs); //playground.fox
int str_ascii(char* in); //playground.fox
char* ping(char* str); //playground.fox
char* outx(char* str); //playground.fox
char* flush_out(); //playground.fox
int ret_print(void* ret); //playground.fox
