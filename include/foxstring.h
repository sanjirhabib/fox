#pragma once
#include <core.h>
#pragma fox module foxstring
#include <unicode.h>
#include <map.h>
char* mstr(char* format, ...); //foxstring.fox
char* cat_char(char* str, char c); //foxstring.fox
char* va_str(char* format, va_list args); //foxstring.fox
void* dx(void* data, char* name, int panic); //foxstring.fox
int neq(char* str, char* str1); //foxstring.fox
int eq(char* str, char* str1); //foxstring.fox
char* cwd(); //foxstring.fox
char* str_unquote(char* str); //foxstring.fox
char* str_escape(char* head); //foxstring.fox
char* str_end(char* str, char* end); //foxstring.fox
char* chop_first(char* str, char* substr); //foxstring.fox
char* chop_last(char* str, char* substr); //foxstring.fox
int str_starti(char* str, char* start); //foxstring.fox
int str_start(char* str, char* start); //foxstring.fox
char* str_chr(char* str, char c); //foxstring.fox
int str_chrs(char* str, char* chars); //foxstring.fox
char* str_ltrim(char* str, char* chars); //foxstring.fox
char* str_rtrim(char* str, char* chars); //foxstring.fox
char* str_trim(char* str, char* chars); //foxstring.fox
int line_isempty(char* line); //foxstring.fox
int line_len(char* line); //foxstring.fox
int lchars(char* str, char* chars); //foxstring.fox
int rchars(char* str, char* chars, int off); //foxstring.fox
int has_str(char* str, char* substr); //foxstring.fox
char* str_replace(char* str, void* find, void* replace); //foxstring.fox
char* str_str(char* in, char* sub); //foxstring.fox
char* str_upper(char* s); //foxstring.fox
char* str_lower(char* s); //foxstring.fox
char fox_at(char* str, int idx); //foxstring.fox
char* indent_str(int i); //foxstring.fox
int str_chars(char* str, char* chars); //foxstring.fox
char* end_word(char* str, char* words); //foxstring.fox
char* has_word(char* str, char* words); //foxstring.fox
int is_word(char* word, char* list); //foxstring.fox
int terminate_words(char* in); //foxstring.fox
int is_alphanum(char c, char* others); //foxstring.fox
int is_alpha(char c, char* others); //foxstring.fox
int is_oper(char c); //foxstring.fox
int str_is_oper(char* str); //foxstring.fox
int is_code(char* str); //foxstring.fox
int char_at(char* str, char* terms); //foxstring.fox
int rchar_at(char* str, char* terms); //foxstring.fox
char* char_str(char c); //foxstring.fox
int has_chars(char* line); //foxstring.fox
char* drop_right(char* str, char* w); //foxstring.fox
char* drop_left(char* str, char* w); //foxstring.fox
char* str_toupper(char* str); //foxstring.fox
char* str_tolower(char* str); //foxstring.fox
char* str_title(char* str); //foxstring.fox
char hex_char(char* in); //foxstring.fox
char* rand_str(int len); //foxstring.fox
int str_char_count(char* str, char c); //foxstring.fox
char* str_join(char* str1, char* joiner, char* str2); //foxstring.fox
char* var_bits(void* v); //foxstring.fox
char* rtrim_upto(char* in, char upto, int keep); //foxstring.fox
map* split_by(char* str, char term, int max); //foxstring.fox
char* str_hex(char* in); //foxstring.fox
char* str_tr(char* in, map* replace); //foxstring.fox
char* skip_over(char* in, char* chars); //foxstring.fox
char* skip_quote(char* str); //foxstring.fox
char* skip_paren(char* str); //foxstring.fox
char closing_paren(char c); //foxstring.fox
int is_numeric(char* str); //foxstring.fox
char* next_word(char* in, char* terminators); //foxstring.fox
char* int_kb(size_t i, char* unit); //foxstring.fox
char* int_human(int i, char* unit, char* zero); //foxstring.fox
char* num_lang(char* in, int langid); //foxstring.fox
