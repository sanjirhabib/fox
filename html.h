#include <fox.h>
#pragma once
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
char* /web/fox/ html_version();
void* /web/fox/ html_invoke(map* v, char* name);
map* /web/fox/ html_reflect();
int exec_cmdline(map* args);
