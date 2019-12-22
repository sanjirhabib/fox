#pragma once
#include <sys/mman.h>
#include <stdint.h>
#include <core.h>
#include "gc.h"
#define PAGES 100*1024
#define MAXLEN 20000
struct vobj_meta {
	unsigned char size : 7;
	unsigned char mark : 1;
};
typedef struct vobj {
	struct vobj_meta meta[240];
	union {
		struct {
			unsigned char m241;
			unsigned char used;
			unsigned char from;
			unsigned char scan;
		};
		char page[241][16];
	};
} vobj;
struct pageno_meta {
	unsigned int type : 4;
	unsigned int objs : 1;
	unsigned int mark : 1;
	unsigned int fixed : 1;
	unsigned int size : 25;
};
struct vinfo {
	int using;
	int limit;
	int used;
	int objs_pages;
	int objs_used;
	void** stack;
	int gc_runs;
	int compacts;
	int objfree[16];
};
struct vpage {
	struct pageno_meta meta[PAGES];
	int move[PAGES];
	union {
		struct vinfo info;
		char page[PAGES][4096];
		struct vobj objs[PAGES];
	};
};
void init_gc(void** stack); //gc.fox
void gc_end(); //gc.fox
void gc_reset(); //gc.fox
void* ptr_head(void* ptr); //gc.fox
int size_blocks(size_t size, int block_size); //gc.fox
int objno_realloc(int no, int pageno, int extra); //gc.fox
int pageno_realloc(int no, int extra); //gc.fox
int objno_free(int objno, int pageno); //gc.fox
int pageno_free(int no); //gc.fox
int vobj_alloc(int pageno, int size); //gc.fox
int pages_alloc(int size, int type, int isobjs); //gc.fox
int inc_limit(int blocks); //gc.fox
int gc_alloc(int blocks, int type, int isobj); //gc.fox
char* type_name(int type); //gc.fox
char* pageno_name(int pageno, char* ret); //gc.fox
char* ptr_name(void* ptr, char* ret); //gc.fox
int find_page(int blocks, int type); //gc.fox
void* fox_alloc(size_t size, int type); //gc.fox
int objs_continuous(vobj* objs, int* from, int space); //gc.fox
struct pageno_meta pageno_meta(int no); //gc.fox
int ptr_pageno(void* ptr); //gc.fox
int ptr_objno(void* ptr, int pageno); //gc.fox
int fox_free(void* ptr); //gc.fox
int mem_size(void* ptr); //gc.fox
int is_vobj(void* ptr); //gc.fox
void* fox_realloc(void* ptr, size_t size, int type); //gc.fox
int ptr_type(void* ptr); //gc.fox
int is_vmem(void* ptr); //gc.fox
int pageno_lastpage(int pageno); //gc.fox
int pageno_firstpage(int pageno); //gc.fox
int objno_firstobj(int objno, int pageno); //gc.fox
int vmem_mark(void* ptr); //gc.fox
int fox_gc(); //gc.fox
char* mem_usage(); //gc.fox
void* stack_ptr(); //gc.fox
map* root_ptrs(); //gc.fox
void frag_info(); //gc.fox
int next_free(int from, int type, int objs); //gc.fox
int move_objs(int into, int from); //gc.fox
void defrag_objs(); //gc.fox
int vmem_compact(); //gc.fox
void update_objs_ptrs(); //gc.fox
int defrag_pages(); //gc.fox
void update_page_ptrs(int into); //gc.fox
void rewrite_ptrs(map* roots, int from_page, int upto_page, void(*rewriter)(void** )); //gc.fox
void rewrite_page(void** ptr); //gc.fox
void rewrite_objs(void** ptr); //gc.fox
int vmem_sweep(); //gc.fox
void benchmark_gc(); //gc.fox
void gc_benchmark(); //gc.fox
void page_dump(int no); //gc.fox
void gc_check(); //gc.fox
void check_all(map* ret, int i, int modval); //gc.fox
