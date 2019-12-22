#pragma once
#include <core.h>
#pragma fox module foxparser
char* fox_read_oper(char** in, char term); //foxparser.fox
char* read_upto_word(char** in, char* upto); //foxparser.fox
char* read_theline(char** in); //foxparser.fox
char* read_newline(char** in); //foxparser.fox
char* read_space(char** in, char* spaces); //foxparser.fox
map* read_data(char** in); //foxparser.fox
char* read_heredoc(char** in); //foxparser.fox
char* read_multistr(char** in); //foxparser.fox
char* read_quote(char** in); //foxparser.fox
char* fox_read_symbol(char** in); //foxparser.fox
map* data_toks(char* in); //foxparser.fox
map* data_tokenizer(char** in, int level); //foxparser.fox
char* read_over(char** in, char* chars); //foxparser.fox
map* prop_vec_toks(char* in); //foxparser.fox
map* prop_toks(char* in, map* name); //foxparser.fox
map* read_paren(map* mp, char** line, map*(*func)(char** )); //foxparser.fox
map* data_quote(char* in); //foxparser.fox
map* c_tokenizer(char** line, char term); //foxparser.fox
static map* add_ctok(void* data, map* mp, int iscode); //foxparser.fox
map* x_map(char* in); //foxparser.fox
int map_has_word(map* mp, char* str); //foxparser.fox
map* toks_split(map* in, char* by, int limit); //foxparser.fox
map* toks_align(map* in); //foxparser.fox
