#line 2 "/web/fox/core.fox"
//#define NDEBUG
#include "fox.h"

#define MAXMEM 10*1024*1024
#define MIN_CHAIN 1

struct gcdata _gcdata={0};
map* _globals=NULL;
size_t _clockstart=0;

int _printed=0;
int _total_time=0;

char* skip=NULL;

int rand();
void* px(void* str,int newline);
void xexit(int val);
void* fox_error(char* msg,int dump);

int mem_total(){
	int ret=0;
	each_mem(pg,mem_i){ ret+=pg->blocks*pg->block_size; };
	return ret;
};
int mem_free(){
	int ret=0;
	each_mem(pg,mem_i){ ret+=pg->free*pg->block_size; };
	return ret;
};
void fox_signal_handler(int sig){ fox_stack_dump(); };
void stack_dump_direct(){
	void *array[400];
	size_t size=backtrace(array,400);
	backtrace_symbols_fd(array,size,STDERR_FILENO);
	xexit(-1);
};
char* stack_str(){
	void *array[400];
	size_t size=backtrace(array,400);
	char** lines=backtrace_symbols(array,size);
	char* ret="\n";
	for(int i=0; i<size; i++){
		char* part=sub_str(lines[i],str_len("3|||index.cgi|||||||||||||||||||||||||||0x0000000109c950d8|"),-2147483648);
		if(strstr(lines[i]," stack_str ") || strstr(lines[i],".dylib ") || strstr(lines[i]," fox_error ") || strstr(lines[i]," start ") || strstr(lines[i]," 0x0 ") || strstr(lines[i]," run ") || strstr(lines[i]," main ")){ continue; };
		ret=xcat(ret,part,"\n", End); };
//		ret.=lines[i].."\n"
	return ret;
};
void fox_stack_dump(){ fox_error("Error:\n",1); };
size_t blob_size(char* in){
	if(!in){ return 0; };
	char* head=ptr_head(in);
	return *(size_t*)head-(in-head-8);
};
int str_len(char* str){
	if(!str){ return 0; };
	int type=ptr_type(str);
	if(type==Blob){ return blob_size(str); };
	if(type==String){ return strlen(str); };
	return 0;
};
char* new_blob(int size){
	char* ret=fox_alloc(size+8+1,Blob);
	*(size_t*)ret=size;
	ret+=sizeof(size_t);
	assert(str_len(ret)==size);
	assert(!ret[size]);
	return ret;
};
char* blob_dup(char* str, int len){
	if(!str||!len){ return NULL; };
	return memcpy(new_blob(len),str,len);
};
char* str_dup_len(char* str, int len){
	if(!str){ return NULL; };
	char* ret=is_blob(str) ? new_blob(len) : new_str(len);
	return memcpy(ret,str,len);
};
char* str_dup(char* str){ return str_dup_len(str,str_len(str)); };
char* sub_str(char* src,int from,int len){
	if(!len){ return NULL; }; //fox_stack_dump()
	if(len==-2147483648){ len=0; };
	int slen=str_len(src);
	if(!slen){ return ""; };
	if(from < 0){ from+=slen; };
	if(len<=0){ len+=slen-from; };
	if(len<=0){ return NULL; };
	if(from>=slen){ return NULL; };
	if(from+len>=slen){ return str_dup((src+from)); };
	return str_dup_len((src+from),len);
};
char* print(char* str,FILE* fp){
	_printed=1;
	int ret=fwrite(null_str(str),1,str_len(null_str(str)),fp);
	assert(ret==str_len(null_str(str)));
	return str;
};
char* str_times(char* str,int times){
	if(times<=0){ return NULL; };
	char* ret=new_str(strlen(str)*times);
	while(times--) {ret=xcat(ret,str, End);};
	return ret;
};
char* xstr(char* ret,...){
	ret=str_dup(ret);
	va_list args;
	va_start(args,ret);
	while(1){
		char* str=va_arg(args,char*);
		if(str==End){ break; };
		ret=xcat(ret,str, End); };
	va_end(args);
	return ret;
};
char* xcat(char* ret,...){
	va_list args;
	va_start(args,ret);
	while(1){
		char* str=va_arg(args,char*);
		if(str==End){ break; };
		ret=cat(ret,to_str(str,"",0),-1020); };
	va_end(args);
	return ret;
};
char* cat(char* str1,char* str2,size_t len){
	assert((!str1 || is_str(str1)) && (!str2 || is_str(str2)));
	if(!str2){ return str1; };
	if(len==-1020){ len=str_len(str2); };
	if(!str1){ return str_dup_len(str2,len); };
	int oldlen=str_len(str1);
	str1=fox_realloc(str1,str_len(str1)+len+1,ptr_type(str1));
	assert(oldlen==str_len(str1));
	memcpy((str1+str_len(str1)),str2,len);
	if(ptr_type(str1)==Blob){ *(size_t*)(str1-8)+=len; };
	assert(oldlen+len==str_len(str1));
	assert(!str1[str_len(str1)]);
	return str1;
};
char* vec_json(map* mp,int indent){
	if(!mp){ return "[]"; };
	if(!is_vec(mp)){ return json(mp,indent); };
	char* ret=NULL;
	ret=xcat(ret,"[", End);
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i);
		if(!v){ ret=xcat(ret,"null", End); }
		else if(is_str(v)){ ret=xcat(ret,str_quote(v), End); }
		else if(is_num(v)){ ret=xcat(ret,v, End); }
		else if(is_map(v)){ ret=xcat(ret,vec_json(is_map(v),indent ? indent+1 : 0), End); };
		ret=xcat(ret,", ", End);
	};
	if(str_len(ret)==1){ ret=xcat(ret,"]", End); }
	else{
		ret[str_len(ret)-2]=']';
		ret[str_len(ret)-1]='\0'; };
	return ret;
};
char* json(map* mp,int indent){
	if(!map_len(mp)){ return "{}"; };
	if(is_vec(mp)){ return vec_json(mp,indent); };
	if(!is_map(mp)){ return NULL; };
	char* ret=NULL;
	ret=xcat(ret,"{", End);
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i); char* k=map_key(mp, i);
		if(indent){ ret=xcat(ret,"\n",str_times("\t",indent-1), End); };
		if(is_i(k)){ ret=xcat(ret,int_str((is_int(k)-1)), End); }
		else if(is_f(k)){ ret=xcat(ret,k, End); }
		else {ret=xcat(ret,str_quote(is_str(k)), End);};
		ret=xcat(ret,":", End);
		if(is_str(v)){ ret=xcat(ret,str_quote(is_str(v)), End); }
		else if(!v){ ret=xcat(ret,"null", End); }
		else if(is_map(v)){ ret=xcat(ret,json(is_map(v),indent ? indent+1 : 0), End); }
		else {ret=xcat(ret,v, End);};
		if(i<mp->len-1){ ret=xcat(ret,", ", End); };
	};
	if(str_len(ret)>1 && indent){ ret=xcat(ret,"\n",str_times("\t",indent-2), End); };
	ret=xcat(ret,"}", End);
	return ret;
};
char* to_str(void* v,char* null,int human){
	if(!v){ return NULL; }
	else if(is_str(v)){ return v; }
	else if(is_i(v)){ return int_str(is_int(v)); }
	else if(is_f(v)){ return double_str(is_double(v)); }
	else if(is_map(v)){ return json(v,human); }
	else {return "";};
};
char* str_quote(char* head){
	if(!head){ return "\"\""; };
	int len=str_len(head);
	int extra=0;
	char c;
	char* str=head;
	char* quotable="\\\"\n\r\t";
	char* quoteto="\\\"nrt";
	while((c=*str++)) {if(strchr(quotable,c)){ extra++; };};
	assert(extra<=len);
	char* ret=new_str(str_len(head)+extra+2);
	ret[0]='"';
	str=head;
	int i=1;
	char* fox_at;
	while((c=*str++)){
		if(!(fox_at=strchr(quotable,c))){ ret[i++]=c; continue; };
		ret[i++]='\\';
		ret[i++]=quoteto[fox_at-quotable];
		assert(i<len+extra+2);
	};
	ret[i]='"';
	assert(!ret[i+1]);
	return ret;
};
char* double_str(double val){
	char ret[21]={0};
	sprintf(ret,"%.14g",val);
	return str_dup(ret);
};
char* int_str(long long value){
	int neg=0;
	if(value<0){
		value*=-1;
		neg=1; };
	char string[21]={0};
	int string_l=(value == 0) ? 1 : (int)log10(value) + 1 + neg;
	long long residual=value;
	int it=0;
	for(it=string_l-1;it>=0;it--){
		int digit;
		digit=residual % 10;
		residual=residual / 10;
		string[it]='0' + digit;
	};
	string[string_l]='\0';
	if(neg){ string[0]='-'; };
	return str_dup(string);
};
int str_eq(char* str,char* str1){
	if(!str && !str1){ return 1; };
	if(is_str(str) && is_str(str1)){ return strcmp(str,str1) ? 0 : 1; };
	if(!str && is_str(str1) && !str_len(str1)){ return 1; };
	if(!str1 && is_str(str) && !str_len(str)){ return 1; };
	return 0;
};
size_t str_hash(unsigned char *str){
	int c;
	size_t ret = 5381;
	while((c=*str++)) {ret=((ret<<5)+ret)+c;};
	return ret ? ret : 0x08db1118c533c700; //don't return 0
};
map* xvec(void* v1,...){
	map* mp=new_vec();
	if(v1==End){ return mp; };
	vec_add(mp,v1);
	va_list args;
	va_start(args,v1);
	while(1){
		void* v=va_arg(args,void*);
		if(v==End){ break; };
		vec_add(mp,v); };
	va_end(args);
	return mp;
};
map* xmap(char* k1,...){
	map* mp=new_map();
	if(k1==End){ return mp; };
	va_list args;
	va_start(args,k1);
	void* v=NULL;
	void* k=k1;
	while(1){
		v=va_arg(args,void*);
		if(v==End){ fox_error(xstr("Value for key ", k, " not provided in xmap", End),0); };
		add(mp,k,v);
		k=va_arg(args,char*);
		if(!k || k==End){ break; }; };
	va_end(args);
	return mp;
};
char* new_str(int len){ return fox_alloc(len+1,String); };
map* new_map(){
	map* ret=fox_alloc(sizeof(map),Map);
	ret->type=Map;
	return ret;
};
map* new_vec(){
	map* ret=fox_alloc(sizeof(map),Vector);
	ret->type=Vector;
	return ret;
};
int key_eq(map* mp,int idx,char* id){
	assert(id);
	assert(idx>=0 && idx<mp->len);
	char* key=map_key(mp,idx);
	if(is_str(key)){ return str_eq(key,is_str(id)); };
	return id==key;
};
int key_hash(char* id){ return is_str(id) ? str_hash((unsigned char*)id) : is_int(id); };
int map_has_key(map* mp,char* id){
	if(!id||!mp){ return 0; };
	assert(is_map(mp));
	if(!mp->len){ return 0; };
	if(ptr_type(mp)==Vector){
		if(is_str(id)){ return 0; };
		long long idx=is_int(id);
		if(idx<1||idx>mp->len){ return 0; };
		return idx;
	};
	int hash=key_hash(id);
	hash=hash & (map_size(mp)-1);
	int idx=mp->pairs[hash].hkey;
	while(idx>0){
		if(key_eq(mp,idx-1,id)){ return idx; };
		idx=mp->pairs[idx-1].nextid;
	};
	return 0;
};
map* vec_realloc(map* mp,int extra){
	int size=map_size(mp);
	int req=mp->len+extra;
	if(req<=size){ return mp; };
	if(!size){ size=2; };
	while(size<req) {size*=2;};
	mp->vars=fox_realloc((char*)mp->vars,size*sizeof(void*),Cell);
	return mp;
};
map* vec_add(map* mp,void* v){
	if(!mp){ return mp; };
	if(mp->type!=Vector){ return add(mp,NULL,v); };
	vec_realloc(mp,1);
	mp->len++;
	mp->vars[mp->len-1]=v;
	return mp;
};
map* set(map* mp,int idx,void* val){
	if(idx<0){ return mp; };
	if(ptr_type(mp)==Map){ mp->pairs[idx].val=val; }
	else {vec_set(mp,idx+1,val);};
	return mp;
};
long long is_int(void* v){
	if(!is_i(v)){ return 0; };
	size_t val=((size_t)v >> 63) & 1 ? 3 : 0;
	return (long long)((((size_t)v) & ~(3ll<<61)) | ((val & 3ll)<<61));
};
double is_double(void* v){
	if(!is_f(v)){ return is_int(v); };
	size_t val=(((size_t)v) >> 60) & 1 ? 1 : 2;
	val=(size_t)((((size_t)v) & ~(3ll<<61)) | ((val & 3ll)<<61));
	return *(double*)&val;
};
void* int_var(size_t i){
	return (void*)((i & ~(3ll<<61)) | ((2ll & 3ll)<<61));
};
void* double_var(double f){
	return (void*)(((*(size_t*)&f) & ~(3ll<<61)) | ((1ll & 3ll)<<61));
};
int has_id(map* mp,int idx){
	if(!is_map(mp)){ return 0; };
	return idx>=0 && idx<mp->len;
};
void* map_idp(map* mp,int idx){
	if(!mp){ return NULL; };
	if(!is_map(mp)){ px((xstr("Error! not map ",str_quote(to_str(mp,"",0)), End)),1); assert(0); };
	assert(is_map(mp));
	if(idx<0 || idx>=mp->len){ return NULL; };
	return mp->type==Vector ? &mp->vars[idx] : &mp->pairs[idx].val;
};
void* map_id(map* mp,int idx){
	if(!mp){ return NULL; };
	if(!is_map(mp)){ return NULL; };
//	if !mp.is_map() => ("Error! not map "..mp.to_str().str_quote()).px(); assert(0)
//	assert(mp.is_map())
	if(idx<0 || idx>=mp->len){ return NULL; };
	return mp->type==Vector ? mp->vars[idx] : mp->pairs[idx].val;
};
int map_len(map* mp){
	if(!mp){ return 0; };
	assert(is_map(mp));
	return mp->len;
};
char* map_key(map* mp,int idx){ return idx<0 ? NULL : (ptr_type(mp)==Map ? mp->pairs[idx].id : int_var((idx+1))); };
char* is_str(void* v){
	if(!v || is_num(v)){ return NULL; };
	int type=ptr_type(v);
	return type==String || type==Blob ? v : NULL;
};
char* is_blob(void* v){ return ptr_type(v)==Blob ? v: NULL; };
map* is_vec(void* v){ return ptr_type(v)==Vector ? v: NULL; };
map* is_hash(void* v){ return ptr_type(v)==Map ? v: NULL; };
map* is_map(void* v){ return ptr_type(v)>=Map ? v: NULL; };
double str_double(char* v){
	double ret=0.0;
	if(!v){ return ret; };
	sscanf(v,"%lf",&ret);
	return ret;
};
double to_double(void* v){
	if(is_str(v)){ return str_double(v); };
	return is_double(v);
};
long long str_int(char* v){
	long long ret=0;
	sscanf(v,"%lld",&ret);
	return ret;
};
long long to_int(void* v){
	if(is_str(v)){ return str_int(v); };
	return is_int(v);
};
int next(map* mp,int idx,char** key,void** val){
	idx++;
	if(!mp||idx>=mp->len){ return -1; };
	while(idx<mp->len && map_id(mp,idx)==(char*)&skip){ idx++; };
	if(idx>=mp->len){ return -1; };
	if(key){ *key=map_key(mp,idx); };
	if(val){ *val=map_id(mp,idx); };
	return idx;
};
int stoi(char* str){
	if(!str){ return 0; };
	return atoi(str);
};
map* globals(){ return _globals; };
int mem_used(int block_size,int type){
	int ret=0;
	each_mem(pg,mem_i){
		if((!block_size || block_size==pg->block_size)){// && (!type || type==pg->type)
			ret+=(pg->blocks-pg->free)*pg->block_size;
		};};
	return ret;
};
mempage* ptr_page(void* ptr){
	if(!ptr || !_gcdata.total_pages || (char*)ptr<_gcdata.pages[0].page || (char*)ptr>_gcdata.pages[_gcdata.total_pages-1].types){ return NULL; };
	int lo=-1;
	int up=_gcdata.total_pages;
	while(up-lo>1){
		int mid=(lo+up)/2;
		if(ptr>=(void*)_gcdata.pages[mid].page && ptr<(void*)_gcdata.pages[mid].types){
			return &_gcdata.pages[mid];
		}else if(ptr<(void*)_gcdata.pages[mid].page){
			up=mid;
		}else{
			lo=mid; }; };
	return NULL;
};
void* block_ptr(int block,mempage* pg){ return pg->page+pg->block_size*block; };
int ptr_block(void* ptr,mempage* pg){ return ((char*)ptr-pg->page)/pg->block_size; };
int ptr_type(void* ptr){
	if(!ptr){ return Null; };
	if(is_i(ptr)){ return Int; };
	if(is_f(ptr)){ return Double; };
	mempage* pg=ptr_page(ptr);
	if(!pg){ return String; };
//	if pg->type => return pg->type
	return pg->types[ptr_block(ptr,pg)] & 31;
};
int cell2_mark(Mapcell* pairs,int size){
	for(int i=0;i<size;i++){
		if(!is_num(pairs[i].id)){ gc_mark(pairs[i].id); };
		gc_mark(pairs[i].val);
	};
	return 0;
};
int cell_mark(void** pairs,int size){
	for(int i=0;i<size;i++){
		gc_mark(pairs[i]);
	};
	return 0;
};
int gc_mark(void* ptr){
	if(!ptr){ return 0; };
	mempage* pg=ptr_page(ptr);
	if(!pg){ return 0; };
	int head=block_head(ptr_block(ptr,pg),pg);
	int len=block_len(head,pg);
	if(pg->types[head] & (1<<7)){ return 1; };
	int type=pg->types[head];
	for(int i=0; i<len; i++){ pg->types[head+i] |= (1<<7); };
	ptr=block_ptr(head,pg);
	if(type==Cell){ cell_mark((void**)ptr,mem_size(ptr)/sizeof(void*)); }
	else if(type==Cell2){ cell2_mark((Mapcell*)ptr,mem_size(ptr)/sizeof(Mapcell)); }
	else if(type==Map){ gc_mark(((map*)ptr)->pairs); }
	else if(type==Vector){ gc_mark(((map*)ptr)->vars); }
	else if(type==Index||type==Keys){ assert(0); };
	return 1;
};
int sweep_page(mempage* pg){
	int ret=0;
	pg->free=0;
	for(int i=0;i<pg->blocks;i++){
		if(!(pg->types[i] & (1<<7))){
			if(pg->types[i]){
				if((pg->types[i] & 31) !=Tail){ ret++; };
				pg->types[i]='\0';
				memset(pg->page+i*pg->block_size,0,pg->block_size); };
			pg->free++;
			continue;
		}else{
			pg->types[i] &= ~(1<<7); }; };
	return ret*pg->block_size;
};
void* data_delete(void* data,int idx,int size,int len){ return data_shift(data,idx+1,-1,size,len); };
void* data_insert(void* data,int idx,int size,int len){
	data_shift(data,idx,1,size,len);
	memset((char*)data+idx*size,0,size);
	return data;
};
void* data_shift(void* data,int idx,int shift,int size,int len){
	if(idx>=len){ return data; };
	memmove((char*)data+(idx+shift)*size,(char*)data+idx*size,(len-idx)*size);
	return data;
};
int cmp_page(const void* pg1,const void* pg2){
	if(((mempage*)pg1)->page<((mempage*)pg2)->page){ return -1; };
	return 1;
};
void reindex_pages(){
	char* lastpage=NULL;
	qsort(_gcdata.pages,_gcdata.total_pages,sizeof(mempage),cmp_page);
	each_mem(pg,mem_i){
		pg->idx=mem_i;
		assert(!lastpage || pg->page>lastpage);
		lastpage=pg->page;
	};
};
mempage* no_page(int no){
	each_mem(pg,idx){ if(pg->no==no){ return pg; }; };
	assert(0);
	return NULL;
};
struct mempage* new_page(int block_size,int blocks){
//	printf("\nGC Page: %d * %d = %d\n",block_size,blocks,block_size*blocks)
	int size=block_size*blocks;
	char* page=malloc((size+blocks));
	_gcdata.pages=realloc(_gcdata.pages,(_gcdata.total_pages+1)*sizeof(mempage));
	_gcdata.pages=data_insert(_gcdata.pages,_gcdata.total_pages,sizeof(mempage),_gcdata.total_pages);
	mempage* ret=&_gcdata.pages[_gcdata.total_pages];
	_gcdata.total_pages++;
	ret->page=page;
	int no=ret->no=_gcdata.page_no++;
	reindex_pages();
	ret=no_page(no);
	assert((long long)ret->page>0);
	ret->blocks=blocks;
	ret->free=blocks;
	ret->block_size=block_size;
	ret->types=ret->page+size;
	memset(ret->types,0,blocks);
	index_free_space(ret);
	_gcdata.curr_mem+=size+blocks*sizeof(char);
	_gcdata.max_mem=max(_gcdata.max_mem,_gcdata.curr_mem);
	return ret;
};
int comp_iptr(const void* sp1,const void* sp2){
	// -1: move sp1 left, +1: move right
	int* i1=*(int**)sp1;
	int* i2=*(int**)sp2;
	if(*i1 > *i2){ return -1; };
	if(*i1 < *i2){ return 1; };
	if(i1 > i2){ return 1; };
	if(i1 < i2){ return -1; };
	return 0;
};
mempage* index_free_space(mempage* pg){
	if(pg->blocks==1){ return pg; };
	int free_len=0;
	int maxchain=max(pg->blocks/20,1);
	int no=0;
	int** chains=realloc(pg->chains.vars,maxchain*sizeof(int*));
	for(int i=0;i<=pg->blocks;i++){
		if(i<pg->blocks && !pg->types[i]){
			free_len++;
		}else if(free_len){
			if(free_len<MIN_CHAIN){
				free_len=0;
				*(int*)(pg->page+(i-1)*pg->block_size)=free_len;
				continue; };
			if(no>=maxchain){
				maxchain*=2;
				chains=realloc(chains,maxchain*sizeof(int*)*2); };
			chains[no]=(int*)(pg->page+(i-free_len)*pg->block_size);
			*chains[no]=free_len;
			no++;
			free_len=0; }; };
	assert(free_len==0);
	qsort(chains,no,sizeof(int*),comp_iptr);
	pg->chains.len=no;
	pg->chains.vars=(void**)chains;
	return pg;
};
void* chain_alloc(mempage* pg, int size, int type, char* ptr){
	int blocks=size_blocks(size,pg);
	int chainid=-1;
	int lo=-1;
	int hi=pg->chains.len;
	if(!ptr){
		while(hi-lo>1){
			chainid=(hi+lo)/2;
			if(blocks==*(int*)(pg->chains.vars[chainid])){ break; }
			else if(blocks<*(int*)(pg->chains.vars[chainid])){ lo=chainid; }
			else {hi=chainid;}; };
		if(hi-lo<2){
			if(lo==-1){ return NULL; }
			else {chainid=lo;}; };
		assert(chainid>=0);
		assert(chainid<pg->chains.len);
		ptr=pg->chains.vars[chainid];
		assert(*(int*)ptr>=blocks);
	}else{
		if(ptr<pg->page || ptr>=pg->types){ return NULL; };
		if(ptr_type(ptr)){ return NULL; };
		if(*(int*)ptr<blocks){ return NULL; };
		if(*(int*)ptr>=MIN_CHAIN){
			while(hi-lo>1){
				int mid=(hi+lo)/2;
				int match=comp_iptr(&ptr,&(pg->chains.vars[mid]));
				if(!match){
					chainid=mid;
					break;
				}else if(match<0){ hi=mid; }
				else {lo=mid;}; };
			assert(chainid>=0); }; };
	if(chainid>=0){
		pg->chains.vars=data_delete(pg->chains.vars,chainid,sizeof(int*),pg->chains.len);
		pg->chains.len--; };
	int rest=*(int*)ptr-blocks;
	assert(rest>=0);
	void* shifted=ptr+pg->block_size*blocks;
	if(rest){
		*(int*)shifted=rest; };
	if(rest>=MIN_CHAIN){
		assert(chainid>=0);
		lo=-1;
		hi=pg->chains.len;
		while(hi-lo>1){
			int mid=(hi+lo)/2;
			if(comp_iptr(&shifted,&(pg->chains.vars[mid]))==1){ lo=mid; }
			else {hi=mid;}; };
		assert(hi>=0 && hi<=pg->chains.len);
		if(hi){ assert(*(int*)shifted<=*(int*)(pg->chains.vars[hi-1])); };
		if(hi<pg->chains.len){ assert(*(int*)shifted>=*(int*)(pg->chains.vars[hi])); };
		pg->chains.vars=data_insert(pg->chains.vars,hi,sizeof(int*),pg->chains.len);
		pg->chains.vars[hi]=shifted;
		pg->chains.len++; };
	int block=ptr_block(ptr,pg);
	pg->types[block]=type;
	type |= (1<<6);
	for(int i=1;i<blocks;i++){
		assert(!pg->types[block+i]);
		pg->types[block+i]=type; };
	pg->free-=blocks;
	memset(ptr,0,blocks*pg->block_size);
	return ptr;
};
void* page_alloc(mempage* pg,int size,int type,int* full){
	if(pg->blocks==1){
		if(!pg->free || size>pg->block_size || size<pg->block_size*.4){ return NULL; };
		pg->types[0]=type;
		pg->free=0;
		memset(pg->page,0,size);
		return pg->page;
	};
	int block_size=pg->block_size;
	int blocks=(size+block_size-1)/block_size;
	assert(blocks);

	if(block_size>16 && size<block_size/2){
		return NULL; };
	if(block_size<1024*4 && blocks>32){
		return NULL; };
	if(blocks > 0.5 * pg->blocks){
		return NULL;
	};
	if(full && (block_size==16 || size>=block_size) && (block_size==1024*4||blocks<16) && blocks<0.4*pg->blocks){ (*full)++; };
	return chain_alloc(pg,size,type,NULL);
};
int gc_sweep(){
	_gcdata.curr_used=0;
	int ret=0;
	each_mem(pg,mem_i){
		ret+=sweep_page(pg);
		if(pg->free==pg->blocks){
			pg->abandoned++;
			if(pg->abandoned>2){
				free_page(pg);
				mem_i--;
				continue; };
		}else{
			pg->abandoned=0; };
		_gcdata.curr_used+=(pg->blocks-pg->free)*pg->block_size;
		index_free_space(pg);
	};	
	_gcdata.max_used=max(_gcdata.curr_used,_gcdata.max_used);
	return ret;
};
map* root_ptrs(){
	void* stack_end=_globals;
	static map ret={0};
	void* ptr=(void*)&stack_end;
	if(!_gcdata.stack_head){ printf("gc_start() was not called fox_at start of application"); exit(-1); };
	int size=((void*)_gcdata.stack_head-ptr)/sizeof(void*);
	ret.vars=size<0 ? _gcdata.stack_head : ptr;
	ret.len=abs(size)+1;
	ret.type=Vector;
	return &ret;
};
int fox_gc(){
	int pre_usage=mem_used(0,0);
	// commenting off setjmp() doesn't make any diff even in -O3
	jmp_buf regs={0};
	setjmp(regs);
	map* roots=root_ptrs();
	for(int i=0; i<roots->len; i++){ gc_mark(roots->vars[i]); };
	int freed=gc_sweep();
	_gcdata.gcruns++;
	int percent=pre_usage ? freed*100/pre_usage : 100;
	if(percent <=5){ _gcdata.gcwaste++; };
	return percent;
};
int gc_runs(){ return _gcdata.gcruns; };
int gc_time(){ return _gcdata.gctime/1000; };
int gc_end(){
	_globals=NULL; _clockstart=0;
	mempage* pg=_gcdata.pages;
	each_mem(pg,i){
		free(pg->page);
	};
	free(_gcdata.pages);
	memset(&_gcdata,0,sizeof(_gcdata));
	return 0;
};
mempage* free_page(mempage* pg){
	int idx=pg->idx;
	free(pg->page);
	_gcdata.pages=data_delete(_gcdata.pages,pg->idx,sizeof(mempage),_gcdata.total_pages);
	_gcdata.total_pages--;
	reindex_pages();
	return idx < _gcdata.total_pages ? &_gcdata.pages[idx] : NULL;
};
int block_len(int block,mempage* pg){
	if(!pg){ return 0; };
	int ret=1;
	for(int i=block+1;i<pg->blocks;i++){
		if(pg->types[i] & (1<<6)){
			ret++;
		}else {break;}; };
	return ret;
};
int mem_size(void* ptr){
	mempage* pg=ptr_page(ptr);
	if(!pg){ return 0; };
	int block=ptr_block(ptr,pg);
	assert(!(pg->types[block] & (1<<6)));
	return pg->block_size*block_len(block,pg);
};
void* expand_inplace(char* ptr,char type,int size,int extra){
	mempage* pg=ptr_page(ptr);
	if(pg->blocks==1){
		assert(ptr==pg->page);
		int newsize=size+extra;
		assert(newsize>pg->block_size);
		if(type==String){
			newsize*=2; };
		newsize=(newsize+8-1) & ~(8-1); //roundup
		pg->block_size=newsize;
		pg->page=realloc(pg->page,newsize+1);
		pg->types=pg->page+newsize;
		pg->types[0]=type;
		memset(pg->page+size,0,newsize-size);
		void* ret=pg->page;
		reindex_pages();
		return ret; };
	void* ret=chain_alloc(pg,extra,type,ptr+pg->block_size);
	if(!ret){ return NULL; };
	pg->types[ptr_block(ret,pg)]=type|(1<<6);
	return ptr;
};
int size_blocks(size_t size,mempage* pg){ return (size + pg->block_size - 1) / pg->block_size; };
void* fox_realloc(void* ptr,size_t size,int type){
	assert(size);
	assert(size<MAXMEM);
	if(!ptr){
		return fox_alloc(size,type); };
	void* head=ptr_head(ptr);
	int oldsize=mem_size(head);
	int offset=head ? (char*)ptr-(char*)head : 0;
	size+=offset;
	if(head){
		if(oldsize>=size){
			return ptr; };
		void* ret=expand_inplace(head,type,oldsize,size-oldsize);
		if(ret){ return ret+offset; }; };	
	void* ret=fox_alloc(size,type);
	assert(ret);
	assert(oldsize || type==String);
	oldsize ? memcpy(ret,head,oldsize) : memcpy(ret,ptr,strlen(ptr)+1);
	return ret+offset;
};
void* fox_alloc(size_t size,int type){
	if(!_gcdata.stack_head){ printf("GC not started!!!"); exit(-1); };
	if(_gcdata.inalloc){ printf("fox_error!!! Recursive fox_alloc() call"); exit(-1); };
	_gcdata.inalloc=1;
	assert(size<MAXMEM);
	struct timeval gc_start=microtime();
	void* ret=_xalloc(size,type);
	if(!ret){ printf("%ld byte allocation failed.",size); exit(-1); };
	int time=elapsed(gc_start);
	_gcdata.gctime+=time;
	_gcdata.gcmax=max(_gcdata.gcmax,time);
	assert(!((char*)ret)[size-1]);
	_gcdata.inalloc=0;
	return ret;
};
void* new_alloc(size_t size,int type){
	int bsize=block_size(size);
	if(!bsize){ return page_alloc(new_page(size,1),size,type,NULL); };
	return page_alloc(new_page(bsize,max(ceil_pow2((mem_used(bsize,type)+size*2))/bsize,64*1024/bsize)),size,type,NULL);
};
void* _xalloc(size_t size,int type){
	if(!_gcdata.pages){ return new_alloc(size,type); };
	if(size>256*256*16){ return new_alloc(size,type); };
	char* ret=NULL;
	int full=0;
	each_mem(pg,mem_i){
		if((ret=page_alloc(pg,size,type,&full))) {return ret;};
	};
	if(!full){ return new_alloc(size,type); };
	int freed=fox_gc();
	if(freed <=5){ return new_alloc(size,type); };
	each_mem(pg,mem_i2) {if((ret=page_alloc(pg,size,type,NULL))) {return ret;};};
	return new_alloc(size,type);
};
int block_size(int size){
	if(size <= 256){ return 16; };
	if(size <= 256*16){ return 256; };
	if(size <= 256*256){ return 256*16; };
	return 0;
};
void start_time(){ _gcdata.time=microtime(); };
void end_time(){ _total_time+=elapsed(_gcdata.time); };
int run_time(){ return elapsed(_gcdata.run_time)/1000; };
int total_time(){ return _total_time/1000; };
void time_max(){
	int curr=elapsed(_gcdata.time);
	_total_time=max(_total_time,curr);
};
struct timeval microtime(){
	struct timeval ret={0};
	gettimeofday(&ret,NULL);
	return ret;
};
int elapsed(struct timeval from){
	struct timeval upto=microtime();
	return (upto.tv_sec-from.tv_sec)*1000000+(upto.tv_usec-from.tv_usec);
};
int map_size(map* mp){
	if(!mp){ return 0; };
	return ceil_pow2(mp->len);
};
int ceil_pow2(int i){
	if(!i){ return 0; };
	i--;
	int ret=2;
	while(i>>=1) {ret<<=1;};
	return ret;
};
map* add(map* mp,void* k,void* v){ return mp->type==Vector ? vec_set(mp,is_int(k),v) : map_add(mp,k,v); };
map* vec_set(map* mp,int idx,void* v){//1 based
	if(!mp){ return mp; };
	if(mp->type!=Vector){ return add(mp,int_var(idx),v); };
	if(idx>0 && idx<=mp->len){ mp->vars[idx-1]=v; return mp; };
	if(idx<1){ return vec_add(mp,v); };
	int diff=idx-mp->len;
	while(diff>1){
		vec_add(mp,NULL);
		diff--; };
	return vec_add(mp,v);
};
map* map_add(map* mp,char* key,void* v){
	if(!mp){ return mp; };
	assert(ptr_type(mp)==Map);
	int idx=map_has_key(mp,key);
	if(idx){
		mp->pairs[idx-1].val=v;
		return mp;
	};
	int reindex=0;
	if(mp->len>=map_size(mp)){
		mp->pairs=map_size(mp) ? fox_realloc((char*)mp->pairs,2*map_size(mp)*sizeof(Mapcell),Cell2)
			: fox_alloc(2*sizeof(Mapcell),Cell2);
		reindex=1;
	};
	mp->pairs[mp->len].val=v;
	mp->pairs[mp->len].id=key;
	mp->pairs[mp->len].nextid=-1;
	mp->len++;
	reindex ? map_reindex(mp) : map_index(mp,mp->len-1);
	return mp;
};
int map_nextno(map* mp){
	for(int i=mp->len-1; i>=0; i--){
		if(is_i(mp->pairs[i].id)){ return is_int(mp->pairs[i].id); }; };
	return 0;
};
map* map_index(map* mp,int idx){
	if(!mp){ return mp; };
	assert(idx>=0 && idx<mp->len);
	mp->pairs[idx].nextid=-1;
	char* id=mp->pairs[idx].id;
	if(!id){ mp->pairs[idx].id=int_var((map_nextno(mp)+1)); };
	int i=key_hash(mp->pairs[idx].id);
	i=i & (map_size(mp)-1);
	if(!mp->pairs[i].hkey){ mp->pairs[i].hkey=idx+1; }
	else{
		i=mp->pairs[i].hkey;
		assert(i>0 && i<=mp->len);
		while(mp->pairs[i-1].nextid>0){
			i=mp->pairs[i-1].nextid;
			if(i==idx+1){ assert(0); }; };
		mp->pairs[i-1].nextid=idx+1; };
	return mp;
};
map* map_reindex(map* mp){
	if(!mp){ return mp; };
	for(int i=0;i<map_size(mp);i++){
		mp->pairs[i].nextid=mp->pairs[i].nextid ? -1 : 0;
		mp->pairs[i].hkey=0;
	};
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ map_index(mp,i); };
	return mp;
};
char* null_str(char* in){ return in ? in : ""; };
map* ptrs_vec(void** ptrs,int len){
	map* ret=new_vec();
	ret->len=len;
	ret->vars=ptrs;
	return ret;
};
int init_gc(void** sp){
	_clockstart=clock_cycles();
	_gcdata.run_time=microtime();
	_gcdata.stack_head=sp;
	*sp=_globals=new_map();
	signal(SIGSEGV,fox_signal_handler);
	signal(SIGABRT,fox_signal_handler);
	init_rand();
	return 0;
};
int exec(char* cmd,char** output){
	FILE* fp;
	char buff[2048];
	buff[sizeof(buff)-1]='\0';
	fp=popen(cmd,"r");
	if(!fp){ fox_error(xstr("Failed to run ", cmd, End),0); return -1; };
	char* ret=NULL;
	while(fgets(buff,sizeof(buff)-1,fp)){ !output ? px(buff,0) : (ret=xcat(ret,buff, End)); };
	int status=pclose(fp);
	status=WEXITSTATUS(status);
	if(output){ *output=ret; };
	return status;
};
size_t clock_cycles(){
	unsigned int lo,hi;
	asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((size_t)hi << 32) | lo;
};
void* ptr_head(void* ptr){
	mempage* pg=ptr_page(ptr);
	if(!pg){ return NULL; };
	return block_ptr(block_head(ptr_block(ptr,pg),pg),pg);
};
int block_head(int no,mempage* pg){
	while(pg->types[no] & (1<<6)){ assert(no>0); no--; };
	return no;
};
void init_rand(){ srand(time(NULL)*2817635252+_gcdata.run_time.tv_usec); };
void* map_val(map* mp,char* key){
	if(!mp||!key||!is_map(mp)){ return NULL; };
//	assert(mp.ptr_type()==Map)
	int i=map_has_key(mp,key);
	return i ? mp->pairs[i-1].val : NULL;
};
extern char **environ;
map* env_vars(){
	if(map_val(_globals,"env")){ return map_val(_globals,"env"); };
	map* ret=new_map();
	for(char **env=environ;*env;++env){
		map* mp=str_split(*env,"=",2);
		add(ret,map_id(mp,0),map_id(mp,1)); };
	add(_globals,"env",ret);
	return ret;
};
map* argv_map(char** argv,int argc){
	map* ret=new_vec();
	ret->len=argc;
	ret->vars=(void**)argv;
	add(_globals,"args",ret);
	return ret;
};
