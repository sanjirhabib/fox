#line 2 "src/core.fox"

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <execinfo.h>
#include <signal.h>
#include <sys/mman.h>

#include <core.h>
//enum Types {
//	Null,Int,Double,String,Blob,Map,Vector,Index,Keys,Cell,Cell2,Tail
//};
//typedef struct Mapcell {
//	int nextid;
//	int hkey;
//	char* id;
//	void* val;
//} Mapcell;
//typedef struct map {
//	int len;
//	char type;
//	unsigned short magic;
//	union {
//		struct Mapcell* pairs;
//		void** vars;
//	};
//} map;

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#if UINTPTR_MAX == 0xffffffff
//typedef long long void*;
#else
//typedef void* var;
#endif
#define is_i(x) ((long long)(x)>>61 & 2)
#define is_f(x) ((*(long long*)&(x))>>61 & 1)
#define is_num(x) (is_i(x)||is_f(x))

//extern map* _globals;

//extern int _printed;
//extern int _is_web;

//extern char* skip;
//extern int _queries;
int _queries;

#define None 0x0F9AD3BA
#define End (char*)(0x0FF1B14E059AD3BA)

char* strstr(const char* str1,const char* str2);
int chdir(const char* path);
int max_mem();
int curr_mem();
// ^^^ habib upto

char* strstr(const char* str1,const char* str2);
int chdir(const char* path);
int max_mem();
int curr_mem();
int run(map* args);
#include <gc.h>
//#include <map.h>
#include <generator.h>

#define MAXMEM 40*1024*1024
#define MIN_CHAIN 1

map* _globals=NULL;

int _printed=0;

char* skip=NULL;

int rand();
void* px(void* str,int newline);
void xexit(int val);
void* fox_error(char* msg,int dump);
static void fox_signal_handler(int sig){ fox_stack_dump(); };
void stack_dump_direct(){
	void* array[400];
	size_t size=backtrace(array,400);
	backtrace_symbols_fd(array,size,STDERR_FILENO);
	return 0;
//	assert(0)
};
char* stack_str(){
	void* array[400];
	size_t size=backtrace(array,400);
	char** lines=backtrace_symbols(array,size);
	char* ret=NULL;
	for(int i=0; i<size; i++){
		if(strstr(lines[i]," stack_str ") || strstr(lines[i],".dylib ") || strstr(lines[i]," fox_error ") || strstr(lines[i]," start ") || strstr(lines[i]," 0x0 ") || strstr(lines[i]," main ")){ continue; };
		char* part=sub_str(lines[i],str_len("3|||index.cgi|||||||||||||||||||||||||||0x0000000109c950d8|"),-2147483648);
		part=sub_str(part,0,strchr(part,' ')-part);
		ret=xcat(ret," -- ",part,"()", End); };
//		lines[i].printf("%s\n")
	return ret;
};
void fox_stack_dump(){ fox_error("Error:",1); };
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
char* blob_cat(char* str1,char* str2,size_t len){
	assert((!str1 || is_str(str1)) && (!str2 || is_str(str2)));
	if(!str2){ return str1; };
	if(len==-1020){ len=str_len(str2); };
	if(!str1){ return str_dup_len(str2,len); };
	int oldlen=str_len(str1);
	if(!is_blob(str1)){
		char* ret=new_blob(oldlen+len);
		memcpy(ret,str1,oldlen);
		memcpy((ret+oldlen),str2,len);
		ret[oldlen+len]='\0';
		assert(str_len(ret)==oldlen+len);
		return ret;
	}else{
		char temp1[128];
		char temp2[128];
		char temp3[128];
		void* temp=fox_realloc(str1,str_len(str1)+len+1,ptr_type(str1));
//		"blob_cat %s + %s = %s\n".printf(str1.ptr_name(temp1), str2.ptr_name(temp2), temp.ptr_name(temp3))
		assert(oldlen==str_len(temp));
		str1=temp;
		memcpy((str1+str_len(str1)),str2,len);
		if(ptr_type(str1)==Blob){
			char* head=ptr_head(str1);
			*(size_t*)(head)+=len; };
		assert(oldlen+len==str_len(str1));
		assert(!str1[str_len(str1)]); };
	return str1;
};
//extern char tempbuff[128];
char* cat(char* str1,char* str2,size_t len){
//	return str1.blob_cat(str2,len)
	assert((!str1 || is_str(str1)) && (!str2 || is_str(str2)));
	if(!str2){ return str1; };
	if(len==-1020){ len=str_len(str2); };
	if(!str1){ return str_dup_len(str2,len); };
	int oldlen=str_len(str1);
	if(oldlen+len>16 || is_blob(str1) || is_blob(str2)){ return blob_cat(str1,str2,len); };
	str1=fox_realloc(str1,str_len(str1)+len+1,ptr_type(str1));
	assert(oldlen==str_len(str1));
	memcpy((str1+str_len(str1)),str2,len);
	if(ptr_type(str1)==Blob){ *(size_t*)(str1-8)+=len; };
	assert(oldlen+len==str_len(str1));
	assert(!str1[str_len(str1)]);
	return str1;
};
static char* vec_json(map* mp,int indent){
	if(!mp){ return "[]"; };
	if(!is_vec(mp)){ return json(mp,indent); };
	char* ret=NULL;
	ret=xcat(ret,"[", End);
	int last=map_len(mp);
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i);
		if(!v){ ret=xcat(ret,"null", End); }
		else if(is_str(v)){ ret=xcat(ret,str_quote(v), End); }
		else if(is_num(v)){ ret=xcat(ret,v, End); }
		else if(is_map(v)){ ret=xcat(ret,vec_json(is_map(v),indent ? indent+1 : 0), End); };
		if(i<last-1){ ret=xcat(ret,", ", End); }; };
	ret=xcat(ret,"]", End);
	return ret;
};
char* json(map* mp,int indent){
	if(!is_map(mp)){ return NULL; };
	if(!map_len(mp)){ return "{}"; };
	if(is_vec(mp)){ return vec_json(mp,indent); };
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
	int string_l=1+neg;
	int x=10;
	while(x<=value){
		x*=10;
		string_l++; };
	//int string_l=(value == 0) ? 1 : (int)value.log10() + 1 + neg
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
	int len1=str_len(str);
	int len2=str_len(str1);
	if(!len1 && !len2){ return 1; };
	if(len1!=len2){ return 0; };
//	if !str && !str1 => return 1
//	if !str || !str1 => return 0
	return strcmp(str,str1) ? 0 : 1;
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
	ret->magic=0xD41E;
	return ret;
};
map* new_vec(){
	map* ret=fox_alloc(sizeof(map),Vector);
	ret->type=Vector;
	ret->magic=0xD41E;
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
int map_hash(map* mp, char* id){
	int hash=key_hash(id);
	hash=hash & (map_size(mp)-1);
	return hash;
};
int map_idx(map* mp,char* id){
	return map_has_key(mp,id);
};
int map_has_key(map* mp,char* id){
	if(!id||!mp){ return 0; };
//	assert(mp.is_map())
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
		idx=mp->pairs[idx-1].nextid; };
//		"loop ".px(0)
//		idx.px()
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
//	if !mp.is_map() => ("Error! not map "..mp.to_str().str_quote()).px(); assert(0)
//	assert(mp.is_map())
	if(idx<0 || idx>=mp->len){ return NULL; };
	return mp->type==Vector ? &mp->vars[idx] : &mp->pairs[idx].val;
};
void* map_id(map* mp,int idx){
	if(!is_map(mp)){ return NULL; };
//	if !mp.is_map() => ("Error! not map "..mp.to_str().str_quote()).px(); assert(0)
//	assert(mp.is_map())
	if(idx<0 || idx>=mp->len){ return NULL; };
	return mp->type==Vector ? mp->vars[idx] : mp->pairs[idx].val;
};
int map_len(map* mp){
	if(!mp){ return 0; };
//	assert(mp.is_map())
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
map* is_map(void* v){
//	return v.is_ptr() && ((map*)v)->magic==0xD41E ? v : NULL
	return ptr_type(v)>=Map ? v: NULL;
};
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
	if(!v || !*v){ return 0; };
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
//		"map old key: %s\n".printf(key)	
		return mp; };
//	"map new key: %s\n".printf(key)	
	int reindex=0;
	if(mp->len>=map_size(mp)){
		mp->pairs=map_size(mp) ? fox_realloc((char*)mp->pairs,2*map_size(mp)*sizeof(Mapcell),Cell2)
			: fox_alloc(2*sizeof(Mapcell),Cell2);
//		"map realloc for key:%s\n".printf(key)
		reindex=1;
	};
	mp->pairs[mp->len].val=v;
	mp->pairs[mp->len].id=key;
	mp->pairs[mp->len].nextid=-1;
	mp->len++;
	reindex ? map_reindex(mp) : map_index(mp,mp->len-1);
	return mp;
};
static int map_nextno(map* mp){
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
void init_rand(){ srand(time(NULL)*2817635252); };
void* map_val(map* mp,char* key){
//	if !mp||!key => return NULL
	if(!mp||!key||!is_map(mp)){ return NULL; };
//	assert(mp.ptr_type()==Map)
	int i=map_has_key(mp,key);
	return i ? mp->pairs[i-1].val : NULL;
};
//extern char **environ;
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
	init_gc((void**)argv);
	_globals=new_map();
	signal(SIGSEGV,fox_signal_handler);
	signal(SIGABRT,fox_signal_handler);
	init_rand();
	map* ret=new_vec();
	ret->len=argc;
	ret->vars=(void**)argv;
	add(_globals,"args",ret);
	return ret;
};
map* str_split(char* str,char* by,int limit){
	if(!str){ return NULL; };
	map* ret=new_vec();
	char* head=str;
	char* end=NULL;
	int found=0;
	while((end=str_has(head,by))){
		vec_add(ret,sub_str(head,0,end-head));
		head=end+str_len(by);
		if(limit && ++found==limit-1){ break; };
	};
	vec_add(ret,str_dup(head)); //head.str_dup()
	return ret;
};
char* str_has(char* str,char* sub){
	if(!str||!sub||!is_str(str)){ return NULL; };
	return strstr(str,sub);
};
char* exec_str(char* in){ char* ret=NULL; exec(in,&ret); return ret; };
int main(int argc, char** argv){
	return run(argv_map(argv,argc));
};
