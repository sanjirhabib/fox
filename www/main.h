#include <fox.h>
void save_refere();
char* home_url(char* path);
char* referer();
char* name_id(char* name);
char* ctrl_html(map* ctrl);
char* form_html(map* mp);
int not_found(char* path);
int show_page(map* data);
map* param_map(char* in);
map* form_data(map* form);
int show_form(map* data);
map* page_data(map* data);
char* page_html(map* data);
char* show_port();
char* full_url(char* url);
int is_user(char* who);
void authorized(char* who);