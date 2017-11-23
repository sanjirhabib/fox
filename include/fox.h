/* This is a generated file. To change it, edit function foxh() in fox.c */
#define _XOPEN_SOURCE
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <setjmp.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#ifndef __MINGW32__
#include <execinfo.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
//#include <ucontext.h>
enum Types {
	Null,Skip,Int,String,Blob,Map,Vector,Index,Keys,Cell,Cell2,Tail
};
typedef struct cons {
	short nextid;
	int hkey;
	char* id;
	void* val;
} cons;
typedef struct map {
	int len;
	char type;
	union {
		struct cons* pairs;
		void** vars;
	};
} map;

#define each_mem(pg,i) int i=0; for(mempage* pg=_gcdata.pages;i<_gcdata.total_pages;i++,pg=_gcdata.pages+i)
#define gc_start() void* __sp=NULL;init_gc(&__sp)
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#ifdef __MINGW32__
#define is_i(x) ((int)(x)>>30 & 1)
#else
#define is_i(x) ((long long)(x)>>62 & 1)
#define is_f(x) ((*(long long*)&(x))>>61 & 1)
#endif

typedef struct mempage {
	int no;
	int idx;
	int block_size;
	int blocks;
	int free;
	char* types;
//	struct mempage* next;
	char* page;
	map chains;
	int abandoned;
//	char type;
} mempage;
struct gcdata {
	int total_pages;
	void** stack_head;
	int page_no;
	long long max_used;
	long long curr_used;
	long long max_mem;
	long long curr_mem;
	mempage* pages;
	int gcruns;
	int gcwaste;
};
extern struct gcdata _gcdata;
extern struct timeval _run_time;
extern struct timeval _time;
extern map* _globals;
extern int _gc;
extern int _nogc;
extern int _inalloc;
extern size_t _clockstart;

extern int _printed;
extern int _total_time;
extern int _gc_time;
extern int _gc_max;
extern int _is_web;

extern char* skip;

#define End (char*)(0xFF12B145)

int rand();
void* px(void* str, int newline);
void xexit(int val);
void* fox_error(char* msg, int dump);
map* args(int argc, char** argv);
int mem_total();
int mem_free();
void fox_signal_handler(int sig);
void stack_dump_direct();
char* stack_str();
void fox_stack_dump();
char* sub_str(char* src, int from, int len);
char* print(char* str, FILE* fp);
char* str_times(char* str, int times);
char* xstr(char* ret, ...);
char* xcat(char* ret, ...);
char* cat(char* str1, char* str2);
char* vec_json(map* mp, int indent);
char* json(map* mp, int indent);
char* to_str(void* v, char* null, int human);
size_t blob_size(char* in);
char* new_blob(int size);
char* str_dup_len(char* str, int len);
char* str_dup(char* str);
char* str_quote(char* head);
char* int_str(long long value);
int str_eq(char* str, char* str1);
size_t str_hash(unsigned char * str);
map* xvec(void* v1, ...);
map* xmap(char* k1, ...);
map* new_map();
map* new_vec();
int key_eq(map* mp, int idx, char* id);
int key_hash(char* id);
int map_has_key(map* mp, char* id);
map* vec_realloc(map* mp, int extra);
map* vec_add(map* mp, void* v);
map* set(map* mp, int idx, void* val);
int is_int(void* v);
double is_double(void* v);
void* int_var(int i);
void* double_var(double f);
int has_id(map* mp, int idx);
void* map_id(map* mp, int idx);
int map_len(map* mp);
char* map_key(map* mp, int idx);
char* is_str(void* v);
char* is_blob(void* v);
map* is_vec(void* v);
map* is_hash(void* v);
map* is_map(void* v);
int to_int(void* v);
int next(map* mp, int idx, char** key, void** val);
int stoi(char* str);
map* globals();
int mem_used(int block_size, int type);
mempage* ptr_page(void* ptr);
void* block_ptr(int block, mempage* pg);
int ptr_block(void* ptr, mempage* pg);
int ptr_type(void* ptr);
int cell2_mark(cons* pairs, int size);
int cell_mark(void** pairs, int size);
int gc_mark(void* ptr);
int sweep_page(mempage* pg);
void* data_delete(void* data, int idx, int size, int len);
void* data_insert(void* data, int idx, int size, int len);
void* data_shift(void* data, int idx, int shift, int size, int len);
int cmp_page(const void* pg1, const void* pg2);
void reindex_pages();
mempage* no_page(int no);
struct mempage* new_page(int block_size, int blocks);
int comp_iptr(const void* sp1, const void* sp2);
mempage* index_free_space(mempage* pg);
void* chain_alloc(mempage* pg, int size, int type, char* ptr);
void* page_alloc(mempage* pg, int size, int type, int* full);
int gc_sweep();
map* root_ptrs();
int fox_gc();
int gc_runs();
int gc_time();
int gc_end();
mempage* free_page(mempage* pg);
int str_len(char* str);
int block_len(int block, mempage* pg);
int mem_size(void* ptr);
void* expand_inplace(char* ptr, char type, int size, int extra);
int size_blocks(size_t size, mempage* pg);
void* fox_realloc(void* ptr, size_t size, int type);
void* fox_alloc(size_t size, int type);
void* new_alloc(size_t size, int type);
void* _xalloc(size_t size, int type);
int block_size(int size);
void start_time();
void end_time();
int run_time();
int total_time();
void time_max();
struct timeval microtime();
int elapsed(struct timeval from);
int map_size(map* mp);
int ceil_pow2(int i);
map* add(map* mp, void* k, void* v);
map* vec_set(map* mp, int idx, void* v);
map* map_add(map* mp, char* key, void* v);
int map_nextno(map* mp);
map* map_index(map* mp, int idx);
map* map_reindex(map* mp);
char* null_str(char* in);
map* ptrs_vec(void** ptrs, int len);
int init_gc(void** sp);
int exec(char* cmd, char** output);
size_t clock_cycles();
void* ptr_head(void* ptr);
int block_head(int no, mempage* pg);
void init_rand();
void* map_val(map* mp, char* key);