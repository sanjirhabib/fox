#include <fox.h>
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
map* trigger_sqls(char* tbl, char* db);
map* tbl_trigger_sqls(map* tbl);
map* create_index_sqls(map* tbl);
char* drop_sql(char* name);
char* create_sql(map* tbl, char* name);
char* meta_type(char* type, int size);
int has_table(char* db, char* tbl);
map* map_cols(void* cols, char* tbl, char* db);
map* cols_table(map* cols, char* tbl, char* db);
map* pre_tables();
map* db_table(char* db, char* tbl);
map* db_tables(char* db);
map* db_cols(char* db, char* tbl);
map* url_gets();
char* sql_table(char* sql);
map* sql_pkeys(char* sql, char* db);
void* sql_value(char* sql, char* db, map* param);
map* sql_rows(char* sql, char* db, map* param);
map* sql_query(char* sql, char* db, map* where);
map* id_ids(void* id, char* tbl, char* db);
map* sql_row(char* sql, char* db, map* param);
map* sql_vector(char* sql, char* db, map* param);
map* sql_pairs(char* sql, char* db, map* param);
map* sql_vec(char* sql, char* db, map* param);
char* sql_drop(char* tbl);
char* to_sql(char* sql);
map* meta_fkeys(char* db);
map* tbl_referred_by(char* tbl, char* db);
void* id_update(void* ids, char* tbl, char* db, map* row);
int row_insert(map* row, char* tbl, char* db);
void* sql_error(char* sql, char* db, sqlite3* conn);
map* sqls_exec(map* sqls, char* db);
map* parse_connection(char* in);
map* conn_db(char* db);
map* sql_exec(char* sql, char* db, map* params);
void* lite_conn(char* db);
map* lite_exec(char* sql, char* db, map* params);
void close_conns();
map* vec_map(map* in);
map* db_table_names(char* db);
map* cols_pkeys(map* cols);
map* types();
char* map_type(map* mp, char* type);
map* where_param(map* where);
map* where_rows(map* where, char* tbl, char* db);
map* where_row(map* where, char* tbl, char* db);
map* id_row(void* ids, char* tbl, char* db);
map* id_param(void* ids, char* tbl, char* db);
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
char* cols_show(map* cols, map* row, char* name, int width);
char* datetime_human(char* in);
char* thumb_name(char* name);
map* row_show(map* row, map* cols, int width);
map* rows_show(map* rows, map* cols, int width);
void* id_delete(void* ids, char* tbl, char* db);
char* sql_rename(char* from, char* into);
map* subtypes(char* type);
int type_distance(char* type1, char* type2);
map* cols_match(map* from, map* into);
char* month_name(int month);
int cmp_ptr_reverse(const void* ptr1, const void* ptr2);
int cmp_ptr(const void* ptr1, const void* ptr2);
int cmp_cons_reverse(const void* ptr1, const void* ptr2);
int cmp_cons(const void* ptr1, const void* ptr2);
map* map_sort(map* mp, int reverse);
char* nearest_table(map* tbls, map* tbl);
map* tbls_sync_sqls(map* new_tbls, map* old_tbls);
map* db_meta(char* db);
map* tbl_cols(char* table, char* db);
map* db_sync(char* db, int go);
map* sync_sqls(map* newt, map* oldt);
char* test_out(char* in);
map* sql_sums(char* sql, char* db, map* cols, map* params);
int sql_count(char* sql, char* db, map* params);
map* regexp(char* in, char* pattern);
char* read_textblock(map* lines, int* lineno, char* terminator, char** outline);
char* http_moved(char* url);
char* http_redirect(char* url, char* msg);
void* http_error(char* msg, char* status);
char* file_mime(char* path);
char* str_html(char* in);
void header(char* str);
char* http_out(char* str, char* status, char* mime, map* headers);
char* static_file(char* path);
char* str_url(char* in);
char* url_str(char* in);
char* map_amps(void* val, char* name);
map* amps_map(char* in);
map* parse_url(char* path);
char* url_host(char* url);
map* sess_init();
void sess_add(char* name, char* value);
char* sess_id();
char* sess_file();
char* sess_newid();
void cookie_set(char* name, char* value, char* path, char* expire);
void sess_delete();
map* link_relative(map* links, char* url);
map* header_map(char* val);
map* http_req();
int ip_connect(char* host, int port, char** err);
int ip_close(int sock);
int ip_send(char* msg, int sock);
char* ip_receive(int sock, char* terminator, int size);
char* remote_ip(int con);
map* sql_tokenizer(char** line);
map* prop_tokenizer(char** line);
map* tokenizer(char** line, char* comment);
char* md_url(char* in, int len, void* junk);
char* file_markdown(char* infile, char* outfile);
char* fox_markdown(char* in);
size_t fox_curl_cat(void* ptr, size_t size, size_t num, void* old);
char* fox_curl(char* url);
char* full_url(char* url);
char* url_abs(char* abs, char* rel);
char* base_url(char* path);
char* home_url(char* path);
char* show_port();
map* lite_trigger_slno(char* name, char* pkey, char* by);
map* lite_trigger_tree(char* name, char* pkey);
char* page_html(map* data);
map* page_data(map* data);
void* not_found(char* path);
int show_page(map* data);
int is_post();
map* post_data();
char* name_tab(char* name);
map* get(char* path, map* names);
void show_body(char* body, map* page);
int days_in_feb(int year);
int days_in_month(int month, int year);
char* add_tab(char* path, char* name);
char* tz_dst(char* tz, char* date);
double tz_offset(char* tz, char* date);
char* tz_utc(char* tz, char* date);
char* offset_utc(double off);
char* date_ymd(char* in, int* year, int* month, int* day);
char* date_human(char* in);
char* tz_human(double tz);
map* tz_data(char* name, char* date);
char* fts5_create(char* db);
void save_caller();
char* url_back();
char* form_html(map* mp);
map* param_map(char* in);
map* form_posted(map* form);
char* ctrl_html(map* ctrl);
char* name_id(char* name);
map* form_gets(map* form);
int show_form(map* data);
int is_user(char* who);
void authorized(char* who);
char* col_align(map* col);
char* rows_data_html(map* in);
map* rows_data(map* rows, map* cols);
char* row_id(map* row, map* cols);
map* row_ids(map* row, map* cols);
void crud(char* sql, char* db);
map* del_keys(map* mp, map* keys);
char* str_bare(char* in, char* accept);
char* word_end(char* in, int len);
