#include <fox.h>

#define MAXMEM 10*1024*1024
#define MIN_CHAIN 1
char* mem_usage(){
	int runtime=run_time();
	int totaltime=total_time();
	int gctime=gc_time();
	int codetime=runtime-totaltime-gctime;
	return mstr("Memory:%s-%s [%d%% garbage], Pages: %d/%d, GC runs=%d-%d, Time=%d[gc]+%d[code]=%d ms [%d%% gc] GC Max: %d msec, Root Objs: %d",
		int_kb(_gcdata.max_mem,"B"),
		int_kb(max_mem(),"B"),
		(_gcdata.max_mem-_gcdata.max_used)*100/_gcdata.max_mem,
		_gcdata.total_pages,
		_gcdata.page_no,
		_gcdata.gcruns,
		_gcdata.gcwaste,
		gctime,
		codetime,
		runtime,
		gctime*100/(codetime+gctime),
		_gcdata.gcmax/1000,
		_gcdata.max_roots
	, End);
};
char* int_kb(size_t i,char* unit){
	size_t ks=1;
	if(i<10*ks*1024){ return int_human(i,NULL,""); };
	if(i<10*ks*1024*1024){ return int_human((i/(1024)),xstr("K",unit, End),""); };
	if(i<10*ks*1024*1024*1024){ return int_human((i/(1024*1024)),xstr("M",unit, End),""); };
	return int_human((i/(1024*1024*1024)),xstr("G",unit, End),"");
};
char* int_human(int i,char* unit,char* zero){
	if(!i){ return zero; };
	char* ret=int_str(i);
	int addat=3;
	while(addat<str_len(ret)){
		ret=xstr(sub_str(ret,0,-addat),",",sub_str(ret,-addat,-2147483648), End);
		addat+=4; };
	if(ret && unit){ ret=xcat(ret,unit, End); };
	return ret;
};
void rewrite_ptr(mempage* pg,void** ptr){
	if(*ptr<(void*)pg->page||*ptr>(void*)pg->types){ return; };
	int off=ptr_block(*ptr,pg);
	if(!(pg->types[off] & (1<<6))){
		*ptr=**(void***)ptr;
		return; };
	int len=1;
	while(len<=off && (pg->types[off-len] & (1<<6))) {len++;};
	assert(len<=off);
	char* head=pg->page+(off-len)*pg->block_size;
	*ptr=*(char**)head+((char*)*ptr-head);
};
void rewrite_ptrs(mempage* old){
	each_mem(pg,mem_i){
		if(pg==old){ continue; };
		for(int i=0;i<pg->blocks;i++){
			if(!pg->types[i] || pg->types[i] & (1<<6)){ continue; };
			char type=pg->types[i] & (31);
			if(type<Map){ continue; };
			int len1=1;
			while(pg->types[i+len1] & (1<<6)) {len1++;};
			int size1=len1*pg->block_size;
			void* data=pg->page+i*pg->block_size;
			void* ptr=NULL;
			if(type==Map||type==Vector){ rewrite_ptr(old,(void**)&(((map*)data)->vars)); }
			else if(type==Cell){
				void** vars=(void**)data;
				size1/=sizeof(void*);
				for(i=0;i<size1;i++) {rewrite_ptr(old,(void**)&(vars[i]));};
			}else if(type==Cell2){
				Mapcell* pairs=(Mapcell*)data;
				size1/=sizeof(Mapcell);
				for(i=0;i<size1;i++){
					rewrite_ptr(old,(void**)&(pairs[i].val));
					rewrite_ptr(old,(void**)&(pairs[i].id)); }; };
			i+=len1-1;
		};};	
	void** sp=NULL;
	map* map_1=root_ptrs(); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); rewrite_ptr(old,v); };
	memset(old->types,0,old->blocks);

	old->free=old->blocks;
	memset(old->types,0,old->blocks);
};
int copy_page(mempage* from,mempage* to){
	for(int i1=0;i1<from->blocks;i1++){
		char type1=from->types[i1];
		type1 &= (31);
		if(!type1){ continue; };
		int len=1;
		while(from->types[i1+len] & (1<<6)) {len++;};
		int size=len*from->block_size;
		void* ptr=from->page+i1*from->block_size;
		void* ret=page_alloc(to,size,type1,NULL);
		assert(ret);
		memcpy(ret,ptr,size);
		*(void**)ptr=ret;
		assert(ptr_type(ret)==type1 && mem_size(ret)>=size);
		i1+=len-1;
	};
	rewrite_ptrs(from);
	printf("End copy mempage");
	return 0;
};
int page_map(mempage* pg){
	if(pg->blocks==1){
		printf("[---- %d KB / %d KB ----]\n",pg->free * pg->block_size / 1024,pg->block_size/1024);
		return 0;
	};
	printf("[");
	int skip=pg->blocks/80;
	int cont=0;
	for(int i=0;i<pg->blocks;i++){
		char type=pg->types[i];
		if(skip && i%skip){ continue; };
		if(type & (1<<6)){
			printf("=");
			continue; };
		type &=(31);
		if(!type){ printf("."); }
		else if(type==String){ printf("s"); }
		else if(type==Blob){ printf("s"); }
		else if(type==Map){ printf("m"); }
		else if(type==Vector){ printf("v"); }
		else if(type==Cell){ printf("v"); }
		else if(type==Cell2){ printf("m"); }
		else if(type==Tail){ printf("t"); }
		else {printf("?");}; };
	printf("]:%d %d*%d=%d KB/%d%%\n",pg->no, pg->blocks,pg->block_size,pg->block_size * pg->blocks / 1024,(pg->blocks-pg->free)*100/pg->blocks);
	return 0;
};
int dump_chain(mempage* pg,int line){
	printf("________________ line=%d, page=%d, free=%d/%d\n",line,pg->no,pg->free,pg->blocks);
	for(int i=0; i<pg->chains.len; i++){
		printf("%d => %p\n",*(int*)(pg->chains.vars[i]),pg->chains.vars[i]); };
	printf("----------------\n");
	assert(0);
//	"Chain fox_error".fox_error()
	return 0;
};
int check_chains(mempage* pg,int line){
	for(int i=1; i<pg->chains.len; i++){
		if(*(int*)(pg->chains.vars[i])<MIN_CHAIN){
			dump_chain(pg,line); };
		if(!(*(int*)(pg->chains.vars[i])<=*(int*)(pg->chains.vars[i-1]))){
			dump_chain(pg,line); };
		if(*(int*)(pg->chains.vars[i])==*(int*)(pg->chains.vars[i-1])){
			if(!(pg->chains.vars[i]>pg->chains.vars[i-1])){
				dump_chain(pg,line); }; }; };
//	"sort ok on %d points\n".printf(pg->chains.len)
	return 0;
};
int page_maps(char* title){
	printf("%s\n",title);
	each_mem(pg,mem_i) {page_map(pg);};
	return 0;
};
