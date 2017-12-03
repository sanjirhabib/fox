#include <fox.h>
int rand();
void* px(void* str, int newline);
void xexit(int val);
void* fox_error(char* msg, int dump);
int mem_total();
int mem_free();
void fox_signal_handler(int sig);
void stack_dump_direct();
char* stack_str();
void fox_stack_dump();
char* sub_str(char* src, int from, int len);
char* print(char* str, FILE* fp);
char* str_times(char* str, int times);
char* xstr(char* ret, ...);
char* xcat(char* ret, ...);
char* cat(char* str1, char* str2, size_t len);
char* vec_json(map* mp, int indent);
char* json(map* mp, int indent);
char* to_str(void* v, char* null, int human);
size_t blob_size(char* in);
char* new_blob(int size);
char* blob_dup(char* str, int len);
char* str_dup_len(char* str, int len);
char* str_dup(char* str);
char* str_quote(char* head);
char* double_str(double val);
char* int_str(long long value);
int str_eq(char* str, char* str1);
size_t str_hash(unsigned char * str);
map* xvec(void* v1, ...);
map* xmap(char* k1, ...);
char* new_str(int len);
map* new_map();
map* new_vec();
int key_eq(map* mp, int idx, char* id);
int key_hash(char* id);
int map_has_key(map* mp, char* id);
map* vec_realloc(map* mp, int extra);
map* vec_add(map* mp, void* v);
map* set(map* mp, int idx, void* val);
long long is_int(void* v);
double is_double(void* v);
void* int_var(size_t i);
void* double_var(double f);
int has_id(map* mp, int idx);
void* map_idp(map* mp, int idx);
void* map_id(map* mp, int idx);
int map_len(map* mp);
char* map_key(map* mp, int idx);
char* is_str(void* v);
char* is_blob(void* v);
map* is_vec(void* v);
map* is_hash(void* v);
map* is_map(void* v);
double to_double(void* v);
long long to_int(void* v);
int next(map* mp, int idx, char** key, void** val);
int stoi(char* str);
map* globals();
int mem_used(int block_size, int type);
mempage* ptr_page(void* ptr);
void* block_ptr(int block, mempage* pg);
int ptr_block(void* ptr, mempage* pg);
int ptr_type(void* ptr);
int cell2_mark(Mapcell* pairs, int size);
int cell_mark(void** pairs, int size);
int gc_mark(void* ptr);
int sweep_page(mempage* pg);
void* data_delete(void* data, int idx, int size, int len);
void* data_insert(void* data, int idx, int size, int len);
void* data_shift(void* data, int idx, int shift, int size, int len);
int cmp_page(const void* pg1, const void* pg2);
void reindex_pages();
mempage* no_page(int no);
struct mempage* new_page(int block_size, int blocks);
int comp_iptr(const void* sp1, const void* sp2);
mempage* index_free_space(mempage* pg);
void* chain_alloc(mempage* pg, int size, int type, char* ptr);
void* page_alloc(mempage* pg, int size, int type, int* full);
int gc_sweep();
map* root_ptrs();
int fox_gc();
int gc_runs();
int gc_time();
int gc_end();
mempage* free_page(mempage* pg);
int str_len(char* str);
int block_len(int block, mempage* pg);
int mem_size(void* ptr);
void* expand_inplace(char* ptr, char type, int size, int extra);
int size_blocks(size_t size, mempage* pg);
void* fox_realloc(void* ptr, size_t size, int type);
void* fox_alloc(size_t size, int type);
void* new_alloc(size_t size, int type);
void* _xalloc(size_t size, int type);
int block_size(int size);
void start_time();
void end_time();
int run_time();
int total_time();
void time_max();
struct timeval microtime();
int elapsed(struct timeval from);
int map_size(map* mp);
int ceil_pow2(int i);
map* add(map* mp, void* k, void* v);
map* vec_set(map* mp, int idx, void* v);
map* map_add(map* mp, char* key, void* v);
int map_nextno(map* mp);
map* map_index(map* mp, int idx);
map* map_reindex(map* mp);
char* null_str(char* in);
map* ptrs_vec(void** ptrs, int len);
int init_gc(void** sp);
int exec(char* cmd, char** output);
size_t clock_cycles();
void* ptr_head(void* ptr);
int block_head(int no, mempage* pg);
void init_rand();
void* map_val(map* mp, char* key);
map* env_vars();
map* argv_map(char** argv, int argc);