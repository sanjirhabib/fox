#pragma once
#include <core.h>
#pragma fox module generator
#include <foxtime.h>
#include <playground.h>
#include <cmd.h>
#include <dir.h>
#include <tsv.h>
#include <map.h>
#include <foxparser.h>
#include <cmdline.h>
extern void* (*invoke)(map* v,char* name);
extern map* (*reflect)();
#define MAXMEM 40*1024*1024
#define MIN_CHAIN 1
int cgi(char* infile, char* xfiles, char* profile, char* outfile, char* opts, int keepfiles); //generator.fox
static char* fox_meta(char* infile, char* name, map* depends, char* outfile); //generator.fox
map* file_deps(char* filename, map* deps); //generator.fox
int vec_has(map* in, char* key); //generator.fox
int cc(char* infile, char* xfiles, char* profile, char* outfile, char* opts, int keepfiles); //generator.fox
char* write_user_meta(char* name, map* depends); //generator.fox
static char* fox_cs(char* name, map* depends, int addmeta); //generator.fox
void write_source(char* infile, ...); //generator.fox
map* source_pragmas(map* infiles, map* ret); //generator.fox
map* fox_includes(char* infile); //generator.fox
char* fox_pragma(char* infile); //generator.fox
char* source_defines(map* infiles); //generator.fox
char* exclude_defines(char* in); //generator.fox
int tok_isdefine(char* in); //generator.fox
char* fox_defines(char* infile); //generator.fox
static map* funcs_nonstatic(map* funcs); //generator.fox
static char* meta_h(char* prefix); //generator.fox
static char* funcs_meta(map* data, char* prefix); //generator.fox
static char* gen_htaccess(char* outfile); //generator.fox
static char* gen_cgi_makefile(char* name, char* outfile); //generator.fox
static char* gen_fox_cgi(char* name, char* outfile); //generator.fox
int init_fox(char* name); //generator.fox
int init_cgi(char* name); //generator.fox
char* c_h(char* infile, char* outfile); //generator.fox
char* fox_h(char* infile, char* outfile); //generator.fox
char* c_x(char* in); //generator.fox
static char* map_ccode(void* mp); //generator.fox
static char* file_foxh(char* infile, char* outfile); //generator.fox
char* funcs_cdecl(map* fns, int show_default); //generator.fox
static char* old_decl_type(map* mp, int idx); //generator.fox
static char* toks_str(map* mp); //generator.fox
static map* toks_keywords(map* mp, char* keywords); //generator.fox
static int is_keyword(char* str); //generator.fox
static char* toks_c(map* mp); //generator.fox
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
map* source_funcs(map* infiles); //generator.fox
map* file_funcs(char* filename, int withbody); //generator.fox
static void src(map* mp, int from, int len, char* msg); //generator.fox
static map* structs(); //generator.fox
static map* macros(); //generator.fox
map* file_blocks(char* infile); //generator.fox
char* file_section(char* file, char* section); //generator.fox
char* files_section(map* files, char* section); //generator.fox
static map* toks_syn(map* toks, int recurse); //generator.fox
char* increase_version(); //generator.fox
static map* toks_replace(map* in, map* replace); //generator.fox
static map* toks_join(map* in, char* by); //generator.fox
static map* expand_macros(map* mp, map* macros); //generator.fox
map* c_macros(char* in); //generator.fox
static map* toks_macros(map* mp); //generator.fox
static int is_inline_vector(map* toks, int idx); //generator.fox
static map* string_operators(map* toks); //generator.fox
static map* inline_vectors(map* toks); //generator.fox
static map* file_map(char* filename); //generator.fox
map* data_map(char* in); //generator.fox
static map* data_map2(char** in, int level); //generator.fox
static char* skip_code(char* in, char* terminator); //generator.fox
static char* skip_upto(char* in, char* chars); //generator.fox
static void* data_unquote(char* in); //generator.fox
static map* prop_vec(char* in); //generator.fox
static map* prop_map(char* in, char* name); //generator.fox
char* callfunc_c(map* funcs); //generator.fox
static char* func_ccall(map* fn); //generator.fox
char* fox_c(char* infile, char* outfile); //generator.fox
char* x_c(char* in); //generator.fox
static map* type_convert(map* tok, char* outtype, map* env, map* fs, map* fn); //generator.fox
static map* param_c(map* params, map* env, map* fs, map* fn); //generator.fox
static char* head_type(map* toks, int idx, int upto, map* env, map* fs); //generator.fox
char* help(char* func); //generator.fox
char* expr_type(map* toks, int idx, int upto, map* env, map* fs); //generator.fox
map* x_toks(char* in, int is_script); //generator.fox
static map* auto_types(map* toks, char* context, int is_script, map* env, map* fns, map* func, int idx); //generator.fox
map* file_callmap(char* filename); //generator.fox
static map* func_depend(map* mp, map* ret); //generator.fox
map* depends(char* filename); //generator.fox
map* file_depends(char* filename, ...); //generator.fox
static map* func_depends(map* mp, char* filename); //generator.fox
int utests(char* test, char* file); //generator.fox
map* funcs(); //generator.fox
void* fox_run(char* in); //generator.fox
static void* fox_eval(map* mp, map* env, int* halt); //generator.fox
static void* eval_toks(map* mp, map* env); //generator.fox
static void* eval_expr(map* mp, int* idx, map* env, int level); //generator.fox
static void* binary_op(void* left, char oper, void* right); //generator.fox
static int is_true(void* val); //generator.fox
char* func_cdecl(map* fn, int show_default); //generator.fox
void* eval(char* in, map* env); //generator.fox
static int eval_expr_cont(map* mp, int idx, map* env, void** last, int level); //generator.fox
void* data_exec(void* data, map* env); //generator.fox
void* call_func(map* params, char* name, map* env); //generator.fox
static map* ping_map(map* in); //generator.fox
map* func_depend_recursive(char* func, char* file, map* funcs, map* files); //generator.fox
int call_variadic_int(map* mp, void* fp, char* name); //generator.fox
void* call_variadic_ptr(map* mp, void* fp, char* name); //generator.fox
