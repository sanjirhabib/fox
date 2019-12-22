#pragma once
#include <core.h>
#pragma fox module map
#include <playground.h>
#include <dir.h>
#include <foxstring.h>
map* xadd(map* mp, ...); //map.fox
map* to_map(void* val); //map.fox
map* del_index(map* mp, int idx); //map.fox
map* map_del_key(map* mp, char* key); //map.fox
map* map_del(map* mp, int idx, int len); //map.fox
map* xjson_map(char* in, int type); //map.fox
map* xjson_map_recurse(char** line, int type); //map.fox
int str_level(char* line); //map.fox
int block_level(char* str); //map.fox
map* xkeys(char* str); //map.fox
map* xindex(char* str); //map.fox
map* xarray(char* str); //map.fox
map* str_vec(void* str); //map.fox
map* str_map(char* str, int type); //map.fox
map* vec_del(map* mp, int from, int len); //map.fox
map* vec_shift(map* mp, int idx, int len, int offset); //map.fox
map* vec_sub(map* mp, int from, int len); //map.fox
map* vec_slice(map* mp, int from, int len); //map.fox
map* map_splice(map* mp, int from, int del, map* ad); //map.fox
void* vec_pop(map* mp); //map.fox
map* vec_splice(map* mp, int from, int del, map* add); //map.fox
map* vec_dup(map* mp); //map.fox
map* map_dup(map* mp); //map.fox
map* map_merge(map* mp1, map* mp2); //map.fox
map* vec_merge(map* mp1, map* mp2); //map.fox
map* map_compact(map* mp); //map.fox
map* vec_reverse(map* mp); //map.fox
map* vec_compact(map* mp); //map.fox
map* change_key(map* mp, int idx, char* id); //map.fox
map* add_id(map* mp, int idx); //map.fox
map* add_key(map* mp, char* k, int type); //map.fox
char* map_join(map* mp, char* joiner); //map.fox
char* map_str(map* mp); //map.fox
char* map_str_indent(map* mp, int indent); //map.fox
map* map_vec(map* mp); //map.fox
map* map_split(map* mp, char* str, int limit); //map.fox
map* map_implode(map* mp, char* joiner); //map.fox
map* file_vec(char* in); //map.fox
map* map_keys(map* mp); //map.fox
map* vec_rdup(map* mp); //map.fox
map* set_map(void* val, map* mp, int idx); //map.fox
char* skip_word(char* in, char* seperators); //map.fox
void* read_value(char** str, char* sep); //map.fox
char* read_upto(char** in, char* terminators); //map.fox
map* map_add_pair(map* mp, void* name, void* value, int type); //map.fox
map* block_map(char** from, int inlevel, int maptype); //map.fox
char* read_num(char** in); //map.fox
char* read_alpha(char** in); //map.fox
static map* add_name_val(map* mp, char* str, char** name, char** val, int type); //map.fox
static char* read_as_block(char** from, int level, char* terminator); //map.fox
static void* marked_str(char* str, char* name); //map.fox
int cmp_ptr_reverse(const void* ptr1, const void* ptr2); //map.fox
int cmp_ptr(const void* ptr1, const void* ptr2); //map.fox
int cmp_cons_reverse(const void* ptr1, const void* ptr2); //map.fox
int cmp_cons(const void* ptr1, const void* ptr2); //map.fox
map* map_sort(map* mp, int reverse); //map.fox
map* vec_map(map* in); //map.fox
map* to_vec(void* val); //map.fox
map* del_keys(map* mp, map* keys); //map.fox
map* merge_soft(map* strong, map* soft); //map.fox
