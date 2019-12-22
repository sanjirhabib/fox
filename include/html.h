#pragma once
#include <core.h>
#include <foxtime.h>
#include <regexp.h>
#include <http.h>
#pragma fox module html
#include <tsv.h>
#pragma fox cc -lfoxdeps
extern char*(*page_html)(map* data);
extern map*(*page_data)(map* data);
extern int _queries;
char* str_show(char* value, char* type, map* op, int width); //html.fox
char* cols_show(map* cols, map* row, char* name, int width); //html.fox
char* datetime_human(char* in); //html.fox
char* thumb_name(char* name); //html.fox
map* row_show(map* row, map* cols, int width); //html.fox
map* rows_show(map* rows, map* cols, int width); //html.fox
char* default_page_html(map* data); //html.fox
map* default_page_data(map* data); //html.fox
char* not_found(char* path); //html.fox
char* show_page(map* data); //html.fox
int is_post(); //html.fox
map* post_data_multipart(); //html.fox
char* name_tab(char* name); //html.fox
map* get(char* path, map* names); //html.fox
char* show_body(char* body, map* page); //html.fox
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
char* form_html(map* mp); //html.fox
map* param_map(char* in); //html.fox
map* post_data(); //html.fox
map* form_posted(map* form); //html.fox
char* ctrl_html(map* ctrl); //html.fox
char* name_id(char* name); //html.fox
map* form_gets(map* form); //html.fox
char* show_form(map* data); //html.fox
int is_user(char* who); //html.fox
char* authorized(char* who); //html.fox
char* str_bare(char* in, char* accept); //html.fox
char* word_end(char* in, int len); //html.fox
char* confirm(char* msg); //html.fox
char* html_text(char* in); //html.fox
char* url_filename(char* url); //html.fox
int init_html(char* name); //html.fox
map* str_list(char* in); //html.fox
map* row_cols(map* row); //html.fox
map* rows_cols(map* rows); //html.fox
char* val_format(char* val, char* type, map* op); //html.fox
char* rows_sum(map* rows, map* cols); //html.fox
char* table_close(char* in, char* tags); //html.fox
char* html(char* in); //html.fox
char* rows_list(map* rows, void* cols, void* callback); //html.fox
char* cols_th(map* cols); //html.fox
char* row_td(void* row, map* cols); //html.fox
map* rows_td(map* rows, map* cols); //html.fox
