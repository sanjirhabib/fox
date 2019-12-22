#pragma once
#include <core.h>
#pragma fox module tsv
map* tsv_load(char* filename, int key); //tsv.fox
map* str_rows(char* in, int key); //tsv.fox
int type_size(char* type); //tsv.fox
char* map_type(map* mp, char* type); //tsv.fox
map* types(); //tsv.fox
map* subtypes(char* type); //tsv.fox
map* str_cols(char* in); //tsv.fox
char* skip_c_comment(char* in); //tsv.fox
char* skip_c_code(char* in); //tsv.fox
map* str_tokenize(char* in, char* terminators, int max); //tsv.fox
map* toks_trim(map* toks, char* terminators); //tsv.fox
map* toks_recs(map* in, char* terminator); //tsv.fox
char* data_type(char* name, void* val); //tsv.fox
char* month_en3(int val); //tsv.fox
double frac(double val); //tsv.fox
