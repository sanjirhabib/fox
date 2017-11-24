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
#include <sqlite3.h>

enum Types {
	Null,Skip,Int,Double,String,Blob,Map,Vector,Index,Keys,Cell,Cell2,Tail
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

void* php_global(char* name);
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
char* var_bits(void* var);
char* to_str(void* v, char* null, int human);
size_t blob_size(char* in);
char* new_blob(int size);
char* str_dup_len(char* str, int len);
char* str_dup(char* str);
char* str_quote(char* head);
char* double_str(double val);
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
long long is_int(void* v);
double is_double(void* v);
void* int_var(size_t i);
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
double to_double(void* v);
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
int chdir(const char* path);
void* invoke(map* v, char* name);
map* reflect();
int max_mem();
int curr_mem();
char* version();
map* args_map();
void* call_php(map* params, char* func);
int make(map* files, char* outdir);
int foxc(char* file);
int compile(map* files, char* outdir, char* outfile, char* options, int release, int exe);
char* file_dir(char* file);
char* file_rename(char* file, char* dir, char* delext, char* addext, char* prefix, char* postfix);
char* sane_dir(char* dir);
map* build(map* files, char* outdir);
char* mem_usage();
char* int_kb(size_t i, char* unit);
char* int_human(int i, char* unit, char* zero);
map* env_vars();
char* substr(char* src, int from, int len);
char* mstr(char* format, ...);
char* str_add(char* str1, char* str2);
char* cat_char(char* str, char c);
char* va_str(char* format, va_list args);
void* verbose(char* msg, ...);
void* dx(void* data, char* name, int panic);
int neq(char* str, char* str1);
int eq(char* str, char* str1);
char* cwd();
char* write_file(char* data, char* filename, int readonly);
char* fox_read_file(char* filename, int error_on_fail);
char* read_stdin(int size, char* terminator);
char* str_unquote(char* str);
char* str_escape(char* head);
char* str_end(char* str, char* end);
int str_start(char* str, char* start);
char* str_chr(char* str, char c);
char* ltrim(char* str, char* chars);
char* rtrim(char* str, char* chars);
char* str_trim(char* str, char* chars);
int line_isempty(char* line);
int line_len(char* line);
int lchars(char* str, char* chars);
int rchars(char* str, char* chars);
int has_str(char* str, char* substr);
char* str_replace(char* str, void* find, void* replace);
char* outx(char* str);
char* flush_out();
map* xadd(map* mp, ...);
int is_numeric(char* str);
map* to_map(void* val);
map* del_index(map* mp, int idx);
map* map_del_key(map* mp, char* key);
map* map_del(map* mp, int idx, int len);
map* xjson_map(char* in, int type);
map* xjson_map_recurse(char** line, int type);
int str_level(char* line);
int block_level(char* str);
void* marked_str(char* str, char* name);
map* map_add_pair(map* mp, void* name, void* value, int type);
map* add_name_val(map* mp, char* str, char** name, char** val, int type);
char* read_as_block(char** from, int level, char* terminator);
map* block_map(char** from, int inlevel, int maptype);
map* xkeys(char* str);
map* xindex(char* str);
map* xarray(char* str);
map* str_vec(void* str);
void cache_blast(char* type);
void* cache(void* key, char* type, void* out);
map* str_map(char* str, int type);
void rewrite_ptr(mempage* pg, void** ptr);
void rewrite_ptrs(mempage* old);
int copy_page(mempage* from, mempage* to);
int page_map(mempage* pg);
int dump_chain(mempage* pg, int line);
int check_chains(mempage* pg, int line);
int total_kb();
int page_maps(char* title);
map* vec_del(map* mp, int from, int len);
map* vec_shift(map* mp, int idx, int len, int offset);
map* vec_sub(map* mp, int from, int len);
map* vec_slice(map* mp, int from, int len);
map* map_splice(map* mp, int from, int del, map* ad);
void* vec_pop(map* mp);
map* vec_splice(map* mp, int from, int del, map* add);
char* str_has(char* str, char* sub);
void gotoxy(int x, int y);
int floor_pow2(int i);
map* vec_dup(map* mp);
map* map_dup(map* mp);
map* map_merge(map* mp1, map* mp2);
map* vec_merge(map* mp1, map* mp2);
int utest(char* found, char* expect, char* in, char* title);
map* map_compact(map* mp);
map* vec_reverse(map* mp);
map* vec_compact(map* mp);
map* change_key(map* mp, int idx, char* id);
char* old_decl_type(map* mp, int idx);
map* add_id(map* mp, int idx);
map* add_key(map* mp, char* k, int type);
char* str_upper(char* s);
char* str_lower(char* s);
map* str_split(char* str, char* by, int limit);
char* toks_str(map* mp);
char* map_join(map* mp, char* joiner);
char fox_at(char* str, int idx);
char* indent_str(int i);
int str_chars(char* str, char* chars);
char* map_str(map* mp);
char* map_str_indent(map* mp, int indent);
map* dir_files(char* path);
char* end_word(char* str, char* words);
char* has_word(char* str, char* words);
int is_word(char* word, char* list);
int terminate_words(char* in);
map* map_vec(map* mp);
map* toks_keywords(map* mp, char* keywords);
map* map_split(map* mp, char* str, int limit);
size_t file_time(char* file);
int is_number(char c);
int is_alphanum(char c, char* others);
int is_alpha(char c, char* others);
int is_oper(char c);
int str_is_oper(char* str);
int str_is_num(char* str);
int is_code(char* str);
char* read_num(char** in);
char* skip_over(char* in, char* chars);
char* skip_word(char* in, char* seperators);
void* read_value(char** str, char* sep);
char* read_upto(char** in, char* terminators);
char* read_alpha(char** in);
char* fox_read_symbol(char** in);
char* fox_read_oper(char** in, char term);
char* read_upto_word(char** in, char* upto);
char* read_theline(char** in);
char* read_newline(char** in);
char* read_space(char** in);
char* read_heredoc(char** in);
char* read_multistr(char** in);
char* read_quote(char** in);
map* tokenizer(char** line, char* comment);
char* xlog(char* str);
char closing_paren(char c);
char char_at(char* str, int i);
char* char_str(char c);
int map_has_word(map* mp, char* str);
int is_keyword(char* str);
char* toks_c(map* mp);
map* add_ctok(void* data, map* mp, int iscode);
map* c_tokenizer(char** line, char term);
int func_dot(map* mp, int idx);
map* heredoc_parts(char* str);
map* heredoc_vars(map* mp);
map* heredoc_str(map* mp);
map* dot_each(map* mp);
map* map_implode(map* mp, char* joiner);
int tok_indent(char* str);
int has_chars(char* line);
int requires_semicolon(map* mp, int idx);
map* add_semicolon(map* mp, int recurse);
char* hello();
int tok_endl(map* mp, int from);
map* force_curly(map* mp);
map* add_curly(map* mp, int recursive);
map* dot_key(map* mp);
int str_hasvar(char* in);
char* str_mstr(char* in);
map* xstr_parts(char* in);
char* str_xstr(char* in);
map* str_dollars(map* mp);
map* single_quotes(map* mp);
map* colon_str(map* mp);
map* dot_func(map* mp);
map* map_tox(map* mp);
int stm_end(map* mp, int idx, char* words, int including);
int stm_start(map* mp, int idx, char* terms);
map* syn_func(map* syn, int with_body);
map* syn_funcs(map* mp, int with_body);
map* file_vec(char* in);
map* file_map(char* in);
map* map_keys(map* mp);
char* drop_right(char* str, char* w);
char* drop_left(char* str, char* w);
map* read_struct(map* toks);
map* c_structs(char* in);
map* x_funcs(char* in, int with_body);
map* x_toks(char* in, int is_script);
map* func_params(map* func);
map* func_param(map* func);
map* func_add_param(map* func, map* add);
map* func_rename(map* func, char* name);
char* func_name(map* func);
map* map_assign(map* exp1, map* exp2);
map* call_count(map* toks, map* counter, char* infunc);
map* file_deadcode(char* file);
map* auto_types(map* toks, char* context, int is_script, map* env, map* fns, map* func, int idx);
char* is_typecast(map* toks, int idx);
char* is_name(char* in);
char* head_type(map* toks, int idx, int upto, map* env, map* fs);
int expr_tail(map* toks, int idx, char* expr);
int expr_head(map* toks, int idx, char* expr);
map* add_return(map* toks);
map* wrap_call(map* tok, char* func);
map* type_convert(map* tok, char* outtype, map* env, map* fs, map* fn);
map* param_c(map* params, map* env, map* fs, map* fn);
char* file_ast(char* in);
char* ast_c(map* ast);
map* block_ast(map* toks, char* seperator);
map* toks_ast(map* toks, int* idx, char* upto);
map* syn_expand_macro(map* syn, int idx);
map* syn_is_macro(map* syn, int idx);
map* syn_set_param(map* syn, int idx, map* params);
map* syn_func_param(map* syn, int idx);
map* syn_func_body(map* syn);
map* syn_remove_default(map* syn);
char* expr_type(map* toks, int idx, int upto, map* env, map* fs);
char* assign_to_func(map* tok);
int next_tok(map* toks, int from, char* tok, int upto);
int syn_has(map* syn, char* words);
char* syn_flow_name(map* syn);
int is_flow(map* syn);
map* syn_flow_condition(map* syn, int no);
map* syn_flow_body(map* syn, int no);
char* syn_is_call(map* syn, int idx);
char* syn_var_name(map* syn);
char* syn_var_type(map* syn);
int syn_assign_val(map* syn);
int is_assign(map* syn);
int is_var_decl(map* syn);
int is_func_decl(map* syn);
char* fox_h(char* infile, char* outfile, int with_foxh);
char* fox_c(char* infile, char* outfile);
map* x_map(char* in);
char* c_x(char* in);
char* x_c(char* in);
char* func_ccall(map* fn);
char* map_ccode(void* mp);
char* callfunc_c(map* funcs);
char* file_foxh(char* infile, char* outfile);
char* func_cdecl(map* fn, int show_default);
char* funcs_cdecl(map* fns, int show_default);
char* foxh();
char* write_foxh(char* outfile);
char* fox_phpc(char* infile, char* outfile);
char* write_phpconfig();
char* write_dynamic(char* outfile);
map* eval_params(map* sent, char* name, map* env);
char* to_c(void* val);
char* str_shorten(char* in, int max);
char* call_c(map* params, char* name);
void* call_func(map* params, char* name, map* env);
void* data_exec(void* data, map* env);
static void* eval_toks(map* mp, map* env);
static void* eval_expr(map* mp, int* idx, map* env, int level);
void* eval(char* in, map* env);
int eval_expr_cont(map* mp, int idx, map* env, void** last, int level);
void* binary_op(void* left, char oper, void* right);
int is_true(void * val);
char* read_line(FILE* fp);
char* skip_quote(char* str);
char* str_toupper(char* str);
char* str_tolower(char* str);
char* str_title(char* str);
char hex_char(char* in);
char* rand_str(int len);
int str_char_count(char* str, char c);
char* write_c(char* infile, char* outfile);
map* command_line(char* in, int argc, char** argv);
int is_web();
map* param_test(char* one, char* two);
map* cmdline_params(map* args, char* func);
int test_add(int a, int b);
map* test_map(map* out);
int run_cmdline(map* args);
int utests(char* test, char* file);
char* file_path(char* file);
map* load_global(char* file);
int err_msg(char* msg, char** ptr);
char* tutorial();
char* h(char* in);
char* type_name(int type);
char* ptr_name(void* var);
int is_file(char* filename);
char* fork_exec(char* cmd, map* params);
map* source_files();
map* source_funcs();
map* file_funcs(char* filename);
char* help();
map* funcs();
void* run(char* in);
void* fox_eval(map* mp, map* env, int* halt);
void src(map* mp, int from, int len, char* msg);
map* structs();
map* macros();
map* source_structs();
map* source_macros();
int fib(int i);
map* toks_syn(map* toks, int recurse);
time_t str_time(char* in);
char* time_str(time_t timer);
char* increase_version();
int call_variadic_int(map* mp, void* fp, char* name);
void* call_variadic_ptr(map* mp, void* fp, char* name);
map* vec_rdup(map* mp);
map* toks_replace(map* in, map* replace);
map* toks_join(map* in, char* by);
map* toks_align(map* in);
map* toks_split(map* in, char* by, int limit);
map* expand_macros(map* mp, map* macros);
map* c_macros(char* in);
map* toks_macros(map* mp);
void devel(char* file);
int is_inline_vector(map* toks, int idx);
map* string_operators(map* toks);
map* inline_vectors(map* toks);
int msleep(int msecs);
char* time_ago(char* stime);
char* ptr_id(void* ptr);
char* today();
char* now();
char* datetime();
void test_loop();
int crash();
int str_ascii(char* in);
map* sql_tokenizer(char** line);
map* prop_tokenizer(char** line);
void benchmark_gc();
char* str_echo(char* str);
map* map_echo(map* in);
char* str_join(char* str1, char* joiner, char* str2);
map* read_paren(map* mp, char** line, map*(*func)(char** ));
map* set_map(void* val, map* mp, int idx);
map* sql_toks(char* line);
map* split_keywords(map* mp, char* words);
void* key_var(char* key);
map* map_key_val(map* mp, int idx);
char* join_clause(char* pre, map* mp, char* clause, char* by, char* sub1, char* sub2, char* use_cls);
map* de_select(map* cls);
map* de_from(map* cls);
map* parse_where(map* cls);
map* de_where(map* cls);
map* de_having(map* cls);
map* de_order(map* cls);
map* de_group(map* cls);
map* de_limit(map* cls);
map* sql_toks_map(map* toks);
map* sql_map(char* sql);
char* sql_map_join(map* mp, char* joiner);
char* var_join(void* v, char* joiner);
map* sql_convert_func(map* mp, char* db);
char* is_func(map* mp, int idx);
map* sql_func_params(map* mp, int idx);
map* get_func(map* mp, int idx);
map* search_sql_func(map* mp, char* db);
map* to_vec(void* val);
map* sql_order(char* sql);
map* sql_tables(char* sql, char* db);
map* sql_col(char* sql, char* db, map* exp);
char* str_code(char* in);
map* crosstab_cols(map* cols, char* sql, char* db, map* params);
map* sql_select_cols(char* sql, char* db, map* params);
char* sql_add_where(char* sql, map* mp);
map* cols_collect(map* cols, void* collect);
map* sql_where_cols(char* sql, char* db);
map* sql_where_vals(char* sql);
char* sql_add_limit(char* sql, int rpp, int page);
char* sql_add_order(char* sql, char* db, map* order);
char* sql_add_filter(char* sql, map* filter);
char* name_type(char* name);
map* to_cols(void* cols, char* sql, char* db);
map* sql_cols(char* sql, char* db, void* cols);
map* sql_add_cols(char* sql, char* db, map* cols);
map* sql_view_cols(char* sql, char* db, map* cols);
map* sql_edit_cols(char* sql, char* db, map* cols);
map* sql_list_cols(char* sql, char* db, map* cols);
char* table_name(char* tbl, char* db);
char* sql_add_cls(char* sql, char* cls, map* vals);
map* sql_cls(char* sql, char* cls);
char* sql_str(void* data);
char* re_select(map* mp);
char* re_from(map* cls);
char* re_where(map* cls);
char* re_group(map* cls);
char* re_having(map* cls);
char* re_order(map* cls);
char* re_union(map* cls);
char* re_limit(map* cls);
char* map_sql(map* mp);
int has_aggregate(char* sql);
char* sql_auto_join(char* sql, char* db);
char* sql_auto_group(char* sql, char* db);
char* sql_lite(char* sql, char* db, map* params);
int sql_utest(char* in, char* out);
int type_size(char* type);
char* lite_create_col(map* col);
int is_indexed(char* type);
map* create_index_sqls(map* tbl);
char* drop_sql(char* name);
char* create_sql(map* tbl, char* name);
char* meta_type(char* type, int size);
int has_table(char* db, char* tbl);
map* map_cols(void* cols, char* tbl, char* db);
map* cols_table(map* cols, char* tbl, char* db);
map* tables(char* db);
int db_has_schema(char* db);
map* tbl_cols(char* tbl, char* db);
map* table(char* tbl, char* db);
map* schema_tables(char* db);
map* schema_table(char* db, char* tbl);
map* db_table(char* db, char* tbl);
map* db_tables(char* db);
map* db_cols(char* db, char* tbl);
map* row_ids(void* row, char* tbl, char* db);
map* pkeys_where(char* tbl, char* db);
map* tbl_row_ids(char* tbl, char* db, map* row);
char* tbl_row_id(char* tbl, char* db, map* row);
char* ids_id(map* ids);
map* url_gets();
map* get_ids(char* tbl, char* db);
char* sql_table(char* sql);
map* sql_pkeys(char* sql, char* db);
void* sql_value(char* sql, char* db, map* param);
map* sql_rows(char* sql, char* db, map* param);
map* sql_query(char* sql, char* db, map* where);
map* tbl_id_ids(char* tbl, char* db, void* id);
map* sql_id_ids(char* sql, char* db, void* id);
map* sql_id(char* sql, char* db, void* ids);
map* sql_row(char* sql, char* db, map* param);
map* sql_vector(char* sql, char* db, map* param);
map* sql_pairs(char* sql, char* db, map* param);
map* sql_vec(char* sql, char* db, map* param);
char* sql_drop(char* tbl);
char* to_sql(char* sql);
map* schema_fkeys(char* db);
map* tbl_referred_by(char* tbl, char* db);
void* id_update(void* ids, char* tbl, char* db, map* row);
map* row_insert(map* row, char* tbl, char* db);
void* sql_error(char* sql, char* db, sqlite3* conn);
map* sqls_exec(map* sqls, char* db);
map* parse_connection2(char* in);
map* parse_connection(char* in);
map* conn_db(char* db);
map* lite_exec(char* sql, char* db, map* params);
map* db_table_names(char* db);
map* schema_table_names(char* db);
map* user_tables(map* tbls);
map* table_names(char* db);
char* col_title(map* col);
map* cols_pkeys(map* cols);
map* crud_save(char* sql, char* db, map* cols, char* back, map* data);
char* maptype(char* type_map, char* type);
map* tbl_pkeys(char* tbl, char* db);
map* tbl_skeys(char* tbl, char* db);
map* cols_skeys(map* cols);
map* toks_sql_params(map* toks, map* ret);
void* is_null(void* val);
map* sql_missing_params(char* sql, map* params);
map* fkey_list(char* sql, char* db, map* params);
map* sql_params(char* sql);
char* fkey_sql(char* sql, char* db);
char* callback_php(map* param, char* name);
char* fkey_show(char* sql, char* db, char* id);
int fkey_valid(char* sql, char* db, char* id);
char* str_show(char* value, char* type, map* op, int width);
char* thumb_name(char* name);
map* row_show(map* row, map* cols, int width);
map* rows_show(map* rows, map* cols, int width);
char* cols_show(map* cols, map* row, char* name, int width);
char* buttons_html(map* buttons);
map* cols_list(map* cols, char* crud, int max);
map* map_table(map* mp, char* tbl, char* db);
map* table_map(map* tbl);
map* tables_map(map* tbls);
char* db_file(char* db, char* file);
char* schema_file(char* db, char* file);
map* file_compile(char* file, char* db, int go);
char* file_schema(char* file, char* db);
char* init_schema(char* db);
char* db_schema(char* db);
char* crud_tables(char* db);
char* crud_list(char* sql, char* db, int limit, char* crud);
char* confirm(char* prompt, char* option1, char* option2, char* title, map* data);
char* sql_delete(void* ids, char* tbl, char* db);
char* crud_delete(char* sql, char* db, char* back);
char* edit_schema(char* sql, char* db, map* cols, char* back);
char* crud_add(char* sql, char* db, map* cols, char* back);
char* crud_edit(char* sql, char* db, map* cols, char* back);
char* render_table(map* rows, map* cols, char* sql, char* crud);
char* cols_html(map* cols, map* buttons, char* title, map* vals, map* errs);
char* rename_sql(char* from, char* into);
map* save_schema(char* db, char* tbl, map* data);
map* cols_ctrls(map* cols, map* vals, map* errs);
map* subtypes(char* type);
int type_distance(char* type1, char* type2);
map* cols_match(map* from, map* into);
map* schema_list(char* db, char* type);
map* schema_item(char* db, char* type, char* name);
char* month_name(int month);
char* date_show(char* in);
void* crud(char* sql, char* db, map* cols, char* crud, int limit, void* link, char* rest, char* table);
int cmp_ptr_reverse(const void* ptr1, const void* ptr2);
int cmp_ptr(const void* ptr1, const void* ptr2);
int cmp_cons_reverse(const void* ptr1, const void* ptr2);
int cmp_cons(const void* ptr1, const void* ptr2);
map* map_sort(map* mp, int reverse);
char* nearest_table(map* tbls, map* tbl);
map* tbls_sync_sqls(map* new_tbls, map* old_tbls);
map* schema_sync_db(char* file, char* db, int go);
map* file_tables(char* file);
map* file_sync_schema(char* file, char* db, int go);
map* file_sync_db(char* file, char* db, int go);
map* sync_sqls(map* newt, map* oldt);
int utf8_strlen(char* in);
int ucs2_to_utf8(int ucs2, char* utf8);
int utf8_to_ucs2(char** end_ptr);
char* test_out(char* in);
map* sql_sums(char* sql, char* db, map* cols, map* params);
int sql_count(char* sql, char* db, map* params);
char* read_token(char** line, char* terminators);
char* read_textblock(map* lines, int* lineno, char* terminator, char** outline);
map* mml_map(map* lines, int* from);
char* val_eval(void* in, map* params, int is_code);
char* load_mml(char* name);
char* map_html(map* in, map* params);
char* mml_html(char* str, map* params);
map* show_map(char* show);
char* data_show(map* data, char* show);
char* http_redirect(char* url);
void* http_error(char* msg, char* status);
map* compile_template(char* tem);
char* render_template(map* data, char* template_name);
char* render(map* data, void* tem);
char* render_data(void* data, map* tp, char* key);
char* file_mime(char* path);
char* str_html(char* in);
void header(char* str);
char* http_out(char* str, char* status, char* mime, map* headers);
char* static_file(char* path);
char* str_url(char* in);
char* url_str(char* in);
map* amps_map(char* in);
map* parse_url(char* path);
void load_theme(char* name);
void sess_load();
void sess_add(char* name, char* value);
char* sess_id();
char* sess_file();
char* sess_newid();
void cookie_set(char* name, char* value, char* path, char* expire);
void sess_delete();
map* link_relative(map* links, char* url);
char* links_ul(map* mp, char* class);
map* post_data();
char* button_clicked(map* process);
map* process_post(map* process);
char* process_logout(char* redirect);
map* process_login(char* username, char* password);
char* page_set_cookie();
char* page_dump();
map* url_data(char* url, char* match);
map* path_params(char* url, char* path);
int path_matches(char* match, void* url);
char* pages_match(map* pages, char* path);
void* pages_exec(void* pages, char* path, map* env);
char* not_found(char* path);
char* dispatch(map* pages, char* path);
char* inet(map* pages, char* path);
char* page(void* body, char* title, int width, void* link, char* theme, char* pg, map* process, map* env);
char* map_template(map* mp, char* template);
map* str_vars(char* str);
char* str_subst(char* in, map* data);
void* map_eval(void* mp, map* env);
char* str_eval(char* in, map* env);
map* header_val(char* line, map* mp);
map* header_vals(char* name);
map* header_map(char* val);
map* read_http_header();
int ip_connect(char* host, int port, char** err);
int ip_close(int sock);
int ip_send(char* msg, int sock);
char* ip_receive(int sock, char* terminator, int size);
char* remote_ip(int con);