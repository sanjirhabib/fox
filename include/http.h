#pragma once
#include <core.h>
#pragma fox module http
void go_back(char* msg); //http.fox
char* http_moved(char* url); //http.fox
char* http_redirect(char* url, char* msg); //http.fox
char* http_error(char* msg, char* status); //http.fox
char* file_mime(char* path); //http.fox
char* header(char* str); //http.fox
char* http_out(char* str, char* status, char* mime, map* headers); //http.fox
char* static_file(char* path); //http.fox
map* sess_init(); //http.fox
void sess_add(char* name, char* value); //http.fox
char* sess_id(); //http.fox
char* sess_file(); //http.fox
char* sess_newid(); //http.fox
void cookie_set(char* name, char* value, char* path, char* expire); //http.fox
void sess_delete(); //http.fox
map* link_relative(map* links, char* url); //http.fox
map* header_map(char* val); //http.fox
map* http_req(char* home); //http.fox
char* full_url(char* url); //http.fox
char* url_abs(char* abs, char* rel); //http.fox
char* base_url(char* path); //http.fox
char* home_url(char* path); //http.fox
char* show_port(); //http.fox
char* h(char* in); //http.fox
char* str_html(char* in); //http.fox
char* str_url(char* in); //http.fox
char* url_str(char* in); //http.fox
char* map_amps(void* val, char* name); //http.fox
map* amps_map(char* in); //http.fox
map* parse_url(char* path); //http.fox
char* param(map* change); //http.fox
void save_caller(); //http.fox
char* url_back(); //http.fox
