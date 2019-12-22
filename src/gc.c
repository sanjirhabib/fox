#line 2 "src/gc.fox"
#include <sys/mman.h>
#include <stdint.h>
#include <core.h>
#include "gc.h"
#define PAGES 100*1024
#define MAXLEN 20000

char tempbuff[128];
char tempbuff2[128];
int _verbose=0;

//struct vobj_meta {
//	unsigned char size : 7;
//	unsigned char mark : 1;
//};
//typedef struct vobj {
//	struct vobj_meta meta[240];
//	union {
//		struct {
//			unsigned char m241;
//			unsigned char used;
//			unsigned char from;
//			unsigned char scan;
//		};
//		char page[241][16];
//	};
//} vobj;
//struct pageno_meta {
//	unsigned int type : 4;
//	unsigned int objs : 1;
//	unsigned int mark : 1;
//	unsigned int fixed : 1;
//	unsigned int size : 25;
//};
//struct vinfo {
//	int using;
//	int limit;
//	int used;
//	int objs_pages;
//	int objs_used;
//	void** stack;
//	int gc_runs;
//	int compacts;
//	int objfree[16];
//};
//struct vpage {
//	struct pageno_meta meta[PAGES];
//	int move[PAGES];
//	union {
//		struct vinfo info;
//		char page[PAGES][4096];
//		struct vobj objs[PAGES];
//	};
//};
struct vpage* pages=NULL;
void init_gc(void** stack){
	assert(sizeof(char*)==sizeof(void*));
	assert(sizeof(struct vobj)==4096);
	assert(sizeof(struct pageno_meta)==4);
	assert(sizeof(struct vinfo)<=4096);
	if(pages){ return; };
	pages=mmap(0, sizeof(struct vpage), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//	pages=malloc(sizeof(struct vpage))
	if(!pages){ //pages==MAP_FAILED
		printf("Memory initialization failed.\n");
		void* pages=NULL;
		exit(-1); };
	if(_verbose){
		printf("page0=%lx\n",pages->page[0]); };
	assert(!((uintptr_t)pages & 0xFFF));
	assert(!((uintptr_t)pages->page[0] & 0xFFF));
	pages->info.limit=16;
	pages->info.stack=stack;
	pages->info.using=1;
	pages->info.used=1;
};
void gc_end(){
	if(!pages){ return; };
	if(_verbose){ printf("pages at: %lx\n",pages); };
//	free(pages)
	int ret=munmap(pages,sizeof(struct vpage));
	if(ret){
		printf("munmap() faild\n"); };
	void* pages=NULL;
};
void gc_reset(){
	if(!pages){ return; };
	if(_verbose){
		printf("resetting GC\n"); };
	void** stack=pages->info.stack;
	map args={0};
	map* temp=map_val(_globals,"args");
	args.vars=temp->vars;
	args.len=temp->len;
	memset(pages->meta,0,sizeof(pages->meta[0])*pages->info.using);
	memset(pages->page[0],0,4096*pages->info.using);
	pages->info.stack=stack;
	pages->info.limit=16;
	pages->info.using=1;
	pages->info.used=1;
	_globals=new_map();
	temp=new_vec();
	temp->vars=args.vars;
	temp->len=args.len;
	add(_globals,"args",temp);
};
void* ptr_head(void* ptr){
	if(!is_vmem(ptr)){ return ptr; };
	int pageno=ptr_pageno(ptr);
	if(is_vobj(ptr)){
		return pages->objs[pageno].page[objno_firstobj(ptr_objno(ptr,pageno),pageno)]; };
	return pages->page[pageno_firstpage(pageno)];
};
int size_blocks(size_t size,int block_size){ return size/block_size+(size % block_size ? 1 : 0); };
int objno_realloc(int no, int pageno, int extra){
	assert(extra>0 && extra<=240);
	vobj* objs=pages->objs+pageno;
	int len=objs->meta[no].size;
	assert(len+extra<128);
	if(no+len+extra>=240){ return 0; };
	int from=no+len;
	for(int i=0; i<extra; i++){
		if(objs->meta[from+i].size){
			return 0; }; };
	for(int i=0; i<len+extra; i++){
		objs->meta[no+i].size=len+extra-i; };
	objs->used+=extra;
	if(from==objs->from){
		objs->from+=extra; };
	return no;
};
int pageno_realloc(int no, int extra){
	assert(extra>0 && extra<MAXLEN);
	assert(no==pageno_firstpage(no));
	int len=pages->meta[no].size;
	assert(len+extra<MAXLEN);
	if(no+len+extra>=pages->info.limit+1){ return 0; };
	int from=no+len;
	for(int i=0; i<extra; i++){
		if(pages->meta[from+i].size){
			return 0; }; };
	int type=pages->meta[no].type;
	for(int i=0; i<len+extra; i++){
		pages->meta[no+i].size=len+extra-i;
		pages->meta[no+i].type=type;
		assert(!pages->meta[no+i].objs); };
	pages->info.used+=extra;
	pages->info.using=max(pages->info.using,no+len+extra);
//	if _verbose => "realloc %d+%d=%s\n".printf(len, extra, no.pageno_name(tempbuff))
	assert(pages->meta[no].size==len+extra);
	assert(pageno_firstpage((no+len+extra-1))==no);
	return no;
};
int objno_free(int objno, int pageno){
	assert(objno>0 && objno<=240);
	vobj* objs=pages->objs+pageno;
//	"objno_free() %s\n".printf(objs->page[objno].ptr_name(tempbuff))
	int len=objs->meta[objno].size;
	struct vobj_meta blank={0};
	for(int i=0; i<len; i++){
		objs->meta[objno+i]=blank; };
	assert(!pages->objs[pageno].meta[objno].size);
	memset(objs->page[objno],0,len*16);
	objs->used-=len;
	while(objs->from>1 && !objs->meta[objs->from-1].size){
		objs->from--; };	
	assert(objs->used || objs->from<=1);
	return len*17;
};
int pageno_free(int no){
	assert(no>0 && no<PAGES);
	assert(pages->meta[no].size);
	assert(no==pageno_firstpage(no));
	int len=pages->meta[no].size;
//	if _verbose => "freed %s\n".printf(no.pageno_name(tempbuff))
	struct pageno_meta blank={0};
	for(int i=0; i<len; i++){
		pages->meta[no+i]=blank; };
	memset(pages->page[no],0,len*4096);
	pages->info.used-=len;
	return len*4096;
};
int vobj_alloc(int pageno, int size){
	assert(size>0 && size<128);
	vobj* objs=pages->objs+pageno;
	if(!(objs->from)){objs->from=1;};
	assert(objs->from+size<=241);
//	"vobj_alloc() %s from:%d-%d\n".printf(pageno.pageno_name(tempbuff),objs->from,objs->from+size)
	int from=objs->from;
	for(int j=1; j<=size; j++){
		objs->meta[from+size-j].size=j;
		assert(!objs->meta[from+size-j].mark); };
	objs->used+=size;
	objs->from+=size;
	return from;
	printf("vobjalloc failed: page:%d, size:%d, used:%d\n",pageno,size,objs->used);
	return 0;
};
int pages_alloc(int size,int type,int isobjs){
	if(pages->info.using+size>pages->info.limit){
		return 0; };
	int from=pages->info.using;
	pages->info.using+=size;
	pages->info.used+=size;
	for(int i=0; i<size; i++){
		pages->meta[from+i].size=size-i;
		pages->meta[from+i].type=type;
		pages->meta[from+i].objs=isobjs; };
	return from;
};
int inc_limit(int blocks){
	int old=pages->info.limit;
	if(_verbose){ frag_info(); };
	pages->info.limit=min(pages->info.limit+blocks, PAGES-1);
	if(_verbose){ printf("Mem increased: %d to %d KB\n",old*4, pages->info.limit*4); };
	return pages->info.limit;
};
int gc_alloc(int blocks, int type,int isobj){
	if(blocks>=pages->info.limit+1){
		//if _verbose
		printf("out of limit %d blocks %s requested\n",blocks, isobj ? "objs" : "4k");
		inc_limit(blocks); };	
	int ret=pages_alloc(blocks,type,isobj);
	if(ret){ return ret; };
	ret=fox_gc();
	int percent=(int)(100.*ret/(4096*pages->info.using));
	if(percent>20){
		ret=pages_alloc(blocks,type,isobj);
		if(ret){ return ret; };
	};
	if(_verbose){
		printf("gc full for %d pages of %s\n",blocks,type_name(type)); };
	inc_limit((blocks*1.4+pages->info.limit*0.4));
	ret=pages_alloc(blocks,type,isobj);
	if(ret){ return ret; };
	printf("gc_alloc() failed for %d blocks, of %s/%s : using:%d, used:%d, limit: %d\n",blocks, type_name(type), isobj ? "objs" : "page", pages->info.using, pages->info.used, pages->info.limit);
	exit(-1);
};
char* type_name(int type){
	char* names[]={"Free","Int","Double","String","Blob","Map","Vector","Index","Keys","Cell","Cell2"};
	return names[type];
};
char* pageno_name(int pageno, char* ret){
	int type=pages->meta[pageno].type;
	sprintf(ret,"%s(%d,%s,%d)",type_name(type), pageno, pages->meta[pageno].objs ? "objs" : "4k", pages->meta[pageno].size);
	return ret;
};
char* ptr_name(void* ptr,char* ret){
	if(!is_vmem(ptr)){
		sprintf(ret,"0x%lx",ptr);
		return ret; };
	int type=ptr_type(ptr);
	int pageno=ptr_pageno(ptr);
	if(is_vobj(ptr)){
		int objno=ptr_objno(ptr,pageno);
		sprintf(ret,"%s(%d,%dobj,%d)",type_name(type), pageno,objno,pages->meta[pageno].size);
		return ret; };
	return pageno_name(pageno,ret);
};
int find_page(int blocks, int type){
	int pageno=pages->info.objfree[type];
	if(!pageno){
		return gc_alloc(4,type,1); };
	if(pages->objs[pageno].from+blocks<=241){
		return pageno; };
	if(pages->meta[pageno].size>1){ //==1 the last page in chain
		assert(!pages->objs[pageno+1].from);
		assert(!pages->objs[pageno+1].used);
		return pageno+1; };
	return gc_alloc(16,type,1);
};
void* fox_alloc(size_t size, int type){
	if(size<=0){ return NULL; };
	int blocks=size_blocks(size,16);
	if(blocks>=127){ return pages->page[gc_alloc(size_blocks(size,4096),type,0)]; };
	int pageno=find_page(blocks,type);
	pages->info.objfree[type]=pageno;
	void* ret=pages->objs[pageno].page[vobj_alloc(pageno,blocks)];
	assert(ptr_type(ret)==type);
	return ret;
};
int objs_continuous(vobj* objs, int* from,int space){
	while(*from<objs->from && !objs->meta[*from].size){ (*from)++; };
	int ret=0;
	while(*from+ret<objs->from && objs->meta[*from+ret].size && objs->meta[*from+ret].size+ret<space){
		ret+=objs->meta[*from+ret].size; };
	return ret;		
};
struct pageno_meta pageno_meta(int no){ return pages->meta[no]; };
int ptr_pageno(void* ptr){ return ((char*)ptr-(char*)pages->page[0])/4096; };
int ptr_objno(void* ptr, int pageno){
	int ret=((char*)ptr-(char*)pages->page[pageno]-sizeof(pages->objs[0].meta))/16;
	assert(ret<241);
	return ret>0 ? ret : 0;
};
int fox_free(void* ptr){
	int pageno=ptr_pageno(ptr);
	if(is_vobj(ptr)){ return objno_free(ptr_objno(ptr,pageno),pageno); };
	return pageno_free(pageno);
};
int mem_size(void* ptr){
	int pageno=ptr_pageno(ptr);
	struct pageno_meta meta=pageno_meta(pageno);
	if(meta.objs){
		return pages->objs[pageno].meta[ptr_objno(ptr,pageno)].size*16; };
	return pages->meta[pageno].size*4096;
};
int is_vobj(void* ptr){ return pages->meta[ptr_pageno(ptr)].objs; };
void* fox_realloc(void* ptr, size_t size, int type){
	if(!is_vmem(ptr)){
		void* ret=fox_alloc(size,type);
		if(ptr && type==String){
			assert(str_len(ptr)<=size-1);
			strcpy(ret,ptr); };
		return ret; };
	assert(ptr_type(ptr)==type);
	void* head=ptr_head(ptr);
	int headlen=mem_size(head);
	int prelen=ptr-head;
	int len=headlen-prelen;
	if(len>=size){ return ptr; };
	int newlen=prelen+size;
	size_t extra=size-len;
	if(is_vobj(ptr) && (headlen+size_blocks(extra,16)*16)<2048){
		int pageno=ptr_pageno(head);
		int ret=objno_realloc(ptr_objno(head,pageno),pageno, size_blocks(extra,16));
		if(ret){ return ptr; }; };
	if(!is_vobj(ptr)){
		int ret=pageno_realloc(ptr_pageno(ptr),size_blocks(extra,4096));
		if(ret){ return ptr; }; };
	//TODO: tell alloc() that we need extra space after this ptr if gc is ran
	void* ret=fox_alloc(newlen,type);
	//"ptr moved from %s to %s\n".printf(temp, head.ptr_name(tempbuff))
	assert(headlen==mem_size(head));
	memcpy(ret,head, headlen);
	fox_free(head);
	return ret+prelen;
};
int ptr_type(void* ptr){
	if(is_vmem(ptr)){ return pages->meta[ptr_pageno(ptr)].type; };
	if(!ptr){ return Null; };
	if(is_i(ptr)){ return Int; };
	if(is_f(ptr)){ return Double; };
	return String; 
};
int is_vmem(void* ptr){ return (char*)ptr>=pages->page[1] && (char*)ptr<pages->page[PAGES+1]; };
int pageno_lastpage(int pageno){ return pageno+pages->meta[pageno].size-1; };
int pageno_firstpage(int pageno){
	while(pageno>1 && pages->meta[pageno-1].size>1){
		pageno--; };
	return pageno;
};
int objno_firstobj(int objno, int pageno){
	vobj* objs=pages->objs+pageno;
	while(objno>1 && objs->meta[objno-1].size==objs->meta[objno].size+1){
		objno--; };
	return objno;
};
int vmem_mark(void* ptr){
	if(!is_vmem(ptr)){
		return 0; };
	int pageno=ptr_pageno(ptr);
	if(!pages->meta[pageno].size){ return; };
	int type=0;
	int size=0;
	int ret=0;
	if(pages->meta[pageno].objs){
		int objno=ptr_objno(ptr,pageno);
		if(!objno){ return 0; };
		vobj* objs=pages->objs+pageno;
		if(!objs->meta[objno].size){
			assert(!objs->meta[objno].mark);
			return 0; };
		int regno=objno+objs->meta[objno].size-1;
		if(objs->meta[regno].mark){
			return ret; };
		if(!objs->meta[regno].size){
			assert(0); //unallocated mem in ptr
			return ret; };
//		pages->meta[pageno+pages->meta[pageno].size-1].mark=1
		objs->meta[regno].mark=1;
		assert(objs->meta[regno].size);
		ret++;
		int headno=objno_firstobj(objno,pageno);
		ptr=objs->page[headno];
		type=pages->meta[pageno].type;
		size=objs->meta[headno].size*16;
	}else{
		int regno=pageno+pages->meta[pageno].size-1;
		if(pages->meta[regno].mark){ return ret; };
		pages->meta[regno].mark=1;
		assert(!pages->meta[regno].objs);
		ret++;
		type=pages->meta[regno].type;
		int headno=pageno_firstpage(pageno);
		ptr=pages->page[headno];
		size=pages->meta[headno].size*4096; };
	if(type==Cell){
		void** pairs=(void**) ptr;
		size/=sizeof(void*);
		for(int i=0;i<size;i++){
			ret+=vmem_mark(pairs[i]); };
	}else if(type==Cell2){
		Mapcell* pairs=(Mapcell*)ptr;
		size/=sizeof(Mapcell);
		for(int i=0;i<size;i++){
			if(!is_num(pairs[i].id)){ ret+=vmem_mark(pairs[i].id); };
			ret+=vmem_mark(pairs[i].val); };
	}else if(type==Map){
		ret+=vmem_mark(((map*)ptr)->pairs);
	}else if(type==Vector){
		ret+=vmem_mark(((map*)ptr)->vars);
	}else if(type==Index||type==Keys){ assert(0); };
	return ret;
};
int fox_gc(){
	if(_verbose){ printf("GC..."); };
	int ret=0;
	jmp_buf regs={0};
	map* roots=root_ptrs();
//	void* myreg[8]={0}
//	myreg.read_register()
//	check_objmark(0)
	if(!setjmp(regs)){
		for(int i=0; i<roots->len; i++){
			ret+=vmem_mark(roots->vars[i]); };
		ret=vmem_mark(_globals);
		longjmp(regs,1); };
//	myreg.update_register()
//	check_objmark(1)
	ret=vmem_sweep();
//	check_objmark(0)
	vmem_compact();
//	check_objmark(0)
	pages->info.gc_runs++;
	int percent=pages->info.using ? ret*100./(pages->info.using*4096) : 0;
	if(_verbose){ printf(" %d%\n",9); };
	assert(percent>=0);
	return ret;
};
//void check_objmark(int what){
//	for int i=1; i<pages->info.using; i++
//		if what
//			assert(!pages->meta[i].objs || !pages->meta[i].mark)
//		else
//			assert(!pages->meta[i].mark)
//}
char* mem_usage(){
	int kb=pages->info.using*4;
	char* unit="kb";
	if(kb>2048){ kb/=1024; unit="mb"; };
	frag_info();
	return xstr("", 
	"Pages: ",int_str( (int)(pages->info.using)), ". ",int_str( kb), " ", unit, ". GC Runs: ",int_str( (int)pages->info.gc_runs), ". Compacts: ",int_str( (int)pages->info.compacts), ".\n", 
	"", 
	"", End);
};
void* stack_ptr(){
	void* sp;
	asm("mov %%rsp, %0" : "=r" ( sp ));
	return sp;
};
map* root_ptrs(){
	static map ret={0};
	void* ptr=stack_ptr();
	int size=((void*)(pages->info.stack)-ptr)/sizeof(void*);
	ret.vars=size<0 ? pages->info.stack : ptr;
	ret.len=abs(size)+1;
	ret.type=Vector;
	return &ret;
};
void frag_info(){
	int total=0;
	int used=0;
	for(int i=1; i<pages->info.using; i++){
		if(pages->meta[i].objs){
			total++;
			used+=pages->objs[i].used;
		}else if(pages->meta[i].size>1){
			i+=pages->meta[i].size-1; }; };
	int objsfree=total-used/240;
	int pagefree=pages->info.using-pages->info.used;
	int totalfree=objsfree+pagefree;
	int percent=pages->info.using ? 100*totalfree/pages->info.using : 100;
	int percent2=total ? objsfree*100/total : 100;
	printf("Fragment: %d+%d=%d free from %d used, %d%% of used, %d%% of objs\n",
		pagefree, objsfree, totalfree,
		pages->info.using, percent, percent2);
};
int next_free(int from, int type, int objs){
	if(!(from)){from=1;};
	while(pages->meta[from].size){
		from+=pages->meta[from].size; };
	assert(from<=pages->info.using);
	if(pages->meta[from-1].type==type && pages->meta[from-1].objs==objs && !pages->meta[from-1].mark){
		int curr=pages->info.objfree[type];
		if(curr){ curr+=pages->meta[curr].size-1; };
		if(from-1!=curr){
			int first=pageno_firstpage((from-1));
			if(from-first<16){
				for(int i=first; i<from; i++){
					pages->meta[i].size++; }; }; }; };
	pages->meta[from].size=1;
	pages->meta[from].fixed=1;
	pages->meta[from].type=type;
	pages->meta[from].objs=objs;
	pages->info.used++;
	pages->info.using=max(pages->info.using,from+1);
	assert(pages->info.using<=PAGES);
	return from;
};
int move_objs(int into, int from){
	if(!pages->meta[from].mark){
		for(int i=from; i<from+pages->meta[from].size; i++){
			pages->meta[i].mark=1; }; };
	if(!into){ return 0; };
	vobj* objs=pages->objs+from;
	vobj* objs2=pages->objs+into;
	if(!(objs->scan)){objs->scan=1;};
	if(!(objs2->from)){objs2->from=1;};
	while(objs->scan<241){
		int size=objs->meta[objs->scan].size;
		if(!size){
			objs->scan++;
			continue; };
		int from=objs2->from;
		if(size+from>241){
			return 0; };
		memcpy(objs2->page[from], objs->page[objs->scan], size*16);
		memcpy(objs2->meta+from, objs->meta+objs->scan, size);
		for(int j=0; j<size; j++){
			*(char**)objs->page[objs->scan+j]=objs2->page[from+j]; };
		objs2->from+=size;
		objs2->used+=size;
		objs->scan+=size; };
	memset(pages->page[from],0,256);
	return 1;
};
void defrag_objs(){
	if(_verbose){
		frag_info(); };
	int typepage[16]={0};
	for(int i=1; i<pages->info.using; i++){

		int len=pages->meta[i].size;

		if(!len){
			continue;
		};
		if(!pages->meta[i].objs){
			i+=len-1;
			continue;
		};
		if(pages->meta[i].fixed){
			pages->meta[i].fixed=0;
			i+=len-1;
			continue;
		};
		int type=pages->meta[i].type;

		// current live page
		if(pages->info.objfree[type]>=i && pages->info.objfree[type]<i+len){
			i+=len-1;
			continue;
		};
		int used=0;
		for(int j=0; j<len; j++){
			used+=pages->objs[i+j].used;
		};

		// empty pages
		if(!used){
			pageno_free(i);
			i+=len-1;
			continue;
		};
		//more than half filled
		if(used>240*len*0.6){
			i+=len-1;
			continue;
		};
		int into=typepage[type];
		for(int j=0; j<len; j++){
			while(!move_objs(into,i+j)){
				into=next_free(into,type, 1); }; };
		typepage[type]=into;
		i+=len-1; };
	for(int i=1; i<pages->info.using; i++){
		pages->meta[i].fixed=0;	
	};};
int vmem_compact(){
	defrag_objs();	
	update_objs_ptrs();
	int ret=defrag_pages();
	update_page_ptrs(ret);
};
void update_objs_ptrs(){
	jmp_buf regs={0};
	if(!setjmp(regs)){
		rewrite_ptrs(root_ptrs(),1, pages->info.using, rewrite_objs);
		longjmp(regs,1); };
	for(int i=1; i<pages->info.using; i++){
		int size=pages->meta[i].size;
		if(!size){ continue; };
		if(pages->meta[i].mark){
			pageno_free(i); };
		i+=size-1;
		continue;
	};};
int defrag_pages(){
	if(!pages->info.using){ return 0; };
	int frags=(pages->info.using-pages->info.used)*100./pages->info.using;
	// don't collect unless 20 is fragmented.
	if(frags<20){
		if(_verbose){
			printf("frags=%d%%, not compacting\n",frags); };
		return 0; };
	if(_verbose){
		printf("frags=%d>20%%, compacting\n",frags); };
	int into=1;
	for(int i=1; i<pages->info.using; i++){
		if(!pages->meta[i].size){ continue; };
		int from=i;
		int len=pages->meta[from].size;
		if(into==i){
			into+=len;
			i+=len-1;
			continue; };
		memmove(pages->page[into], pages->page[from], len*4096);
		memmove(pages->meta+into, pages->meta+from, len*sizeof(struct pageno_meta));
		for(int j=0; j<len; j++){
			pages->move[from+j]=into+j; };
		into+=len;
		i+=len-1; };
	assert(into<=pages->info.using);
	return into;
};
void update_page_ptrs(int into){
	if(!into){ return; };
	jmp_buf regs={0};
	if(!setjmp(regs)){
		rewrite_ptrs(root_ptrs(),1,pages->info.using, rewrite_page);
		longjmp(regs,1); };
	if(_verbose){ printf("blocks compacted from %d to %d=%d saved\n",pages->info.using, into, pages->info.using-into); };
	int gcupto=pages->info.using;
	pages->info.using=into;
	int gcfrom=into;
	for(int type=0; type<16; type++){
		int pageno=pages->info.objfree[type];
		if(!pageno){continue; };
		if(!pages->move[pageno]){ continue; };
		assert(pages->meta[pages->move[pageno]].type==type);
		pages->info.objfree[type]=pages->move[pageno]; };
	memset(pages->move,0,gcupto*sizeof(int));
	memset(pages->meta+gcfrom,0,(gcupto-gcfrom)*sizeof(int));
	memset(pages->page[gcfrom],0,(gcupto-gcfrom)*4096);
	for(int j=0; j<PAGES; j++){
		assert(!pages->move[j]);
		assert(!pages->meta[j].fixed); };
	if(_verbose){
		printf("after compact ");
		frag_info();
	};};
void rewrite_ptrs(map* roots, int from_page, int upto_page, void(*rewriter)(void**)){
	void* from=pages->page[from_page];
	void* upto=pages->page[upto_page];
	for(int i=1; i<pages->info.using; i++){
		if(!pages->meta[i].size){ continue; };
		int plen=pages->meta[i].size;
		int regno=i+plen-1;
		int type=pages->meta[regno].type;
		if(type!=Cell2 && type!=Cell && type!=Map && type!=Vector){
			continue; };
		if(pages->meta[regno].objs){
			vobj* objs=pages->objs+i;
			for(int j=1; j<241; j++){
				if(!objs->meta[j].size){
					continue; };
				int len=(objs->meta[j].size*16)/sizeof(void*);
				void** ptr=objs->page[j];
				for(int k=0; k<len; k++){
					if(type==Cell2 && !(k%3)){
						continue; };
					if((type==Map || type==Vector) && !(k%2)){
						continue; };
					if(ptr[k]>=from && ptr[k]<upto){
						rewriter((void**)objs->page[j]+k); }; };
				j+=objs->meta[j].size-1; };
			continue; };
		void** ptr=pages->page[i];
		plen=plen*4096/sizeof(void*);
		for(int k=0; k<plen; k++){
			if(type==Cell2 && !(k%3)){
				continue; };
			if((type==Map || type==Vector) && !(k%2)){
				continue; };
			if(ptr[k]>=from && ptr[k]<upto){
				rewriter((void**)pages->page[i]+k); }; };
		i=regno; };
	for(int i=0; i<roots->len; i++){
		void* ptr=roots->vars[i];
		if(ptr>=from && ptr<upto){
			rewriter((void**)roots->vars+i); }; };
	if((void*)_globals>=from && (void*)_globals<upto){
		rewriter(&_globals);
	};
};
void rewrite_page(void** ptr){
//	void* oldval=*ptr
	int page2=pages->move[(*ptr-(void*)pages->page[0])/4096];
	if(!page2){ return; };
	*ptr=pages->page[0]+page2*4096+((uintptr_t)*ptr & 0xFFF);
};
void rewrite_objs(void** ptr){
//	void* oldval=*ptr
	if(!pages->meta[((char*)*ptr-(char*)pages->page[0]) >> 12].mark){ return; };
	void* newptr=*(void**)((uintptr_t)*ptr & ~0xF);
	if(!newptr){ return; };
	*ptr=newptr+((uintptr_t)*ptr & 0xF);
	assert(*ptr);
};
int vmem_sweep(){
	static int runs=0;
	runs++;
	int ret=0;
	pages->info.objs_pages=0;
	pages->info.objs_used=0;
	for(int i=1; i<pages->info.using;i++){
		if(!pages->meta[i].size){ continue; };
		int regno=i+pages->meta[i].size-1;
		if(pages->meta[regno].objs){
			vobj* objs=pages->objs+i;
			for(int j=1; j<241;){
				if(!objs->meta[j].size){
					assert(!objs->meta[j].mark);
					j++;
					continue; };
				int oregno=j+objs->meta[j].size-1;
				if(!objs->meta[oregno].mark){
					ret+=objno_free(j,i); };
				objs->meta[oregno].mark=0;
				j=oregno+1; };
			pages->info.objs_pages++;
			pages->info.objs_used+=objs->used;
			continue; };
		if(!pages->meta[regno].mark){
			ret+=pageno_free(i);
		}else{
			pages->meta[regno].mark=0; };
		i=regno; };
	assert(ret>=0);
	return ret;
};
void benchmark_gc(){
	map* ret=new_vec();
	for(int i=0;i<1000000;i++){
		set(ret,i%200000,new_str(1023)); };
	dx(int_var(map_len(ret)),NULL,0);
	dx(mem_usage(),NULL,0);
};
void gc_benchmark(){
	map* ret=new_map();
	for(int i=0;i<5000000;i++){
		add(ret,xstr("id.",int_str((i%500000)), End),xstr("val.",int_str(i), End)); };
	if(map_len(ret)==500000){
		px("OK",1);
	}else{
		px("ERROR",1); };
	px(int_var(map_len(ret)),1);
	px(mem_usage(),1);
};
void page_dump(int no){
	if(!pages->meta[no].objs){
		printf("Not a objs page!\n");
		return; };
	printf("no: %d, size: %d, isobjs: %d, type:%s, marked:%d, fixed: %d\n",
		no,
		pages->meta[no].size,
		pages->meta[no].objs,
		type_name(pages->meta[no].type),
		pages->meta[no].mark,
		pages->meta[no].fixed
	);
	int type=pages->meta[no].type;
	vobj* objs=pages->objs+no;
	printf("m241: %d, using: %d, from: %d, scan:%d\n",
		objs->m241,
		objs->used,
		objs->from,
		objs->scan
	);
	for(int i=0; i<241; i++){
		printf("obj#%d, size:%d, mark:%d\n",
			i,
			objs->meta[i].size,
			objs->meta[i].mark
		); };
	for(int i=1; i<241; i++){
		printf("#%d: ",i);
		for(int j=0; j<16; j++){
			unsigned char c=objs->page[i][j];
			if(c>=' ' && c<='~'){ printf("%c",c); }
			else {printf("\\%d",c);};
			if(type==Blob) {}; };
		printf(" size:%ld",*(size_t*)(objs->page[i]));
		printf("\n");
	};};
void gc_check(){
	_verbose=1;
	gc_reset();
	map* ret=new_map();
	char* val=NULL;
	char* id=NULL;
	int no=0;
	int modval=500000;
	int gcruns=0;
	for(int i=0;i<5000000;i++){
		if(!(i%100000)){ printf("%d\n",i,modval); };
		no=i%modval;
		id=xstr("id.",int_str(no), End);
//		"adding: %s\n".printf(id)
		val=xstr("val.",int_str(i), End);
		add(ret,id,val);
		if(gcruns!=pages->info.gc_runs){
			gcruns=pages->info.gc_runs;
			check_all(ret,i,modval); };
		if(!str_eq(map_key(ret,0),"id.0") || map_len(ret)>modval){
			page_dump(ptr_pageno(map_key(ret,no)));
			printf("Crash!\n");
			printf("i=%d\n",i);
			printf("len=%d\n",ret->len);
			printf("key=%s\n",map_key(ret,no));
			printf("keylen=%s\n",str_len(map_key(ret,no)));

			printf("val=%s\n",map_id(ret,no));
			printf("key=%s\n",ptr_name(map_key(ret,no),tempbuff));
			printf("val=%s\n",ptr_name(map_id(ret,no),tempbuff));
			printf("vars=%s\n",ptr_name(ret->vars,tempbuff));
			printf("vars/page=%lx\n",ret->pairs+no);
			printf("vars/page=%s\n",ptr_name((ret->pairs+no),tempbuff));
			printf("\n");
			assert(0);
			exit(-1); };
	};
	if(map_len(ret)==modval){
		px("OK",1);
	}else{
		printf("ERROR: map_len()=%d\n",map_len(ret)); };
	px(int_var(map_len(ret)),1);
//	ret.each v,n
//		"%s\t%s\n".printf(n,v)
	fox_gc();
	px(mem_usage(),1);
};
void check_all(map* ret, int i, int modval){
	char thekey[128];
	int mods=i%modval;
	int maxval=mods;
	if(i>modval){ maxval=modval-1; };
	for(i=0; i<=maxval; i++){
		sprintf(thekey,"id.%d",i);
		if(!str_eq(map_key(ret,i),thekey)){
			page_dump(ptr_pageno(map_key(ret,i)));
			printf("%s!=%s\n",map_key(ret,i),thekey);
			printf("i=%d\n",i);
			printf("len=%d\n",ret->len);
			printf("key=%s\n",map_key(ret,i));
			printf("key=%s\n",ptr_name(map_key(ret,i),tempbuff));
			printf("vars=%s\n",ptr_name(ret->vars,tempbuff));
			printf("keylen=%s\n",str_len(map_key(ret,i)));
			assert(str_eq(map_key(ret,i),thekey)); }; };
//	"%d ok\n".printf(i)
};
//void update_register(void** regs){
//	asm("movq %0, %%r8" : : "r"(regs[0]))
//	asm("movq %0, %%r9" : : "r"(regs[1]))
//	asm("movq %0, %%r10" : : "r"(regs[2]))
//	asm("movq %0, %%r11" : : "r"(regs[3]))
//	asm("movq %0, %%r12" : : "r"(regs[4]))
//	asm("movq %0, %%r13" : : "r"(regs[5]))
//	asm("movq %0, %%r14" : : "r"(regs[6]))
//	asm("movq %0, %%r15" : : "r"(regs[7]))
//}
//void read_register(void** regs){
//	asm("movq %%r8, %0" : "=r"(regs[0]) :);
//	asm("movq %%r9, %0" : "=r"(regs[1]) :);
//	asm("movq %%r10, %0" : "=r"(regs[2]) :);
//	asm("movq %%r11, %0" : "=r"(regs[3]) :);
//	asm("movq %%r12, %0" : "=r"(regs[4]) :);
//	asm("movq %%r13, %0" : "=r"(regs[5]) :);
//	asm("movq %%r14, %0" : "=r"(regs[6]) :);
//	asm("movq %%r15, %0" : "=r"(regs[7]) :);
//}
