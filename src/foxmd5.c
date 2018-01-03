#include <fox.h>

char* c_md5(char* in, size_t initial_len, char* out);
char* md5(char* in){ char ret[33]; return str_dup(c_md5(in,str_len(in),ret)); };
