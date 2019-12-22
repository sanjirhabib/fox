#pragma once
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <execinfo.h>
#include <signal.h>
#include <sys/mman.h>
#pragma fox module core
enum Types {
	Null,Int,Double,String,Blob,Map,Vector,Index,Keys,Cell,Cell2,Tail
};
typedef struct Mapcell {
	int nextid;
	int hkey;
	char* id;
	void* val;
} Mapcell;
typedef struct map {
	int len;
	char type;
	unsigned short magic;
	union {
		struct Mapcell* pairs;
		void** vars;
	};
} map;
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#if UINTPTR_MAX == 0xffffffff
typedef long long void*;
#else
typedef void* var;
#endif
#define is_i(x) ((long long)(x)>>61 & 2)
#define is_f(x) ((*(long long*)&(x))>>61 & 1)
#define is_num(x) (is_i(x)||is_f(x))
extern map* _globals;
extern int _printed;
extern int _is_web;
extern char* skip;
extern int _queries;
#define None 0x0F9AD3BA
#define End (char*)(0x0FF1B14E059AD3BA)
#include <gc.h>
#include <generator.h>
#define MAXMEM 40*1024*1024
#define MIN_CHAIN 1
extern char tempbuff[128];
extern char **environ;
char* strstr(const char* str1, const char* str2); //core.fox
int chdir(const char* path); //core.fox
int max_mem(); //core.fox
int curr_mem(); //core.fox
int run(map* args); //core.fox
int rand(); //core.fox
void* px(void* str, int newline); //core.fox
void xexit(int val); //core.fox
void* fox_error(char* msg, int dump); //core.fox
static void fox_signal_handler(int sig); //core.fox
void stack_dump_direct(); //core.fox
char* stack_str(); //core.fox
void fox_stack_dump(); //core.fox
size_t blob_size(char* in); //core.fox
int str_len(char* str); //core.fox
char* new_blob(int size); //core.fox
char* blob_dup(char* str, int len); //core.fox
char* str_dup_len(char* str, int len); //core.fox
char* str_dup(char* str); //core.fox
char* sub_str(char* src, int from, int len); //core.fox
char* print(char* str, FILE* fp); //core.fox
char* str_times(char* str, int times); //core.fox
char* xstr(char* ret, ...); //core.fox
char* xcat(char* ret, ...); //core.fox
char* blob_cat(char* str1, char* str2, size_t len); //core.fox
char* cat(char* str1, char* str2, size_t len); //core.fox
static char* vec_json(map* mp, int indent); //core.fox
char* json(map* mp, int indent); //core.fox
char* to_str(void* v, char* null, int human); //core.fox
char* str_quote(char* head); //core.fox
char* double_str(double val); //core.fox
char* int_str(long long value); //core.fox
int str_eq(char* str, char* str1); //core.fox
size_t str_hash(unsigned char * str); //core.fox
map* xvec(void* v1, ...); //core.fox
map* xmap(char* k1, ...); //core.fox
char* new_str(int len); //core.fox
map* new_map(); //core.fox
map* new_vec(); //core.fox
int key_eq(map* mp, int idx, char* id); //core.fox
int key_hash(char* id); //core.fox
int map_hash(map* mp, char* id); //core.fox
int map_idx(map* mp, char* id); //core.fox
int map_has_key(map* mp, char* id); //core.fox
map* vec_realloc(map* mp, int extra); //core.fox
map* vec_add(map* mp, void* v); //core.fox
map* set(map* mp, int idx, void* val); //core.fox
long long is_int(void* v); //core.fox
double is_double(void* v); //core.fox
void* int_var(size_t i); //core.fox
void* double_var(double f); //core.fox
int has_id(map* mp, int idx); //core.fox
void* map_idp(map* mp, int idx); //core.fox
void* map_id(map* mp, int idx); //core.fox
int map_len(map* mp); //core.fox
char* map_key(map* mp, int idx); //core.fox
char* is_str(void* v); //core.fox
char* is_blob(void* v); //core.fox
map* is_vec(void* v); //core.fox
map* is_hash(void* v); //core.fox
map* is_map(void* v); //core.fox
double str_double(char* v); //core.fox
double to_double(void* v); //core.fox
long long str_int(char* v); //core.fox
long long to_int(void* v); //core.fox
int next(map* mp, int idx, char** key, void** val); //core.fox
int stoi(char* str); //core.fox
map* globals(); //core.fox
int map_size(map* mp); //core.fox
int ceil_pow2(int i); //core.fox
map* add(map* mp, void* k, void* v); //core.fox
map* vec_set(map* mp, int idx, void* v); //core.fox
map* map_add(map* mp, char* key, void* v); //core.fox
static int map_nextno(map* mp); //core.fox
map* map_index(map* mp, int idx); //core.fox
map* map_reindex(map* mp); //core.fox
char* null_str(char* in); //core.fox
map* ptrs_vec(void** ptrs, int len); //core.fox
int exec(char* cmd, char** output); //core.fox
void init_rand(); //core.fox
void* map_val(map* mp, char* key); //core.fox
map* env_vars(); //core.fox
map* argv_map(char** argv, int argc); //core.fox
map* str_split(char* str, char* by, int limit); //core.fox
char* str_has(char* str, char* sub); //core.fox
char* exec_str(char* in); //core.fox
