#pragma once
#include <core.h>
#include <sql.h>
#pragma fox module schema
#include <tsv.h>
#include <foxparser.h>
void schema_sync(char* db, char* file); //schema.fox
void schema_sql(char* db, char* file); //schema.fox
char* schema_write(char* db, char* file); //schema.fox
map* schema_load(char* file, char* db); //schema.fox
char* schema_str(map* in); //schema.fox
char* rows_str(map* in, void* cols); //schema.fox
char* col_str(map* in); //schema.fox
char* sql_rename(char* from, char* into); //schema.fox
int type_distance(char* type1, char* type2); //schema.fox
map* cols_match(map* from, map* into); //schema.fox
char* nearest_table(map* tbls, map* tbl); //schema.fox
map* lite_trigger_slno(char* name, char* pkey, char* by); //schema.fox
map* lite_trigger_tree(char* name, char* pkey); //schema.fox
map* tbls_sync_sqls(map* new_tbls, map* old_tbls); //schema.fox
map* tbl_cols(char* table, char* db); //schema.fox
map* sync_sqls(map* newt, map* oldt); //schema.fox
map* trigger_sqls(char* tbl, char* db); //schema.fox
void* id_update(void* ids, char* tbl, char* db, map* row); //schema.fox
int row_insert(map* row, char* tbl, char* db); //schema.fox
map* toks_sql_params(map* toks, map* ret); //schema.fox
char* data1_str(void* in); //schema.fox
char* data_str(map* in, int level); //schema.fox
char* fkey_show(char* sql, char* db, char* id); //schema.fox
int fkey_valid(char* sql, char* db, char* id); //schema.fox
map* lite_schema(char* db); //schema.fox
map* sql_order(char* sql); //schema.fox
map* crosstab_cols(map* cols, char* sql, char* db, map* params); //schema.fox
map* sql_select_cols(char* sql, char* db, map* params); //schema.fox
char* sql_add_where(char* sql, map* mp); //schema.fox
map* sql_where_cols(char* sql, char* db); //schema.fox
map* sql_where_vals(char* sql); //schema.fox
char* sql_add_limit(char* sql, int rpp, int page); //schema.fox
char* sql_add_order(char* sql, char* db, map* order); //schema.fox
char* sql_add_filter(char* sql, map* filter); //schema.fox
char* sql_str(void* data); //schema.fox
char* re_select(map* mp); //schema.fox
char* re_from(map* cls); //schema.fox
char* re_where(map* cls); //schema.fox
char* re_group(map* cls); //schema.fox
char* re_having(map* cls); //schema.fox
char* re_order(map* cls); //schema.fox
char* re_union(map* cls); //schema.fox
char* re_limit(map* cls); //schema.fox
char* map_sql(map* mp); //schema.fox
int has_aggregate(char* sql); //schema.fox
char* sql_auto_join(char* sql, char* db); //schema.fox
char* sql_auto_group(char* sql, char* db); //schema.fox
char* sql_add_cls(char* sql, char* cls, map* vals); //schema.fox
map* sql_cls(char* sql, char* cls); //schema.fox
map* sql_tables(char* sql, char* db); //schema.fox
map* sql_col(char* sql, char* db, map* exp); //schema.fox
char* sql_table(char* sql); //schema.fox
map* sql_pkeys(char* sql, char* db); //schema.fox
map* to_cols(void* cols, char* sql, char* db); //schema.fox
map* sql_cols(char* sql, char* db, void* cols); //schema.fox
map* sql_add_cols(char* sql, char* db, map* cols); //schema.fox
map* sql_view_cols(char* sql, char* db, map* cols); //schema.fox
map* sql_edit_cols(char* sql, char* db, map* cols); //schema.fox
map* sql_list_cols(char* sql, char* db, map* cols); //schema.fox
char* sql_lite(char* sql, char* db, map* params); //schema.fox
int sql_utest(char* in, char* out); //schema.fox
map* sql_missing_params(char* sql, map* params); //schema.fox
map* fkey_list(char* sql, char* db, map* params); //schema.fox
char* fkey_sql(char* sql, char* db); //schema.fox
map* sql_params(char* sql); //schema.fox
void* id_delete(void* ids, char* tbl, char* db); //schema.fox
char* read_textblock(map* lines, int* lineno, char* terminator, char** outline); //schema.fox
map* sql_tokenizer(char** line); //schema.fox
map* prop_tokenizer(char** line); //schema.fox
map* tokenizer(char** line, char* comment); //schema.fox
char* to_sql(char* sql); //schema.fox
map* schema_fkeys(char* db); //schema.fox
map* tbl_referred_by(char* tbl, char* db); //schema.fox
map* sql_sums(char* sql, char* db, map* cols, map* params); //schema.fox
int sql_count(char* sql, char* db, map* params); //schema.fox
map* sql_query(char* sql, char* db, map* where); //schema.fox
char* join_clause(char* pre, map* mp, char* clause, char* by, char* sub1, char* sub2, char* use_cls); //schema.fox
map* de_select(map* cls); //schema.fox
map* de_from(map* cls); //schema.fox
map* parse_where(map* cls); //schema.fox
map* de_where(map* cls); //schema.fox
map* de_having(map* cls); //schema.fox
map* de_order(map* cls); //schema.fox
map* de_group(map* cls); //schema.fox
map* de_limit(map* cls); //schema.fox
map* sql_toks_map(map* toks); //schema.fox
map* sql_map(char* sql); //schema.fox
char* sql_map_join(map* mp, char* joiner); //schema.fox
char* var_join(void* v, char* joiner); //schema.fox
map* sql_convert_func(map* mp, char* db); //schema.fox
char* is_func(map* mp, int idx); //schema.fox
map* sql_func_params(map* mp, int idx); //schema.fox
map* get_func(map* mp, int idx); //schema.fox
map* search_sql_func(map* mp, char* db); //schema.fox
void* key_var(char* key); //schema.fox
map* map_key_val(map* mp, int idx); //schema.fox
map* db_table_names(char* db); //schema.fox
map* cols_pkeys(map* cols); //schema.fox
map* where_param(map* where); //schema.fox
void* is_null(void* val); //schema.fox
map* db_table(char* db, char* tbl); //schema.fox
map* db_tables(char* db); //schema.fox
map* where_rows(map* where, char* tbl, char* db); //schema.fox
map* where_row(map* where, char* tbl, char* db); //schema.fox
map* id_row(void* ids, char* tbl, char* db); //schema.fox
map* id_param(void* ids, char* tbl, char* db); //schema.fox
map* id_ids(void* id, char* tbl, char* db); //schema.fox
map* db_cols(char* db, char* tbl); //schema.fox
map* tbl_pkeys(char* tbl, char* db); //schema.fox
map* tbl_skeys(char* tbl, char* db); //schema.fox
map* cols_skeys(map* cols); //schema.fox
map* sql_toks(char* line); //schema.fox
map* split_keywords(map* mp, char* words); //schema.fox
char* lite_create_col(map* col); //schema.fox
int is_indexed(char* type); //schema.fox
map* tbl_trigger_sqls(map* tbl); //schema.fox
map* create_index_sqls(map* tbl); //schema.fox
char* drop_sql(char* name); //schema.fox
char* create_sql(map* tbl, char* name); //schema.fox
char* schema_type(char* type, int size); //schema.fox
int has_table(char* db, char* tbl); //schema.fox
map* map_cols(void* cols, char* tbl, char* db); //schema.fox
map* cols_table(map* cols, char* tbl, char* db); //schema.fox
map* cols_collect(map* cols, void* collect); //schema.fox
char* name_type(char* name); //schema.fox
void retree(char* db); //schema.fox
