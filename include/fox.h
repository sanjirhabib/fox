/* This is a generated file. To change it, edit function foxh() in fox.c */
#pragma once
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
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
#include <regex.h>
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
#include <sqlite3.h>
//	#include <openssl/md5.h>

enum Types {
	Null,Int,Double,String,Blob,Map,Vector,Index,Keys,Cell,Cell2,Tail
};
typedef struct Mapcell {
	short nextid;
	int hkey;
	char* id;
	void* val;
} Mapcell;
typedef struct map {
	int len;
	char type;
	union {
		struct Mapcell* pairs;
		void** vars;
	};
} map;

#define each_mem(pg,i) int i=0; for(mempage* pg=_gcdata.pages;i<_gcdata.total_pages;i++,pg=_gcdata.pages+i)
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#ifdef __MINGW32__
#define is_i(x) ((int)(x)>>30 & 1)
#else
#define is_i(x) ((long long)(x)>>61 & 2)
#define is_f(x) ((*(long long*)&(x))>>61 & 1)
#define is_num(x) ((*(long long*)&(x))>>61 & 3)
#endif

typedef struct mempage {
	int no;
	int idx;
	int block_size;
	int blocks;
	int free;
	char* types;
	char* page;
	map chains;
	int abandoned;
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
	int inalloc;
	int gctime;
	int gcmax;
	int max_roots;
	struct timeval run_time;
	struct timeval time;
	size_t clockstart;
	int total_time;
};
extern struct gcdata _gcdata;
extern map* _globals;

extern int _printed;
extern int _is_web;

extern char* skip;

#define None 0x0F9AD3BA
#define End (char*)(0x0FF1B14E059AD3BA)

void* invoke(map* v,char* name);
char* strstr(const char* str1,const char* str2);
int chdir(const char* path);
int max_mem();
int curr_mem();
map* reflect();
char* version();
void hijri_month_cache_bd(); //astrostr.fox
void hijri_month_cache(); //astrostr.fox
double dhaka_lng(); //astrostr.fox
double dhaka_lat(); //astrostr.fox
double kaba_lng(); //astrostr.fox
double kaba_lat(); //astrostr.fox
double qibla_direction(double lng, double lat); //astrostr.fox
double kaba_sun_set(double jd); //astrostr.fox
double kaba_moon_set(double jd); //astrostr.fox
double hijri_month_length(int lun); //astrostr.fox
double hijri_month_length_bd(int lun); //astrostr.fox
double hijri_month_start3(int lun); //astrostr.fox
double hijri_month_start_bd3(int lun); //astrostr.fox
double hijri_month_start_bd(int lun); //astrostr.fox
double hijri_month_start(int lun); //astrostr.fox
double hijri_month_start_bd2(int lun); //astrostr.fox
double kaba_tz(); //astrostr.fox
double hijri_month_start75(int lun); //astrostr.fox
double hijri_month_start2(int lun); //astrostr.fox
double hijri_lunation_bd(double jd); //astrostr.fox
char* hijri_date_bd(double jd); //astrostr.fox
double hijri_lunation(double jd); //astrostr.fox
double current_hijri_month_bd(); //astrostr.fox
double current_hijri_month(); //astrostr.fox
double current_hijri_year(); //astrostr.fox
double jd_now(); //astrostr.fox
char* hijri_now(double tz); //astrostr.fox
char* hijri_now_bd(double tz); //astrostr.fox
double hijri_day_bd(double jd); //astrostr.fox
double hijri_day(double jd); //astrostr.fox
double hijri2jd(int y, int m, int d); //astrostr.fox
char* hijri_date(double jd, double tz); //astrostr.fox
double jd(char* date, double tz); //astrostr.fox
char* jd_str(double jd, double tz); //astrostr.fox
double frac(double val); //astrostr.fox
char* jdstring_ar(double jd, double tz); //astrostr.fox
char* jdstring_bg(double jd, double tz); //astrostr.fox
char* date_only(char* in); //astrostr.fox
char* jd_human(double jd, double tz); //astrostr.fox
char* wday_en(int val); //astrostr.fox
char* wday_en3(int val); //astrostr.fox
char* month_bg3(int val); //astrostr.fox
char* month_bg(int val); //astrostr.fox
char* ar_wday(int wday); //astrostr.fox
char* wday_bg(int wday); //astrostr.fox
char* month_ar(int month); //astrostr.fox
char* month_en(int val); //astrostr.fox
char* month_en3(int val); //astrostr.fox
char* astro_nth(int n); //astrostr.fox
int cemonth(int ym); //astrostr.fox
int ceyear(int ym); //astrostr.fox
char* hijri_month_name_bg(int v); //astrostr.fox
char* hijri_month_name_ar(int v); //astrostr.fox
char* hijri_month_name(int v); //astrostr.fox
int jd_wday(double jd, double tz); //astrostr.fox
double tz(); //astrostr.fox
void rgb2hsl(int r, int g, int b, double* out_h, double* out_s, double* out_l); //astrostr.fox
double jday(double jd); //astrostr.fox
char* bearing_name3(double lng1, double lat1, double lng2, double lat2); //astrostr.fox
char* bearing_name(double lng1, double lat1, double lng2, double lat2); //astrostr.fox
static double hue_2_rgb(double v1, double v2, double v_h); //astrostr.fox
void hsl2rgb(double h, double s, double l, int* ret_r, int* ret_g, int* ret_b); //astrostr.fox
map* salat_time(double jd, double lng, double lat); //astrostr.fox
double esha(double jd, double lng, double lat); //astrostr.fox
double fajar18(double jd, double lng, double lat); //astrostr.fox
double noon(double jd, double lng, double lat); //astrostr.fox
double asar2(double jd, double lng, double lat); //astrostr.fox
double asar1(double jd, double lng, double lat); //astrostr.fox
void* px(void* str, int newline); //core.fox
void xexit(int val); //core.fox
void* fox_error(char* msg, int dump); //core.fox
void* call_php(map* params, char* func); //cmd.fox
int rand(); //core.fox
int mem_total(); //core.fox
int mem_free(); //core.fox
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
int mem_used(int block_size, int type); //core.fox
mempage* ptr_page(void* ptr); //core.fox
static void* block_ptr(int block, mempage* pg); //core.fox
int ptr_block(void* ptr, mempage* pg); //core.fox
int ptr_type(void* ptr); //core.fox
static int cell2_mark(Mapcell* pairs, int size); //core.fox
static int cell_mark(void** pairs, int size); //core.fox
static int gc_mark(void* ptr); //core.fox
static int sweep_page(mempage* pg); //core.fox
static void* data_delete(void* data, int idx, int size, int len); //core.fox
static void* data_insert(void* data, int idx, int size, int len); //core.fox
static void* data_shift(void* data, int idx, int shift, int size, int len); //core.fox
static int cmp_page(const void* pg1, const void* pg2); //core.fox
static void reindex_pages(); //core.fox
static mempage* no_page(int no); //core.fox
static struct mempage* new_page(int block_size, int blocks); //core.fox
static int comp_iptr(const void* sp1, const void* sp2); //core.fox
static mempage* index_free_space(mempage* pg); //core.fox
static void* chain_alloc(mempage* pg, int size, int type, char* ptr); //core.fox
static int copy_page(mempage* from, mempage* to); //core.fox
static void* page_alloc(mempage* pg, int size, int type, int* full); //core.fox
static int gc_sweep(); //core.fox
map* root_ptrs(); //core.fox
static int fox_gc(); //core.fox
int gc_runs(); //core.fox
int gc_time(); //core.fox
int gc_end(); //core.fox
static mempage* free_page(mempage* pg); //core.fox
int block_len(int block, mempage* pg); //core.fox
int mem_size(void* ptr); //core.fox
void* expand_inplace(char* ptr, char type, int size, int extra); //core.fox
static int size_blocks(size_t size, mempage* pg); //core.fox
void* fox_realloc(void* ptr, size_t size, int type); //core.fox
void* fox_alloc(size_t size, int type); //core.fox
static void* new_alloc(size_t size, int type); //core.fox
static void* _xalloc(size_t size, int type); //core.fox
static int block_size(int size); //core.fox
void start_time(); //core.fox
void end_time(); //core.fox
int run_time(); //core.fox
int total_time(); //core.fox
static void time_max(); //core.fox
struct timeval microtime(); //core.fox
static int elapsed(struct timeval from); //core.fox
int map_size(map* mp); //core.fox
static int ceil_pow2(int i); //core.fox
map* add(map* mp, void* k, void* v); //core.fox
map* vec_set(map* mp, int idx, void* v); //core.fox
map* map_add(map* mp, char* key, void* v); //core.fox
static int map_nextno(map* mp); //core.fox
map* map_index(map* mp, int idx); //core.fox
map* map_reindex(map* mp); //core.fox
char* null_str(char* in); //core.fox
map* ptrs_vec(void** ptrs, int len); //core.fox
int exec(char* cmd, char** output); //core.fox
static size_t clock_cycles(); //core.fox
static void* ptr_head(void* ptr); //core.fox
int block_head(int no, mempage* pg); //core.fox
void init_rand(); //core.fox
void* map_val(map* mp, char* key); //core.fox
map* env_vars(); //core.fox
map* init_gc(void** sptr); //core.fox
map* argv_map(char** argv, int argc, void** globals); //core.fox
map* str_split(char* str, char* by, int limit); //core.fox
char* str_has(char* str, char* sub); //core.fox
char* exec_str(char* in); //core.fox
size_t fox_curl_cat(void* ptr, size_t size, size_t num, void* old); //curl.fox
char* url_fetch(char* url); //curl.fox
char* db_write_meta(char* db); //dbmeta.fox
char* db_meta_str(char* db); //dbmeta.fox
char* data1_str(void* in); //dbmeta.fox
char* data_str(map* in, int level); //dbmeta.fox
int is_file(char* filename); //dir.fox
int file_size(char* filename); //dir.fox
char* file_dir(char* file); //dir.fox
char* file_rename(char* file, char* dir, char* delext, char* addext, char* prefix, char* postfix); //dir.fox
char* sane_dir(char* dir); //dir.fox
char* write_file(char* data, char* filename, int readonly, int verbose); //dir.fox
char* file_read(char* filename, int istext, int error_on_fail); //dir.fox
char* read_stdin(int size, char* terminator); //dir.fox
map* dir_files(char* path); //dir.fox
size_t file_time(char* file); //dir.fox
char* file_path(char* file); //dir.fox
char* file_nodir(char* filename); //dir.fox
char* file_noext(char* filename); //dir.fox
char* file_ext(char* filename, char* def); //dir.fox
map* dir_glob(char* name); //dir.fox
void cache_blast(char* type); //fox.fox
void* cache(void* key, char* type, void* out); //fox.fox
void gotoxy(int x, int y); //fox.fox
int floor_pow2(int i); //fox.fox
int utest(char* found, char* expect, char* in, char* title); //fox.fox
char* xlog(char* str); //fox.fox
char* hello(); //fox.fox
map* command_line(char* in, int argc, char** argv); //fox.fox
map* param_test(char* one, char* two); //fox.fox
int test_add(int a, int b); //fox.fox
map* test_map(map* out); //fox.fox
map* load_global(char* file); //fox.fox
int err_msg(char* msg, char** ptr); //fox.fox
char* tutorial(); //fox.fox
char* fork_exec(char* cmd, map* params); //fox.fox
int fib(int i); //fox.fox
int msleep(int msecs); //fox.fox
int str_ascii(char* in); //fox.fox
char* ping(char* str); //fox.fox
char* c_md5(char* in, size_t initial_len, char* out); //foxmd5.fox
char* md5(char* in); //foxmd5.fox
int cgi(char* infile, char* xfiles, char* profile, char* outfile, char* opts, int keepfiles); //generator.fox
static char* fox_meta(char* infile, char* name, char* outfile); //generator.fox
static char* fox_cs(char* name, map* depends); //generator.fox
int cc(char* infile, char* xfiles, char* profile, char* outfile, char* opts, int keepfiles); //generator.fox
static char* write_configm4(char* name, char* outfile); //generator.fox
void write_source(char* infile, ...); //generator.fox
static char* write_foxh(char* outfile); //generator.fox
static map* funcs_nonstatic(map* funcs); //generator.fox
static char* fox_phpc(char* infile, char* outfile); //generator.fox
static char* write_phpconfig(); //generator.fox
static char* meta_h(char* prefix); //generator.fox
static char* funcs_meta(map* funcs, map* macros, map* structs, char* prefix); //generator.fox
static char* gen_htaccess(char* outfile); //generator.fox
static char* gen_cgi_makefile(char* name, char* outfile); //generator.fox
static char* gen_fox_cgi(char* name, char* outfile); //generator.fox
int init_cgi(char* name); //generator.fox
char* c_h(char* infile, char* outfile); //generator.fox
char* fox_h(char* infile, char* outfile); //generator.fox
static map* x_map(char* in); //generator.fox
char* c_x(char* in); //generator.fox
static char* map_ccode(void* mp); //generator.fox
static char* file_foxh(char* infile, char* outfile); //generator.fox
static char* func_cdecl(map* fn, int show_default); //generator.fox
static char* funcs_cdecl(map* fns, int show_default); //generator.fox
static char* foxh(); //generator.fox
static void* marked_str(char* str, char* name); //generator.fox
map* map_add_pair(map* mp, void* name, void* value, int type); //generator.fox
static map* add_name_val(map* mp, char* str, char** name, char** val, int type); //generator.fox
static char* read_as_block(char** from, int level, char* terminator); //generator.fox
map* block_map(char** from, int inlevel, int maptype); //generator.fox
static char* old_decl_type(map* mp, int idx); //generator.fox
static char* toks_str(map* mp); //generator.fox
static map* toks_keywords(map* mp, char* keywords); //generator.fox
char* read_num(char** in); //generator.fox
char* skip_word(char* in, char* seperators); //generator.fox
void* read_value(char** str, char* sep); //generator.fox
char* read_upto(char** in, char* terminators); //generator.fox
char* read_alpha(char** in); //generator.fox
char* fox_read_symbol(char** in); //generator.fox
char* fox_read_oper(char** in, char term); //generator.fox
char* read_upto_word(char** in, char* upto); //generator.fox
char* read_theline(char** in); //generator.fox
char* read_newline(char** in); //generator.fox
char* read_space(char** in, char* spaces); //generator.fox
static map* read_data(char** in); //generator.fox
static char* read_heredoc(char** in); //generator.fox
static char* read_multistr(char** in); //generator.fox
char* read_quote(char** in); //generator.fox
int map_has_word(map* mp, char* str); //generator.fox
static int is_keyword(char* str); //generator.fox
static char* toks_c(map* mp); //generator.fox
static map* add_ctok(void* data, map* mp, int iscode); //generator.fox
static map* c_tokenizer(char** line, char term); //generator.fox
static int func_dot(map* mp, int idx); //generator.fox
static map* multiline_parts(char* str); //generator.fox
static map* heredoc_parts(char* str); //generator.fox
static map* heredoc_str(map* mp); //generator.fox
static map* dot_each(map* mp); //generator.fox
static int tok_indent(char* str); //generator.fox
static int requires_semicolon(map* mp, int idx); //generator.fox
static map* add_semicolon(map* mp, int recurse); //generator.fox
static int tok_endl(map* mp, int from); //generator.fox
static map* force_curly(map* mp); //generator.fox
static map* expand_main(map* mp); //generator.fox
static map* add_curly(map* mp, int recursive); //generator.fox
static map* dot_key(map* mp); //generator.fox
static int str_hasvar(char* in); //generator.fox
static char* str_mstr(char* in); //generator.fox
static map* xstr_parts(char* in); //generator.fox
static char* str_xstr(char* in); //generator.fox
static map* str_dollars(map* mp); //generator.fox
static map* single_quotes(map* mp); //generator.fox
static map* colon_str(map* mp); //generator.fox
static map* dot_func(map* mp); //generator.fox
static map* map_tox(map* mp); //generator.fox
static int stm_start(map* mp, int idx, char* terms); //generator.fox
static map* syn_func(map* syn, int with_body, char* filename); //generator.fox
static map* syn_funcs(map* mp, int with_body, char* filename); //generator.fox
static map* read_struct(map* toks); //generator.fox
map* c_structs(char* in); //generator.fox
static map* x_funcs(char* in, int with_body, char* filename); //generator.fox
static map* func_params(map* func); //generator.fox
static map* func_param(map* func); //generator.fox
static map* func_add_param(map* func, map* add); //generator.fox
static map* func_rename(map* func, char* name); //generator.fox
static char* func_name(map* func); //generator.fox
static map* map_assign(map* exp1, map* exp2); //generator.fox
static map* call_count(map* toks, map* counter, char* infunc); //generator.fox
static map* file_deadcode(char* file); //generator.fox
static char* is_typecast(map* toks, int idx); //generator.fox
static char* is_name(char* in); //generator.fox
static int expr_tail(map* toks, int idx, char* expr); //generator.fox
static int expr_head(map* toks, int idx, char* expr); //generator.fox
static map* add_return(map* toks); //generator.fox
static map* wrap_call(map* tok, char* func); //generator.fox
static map* syn_expand_macro(map* syn, int idx); //generator.fox
static map* syn_is_macro(map* syn, int idx); //generator.fox
static map* syn_set_param(map* syn, int idx, map* params); //generator.fox
static map* syn_func_param(map* syn, int idx); //generator.fox
static map* syn_func_body(map* syn); //generator.fox
static map* syn_remove_default(map* syn); //generator.fox
static char* assign_to_func(map* tok); //generator.fox
static int next_tok(map* toks, int from, char* tok, int upto); //generator.fox
static int syn_has(map* syn, char* words); //generator.fox
static char* syn_flow_name(map* syn); //generator.fox
static int is_flow(map* syn); //generator.fox
static map* syn_flow_condition(map* syn, int no); //generator.fox
static map* syn_flow_body(map* syn, int no); //generator.fox
static char* syn_is_call(map* syn, int idx); //generator.fox
static char* syn_var_name(map* syn); //generator.fox
static char* syn_var_type(map* syn); //generator.fox
static int syn_assign_val(map* syn); //generator.fox
static int is_assign(map* syn); //generator.fox
static int is_var_decl(map* syn); //generator.fox
static int is_func_decl(map* syn); //generator.fox
static char* to_c(void* val); //generator.fox
static char* str_shorten(char* in, int max); //generator.fox
static char* call_c(map* params, char* name); //generator.fox
char* read_line(FILE* fp); //generator.fox
static char* type_name(int type); //generator.fox
static char* ptr_name(void* var); //generator.fox
static map* source_funcs(map* infiles); //generator.fox
static map* file_funcs(char* filename, int withbody); //generator.fox
static void src(map* mp, int from, int len, char* msg); //generator.fox
static map* structs(); //generator.fox
static map* macros(); //generator.fox
static map* source_structs(); //generator.fox
static map* source_macros(); //generator.fox
static map* toks_syn(map* toks, int recurse); //generator.fox
static char* increase_version(); //generator.fox
int call_variadic_int(map* mp, void* fp, char* name); //generator.fox
void* call_variadic_ptr(map* mp, void* fp, char* name); //generator.fox
static map* toks_replace(map* in, map* replace); //generator.fox
static map* toks_join(map* in, char* by); //generator.fox
static map* toks_align(map* in); //generator.fox
map* toks_split(map* in, char* by, int limit); //generator.fox
static map* expand_macros(map* mp, map* macros); //generator.fox
static map* c_macros(char* in); //generator.fox
static map* toks_macros(map* mp); //generator.fox
static int is_inline_vector(map* toks, int idx); //generator.fox
static map* string_operators(map* toks); //generator.fox
static map* inline_vectors(map* toks); //generator.fox
static map* data_toks(char* in); //generator.fox
static map* data_tokenizer(char** in, int level); //generator.fox
static char* read_over(char** in, char* chars); //generator.fox
static map* data_quote(char* in); //generator.fox
static map* prop_vec_toks(char* in); //generator.fox
static map* prop_toks(char* in, map* name); //generator.fox
map* read_paren(map* mp, char** line, map*(*func)(char** )); //generator.fox
static map* file_map(char* filename); //generator.fox
map* data_map(char* in); //generator.fox
static map* data_map2(char** in, int level); //generator.fox
static char* skip_code(char* in, char* terminator); //generator.fox
static char* skip_upto(char* in, char* chars); //generator.fox
static void* data_unquote(char* in); //generator.fox
static map* prop_vec(char* in); //generator.fox
static map* prop_map(char* in, char* name); //generator.fox
char* callfunc_c(map* funcs); //generator.fox
char* write_c(char* infile, char* outfile); //generator.fox
static char* func_ccall(map* fn); //generator.fox
char* fox_c(char* infile, char* outfile); //generator.fox
char* x_c(char* in); //generator.fox
static map* type_convert(map* tok, char* outtype, map* env, map* fs, map* fn); //generator.fox
static map* param_c(map* params, map* env, map* fs, map* fn); //generator.fox
static char* head_type(map* toks, int idx, int upto, map* env, map* fs); //generator.fox
char* help(); //generator.fox
char* expr_type(map* toks, int idx, int upto, map* env, map* fs); //generator.fox
map* x_toks(char* in, int is_script); //generator.fox
static map* auto_types(map* toks, char* context, int is_script, map* env, map* fns, map* func, int idx); //generator.fox
map* file_callmap(char* filename); //generator.fox
static map* func_depend(map* mp, map* ret); //generator.fox
map* file_depends(char* filename, ...); //generator.fox
static map* func_depends(map* mp, char* filename); //generator.fox
int utests(char* test, char* file); //generator.fox
map* cmdline_params(map* args, map* funcs); //generator.fox
map* funcs(); //generator.fox
void* fox_run(char* in); //generator.fox
static void* fox_eval(map* mp, map* env, int* halt); //generator.fox
static void* eval_toks(map* mp, map* env); //generator.fox
static void* eval_expr(map* mp, int* idx, map* env, int level); //generator.fox
static void* binary_op(void* left, char oper, void* right); //generator.fox
static int is_true(void * val); //generator.fox
void* eval(char* in, map* env); //generator.fox
static int eval_expr_cont(map* mp, int idx, map* env, void** last, int level); //generator.fox
static map* eval_params(map* sent, char* name, map* env, map* fns); //generator.fox
void* data_exec(void* data, map* env); //generator.fox
void* call_func(map* params, char* name, map* env); //generator.fox
static map* ping_map(map* in); //generator.fox
map* func_depend_recursive(char* func, char* file, map* funcs, map* files); //generator.fox
char* mem_usage(); //generator.fox
char* int_kb(size_t i, char* unit); //generator.fox
char* int_human(int i, char* unit, char* zero); //generator.fox
void rewrite_ptr(mempage* pg, void** ptr); //generator.fox
void rewrite_ptrs(mempage* old); //generator.fox
int page_map(mempage* pg); //generator.fox
int dump_chain(mempage* pg, int line); //generator.fox
int check_chains(mempage* pg, int line); //generator.fox
int page_maps(char* title); //generator.fox
char* ptr_id(void* ptr); //generator.fox
void benchmark_gc(); //generator.fox
char* fkey_sql(char* sql, char* db); //html.fox
char* callback_php(map* param, char* name); //html.fox
char* fkey_show(char* sql, char* db, char* id); //html.fox
int fkey_valid(char* sql, char* db, char* id); //html.fox
char* str_show(char* value, char* type, map* op, int width); //html.fox
char* cols_show(map* cols, map* row, char* name, int width); //html.fox
char* datetime_human(char* in); //html.fox
char* thumb_name(char* name); //html.fox
map* row_show(map* row, map* cols, int width); //html.fox
map* rows_show(map* rows, map* cols, int width); //html.fox
char* month_name(int month); //html.fox
char* http_moved(char* url); //html.fox
char* http_redirect(char* url, char* msg); //html.fox
void* http_error(char* msg, char* status); //html.fox
char* file_mime(char* path); //html.fox
char* str_html(char* in); //html.fox
void header(char* str); //html.fox
char* http_out(char* str, char* status, char* mime, map* headers); //html.fox
char* static_file(char* path); //html.fox
char* str_url(char* in); //html.fox
char* url_str(char* in); //html.fox
char* map_amps(void* val, char* name); //html.fox
map* amps_map(char* in); //html.fox
map* parse_url(char* path); //html.fox
char* url_host(char* url); //html.fox
map* sess_init(); //html.fox
void sess_add(char* name, char* value); //html.fox
char* sess_id(); //html.fox
char* sess_file(); //html.fox
char* sess_newid(); //html.fox
void cookie_set(char* name, char* value, char* path, char* expire); //html.fox
void sess_delete(); //html.fox
map* link_relative(map* links, char* url); //html.fox
map* header_map(char* val); //html.fox
map* http_req(); //html.fox
char* full_url(char* url); //html.fox
char* url_abs(char* abs, char* rel); //html.fox
char* base_url(char* path); //html.fox
char* home_url(char* path); //html.fox
char* show_port(); //html.fox
char* h(char* in); //html.fox
char* page_html(map* data); //html.fox
map* page_data(map* data); //html.fox
void* not_found(char* path); //html.fox
int show_page(map* data); //html.fox
int is_post(); //html.fox
map* post_data_multipart(); //html.fox
char* name_tab(char* name); //html.fox
map* get(char* path, map* names); //html.fox
void show_body(char* body, map* page); //html.fox
int days_in_feb(int year); //html.fox
int days_in_month(int month, int year); //html.fox
char* add_tab(char* path, char* name); //html.fox
char* tz_dst(char* tz, char* date); //html.fox
double tz_offset(char* tz, char* date); //html.fox
char* tz_utc(char* tz, char* date); //html.fox
char* offset_utc(double off); //html.fox
char* date_ymd(char* in, int* year, int* month, int* day); //html.fox
char* date_human(char* in); //html.fox
char* tz_human(double tz); //html.fox
map* tz_data(char* name, char* date); //html.fox
char* fts5_create(char* db); //html.fox
map* merge_soft(map* strong, map* soft); //html.fox
char* param(map* change); //html.fox
void save_caller(); //html.fox
char* url_back(); //html.fox
char* form_html(map* mp); //html.fox
map* param_map(char* in); //html.fox
map* post_data(); //html.fox
map* form_posted(map* form); //html.fox
char* ctrl_html(map* ctrl); //html.fox
char* name_id(char* name); //html.fox
map* form_gets(map* form); //html.fox
int show_form(map* data); //html.fox
int is_user(char* who); //html.fox
void authorized(char* who); //html.fox
char* col_align(map* col); //html.fox
char* rows_data_html(map* in); //html.fox
map* rows_data(map* rows, map* cols); //html.fox
char* row_id(map* row, map* cols); //html.fox
map* row_ids(map* row, map* cols); //html.fox
void crud(char* sql, char* db); //html.fox
map* del_keys(map* mp, map* keys); //html.fox
char* str_bare(char* in, char* accept); //html.fox
char* word_end(char* in, int len); //html.fox
void go_back(char* msg); //html.fox
int confirm(char* msg); //html.fox
void retree(char* db); //html.fox
char* html_text(char* in); //html.fox
char* url_filename(char* url); //html.fox
char* wand_img(void* mw); //image.fox
void* img_wand(char* img); //image.fox
char* img_resize(char* img, double enlarge, int width, int height); //image.fox
int run(map* args); //run.fox
map* xadd(map* mp, ...); //map.fox
map* to_map(void* val); //map.fox
map* del_index(map* mp, int idx); //map.fox
map* map_del_key(map* mp, char* key); //map.fox
map* map_del(map* mp, int idx, int len); //map.fox
map* xjson_map(char* in, int type); //map.fox
map* xjson_map_recurse(char** line, int type); //map.fox
int str_level(char* line); //map.fox
int block_level(char* str); //map.fox
map* xkeys(char* str); //map.fox
map* xindex(char* str); //map.fox
map* xarray(char* str); //map.fox
map* str_vec(void* str); //map.fox
map* str_map(char* str, int type); //map.fox
map* vec_del(map* mp, int from, int len); //map.fox
map* vec_shift(map* mp, int idx, int len, int offset); //map.fox
map* vec_sub(map* mp, int from, int len); //map.fox
map* vec_slice(map* mp, int from, int len); //map.fox
map* map_splice(map* mp, int from, int del, map* ad); //map.fox
void* vec_pop(map* mp); //map.fox
map* vec_splice(map* mp, int from, int del, map* add); //map.fox
map* vec_dup(map* mp); //map.fox
map* map_dup(map* mp); //map.fox
map* map_merge(map* mp1, map* mp2); //map.fox
map* vec_merge(map* mp1, map* mp2); //map.fox
map* map_compact(map* mp); //map.fox
map* vec_reverse(map* mp); //map.fox
map* vec_compact(map* mp); //map.fox
map* change_key(map* mp, int idx, char* id); //map.fox
map* add_id(map* mp, int idx); //map.fox
map* add_key(map* mp, char* k, int type); //map.fox
char* map_join(map* mp, char* joiner); //map.fox
char* map_str(map* mp); //map.fox
char* map_str_indent(map* mp, int indent); //map.fox
map* map_vec(map* mp); //map.fox
map* map_split(map* mp, char* str, int limit); //map.fox
map* map_implode(map* mp, char* joiner); //map.fox
map* file_vec(char* in); //map.fox
map* map_keys(map* mp); //map.fox
map* vec_rdup(map* mp); //map.fox
map* set_map(void* val, map* mp, int idx); //map.fox
char* file_markdown(char* infile, char* outfile); //markdown.fox
char* fox_markdown(char* in); //markdown.fox
int is_numeric(char* str); //number.fox
char* outx(char* str); //print.fox
char* flush_out(); //print.fox
int ret_print(void* ret); //print.fox
map* regexp(char* in, char* pattern); //regexp.fox
int ip_connect(char* host, int port, char** err); //socket.fox
int ip_close(int sock); //socket.fox
int ip_send(char* msg, int sock); //socket.fox
char* ip_receive(int sock, char* terminator, int size); //socket.fox
char* remote_ip(int con); //socket.fox
map* sql_toks(char* line); //sql.fox
map* split_keywords(map* mp, char* words); //sql.fox
void* key_var(char* key); //sql.fox
map* map_key_val(map* mp, int idx); //sql.fox
char* join_clause(char* pre, map* mp, char* clause, char* by, char* sub1, char* sub2, char* use_cls); //sql.fox
map* de_select(map* cls); //sql.fox
map* de_from(map* cls); //sql.fox
map* parse_where(map* cls); //sql.fox
map* de_where(map* cls); //sql.fox
map* de_having(map* cls); //sql.fox
map* de_order(map* cls); //sql.fox
map* de_group(map* cls); //sql.fox
map* de_limit(map* cls); //sql.fox
map* sql_toks_map(map* toks); //sql.fox
map* sql_map(char* sql); //sql.fox
char* sql_map_join(map* mp, char* joiner); //sql.fox
char* var_join(void* v, char* joiner); //sql.fox
map* sql_convert_func(map* mp, char* db); //sql.fox
char* is_func(map* mp, int idx); //sql.fox
map* sql_func_params(map* mp, int idx); //sql.fox
map* get_func(map* mp, int idx); //sql.fox
map* search_sql_func(map* mp, char* db); //sql.fox
map* to_vec(void* val); //sql.fox
map* sql_order(char* sql); //sql.fox
map* sql_tables(char* sql, char* db); //sql.fox
map* sql_col(char* sql, char* db, map* exp); //sql.fox
char* str_code(char* in); //sql.fox
map* crosstab_cols(map* cols, char* sql, char* db, map* params); //sql.fox
map* sql_select_cols(char* sql, char* db, map* params); //sql.fox
char* sql_add_where(char* sql, map* mp); //sql.fox
map* cols_collect(map* cols, void* collect); //sql.fox
map* sql_where_cols(char* sql, char* db); //sql.fox
map* sql_where_vals(char* sql); //sql.fox
char* sql_add_limit(char* sql, int rpp, int page); //sql.fox
char* sql_add_order(char* sql, char* db, map* order); //sql.fox
char* sql_add_filter(char* sql, map* filter); //sql.fox
char* name_type(char* name); //sql.fox
map* to_cols(void* cols, char* sql, char* db); //sql.fox
map* sql_cols(char* sql, char* db, void* cols); //sql.fox
map* sql_add_cols(char* sql, char* db, map* cols); //sql.fox
map* sql_view_cols(char* sql, char* db, map* cols); //sql.fox
map* sql_edit_cols(char* sql, char* db, map* cols); //sql.fox
map* sql_list_cols(char* sql, char* db, map* cols); //sql.fox
char* sql_add_cls(char* sql, char* cls, map* vals); //sql.fox
map* sql_cls(char* sql, char* cls); //sql.fox
char* sql_str(void* data); //sql.fox
char* re_select(map* mp); //sql.fox
char* re_from(map* cls); //sql.fox
char* re_where(map* cls); //sql.fox
char* re_group(map* cls); //sql.fox
char* re_having(map* cls); //sql.fox
char* re_order(map* cls); //sql.fox
char* re_union(map* cls); //sql.fox
char* re_limit(map* cls); //sql.fox
char* map_sql(map* mp); //sql.fox
int has_aggregate(char* sql); //sql.fox
char* sql_auto_join(char* sql, char* db); //sql.fox
char* sql_auto_group(char* sql, char* db); //sql.fox
char* sql_lite(char* sql, char* db, map* params); //sql.fox
int sql_utest(char* in, char* out); //sql.fox
int type_size(char* type); //sql.fox
char* lite_create_col(map* col); //sql.fox
int is_indexed(char* type); //sql.fox
map* trigger_sqls(char* tbl, char* db); //sql.fox
map* tbl_trigger_sqls(map* tbl); //sql.fox
map* create_index_sqls(map* tbl); //sql.fox
char* drop_sql(char* name); //sql.fox
char* create_sql(map* tbl, char* name); //sql.fox
char* meta_type(char* type, int size); //sql.fox
int has_table(char* db, char* tbl); //sql.fox
map* map_cols(void* cols, char* tbl, char* db); //sql.fox
map* cols_table(map* cols, char* tbl, char* db); //sql.fox
map* pre_tables(); //sql.fox
map* db_table(char* db, char* tbl); //sql.fox
map* db_tables(char* db); //sql.fox
map* db_cols(char* db, char* tbl); //sql.fox
map* url_gets(); //sql.fox
char* sql_table(char* sql); //sql.fox
map* sql_pkeys(char* sql, char* db); //sql.fox
void* sql_value(char* sql, char* db, map* param); //sql.fox
map* sql_rows(char* sql, char* db, map* param); //sql.fox
map* sql_query(char* sql, char* db, map* where); //sql.fox
map* id_ids(void* id, char* tbl, char* db); //sql.fox
map* sql_row(char* sql, char* db, map* param); //sql.fox
map* sql_vector(char* sql, char* db, map* param); //sql.fox
map* sql_pairs(char* sql, char* db, map* param); //sql.fox
map* sql_vec(char* sql, char* db, map* param); //sql.fox
char* sql_drop(char* tbl); //sql.fox
char* to_sql(char* sql); //sql.fox
map* meta_fkeys(char* db); //sql.fox
map* tbl_referred_by(char* tbl, char* db); //sql.fox
void* id_update(void* ids, char* tbl, char* db, map* row); //sql.fox
int row_insert(map* row, char* tbl, char* db); //sql.fox
void* sql_error(char* sql, char* db, sqlite3* conn); //sql.fox
map* sqls_exec(map* sqls, char* db); //sql.fox
map* parse_connection(char* in); //sql.fox
map* conn_db(char* db); //sql.fox
map* sql_exec(char* sql, char* db, map* params); //sql.fox
void* lite_conn(char* db); //sql.fox
map* lite_exec(char* sql, char* db, map* params); //sql.fox
void close_conns(); //sql.fox
map* vec_map(map* in); //sql.fox
map* db_table_names(char* db); //sql.fox
map* cols_pkeys(map* cols); //sql.fox
map* types(); //sql.fox
char* map_type(map* mp, char* type); //sql.fox
map* where_param(map* where); //sql.fox
map* where_rows(map* where, char* tbl, char* db); //sql.fox
map* where_row(map* where, char* tbl, char* db); //sql.fox
map* id_row(void* ids, char* tbl, char* db); //sql.fox
map* id_param(void* ids, char* tbl, char* db); //sql.fox
map* tbl_pkeys(char* tbl, char* db); //sql.fox
map* tbl_skeys(char* tbl, char* db); //sql.fox
map* cols_skeys(map* cols); //sql.fox
map* toks_sql_params(map* toks, map* ret); //sql.fox
void* is_null(void* val); //sql.fox
map* sql_missing_params(char* sql, map* params); //sql.fox
map* fkey_list(char* sql, char* db, map* params); //sql.fox
map* sql_params(char* sql); //sql.fox
void* id_delete(void* ids, char* tbl, char* db); //sql.fox
char* sql_rename(char* from, char* into); //sql.fox
map* subtypes(char* type); //sql.fox
int type_distance(char* type1, char* type2); //sql.fox
map* cols_match(map* from, map* into); //sql.fox
int cmp_ptr_reverse(const void* ptr1, const void* ptr2); //sql.fox
int cmp_ptr(const void* ptr1, const void* ptr2); //sql.fox
int cmp_cons_reverse(const void* ptr1, const void* ptr2); //sql.fox
int cmp_cons(const void* ptr1, const void* ptr2); //sql.fox
map* map_sort(map* mp, int reverse); //sql.fox
char* nearest_table(map* tbls, map* tbl); //sql.fox
map* tbls_sync_sqls(map* new_tbls, map* old_tbls); //sql.fox
map* db_meta(char* db); //sql.fox
map* tbl_cols(char* table, char* db); //sql.fox
map* db_sync(char* db, int go); //sql.fox
map* sync_sqls(map* newt, map* oldt); //sql.fox
map* sql_sums(char* sql, char* db, map* cols, map* params); //sql.fox
int sql_count(char* sql, char* db, map* params); //sql.fox
char* read_textblock(map* lines, int* lineno, char* terminator, char** outline); //sql.fox
map* sql_tokenizer(char** line); //sql.fox
map* prop_tokenizer(char** line); //sql.fox
map* tokenizer(char** line, char* comment); //sql.fox
map* lite_trigger_slno(char* name, char* pkey, char* by); //sql.fox
map* lite_trigger_tree(char* name, char* pkey); //sql.fox
char* mstr(char* format, ...); //string.fox
char* cat_char(char* str, char c); //string.fox
char* va_str(char* format, va_list args); //string.fox
void* dx(void* data, char* name, int panic); //string.fox
int neq(char* str, char* str1); //string.fox
int eq(char* str, char* str1); //string.fox
char* cwd(); //string.fox
char* str_unquote(char* str); //string.fox
char* str_escape(char* head); //string.fox
char* str_end(char* str, char* end); //string.fox
int str_starti(char* str, char* start); //string.fox
int str_start(char* str, char* start); //string.fox
char* str_chr(char* str, char c); //string.fox
int str_chrs(char* str, char* chars); //string.fox
char* str_ltrim(char* str, char* chars); //string.fox
char* str_rtrim(char* str, char* chars); //string.fox
char* str_trim(char* str, char* chars); //string.fox
int line_isempty(char* line); //string.fox
int line_len(char* line); //string.fox
int lchars(char* str, char* chars); //string.fox
int rchars(char* str, char* chars, int off); //string.fox
int has_str(char* str, char* substr); //string.fox
char* str_replace(char* str, void* find, void* replace); //string.fox
char* str_str(char* in, char* sub); //string.fox
char* str_upper(char* s); //string.fox
char* str_lower(char* s); //string.fox
char fox_at(char* str, int idx); //string.fox
char* indent_str(int i); //string.fox
int str_chars(char* str, char* chars); //string.fox
char* end_word(char* str, char* words); //string.fox
char* has_word(char* str, char* words); //string.fox
int is_word(char* word, char* list); //string.fox
int terminate_words(char* in); //string.fox
int is_number(char c); //string.fox
int is_alphanum(char c, char* others); //string.fox
int is_alpha(char c, char* others); //string.fox
int is_oper(char c); //string.fox
int str_is_oper(char* str); //string.fox
int str_is_num(char* str); //string.fox
int is_code(char* str); //string.fox
int char_at(char* str, char* terms); //string.fox
int rchar_at(char* str, char* terms); //string.fox
char* char_str(char c); //string.fox
int has_chars(char* line); //string.fox
char* drop_right(char* str, char* w); //string.fox
char* drop_left(char* str, char* w); //string.fox
char* str_toupper(char* str); //string.fox
char* str_tolower(char* str); //string.fox
char* str_title(char* str); //string.fox
char hex_char(char* in); //string.fox
char* rand_str(int len); //string.fox
int str_char_count(char* str, char c); //string.fox
char* str_join(char* str1, char* joiner, char* str2); //string.fox
char* var_bits(void* var); //string.fox
char* rtrim_upto(char* in, char upto, int keep); //string.fox
map* split_by(char* str, char term, int max); //string.fox
char* str_hex(char* in); //string.fox
char* str_tr(char* in, map* replace); //string.fox
char* skip_over(char* in, char* chars); //string.fox
char* skip_quote(char* str); //string.fox
char* skip_paren(char* str); //string.fox
char closing_paren(char c); //string.fox
char* ucs_str(int* in, int len); //text.fox
int utf_lang(char* in); //text.fox
int is_letter(int code); //text.fox
map* str_stem(char* in); //text.fox
map* words_stem(map* in); //text.fox
char* word_stem(char* in); //text.fox
char* utf_word_end(char* in); //text.fox
char* utf_word_start(char* in); //text.fox
int* ucs_substr(int* in, int from, int upto); //text.fox
int ucs_len(int* in); //text.fox
int* str_ucs(char* in); //text.fox
map* str_words(char* in); //text.fox
char* stem_arabic(char* in); //text.fox
char* stem_english(char* in); //text.fox
char* stem_bangla(char* in); //text.fox
int stem_test(); //text.fox
char* bangla_norm(char* in); //text.fox
int xTokenize(Fts5Tokenizer* unused1, void * pCtx, int flags, const char * pText, int nText, void* callback); //text.fox
int xCreate(void* unused, const char ** azArg, int nArg, Fts5Tokenizer ** ppOut); //text.fox
static int xDelete(Fts5Tokenizer* unused); //text.fox
char* db_init_tokenizer(char* db); //text.fox
map* bangla_joins(); //text.fox
map* bangla_english(char* in); //text.fox
char* soundex(char * s); //text.fox
char* num_lang(char* in, int langid); //text.fox
char* time_ago(char* stime); //time.fox
time_t str_time(char* in); //time.fox
char* time_str(time_t timer); //time.fox
char* today(); //time.fox
char* now(); //time.fox
int this_year(); //time.fox
char* datetime(); //time.fox
char* utf_letter(char* in, char* out); //unicode.fox
int utf_strlen(char* in); //unicode.fox
int unicode_len(int ucs2); //unicode.fox
char* unicode_utf(int ucs2, char* ret); //unicode.fox
int utf_len(char* in); //unicode.fox
int utf_unicode(char* ptr); //unicode.fox
map* xpath(char* in, char* x_path); //xpath.fox
double circular(double v, double from, double upto); //astro.h
double jd3(int y, int m, double d); //astro.h
double year(double jd); //astro.h
double jd_month(double jd); //astro.h
double delta_t(double jd); //astro.h
double delta_t2(double jd); //astro.h
double julian_centuries(double jd); //astro.h
double sun_mean_lng(double t); //astro.h
double sun_mean_ano(double t); //astro.h
double sun_mean_ano5(double t); //astro.h
double ecentricity(double t); //astro.h
double _ceil(double value); //astro.h
double sind(double v); //astro.h
double cosd(double v); //astro.h
double tand(double v); //astro.h
double asind(double v); //astro.h
double acosd(double v); //astro.h
double atand(double v); //astro.h
double atand2(double y, double x); //astro.h
double round2(double v, double d); //astro.h
double ahavd(double v); //astro.h
double havd(double v); //astro.h
double bearing(double lng1, double lat1, double lng2, double lat2); //astro.h
double diff_angle(double lng1, double lat1, double lng2, double lat2); //astro.h
double diff_angle2(double lng1, double lat1, double lng2, double lat2); //astro.h
double moon_mean_lng(double t); //astro.h
double moon_mean_elong(double t); //astro.h
double moon_mean_ano(double t); //astro.h
double moon_arg_lat(double t); //astro.h
double sun_eqn(double t, double meanano); //astro.h
double refraction(double alt); //astro.h
double refraction2(double alt, double temp, double press); //astro.h
double parallax_au(double alt, double r); //astro.h
double parallax(double alt, double r); //astro.h
double dip(double height); //astro.h
double geodetic(double lat); //astro.h
double moon_mean_node_lng(double t); //astro.h
double nutation_obl(double t); //astro.h
double nutation_lng(double t); //astro.h
double sidereal4(double jd); //astro.h
double az(double lha, double dec, double lat); //astro.h
double alt(double lha, double dec, double lat); //astro.h
double sun_lng4(double t); //astro.h
double sun_ano4(double t); //astro.h
double sun_r(double t); //astro.h
double dms(double d, double m, double s); //astro.h
double obl4(double t); //astro.h
double ra(double lng, double lat, double obl); //astro.h
double decli(double lng, double lat, double obl); //astro.h
double sun_semi_dia(double t); //astro.h
double moon_semi_dia(double t); //astro.h
double moon_month_phase(double t); //astro.h
double moon_visual_phase(double t); //astro.h
double moon_lng4(double t); //astro.h
double moon_lat4(double t); //astro.h
double moon_r(double t); //astro.h
double moon_mean_phase(double k, double t); //astro.h
double is_eql(double v1, double v2, double maxdiff); //astro.h
double lunation_date(double lun); //astro.h
double lha(double ra, double sidereal, double lng); //astro.h
double cos_ha(double alt, double decl, double lat); //astro.h
double alt_ha(double alt, int direction, double dec, double lat); //astro.h
double rise_set(double jd, double t, double elng, double elat, double alt, int direction, double lng, double lat); //astro.h
double sun_transit(double jd, double lng, double lat); //astro.h
double sun_rise_set(double alt, int direction, double jd, double lng, double lat); //astro.h
double moon_rise_set(double alt, int direction, double jd, double lng, double lat); //astro.h
double sun_rise(double jd, double lng, double lat); //astro.h
double sun_set(double jd, double lng, double lat); //astro.h
double moon_rise(double jd, double lng, double lat); //astro.h
double moon_set(double jd, double lng, double lat); //astro.h
double sun_ra(double t); //astro.h
double sun_dec(double t); //astro.h
double moon_ra(double t); //astro.h
double moon_dec(double t); //astro.h
double sun_az(double jd, double lng, double lat); //astro.h
double sun_alt(double jd, double lng, double lat); //astro.h
double moon_az(double jd, double lng, double lat); //astro.h
double moon_alt(double jd, double lng, double lat); //astro.h
double moon_position_angle(double t); //astro.h
double moon_zenith_angle(double jd, double lng, double lat); //astro.h
double yallop_best_time(double jd, double lng, double lat); //astro.h
double arcl(double jd, double lng, double lat); //astro.h
double arcv(double jd, double lng, double lat); //astro.h
double moon_pi(double t); //astro.h
double angle_sun_moon(double t); //astro.h
char* yallop_name(int zone); //astro.h
double translate_yallop_q(double q); //astro.h
double yallop_q2(double moon_alt, double arc_light, double arc_vision, double moon_r); //astro.h
double yallop_q(double jd, double lng, double lat); //astro.h
int hijri_year(int lun); //astro.h
int hijri_month(int lun); //astro.h
double lunation(double jd); //astro.h
double new_moon(double lun); //astro.h
double full_moon(double lun); //astro.h
double moon_phase_angle(double t); //astro.h
double moonmag(double t); //astro.h
double moon_mag3(double phaseangle); //astro.h
double moon_mag4(double dist, double phase); //astro.h
double moon_mag2(double t); //astro.h
int array_seek(char** arr, char* val); //astro.h
