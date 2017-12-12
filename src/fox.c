#line 2 "/web/fox/fox.fox"
/*
NOTE:
	Haven't used LLVM. That would force the whole application to be JITed.
TODO:
	eval case:
	eval << >>
CONSIDER:
	sym(char*);
*/
#include "fox.h"

#define MAXMEM 10*1024*1024
#define MIN_CHAIN 1
#ifdef PHP_MOD
#undef assert
#define assert(x) if(!(x)){ fox_error("Assert failed!",1); }
#endif


char* strstr(const char* str1,const char* str2);
int chdir(const char* path);
void* invoke(map* v,char* name);
map* reflect();
int max_mem();
int curr_mem();
char* version();
map* args_map();
void* px(void* str,int newline);
void xexit(int val);
void* fox_error(char* msg,int dump);
void* call_php(map* params,char* func);

int cgi(char* infile, char* outfile,char* opts,int keepfiles){
	int ret=cc(infile,outfile, "cgi", opts, keepfiles);
	if(ret){ return ret; };
	write_htaccess();
	return ret;
};
char* write_htaccess(){
	if(is_file(".htaccess")){ return NULL; };
	return write_file(""
	"RewriteEngine On\n"
	"RewriteCond %{REQUEST_FILENAME} !-f\n"
	"RewriteCond %{REQUEST_FILENAME} !-d\n"
	"RewriteRule ^(.+)$ index.cgi [L,QSA]\n"
	""
	"",".htaccess",0,1);
};
int cc(char* infile, char* outfile, char* profile, char* opts, int keepfiles){
	infile=file_rename(infile,NULL,".fox",NULL,NULL,NULL);
	if(!outfile){ outfile=infile; };
	char* in=xstr(infile,".fox", End);
	fox_c(in,xstr(infile,".c", End));
	fox_h(in,xstr(infile,".h", End));
	char* cflags="-std=gnu99 -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -Wno-unused-command-line-argument";
	char* xlibs="-lcrypto -lmarkdown -lcurl -lsqlite3";
	map* switches=xmap(
		"debug", xstr("-O0 -lfox ", cflags, " ", xlibs, " -lfoxmain", End),
		"speed", xstr("-O3 -lfox ", cflags, " ", xlibs, " -lfoxmain", End),
		"size", xstr("-Os -lfox ", cflags, " -lfoxmain", End),
		"static", xstr("-lfoxstatic -lfoxcmdstatic ", cflags, " -fdata-sections -ffunction-sections -Wl,-dead_strip -lfoxmain", End),
		"cgi", xstr("-Os -lfoxstatic -lfoxcgistatic ", cflags, " ", xlibs, " -lfoxmaincgi", End)
	, End);
	profile = (map_val(switches,profile) ? map_val(switches,profile) : map_val(switches,"debug"));
	int ret=exec(
		px(
		xstr("gcc ", infile, ".c -o ", outfile, " ", profile, " ", opts, " -std=gnu99 -Wno-logical-op-parentheses -lm 2>&1", End),1),NULL);
	if(!keepfiles){
		remove((xstr(infile,".c", End)));	
		remove((xstr(infile,".h", End))); };	
	return ret;
};
char* file_dir(char* file){
	int len=str_len(file);
	int i=0;
	for(i=len-1;i>=0;i--){
		if(file[i]=='/'){ break; }; };
	if(i==-1){ return NULL; };
	return sub_str(file,0,i+1);
};
void write_source(){
	source_funcs();
	map* map_1=source_files(); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* infile=map_id(map_1,next1);
		if(str_end(infile,".h")){ continue; };
		fox_c(infile,xstr(file_rename(infile,"src",".fox",NULL,NULL,NULL),".c", End));
		fox_h(infile,xstr(file_rename(infile,"include",".fox",NULL,NULL,NULL),".h", End)); };
	write_foxh(file_rename("fox.h","include",NULL,NULL,NULL,NULL));
	write_meta(file_rename("meta.c","src",NULL,NULL,NULL,NULL));
};
char* file_rename(char* file,char* dir,char* delext,char* addext,char* prefix,char* postfix){
	char* olddir=file_dir(file);
	char* ret=file;
	if(olddir){ ret=sub_str(ret,str_len(olddir),-2147483648); };
	if(delext && str_end(ret,delext)){ ret=sub_str(ret,0,-str_len(delext)); };
	if(prefix){ ret=xstr(prefix,ret, End); };
	if(postfix){ ret=xcat(ret,postfix, End); };
	if(addext){ ret=xcat(ret,addext, End); };
	if(dir){ olddir=dir; };
	return xstr(sane_dir(olddir),ret, End);
};
char* sane_dir(char* dir){ return dir ? xstr(str_rtrim(dir,"/"),"/", End) : NULL; };
char* write_configm4(char* name, char* outfile){
	char* NAME=str_upper(str_dup(name));
	return write_file(xstr("", 
	"PHP_ARG_WITH(", name, ", for ", name, " support,\n", 
	"Make sure that the comment is aligned:\n", 
	"[  --with-", name, "             Include ", name, " support])\n", 
	"if test \"$PHP_", NAME, "\" != \"no\"; then\n", 
	"\n", 
	NAME, "_DIR=\"/usr/local\"\n", 
	"\n", 
	"if test -z \"", xstr("$",NAME, End), "_DIR\"; then\n", 
	"  AC_MSG_RESULT([not found])\n", 
	"  AC_MSG_ERROR([Please reinstall the ", name, " distribution])\n", 
	"fi\n", 
	"\n", 
	"# --with-", name, " -> add include path\n", 
	"PHP_ADD_INCLUDE(", xstr("$",NAME, End), "_DIR/include)\n", 
	"\n", 
	"# --with-", name, " -> check for lib and symbol presence\n", 
	"LIBNAME=fox # you may want to change this\n", 
	"LIBSYMBOL=init_gc # you most likely want to change this \n", 
	"\n", 
	"PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,\n", 
	"[\n", 
	"  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, ", xstr("$",NAME, End), "_DIR/lib, ", NAME, "_SHARED_LIBADD)\n", 
	"  AC_DEFINE(HAVE_", NAME, "LIB,1,[ ])\n", 
	"],[\n", 
	"  AC_MSG_ERROR([wrong fox lib version or lib not found])\n", 
	"],[\n", 
	"  -L", xstr("$",NAME, End), "_DIR/$PHP_LIBDIR -lm\n", 
	"])\n", 
	"\n", 
	"PHP_SUBST(", NAME, "_SHARED_LIBADD)\n", 
	"\n", 
	"  PHP_NEW_EXTENSION(", name, ", ", name, ".c ", name, "php.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)\n", 
	"fi\n", 
	"", 
	"", End),outfile,0,1);
};
char* mem_usage(){
	int runtime=run_time();
	int totaltime=total_time();
	int gctime=gc_time();
	int codetime=runtime-totaltime-gctime;
	return mstr("Memory:%s-%s [%d%% garbage], Pages: %d/%d, GC runs=%d-%d, Time=%d[gc]+%d[code]=%d ms [%d%% gc] GC Max: %d msec",
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
		_gcdata.gcmax/1000
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
char* mstr(char* format, ...){
	char* ret=NULL;
	va_list args;
	va_start(args,format);
	ret=va_str(format,args);
	va_end(args);
	return ret;
};
char* cat_char(char* str, char c){
	if(!str){ return char_str(c); };
	int len=str_len(str); //lima
	str=fox_realloc(str,len+2,String);	
	str[len]=c;
	assert(!str[len+1]);
	return str;
};
char* va_str(char* format,va_list args){
	if(!format){ return NULL; };
	va_list copy;
	va_copy(copy,args);
	int len=vsnprintf(NULL,0,format,args);
	char* ret=new_str(len);
	vsnprintf(ret,len+1,format,copy);
	va_end(copy);
	return ret;
};
void* dx(void* data,char* name,int panic){
	if(name){
		px(xstr(name, "=", End),0); };
	px(to_str(data,"NULL",1),1);
	if(panic==1){ xexit(0); }
	else if(panic==2){ fox_stack_dump(); };
	return data;
};
int neq(char* str,char* str1){ return !str_eq(str,str1); };
int eq(char* str,char* str1){ return !str||!str1||!is_str(str) ? 0 : strcmp(str,str1)==0; };
char* cwd(){
	char* ret=getcwd(NULL,0);
	char* ret2=str_dup(ret);
	free(ret);
	return ret2;
};
char* write_file(char* data,char* filename,int readonly,int verbose){
	if(!filename){ return data; };
	if(str_eq(filename,"-")){ return px(data,1); };
	if(verbose){ px(xstr("-> ", filename, End),1); };
	if(readonly && is_file(filename)){ chmod(filename,0666); };
	FILE* fp=fopen(filename,"w");
	if(!fp){ fox_error(xstr("writting to file ", filename, " failed", End),0); };
	print(data,fp);
	fclose(fp);
	if(readonly){ chmod(filename,0444); };
	return data;
};
char* file_read(char* filename,int error_on_fail){
	assert(filename);
	FILE* fp=fopen(filename,"r");
	size_t size=0;
	size_t read=0;
	if(!fp){ return error_on_fail ? fox_error(xstr("Can't read file ", filename, End),1) : NULL; };
	fseek(fp,0,SEEK_END);
	size=ftell(fp);
	char* ret=new_blob(size);
	fseek(fp,0,SEEK_SET);
	read=fread(ret,1,size,fp);
	fclose(fp);
	if(read!=size){ return NULL; };
	assert(!ret[str_len(ret)]);
	assert(str_len(ret)==size);
	return ret;
};
char* read_stdin(int size,char* terminator){
	char* ret=NULL;
	char buff[1024+1]={0};
	int block=1024;
	if(size && size<block){
		block=size; };
	int done=0;
	while((done=fread(buff,1,block,stdin))){
		buff[done]='\0';
		ret=xcat(ret,buff, End);
		if(size){
			size-=done;
			if(size<=0){ break; };
			block=min(size,1024); };
	};
	return ret;
};
char* str_unquote(char* str){
	if(!str||!*str){ return NULL; };
	if(str_eq(str,"\"\"") || str_eq(str,"''")){ return ""; };
	if(*str=='\'' || *str=='"'){ str=sub_str(str,1,-1); };
	if(!str_chr(str,'\\')){ return str; };
	char* temp=str_dup(str);
	char* ret=temp;
	while(*str){
		int other=0;
		if(*str=='\\'){
			str++;
			if(!*str){ break; };
			switch(*str){
				case 'n': *temp='\n'; break;
				case 'r': *temp='\r'; break;
				case 't': *temp='\t'; break;
				case '0': *temp='\0'; break;
				default: other=1; break; };
			other=!other; };
		if(!other){ *temp=*str; };
		temp++;
		str++; };
	*temp=0;
	return ret;
};
char* str_escape(char* head){
	if(!head){ return ""; };
	int len=str_len(head);
	assert(len<4000);
	int extra=0;
	char c;
	char* str=head;
	char* quotable="\\\"";
	char* quoteto="\\\"";
	while((c=*str++)) {if(strchr(quotable,c)){ extra++; };};
	assert(extra<=len);
	char* ret=new_str(str_len(head)+extra);
	str=head;
	int i=0;
	char* fox_at=NULL;
	while((c=*str++)){
		if(!(fox_at=strchr(quotable,c))){ ret[i++]=c; continue; };
		ret[i++]='\\';
		ret[i++]=quoteto[fox_at-quotable];
		assert(i<len+extra+2);
	};
	assert(!ret[i]);
	return ret;
};
char* str_end(char* str,char* end){
	if(str_len(str)<str_len(end)){ return NULL; };
	return strcmp(str+str_len(str)-str_len(end),end)==0 ? str : NULL;
};
int str_start(char* str,char* start){ return str && start && is_str(str) && strncmp(str,start,strlen(start))==0; };
char* str_chr(char* str,char c){ return !str||!c||!is_str(str) ? NULL : strchr(str,c); };
char* str_ltrim(char* str,char* chars){
	if(!str || !*str){ return NULL; };
	return str+lchars(str,chars);
};
char* str_rtrim(char* str,char* chars){
	if(!str || !*str){ return NULL; };
	int r=rchars(str,chars);
	if(!r){ return str; };
	return sub_str(str,0,str_len(str)-r);
};
char* str_trim(char* str,char* chars){
	int l=lchars(str,chars);
	int r=rchars(str,chars);
	if(!l && !r){ return str; };
	return sub_str(str,l,str_len(str)-l-r);
};
int line_isempty(char* line){
	while(*line && *line!='\n' && *line!='\r'){ if(*line!=' ' && *line!='\t') {return 0;} line++; };
	return 1;
};
int line_len(char* line){
	int ret=0;
	while(*line && *line!='\n' && *line!='\r'){ ret++; line++; };
	if(*line=='\r'){ ret++; line++; };
	if(*line=='\n'){ ret++; };
	return ret;
};
int lchars(char* str,char* chars){
	if(!str||!chars){ return 0; };
	int ret=0;
	while(str[ret]){
		if(strchr(chars,str[ret])){ ret++; }
		else {break;};
	};
	return ret;
};
int rchars(char* str,char* chars){
	if(!str||!chars){ return 0; };
	int ret=0;
	int len=str_len(str);
	while(ret<len){
		if(strchr(chars,str[len-ret-1])){ ret++; }
		else {break;};
	};
	return ret;
};
int has_str(char* str,char* substr){
	if(!str||!substr){ return 0; };
	int ret=0;
	while(*str){
		if(str_start(str,substr)){ ret++; };
		str++; };
	return ret;
};
char* str_replace(char* str,void* find,void* replace){
	if(!str || !find){ return str; };
	assert(is_str(str));
	if(is_str(find)){
		if(!str_has(str,find)){ return str; };
		assert(is_str(replace));
		find=xmap((find) , replace, End); };
	assert(is_map(find));
	int dels=0;
	int adds=0;
	for(int i2=next(find,-1,NULL,NULL); has_id(find,i2); i2++){ void* v=map_id(find,i2); char* k=map_key(find, i2);
		assert(is_str(k));
		if(is_i(v)){ set(find,i2,(v=int_str(is_int(v)))); };
		assert(is_str(v));
		int i=has_str(str,k);
		if(!i){ continue; };
		dels+=i*str_len(k);
		adds+=i*str_len(v); };
	if(!dels){ return str; };
	char* ret=new_str(str_len(str)+adds-dels-1);
	char* temp=str;
	char* temp2=ret;
	for(;*temp;temp++,temp2++){
		int found=0;
		for(int  i3=next(find,-1,NULL,NULL); has_id(find, i3);  i3++){ void* v3=map_id(find, i3); char*  k3=map_key(find,  i3);
			if(str_start(temp,k3)){
				strcat(temp2,v3);
				temp2+=strlen(v3)-1;
				temp+=strlen(k3)-1;
				found=1;
				break; }; };
		if(!found){ *temp2=*temp; }; };
	return ret;
};
char* outx(char* str){
	px(str,1);
	xexit(0);
	return NULL;
};
char* flush_out(){ char* ret=map_val(_globals,"out"); add(_globals,"out",NULL); return ret; };
map* xadd(map* mp,...){
	int type=ptr_type(mp);
	va_list args;
	va_start(args,mp);
	while(1){
		if(type==Map){
			char* n=va_arg(args,char*);
			if(n==End){ break; };
			void* v=va_arg(args,void*);
			add(mp,n,v);
		}else{
			void* v=va_arg(args,void*);
			if(v==End){ break; };
			vec_add(mp,v); }; };
	va_end(args);
	return mp;
};
int is_numeric(char* str){
	if(!str){ return 0; };
	int deci=0;
	for(;*str;str++){ if((*str<'0' || *str>'9') && (*str!='.' || deci++)){ return 0; }; };
	return 1;
};

map* to_map(void* val){ return is_map(val) ? val : str_map(to_str(val,"",0),Map); };
map* del_index(map* mp,int idx){
	if(!mp){ return mp; };
	assert(idx>=0 && idx<mp->len);
	char* id=map_key(mp,idx);
	if(!id){
		assert(0);
		return mp; };
	int next=mp->pairs[idx].nextid;
	mp->pairs[idx].nextid=-1;
	int i=key_hash(id) & (map_size(mp)-1);
	if(mp->pairs[i].hkey==idx+1){ mp->pairs[i].hkey=next > 0 ? next : 0; } //0=empty for hkey. -1=end for nextid
	else{
		i=mp->pairs[i].hkey;
		int nextid=0;
		while((nextid=mp->pairs[i-1].nextid)!=idx+1 && nextid>0) {i=nextid;};
		if(nextid==idx+1){
			if(i==next){ fox_error(xstr("inf loop with i(",int_str( i), ")=next-1(",int_str( next), "-1)", End),0); };
			mp->pairs[i-1].nextid=next;
		}else {assert(0);}; };
	return mp;
};
map* map_del_key(map* mp,char* key){ return map_del(mp,map_has_key(mp,key)-1,1); };
map* map_del(map* mp,int idx,int len){
	if(!mp||idx<0){ return mp; };
	if(len<=0){ len+=(mp->len-idx); };
	if(len<=0){ return mp; };
	if(ptr_type(mp)==Vector){ return vec_del(mp,idx,len); };
	// don't memset(0) as there is hash indexing info here too.
	while(len--){
		del_index(mp,idx+len);
		mp->pairs[idx+len].val=&skip;
		mp->pairs[idx+len].nextid=0;
		mp->pairs[idx+len].id=(char*)&skip; };
	return mp;
};
map* xjson_map(char* in,int type){ return xjson_map_recurse(&in,type); };
map* xjson_map_recurse(char** line,int type){
	if(!line||!*line||!**line){ return type==Vector ? new_vec() : new_map(); };
	char* str=*line;
	char term='}';
	map* ret=NULL;
	if(*str=='['){
		type=Vector;
		term=']';
		ret=new_vec();
		str++;
	}else if(*str=='{'){
		ret=new_map();
		str++;
	}else{
		ret=type==Vector ? new_vec() : new_map();
		term='\0'; };
	char* last=NULL;
	char* sep=" \t\n\r";
	char* temp=str;
	temp=skip_word(temp,":=,");
	if(strchr(":=",*temp)){
		temp=skip_word(temp,",");
			if(*temp==','){ sep="\t\n\r,"; };
	}else if(*temp==','){ sep="\t\n\r,"; };
	if(term){ sep=xcat(sep,char_str(term), End); };
	while(*str && *str!=term){
		void* name=NULL;
		void* value=NULL;
		if(str==last){ px(str,1); };
		assert(str!=last);
		last=str;
		str=skip_over(str," \t\n\r,]}");
		if(*str=='{'||*str=='['){
			value=xjson_map_recurse(&str,type);
		}else if(*str==':' || *str=='='){
			str=skip_over((str+1)," \t\n\r");
			value=read_value(&str,sep);
		}else if(type==Vector){
			value=read_upto(&str,sep);
		}else{
			name=read_upto(&str," \t\n\r,=:]}");
			str=skip_over(str," \n\r\t");
			if(str_chr(":=",*str)){
				str=skip_over((str+1)," \n\r\t");
				value=read_value(&str,sep);
			}else{
				if(type!=Keys){
					value=name; }; }; };//.str_title()
		map_add_pair(ret,name,value,type); };
	if(*str==term){ str++; };
//	ret.map_add_pair(name,value,type)
//	if type!=Vector && ret.type && ret.name => ret=xmap(ret.name,ret)
	*line=str;
	return ret;
};
int str_level(char* line){
	if(!line||!is_str(line)){ return 0; };
	int ret=0;
	while(*line=='\t'||*line==' '){ ret++; line++; };
	return ret;
};
int block_level(char* str){
	int ret=0;
	if(!str){ return 0; };
	while(*str){
		if(*str=='\n'||*str=='\r'){ ret=0; }
		else if(*str=='\t'||*str==' '){ ret++; }
		else {return ret;};
		str++;
	};
	return ret;
};
void* marked_str(char* str,char* name){
	if(!str){ return NULL; };
	str=str_trim(str," \t");
	if(str_eq(str,"\"\"")){ return ""; };
	if(!str_len(str)){ return NULL; };
	if(str_eq(str,"NULL")){
		return NULL;
	};
	if(str[0]=='{'){
		if(str[str_len(str)-1]=='}'){ str[str_len(str)-1]='\0'; };
		map* mp=xjson_map(str+1,Map);
		return mp;
	};
	if(is_numeric(str)){ return int_var(stoi(str)); };
	if(str[0]=='['){
		if(str[str_len(str)-1]==']'){ str[str_len(str)-1]='\0'; };
		map* mp=xjson_map(str+1,Vector);
		return mp;
	};
	if(str[0]=='\\'){ return sub_str(str,1,-2147483648); };
	if(str[0]==':'){
		str++;
		char* thealpha=read_alpha(&str);
		return map_merge(xmap("type", thealpha,"name", name, End),xjson_map(str+1,Index)); };
	return str;
};
map* map_add_pair(map* mp,void* name,void* value,int type){
	if(!name && !value){ return mp; };	

	if(str_eq(name,"null")){ name=NULL; }
	else if(is_numeric(name)){ name=int_var((to_int(name)+1)); }
	else {name=str_unquote(name);};

	if(name && value && *(char*)value==':'){
		if(!map_len(mp)){
			return xadd(mp,"type",sub_str(value,1,-2147483648),"name",name, End);
		}else{
			char* str=value;
			str++;
			char* thealpha=read_alpha(&str);
			value=map_merge(xmap("type", thealpha,"name", name, End),xjson_map(str+1,Index)); };
	};
	if(str_eq(value,"null")){ value=NULL; }
	else if(is_numeric(value)){ value=int_var(to_int(value)); }
	else {value=str_unquote(value);};

	if(type==Vector){ return vec_add(mp,value); };
	return add(mp,name,value);
};
map* add_name_val(map* mp,char* str,char** name,char** val,int type){
	char* name1=*name;
	char* val1=*val;
	if(!name1 && !val1){ return mp; };
	*name=NULL;
	*val=NULL;
	void* v=NULL;
	if(type==Vector){
		char* s=str_trim(sub_str(name1,0,str-name1)," \t\n\r");
		return vec_add(mp,marked_str(s,NULL));
	};
	if(val1){
		name1=str_trim(name1," \t");
		name1=str_replace(name1,"\\=","=");
		if(!str_len(name1)){ name1=NULL; };
		v=marked_str(sub_str(val1,0,str-val1),name1);
	}else if(name1){ v=marked_str(sub_str(name1,0,str-name1),NULL); name1=NULL; };
	if(type==Index && !str_len(name1)){ name1=to_str(v,"",0); }
	else if(type==Keys && !name1){
		name1=to_str(v,"",0);
		v=NULL; };
	map_add(mp,name1,v);
	return mp;
};
char* read_as_block(char** from,int level,char* terminator){
	char* str=*from;
	char* ret=NULL;
	while(*str){
		if(str_start(str,"//")){ str+=line_len(str); continue; };
		int tabs=str_level(str);
		int len=line_len(str);
		if(tabs>=level||line_isempty(str)){
			ret=xcat(ret,len-level>0 ? sub_str(str,level,len-level) : "\n", End);
		}else {break;};
		str+=len; };
	if(str_eq(str_trim(sub_str(str,str_level(str),line_len(str)-str_level(str))," \t\n\r"),terminator)){
		int len2=line_len(str);
		str+=len2; };
	*from=str;
	if(str_len(ret) && ret[str_len(ret)-1]=='\n'){ ret=sub_str(ret,0,-1); };
	return ret;
};
map* block_map(char** from,int inlevel,int maptype){
	if(!from){ return NULL; };
	map* ret=NULL;
	if(maptype==Vector){ ret=new_vec(); }
	else {ret=new_map();};
	char* str=*from;
	if(!str_len(str)){ return NULL; };
	int level=0;
	int iscomment=0;
	char* blk=NULL;
	char* name=NULL;
	char* val=NULL;
	for(;*str;str++){
		if(*str=='\n'||*str=='\r'){
			add_name_val(ret,str,&name,&val,maptype);
			level=0;
			iscomment=0;
			blk=NULL;
			continue; };
		if(!name && !val){
			if(iscomment){ continue; };
			if(!blk){ blk=str; };
			if(*str=='/' && str[1]=='/'){ str++; iscomment=1; continue; };
			if(*str=='\t'){ level++; continue; };
			if(level<inlevel){
				*from=blk;
				return ret; };
			if(level>inlevel){
				char* s=is_str(map_id(ret,ret->len-1));
				if(str_eq(s,"|")){ set_map(read_as_block(&blk,inlevel+1,"-"),ret,ret->len-1); }
				else if(s && str_start(s,"---")){ set_map(read_as_block(&blk,inlevel+1,s),ret,ret->len-1); }
				else if(ptr_type(map_id(ret,ret->len-1))==Vector){ set_map(block_map(&blk,inlevel+1,Vector),ret,ret->len-1); }
				else if(ptr_type(map_id(ret,ret->len-1))==Map){ set_map(block_map(&blk,inlevel+1,Map),ret,ret->len-1); }
				else if(maptype==Vector){
					continue;
				}else{
					if((str_len(s) && is_int(map_key(ret,ret->len-1)))){ change_key(ret,ret->len-1,s); };
					set_map(block_map(&blk,inlevel+1,maptype),ret,ret->len-1); };
				str=blk-1;
				level=0;
				iscomment=0;
				blk=NULL;
				name=NULL;
				val=NULL;
				continue;
			};
			if(maptype!=Vector && *str=='='){ val=str+1; continue; };
			name=str;
		};
		if(!val){
			if(*str=='\\'){ str++; continue; };
			if(maptype!=Vector && *str=='='){
				name=sub_str(name,0,str-name);
				val=str+1;
				continue; };
			continue; };
		continue; };
	add_name_val(ret,str,&name,&val,maptype);
	*from=str;
	return ret;
};
map* xkeys(char* str){ return xjson_map(str,Keys); };
map* xindex(char* str){ return xjson_map(str,Index); };
map* xarray(char* str){ return str_map(str,Vector); };
map* str_vec(void* str){
	if(is_map(str)){ return str; };
	return str ? str_map(str,Vector) : NULL;
};
void cache_blast(char* type){
	add(add_key(_globals,"cache",Map),type,new_map());
};
void* cache(void* key,char* type,void* out){
	if(is_str(key)){ key=int_var(str_hash((unsigned char*)key)); }
	else if(!key){ key=int_var(0); };
	if(!out){
		return map_val(map_val(map_val(_globals,"cache"),type),key); };
	if(!map_has_key(map_val(map_val(_globals,"cache"),type),key)){ add(add_key(add_key(_globals,"cache",Map),type,Map),key,NULL); return out; };
	add(add_key(add_key(_globals,"cache",Map),type,Map),key,out);
	return out;
};
map* str_map(char* str,int type){
	if(is_map(str)){ return is_map(str); };
	if(!str_len(str)){ return xjson_map(str,type); };
	void* ret=cache(str,"str_map",NULL);
	if(ret){ return ret; };
	if((!strchr(str,'\n') && !strchr(str,'\r'))){ ret=xjson_map(str,type); }
	else {ret=block_map(&str,block_level(str),type);};
	return cache(str,"str_map",ret);
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
int total_kb(){ return _gcdata.max_mem/1024; };
int page_maps(char* title){
	printf("%s\n",title);
	each_mem(pg,mem_i) {page_map(pg);};
	return 0;
};
map* vec_del(map* mp,int from,int len){
	if(!map_len(mp)){ return NULL; };
	assert(is_vec(mp));
	if(from<0){ from+=mp->len; };
	if(len<=0){ len+=mp->len; };
	assert(from>=0 && len>0);
	assert(from<mp->len);
	//assert(from+len<=mp->len);// fix it by, len=mp->len-from
	if(from+len>mp->len){ len=mp->len-from; };
	int rest=mp->len-(from+len);
	if(rest){ for(int i=0;i<len;i++) {set(mp,i+from,&skip);}; }
	else {mp->len-=len;};
	return mp;
};
map* vec_shift(map* mp,int idx,int len,int offset){
	if(!mp){ return mp; };
	if(mp->len<idx+len){ len=mp->len-idx; };
	if(!len){ return mp; };
	int ofrom=offset<0 ? offset+idx : idx+len;
	int olen=abs(offset);
	assert(ofrom>=0 && ofrom<mp->len);
	map* old=vec_sub(mp,ofrom,olen);
	memmove(mp->vars+ofrom,mp->vars+idx,olen*sizeof(void*));
	int ointo=offset<0 ? ofrom+len : idx;
	memcpy(mp->vars+ointo,old->vars,old->len*sizeof(void*));
	return mp;
};
map* vec_sub(map* mp,int from,int len){
	if(from<0){ from=0; };
	if(!mp){ return NULL; };
	if(ptr_type(mp)!=Vector){ return mp; };
	if(from>mp->len){ from=mp->len; len=0; }
	else if(from+len>mp->len){ len=mp->len-from; };
	if(!len){ len=mp->len-from; };
	if(len<=0){ return NULL; };
	map* ret=new_vec();
	vec_realloc(ret,len);	
	memcpy(ret->vars,mp->vars+from,sizeof(void*)*len);
	ret->len=len;
	return ret;
};
map* vec_slice(map* mp,int from,int len){
	map* ret=vec_sub(mp,from,len);
	vec_del(mp,from,len);
	return ret;
};
map* map_splice(map* mp,int from,int del,map* ad){
	map* ret=new_map();
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){ void* v=map_id(mp,idx); char* k=map_key(mp, idx);
		if(idx==from){
			for(int idx2=next(ad,-1,NULL,NULL); has_id(ad,idx2); idx2++){ void* v2=map_id(ad,idx2); char* k2=map_key(ad, idx2);
				if(is_str(k2)){
					if(!map_val(mp,k2)){ add(ret,k2,v2); };
				}else {vec_add(ret,v2);}; }; };
		if(idx>=from && idx<from+del){ continue; };
		if(is_str(k)){
			if(!map_val(mp,k)){ add(ret,k,v); };
		}else {vec_add(ret,v);}; };
	return ret;
};
void* vec_pop(map* mp){
	if(!mp||!mp->len){ return NULL; };
	void* ret=map_id(mp,mp->len-1);
	vec_del(mp,mp->len-1,1);
	return ret;
};
map* vec_splice(map* mp,int from,int del,map* add){
	if(!mp){ return mp; };
	assert(ptr_type(mp)==Vector);
	if(from < 0){ from+=mp->len; };
	assert(del>=0);
	assert(from>=0 && from<=mp->len);

	int inc=add ? add->len : 0;
	int len=mp->len;

	if(from>len){ from=len; del=0; }
	else if(from+del>len){ del=len-from; };

	int offset=inc-del;
	if(offset>0){ vec_realloc(mp,offset); };

	int moved=len-(from+del);
	if(offset && moved){ memmove(mp->vars+from+inc,mp->vars+from+del,sizeof(void*)*moved); };
	if(add){ memcpy(mp->vars+from,add->vars,add->len*sizeof(void*)); };
	mp->len+=offset;
	if(offset<0){ memset(mp->vars+mp->len,0,sizeof(void*)*offset*-1); };
	return mp;
};
char* str_has(char* str,char* sub){
	if(!str||!sub||!is_str(str)){ return NULL; };
	return strstr(str,sub);
};
void gotoxy(int x,int y){
	_printed=1;
	printf("%c[%d;%df",0x1B,y,x);
};
int floor_pow2(int i){
	if(!i){ return 0; };
	int ret=1;
	while(i>>=1) {ret<<=1;};
	return ret;
};
map* vec_dup(map* mp){
	if(!mp||!mp->len){ return new_vec(); };
	map* ret=new_vec();
	vec_realloc(ret,mp->len);
	memcpy(ret->vars,mp->vars,mp->len*sizeof(void*));
	ret->len=mp->len;
	return ret;
};
map* map_dup(map* mp){
	if(!mp){ return NULL; };
	map* ret=new_map();
	if(!map_len(mp)){ return ret; };
	assert(ptr_type(mp)==Map);
	ret->pairs=fox_alloc(map_size(mp)*sizeof(Mapcell),Cell2);
	memcpy(ret->pairs,mp->pairs,map_size(mp)*sizeof(Mapcell));
	ret->len=mp->len;
	return ret;
};
map* map_merge(map* mp1,map* mp2){
	if(!mp1){ mp1=new_map(); };
	for(int idx=next(mp2,-1,NULL,NULL); has_id(mp2,idx); idx++){ void* v =map_id(mp2,idx); char* k=map_key(mp2, idx); map_add(mp1,is_int(k) ? NULL : k,v); };
	return mp1;
};
map* vec_merge(map* mp1,map* mp2){
	if(!mp2){ return mp1; };
	if(!mp1){ return vec_dup(mp2); };
	vec_realloc(mp1,mp2->len);
	memcpy(((char*)mp1->vars)+mp1->len*sizeof(void*),mp2->vars,mp2->len*sizeof(void*));
	mp1->len+=mp2->len;
	return mp1;
};
int utest(char* found,char* expect,char* in,char* title){
	if(expect==found){ return 0; };
	expect=to_str(expect,"",0);
	found=to_str(found,"",0);
	if(!str_len(expect) && !str_len(found)){ return 0; };
	if(str_eq(expect,found)){ return 0; };
	px(xstr("\n", 
	"TEST: ", title, "\n", 
	"[IN]_______________________\n", 
	in, "\n", 
	"[CORRECT]__________________\n", 
	expect, "\n", 
	"[WRONG]\n", 
	found, "\n", 
	"[DIFF]_____________________\n", 
	str_quote(expect), "\n", 
	str_quote(found), "\n", 
	"_______________________________________________[ERROR]\n", 
	"", End),1);
	return -1;
};
map* map_compact(map* mp){
	if(!mp || ptr_type(mp)!=Map){ return mp; };
	int off=0;
	int i=0;
	for(i=0;i+off<mp->len;i++){
		while(i+off<mp->len && mp->pairs[i+off].id==(char*)&skip) {off++;};
		if(i+off>=mp->len){ break; };
		if(off){ mp->pairs[i]=mp->pairs[i+off]; }; };
	mp->len-=off;
	if(off){ map_reindex(mp); };
	return mp;
};
map* vec_reverse(map* mp){
	if(!mp){ return mp; };
	int len=mp->len;
	for(int i=0; i<len/2; i++){
		void* temp=mp->vars[len-i-1];
		mp->vars[len-i-1]=mp->vars[i];
		mp->vars[i]=temp; };
	return mp;
};
map* vec_compact(map* mp){
	if(!mp || ptr_type(mp)!=Vector){ return mp; };
	int off=0;
	int i=0;
	for(i=0;i+off<mp->len;i++){
		while(i+off<mp->len && mp->vars[i+off]==&skip) {off++;};
		if(i+off>=mp->len){ break; };
		if(off){ mp->vars[i]=mp->vars[i+off]; };
	};
	while(i<mp->len) {mp->vars[i++]=NULL;};
	mp->len-=off;
	return mp;
};
map* change_key(map* mp,int idx,char* id){
	if(!mp){ return mp; };
	assert(idx>=0 && idx<=mp->len);
	assert(ptr_type(mp)==Map);
	if(key_eq(mp,idx,id)){ return mp; };
	del_index(mp,idx);
	map_del(mp,map_has_key(mp,id)-1,1);
	mp->pairs[idx].id=id;
	map_index(mp,idx);
	return mp;
};
char* old_decl_type(map* mp,int idx){
	int from=stm_start(mp,idx,"{};,().>:=><!&-+/?");
	for(int i=from;i<idx;i+=2){
		if(is_word(map_id(mp,i),"int long double float char void struct map static extern unsigned register mempage size_t time_t const")){
			return toks_c(vec_sub(mp,from,idx-from-1)); }; };
	return NULL;
};
map* add_id(map* mp,int idx){
	if(!is_map(map_id(mp,idx))){ set(mp,idx,new_map()); };
	return map_id(mp,idx);
};
map* add_key(map* mp,char* k,int type){
	if(!is_map(map_val(mp,k))){ add(mp,k,type==Vector ? new_vec() : new_map()); };
	return map_val(mp,k);
};
char* str_upper(char* s){
	char* ret=s;
	for(;*s;++s) {*s=toupper(*s);};
	return ret;
};
char* str_lower(char* s){
	char* ret=s;
	for(;*s;++s) {*s=tolower(*s);};
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
char* toks_str(map* mp){
	char* ret=NULL;
	for(int next1=next(mp,-1,NULL,NULL); has_id(mp,next1); next1++){ void* val=map_id(mp,next1); ret=xcat(ret,is_map(val) ? toks_str(val) : val, End); };
	return ret;
};
char* map_join(map* mp,char* joiner){
	if(!is_map(mp)){ return to_str(mp,"",0); };
	char* ret=NULL;
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){ void* v=map_id(mp,idx);
		if(idx){ ret=xcat(ret,joiner, End); };
		if(is_map(v)){ ret=xcat(ret,map_join(v,joiner), End); }
		else {ret=xcat(ret,to_str(v,"",0), End);};
	};
	return ret;
};
char fox_at(char* str,int idx){
	if(!str || !is_str(str)){ return '\0'; };
	if(idx<0) {idx+=str_len(str);};
	return idx<str_len(str) ? str[idx] : '\0';
};
char* indent_str(int i){
	char* ret=new_str(i+1);
	memset(ret,'\t',i+1);
	ret[0]='\n';
	return ret;
};
int str_chars(char* str,char* chars){
	if(!str || !chars || !is_str(str)){ return 0; };
	int len1=strlen(str);
	int len2=strlen(chars);
	for(int i=0;i<len1;i++){
		for(int j=0;j<len2;j++){
			if(str[i]==chars[j]){ return 1; }; }; };
	return 0;
};
char* map_str(map* mp){ return map_str_indent(mp,0); };
char* map_str_indent(map* mp,int indent){
	char* ret=NULL;
	if(!mp){ return ret; };
	int type=ptr_type(mp);
	char* istr=str_times("\t",indent);
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i); char* k=map_key(mp, i);
		ret=xcat(ret,istr, End);
		if(is_map(v)){
			if(ptr_type(v)!=Vector && map_val(v,"type")){
				ret=xcat(ret,k,"=:",map_val(v,"type"), End);
				for(int i2=next(v,-1,NULL,NULL); has_id(v,i2); i2++){ void* v2=map_id(v,i2); char* k2=map_key(v, i2);
					if(str_eq(k2,"type") || str_eq(k2,"name") && map_val(v,"type")){
						continue;
					}else if(is_map(v2)){
						ret=xcat(ret," ",k2,"=",json(v2,0), End);
					}else if(!v2 || str_eq(k2,v2) || is_int(v2)==1){
						ret=xcat(ret," ",k2, End);
					}else if(is_code(v2)){
						ret=xcat(ret," ",k2,"=",v2, End);
					}else if(is_num(v2)){
						if(is_int(v2)){
							ret=xcat(ret," ",k2,"=#",int_str(is_int(v2)), End);
						}else {is_double(v2);};
							ret=xcat(ret," ",k2,"=#",double_str(is_double(v2)), End);
					}else{
						ret=xcat(ret," ",k2,"=",str_quote(v2), End); }; };
				ret=xcat(ret,"\n", End);
			}else{
				int eq=0;
				if(type==Vector||is_int(k)){ ret=xcat(ret,"=", End); eq=1; }
				else {ret=xcat(ret,k, End);};
				if(ptr_type(v)==Vector){
					if(!eq){ ret=xcat(ret,"=", End); };
					ret=xcat(ret,"[]", End);
				};
				char* bl=map_str_indent(v,indent+1);
				if(bl){ ret=xcat(ret,"\n",bl, End); }; };
		}else if(type==Vector||is_int(k)){
			ret=xcat(ret,to_str(v,"",0),"\n", End);
		}else if(is_i(v)){ ret=xcat(ret,k,"=#",int_str(is_int(v)),"\n", End); }
		else if(is_double(v)){ ret=xcat(ret,k,"=#",double_str(is_double(v)),"\n", End); }
		else if(is_str(v)){
			if(strchr(v,'\n')){
				ret=xcat(ret,k,"=|\n", End);
				map* map_1=str_split(v,"\n",0); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* v2=map_id(map_1,i2); char* k2=map_key(map_1, i2);
					ret=xcat(ret,str_times("\t",indent+1),v2,"\n", End); };
			}else{
				char* marker="\\";
				if(is_alpha(fox_at(v,0),NULL)){ marker=NULL; };
				ret=xcat(ret,k,"=",marker,v,"\n", End); }; };
	};
	return ret;
};
map* dir_files(char* path){
	if(!path){ return NULL; };
	DIR *d=opendir(path);
	if(!d){ return NULL; };
	map* ret=new_vec();
	struct dirent *dir;
	char* padd=NULL;
	if(!str_eq(path,".")){
		padd=path;
		if(fox_at(padd,-1)!='/'){ padd=xcat(padd,"/", End); };
	};
	while((dir=readdir(d))){
		char* name=dir->d_name;
		if(is_word(name,". ..")){ continue; };
		vec_add(ret,xstr(padd, name, End));
	};
	closedir(d);
	return ret;
};
char* end_word(char* str,char* words){
	if(!str||!words){ return NULL; };
	char* word=str_dup(words);
	int total=terminate_words(word);
	for(int i=0;i<total;i++){
		char* ret=str_end(str,word);
		if(ret){ return ret; };
		word+=str_len(word)+1;
	};
	return NULL;
};
char* has_word(char* str,char* words){
	if(!str||!words){ return NULL; };
	char* word=str_dup(words);
	int total=terminate_words(word);
	for(int i=0;i<total;i++){
		char* ret=strstr(str,word);
		if(ret){ return ret; };
		word+=str_len(word)+1;
	};
	return NULL;
};
int is_word(char* word,char* list){
	assert(list);
	assert(is_str(list));
	if(!is_str(word)){ return 0; };
	char* ret=list;
	while((ret=str_has(ret,word))){
		if(ret && (ret==list||ret[-1]==' ') && (ret[strlen(word)]==' '||!ret[strlen(word)])){ return ret-list+1; };
		ret+=strlen(word); };
	return 0;
};
int terminate_words(char* in){//Takes a string and adds NULL between eash words. Returns total number of words terminated with NULL
	if(!in){ return 0; };
	char* s=in;
	int ret=1;
	for(;*s;s++) {if(*s==' '){ ret++; *s='\0'; };};
	return ret;
};
map* map_vec(map* mp){
	if(ptr_type(mp)==Vector){ return mp; };
	map* ret=new_vec();
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i); vec_add(ret,v); };
	return ret;
};
map* toks_keywords(map* mp,char* keywords){
	if(!mp){ return mp; };
	map* ret=new_map();
	map* curr=new_vec();
	char* curr_key=NULL;
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){
		if(is_word(map_id(mp,i),keywords)){
			if(curr->len){
				add(ret,curr_key,toks_align(curr));
				curr=new_vec(); };
			curr_key=map_id(mp,i);
			continue; };
		vec_add(curr,map_id(mp,i)); };
	if(curr->len){ add(ret,curr_key,toks_align(curr)); };
	return ret;
};
map* map_split(map* mp,char* str,int limit){
	if(!mp){ return mp; };
	if(limit==1){ return map_vec(mp); };
	map* ret=new_vec();
	map* curr=new_vec();
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){
		if(str_eq(map_id(mp,i),str)){
			if(curr->len){
				vec_add(ret,curr);
				curr=new_vec(); };
			if(limit==1){ break; };
			limit--;
			continue; };
		vec_add(curr,map_id(mp,i)); };
	if(curr->len){ vec_add(ret,curr); };
	return ret;
};
size_t file_time(char* file){
	struct stat statbuf;
	if(stat(file,&statbuf)==-1){
		perror(file);
		return 0;
	};
	return statbuf.st_mtime;
};
int is_number(char c){ return (c>='0' && c<='9') || c=='.' || c=='-' || c=='+'; };
int is_alphanum(char c,char* others){
	if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_'){ return 1; };
	if(others && strchr(others,c)){ return 1; };
	return 0;
};
int is_alpha(char c,char* others){
	if(c>='a' && c<='z' || c>='A' && c<='Z' || c=='_'){ return 1; };
	if(others && strchr(others,c)){ return 1; };
	return 0;
};
int is_oper(char c){ return strchr("~!@#$%^&*-=+|<>?/\\}]).",c) ? 1 : 0; };
int str_is_oper(char* str){
	if(!str_len(str)){ return 0; };
	while(*str++) {if(!strchr("~!@#$%^&*-=+|:<>?/\\{[(}]),.",str[-1])){ return 0; };};
	return 1;
};
int str_is_num(char* str){
	if(!str_len(str)){ return 0; };
	while(*str++) {if(!is_number(str[-1])){ return 0; };};
	return 1;
};
int is_code(char* str){
	if(!str_len(str)){ return 0; };
	if(!is_alpha(*str,NULL)){ return 0; };
	while(*++str) {if(!is_alphanum(*str,NULL)){ return 0; };};
	return 1;
};
char* read_num(char** in){
	char* str=*in;
	int deci=0;
	str++;
	while(*str && ((*str>='0' && *str<='9')||(*str=='.' && !deci++ && !is_alpha(str[1],NULL)))){
		str++; };
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
char* skip_over(char* in,char* chars){
	while(*in && strchr(chars,*in)) {in++;};
	return in;
};
char* skip_word(char* in,char* seperators){
	if(!in || !*in){ return in; };
	char term='\0';
	int counter=0;
	char opener='\0';
	if(strchr("[({",*in)){ opener=*in; };
	if(strchr("\"'`",*in)){ term=*in; in++; }
	else if(*in=='['){ term=']'; in++; }
	else if(*in=='('){ term=')'; in++; }
	else if(*in=='{'){ term='}'; in++; };
	while(*in){
		if(opener){
			if(strchr("\"'`",*in)){ in=skip_word(in,seperators); continue; }
			else if(*in==opener){ counter++; }; };
		if(*in==term){
			if(!counter){ in++; break; };
			counter--;
		}else if(!term && strchr(seperators,*in)){ break; }
		else if(*in=='\\' && in[1]){ in++; };
		in++; };
	return in;
};
void* read_value(char** str, char* sep){
	if(**str=='{'||**str=='['){ return xjson_map_recurse(str,Map); };
	char* from=*str;
	*str=skip_word(from,sep);
	char* ret=sub_str(from,0,*str-from);
	return ret ? ret : "null";
};
char* read_upto(char** in,char* terminators){
	char* str=*in;
	int isquote=0;
	if(*str=='"'){ terminators="\""; str++; }
	else if(*str=='\''){ terminators="'"; str++; };
	while(*str && !strchr(terminators,*str)){
		if(*str=='\\' && str[1]){ str++; };
		str++; };
	if((*terminators=='\'' || *terminators=='"') && *str==*terminators){ str++; };
	char* from=*in;
	*in=str;
	return sub_str(from,0,str-from);
};
char* read_alpha(char** in){
	char* str=*in;
	str++;
	while(*str && is_alphanum(*str,NULL)) {str++;};
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
char* fox_read_symbol(char** in){
	char* str=*in;
	str++;
	while(*str){
		if(*str=='\\'){
			str+=2;
			continue; };
		if(!is_alphanum(*str,NULL)){ break; };
		str++; };
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
char* fox_read_oper(char** in,char term){
	char* str=*in;
	char* from=*in;
	while(*str && is_oper(*str) && *str!=term) {str++;};
	char* ret=sub_str(from,0,str-from);
	int less=0;
	while(strlen(ret) && !is_word(ret,"~ ! @ # $ % ^ & * - = + | < > ? / \\ } ] ) . != >= <= += -= /= *= || && << >> => ++ -- ** *** **** |= &= >>= <<= ^^ ... -> == === !== .= .. ||= &&=")){
		ret[strlen(ret)-1]='\0';
		less++; };
	*in=str-1-less;
	return ret;
};
char* read_upto_word(char** in,char* upto){
	char* str=*in;
	while(*str && !str_start(str,upto)) {str++;};
	char* from=*in;
	if(*str){
		*in=str+strlen(upto)-1;
		return sub_str(from,0,str-from+strlen(upto));
	}else{
		*in=str-1;
		return sub_str(from,0,str-from);
	};};
char* read_theline(char** in){
	if(!in||!*in||!**in){ return NULL; };
	char* str=*in;
	while(*str && !strchr("\n\r",*str)) {str++;};
	if(*str=='\r' && str[1]=='\n'){ str++; };
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
char* read_newline(char** in){
	char* str=*in;
	while(*str && strchr("\n\r",*str)) {str++;};
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
char* read_space(char** in,char* spaces){
	char* str=*in;
	while(*str && strchr(spaces,*str)) {str++;};
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
map* read_data(char** in){
	char* str=*in;
	char* from=*in;
	while(*str && !str_start(str,"}}")){
		str+=line_len(str);
		str+=str_level(str); };
	if(*str){ str+=2; };
	*in=str-1;
	return data_toks(sub_str(from,0,str-from));
};
char* read_heredoc(char** in){
	char* str=*in;
	int len=line_len(str);
	char* hd=str_trim(sub_str(str,0,len)," \t\n\r");
	char* from=*in;
	int lno=0;
	str+=len;
	int tabs=str_level(str);
	str+=tabs;
	while(*str && !str_start(str,hd)){
		len=line_len(str);
		tabs=str_level((str+len));
		str+=len+tabs; };
	if(*str){ str+=str_len(hd)-1; };
	*in=str;
	return sub_str(from,0,str-from+1);
};
char* read_multistr(char** in){
	char* str=*in;
	char end=*str;
	char* hd=NULL;
	char* from=*in;
	int lno=0;
	char* word_start=NULL;
	str++;
	while(*str){
		if(*str=='\\'){ if(!hd) {str++;}; }
		else if(*str==end){ if(!hd) {break;}; }
		else if(*str=='\n'||*str=='\r'){
			if(!lno && word_start){
				hd=sub_str(word_start,0,str-word_start); };
			lno++;
			word_start=NULL;
		}else if(*str!=' ' && *str!='\t'){
			if(!word_start){
				word_start=str;
				if(hd && lno && str_start(str,hd) && str[str_len(hd)]==end){
					str+=str_len(hd);
					break; }; }; };
		str++; };
	*in=str;
	return sub_str(from,0,str-from+1);
};
char* read_quote(char** in){
	char* str=*in;
	char end=*str;
	str++;
	while(*str){
		if(*str=='\\'){ str++; }
		else if(*str==end){ break; };
		str++;
	};
	char* from=*in;
	*in=str;
	return sub_str(from,0,str-from+1);
};
char* xlog(char* str){
	FILE* fp=fopen("/tmp/log.txt","a+");
	fprintf(fp,"%s\n",str);
	fclose(fp);
	return str;
};
char closing_paren(char c){ switch(c){ case '{': return '}'; case '[': return ']'; case '(': return ')'; default: return '\0'; }; };
char char_at(char* str,int i){
	if(!str || !is_str(str)){ return '\0'; };
	if(!i){ return *str; };
	if(i>0 && i<str_len(str)){ return str[i]; };
	return '\0';
};
char* char_str(char c){
	char ret[2]={0};
	ret[0]=c;
	ret[1]='\0';
	char* str=str_dup(ret);
	assert(str_len(str)<=1);
	return str;
};
int map_has_word(map* mp,char* str){
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){ void* v=map_id(mp,idx); if(str_eq(v,str)){ return idx+1; }; };
	return 0;
};
int is_keyword(char* str){ return is_word(str,"abstract and as break callable case catch class clone const continue declare default do echo else elseif enddeclare endfor endforeach endif endswitch endwhile extends final for foreach def function global goto if implements include include_once instanceof insteadof interface namespace new or private protected public require require_once return static switch throw trait try use while xor yield false true null"); };
char* toks_c(map* mp){
	char* ret=NULL;
	if(!mp){ return ret; };
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){
		if(is_map(map_id(mp,i))){
			ret=xcat(ret,toks_c(map_id(mp,i)), End);
			continue; };
		char* str=is_str(map_id(mp,i));
		assert(str || !is_str(map_id(mp,i)));
		if(!str){ continue; };
		ret=xcat(ret,map_id(mp,i), End); };
	return ret;
};
map* add_ctok(void* data,map* mp,int iscode){
	if(!data){ return mp; };
	assert(data);
	assert(ptr_type(mp)==Vector);
	int incode=!(mp->len%2);
	if(incode && iscode){
		vec_add(mp,NULL);
		vec_add(mp,data);
	}else if(incode && !iscode){ vec_add(mp,data); }
	else if(!incode && !iscode){
		if(mp->len){
			char* old=is_str(map_id(mp,mp->len-1));
			mp->vars[mp->len-1]=xstr(old,data, End);
		}else {vec_add(mp,data);};
	}else if(!incode && iscode){ vec_add(mp,data); };
	return mp;
};
map* c_tokenizer(char** line,char term){
	if(!line||!*line||!**line){ return NULL; };
	char* head=*line;
	int headlen=str_len(head);
	char* expected_end=head+str_len(head);
	char* str=*line;
	map* mp=new_vec();
	char* space=NULL;
	int code=0;
	char* last=NULL;
	while(*str && *str!=term){
		assert(last!=str);
		assert(str_len(head)==headlen);
		last=str;
		if(str_start(str,"//")){ add_ctok(read_theline(&str),mp,0); }
		else if(str_start(str,"/*")){ add_ctok(read_upto_word(&str,"*/"),mp,0); }
		else if(str_start(str,"#")){ add_ctok(read_theline(&str),mp,0); }
		else if(strchr(" \t",*str)){ add_ctok(read_space(&str," \t"),mp,0); }
		else if(strchr("\n\r",*str)){ add_ctok(read_newline(&str),mp,0); }
		else if(strchr("\"`",*str)){ add_ctok(read_quote(&str),mp,1); }
		else if(*str=='\''){ add_ctok(read_multistr(&str),mp,1); }
		else if(str_start(str,"{{")){
			if(!(mp->len%2)){ vec_add(mp,NULL); };
			vec_merge(mp,read_data(&str));
		}else if(str_start(str,"---")){ add_ctok(read_heredoc(&str),mp,1); }
		else if(*str==':'){ add_ctok(fox_read_symbol(&str),mp,1); }
		else if(is_oper(*str)){ add_ctok(fox_read_oper(&str,term),mp,1); }
		else if(*str>='0' && *str<='9'){ add_ctok(read_num(&str),mp,1); }
		else if(is_alpha(*str,NULL)){ add_ctok(read_alpha(&str),mp,1); }
		else if(strchr("([{",*str)){
			char c=*str;
			str++;
			add_ctok(char_str(c),mp,1);
			add_ctok(c_tokenizer(&str,closing_paren(c)),mp,1);
			if(*str){ add_ctok(char_str(closing_paren(c)),mp,1); };
		}else if(','==*str){ add_ctok(sub_str(str,0,1),mp,1); }
		else if(';'==*str){ add_ctok(sub_str(str,0,1),mp,1); };
		if(*str){ str++; }; };
	assert(term || str==expected_end);
	*line=str;
	return mp;
};
int func_dot(map* mp,int idx){
	idx++;
	if(!str_eq(map_id(mp,idx),"(")){ return idx; };
	if(!is_code(is_str(map_id(mp,idx-2)))){ return idx; };
	if(str_eq(map_id(mp,idx-4),".")){ return idx; };
	if(is_keyword(is_str(map_id(mp,idx-2)))){ return idx; };
	map* params=is_map(map_id(mp,idx+2));
	if(!params->len){ return idx; };
	if(ptr_type(map_id(params,1))!=String && !is_code(is_str(map_id(params,1)))){ return idx; };
	int len=2;
	while(1){
		char* s=is_str(map_id(params,len+1));
		if(is_word(s,"( [ {")){ len+=6; }
		else if(is_word(s,". ->") || is_code(s)){ len+=2; }
		else {break;};
	};
	if(params->len>len && !str_eq(map_id(params,len+1),",")){ return idx; };
	map* out=vec_slice(params,1,len-1);
	vec_del(params,0,1);
	if(params->len>len){ vec_del(params,len,2); };
	vec_compact(params);
	xadd(out,NULL,".",NULL, End);
	vec_splice(mp,idx-1-1,0,out);
	vec_compact(mp);
	return idx-3;
};
map* multiline_parts(char* str){
	map* lines=str_split(sub_str(str,1,-1),"\n",0);
	int indent=0;
	char* end=str+str_len(str)-1;
	while(end>str){ if(strchr("\t\n\r",*end)) {break;} end--; };
	while(end>str){ if(*end!='\t') {break;} end--; indent++; };
	char* tabs=null_str(str_times("\t",indent));
	map* parts=new_vec();
	for(int i=next(lines,-1,NULL,NULL); has_id(lines,i); i++){ void* v=map_id(lines,i);
		int cut=str_level(v);
		if(cut>indent){ cut=indent; };	
		char* nl="";
		if(i<map_len(lines)-1){
			nl="\\n"; };
		if(i){ vec_add(parts,xstr("\n",tabs, End)); };
		vec_add(parts,mstr("\"%s%s\"",str_escape(sub_str(v,cut,-2147483648)),nl, End)); };
	return parts;
};
map* heredoc_parts(char* str){
//	if !str.is_str() => return NULL
	if(!str || !is_str(str) ||!*str){ return NULL; };
	if(!((*str=='\'' && strchr(str,'\n'))||(*str=='"' && str[1]!='"' && strchr(str,'\n'))||str_start(str,"---"))){ return NULL; };
	if(str_chr("\"`",*str)){ return multiline_parts(str); };
	map* lines=str_split(str,"\n",0);
	int indent=0;
	char* end=str+str_len(str)-1;
	while(end>str){ if(strchr("\t\n\r",*end)) {break;} end--; };
	while(end>str){ if(*end!='\t') {break;} end--; indent++; };
	char* tabs=null_str(str_times("\t",indent));
	map* parts=xvec("\"\"", End);
	for(int i=next(lines,-1,NULL,NULL); has_id(lines,i); i++){ void* v=map_id(lines,i);
		if(i==0){ continue; };
		if(i==lines->len-1){ continue; };
		int cut=str_level(v);
		if(cut>indent){ cut=indent; };	
		char* nl="\\n";
		if(i==lines->len-2){ nl=""; };
		vec_add(parts,xstr("\n",tabs, End));
		vec_add(parts,mstr("\"%s%s\"",str_escape(sub_str(v,cut,-2147483648)),nl, End)); };
	xadd(parts,xstr("\n",tabs, End),"\"\"", End);
	return parts;
};
map* heredoc_str(map* mp){
	for(int idx=1; idx<map_len(mp); idx+=2){
		if(is_map(map_id(mp,idx))){ heredoc_str(map_id(mp,idx)); };
		map* parts=heredoc_parts(map_id(mp,idx));
		if(!parts){ continue; };
		vec_splice(mp,idx,1,parts); };
	return mp;
};
map* dot_each(map* mp){
	int temp=0;
	for(int idx=1; idx<=mp->len; idx+=2){
		if(is_map(map_id(mp,idx))){ dot_each(map_id(mp,idx)); };
		if(idx<2 || !is_word(is_str(map_id(mp,idx-2)),"each each_mem")){
			continue; };
		map* params=toks_split(is_map(map_id(mp,idx+2)),",",0);
		map* newp=toks_split(is_map(map_id(mp,idx+2)),",",0);
		if(map_len(map_id(params,0))>=4){
			temp++;
			char* temp_name=xstr("map_",int_str(temp), End);
			map* pre=vec_compact(vec_del(x_map(xcat("map* ",xstr(temp_name,"=", End),toks_c(map_id(params,0)),"; ", End)),0,1));
			set(newp,0,x_map(temp_name));
			set(mp,idx+2,toks_join(newp,","));
			vec_splice(mp,idx-2,0,pre);
			assert(str_eq(map_id(mp,map_len(pre)-3+idx)," ")); };
		if(str_eq(map_id(mp,idx-2),"each")){
			char* next1="next1";
			char* start="-1";
			char* upto=NULL;
			char* step="++";
			if(map_id(map_id(params,3),1)){
				map* incs=toks_keywords(map_id(params,3),"= to step");
				next1=toks_c(map_id(incs,0));
				if(map_val(incs,"=")){
					start=str_trim(toks_c(map_val(incs,"="))," \t\n\r");
					if(is_numeric(start)){ start=int_str((to_int(start)-1)); }
					else {start=xcat(start,"-1", End);};
					if(map_val(incs,"to")){
						upto=xstr(xstr(" ", next1, "<=", End),str_trim(toks_c(map_val(incs,"to"))," \t\n\r")," &&", End); };
					if(map_val(incs,"step")){
						step=str_trim(toks_c(map_val(incs,"step"))," \t\n\r");
						if(is_numeric(step) && to_int(step)==1){ step="++"; }
						else if(step[0]=='-'){
							step=xstr("-=",(step+1), End);
						}else {step=xstr("+=",step, End);}; }; };
			};
			char* mpname=toks_c(map_id(params,0));
			char* expr=xstr("int ", next1, "=next(", mpname, ",", start, ");", upto, " has_id(", mpname, ",", next1, "); ", next1, step, End);
			set(mp,idx-2,"for");
			set(mp,idx+2,x_map(expr));
			expr=NULL;
			if(map_id(map_id(params,1),1)){
				if(map_len(map_id(params,1))<=2){ expr=xcat(expr," void* ", End); }
				else {expr=xcat(expr," ", End);};
				expr=xcat(expr,toks_c(map_id(params,1)),xstr("=map_id(", mpname, ",", next1, ");", End), End); };
			if(map_id(map_id(params,2),1)){
				expr=xcat(expr," char* ",toks_c(map_id(params,2)),xstr("=map_key(", mpname, ", ", next1, ");", End), End); };
			vec_splice(map_id(mp,idx+8),0,0,x_map(expr)); }; };
	return mp;
};
map* map_implode(map* mp,char* joiner){
	map* ret=new_vec();
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i);
		if(i>0){ vec_add(ret,joiner); };
		vec_merge(ret,v); };
	return ret;
};
int tok_indent(char* str){
	if(!str){ return 0; };
	if(!strchr(str,'\n')){ return 0; };
	int ret=1;
	for(int i=str_len(str)-1;i>=0;i--){
		if(strchr("\t ",str[i])){ ret++; }
		else if(strchr("\n\r",str[i])){ break; }
		else {ret=1;};
	};
	return ret;
};
int has_chars(char* line){
	if(!line){ return 0; };
	for(;*line;line++){ if(!strchr("\n\r \t",*line)){ return 1; }; };
	return 0;	
};
int requires_semicolon(map* mp,int idx){
	idx++;
	if(idx<2){ return 0; };
	if(!str_chr(map_id(mp,idx-1),'\n')){ return 0; };
	char* pre=is_str(map_id(mp,idx-2));
	if(!pre){ return 0; };
	char* post=is_str(map_id(mp,idx));
	if(str_eq(pre,"}") && is_word(post,"else")){ return 0; };
	if(str_eq(post,")")){ return 0; };
	if(str_eq(pre,";") || str_eq(post,";")){ return 0; };
	char* forbidden="+=-/?:.&,|!%";
	if((strchr(forbidden,fox_at(pre,-1))||post && strchr(forbidden,post[0])) && !is_word(pre,"++ --") && !str_start(post,"---")){ return 0; };
	return 1;
};
map* add_semicolon(map* mp,int recurse){
	if(!mp) {return mp;};
	for(int idx=1;idx<mp->len;idx+=2){
		if(recurse && str_eq(map_id(mp,idx),"{") && (str_eq(map_id(mp,idx-2),")") || (is_keyword(map_id(mp,idx-2)) && !is_word(map_id(mp,idx-2),"return")))){
			add_semicolon(map_id(mp,idx+2),1);
			idx+=4;
			continue; };
		if(requires_semicolon(mp,idx-1)) {vec_splice(mp,idx-1,0,xvec(NULL,";", End));}; };//.vec_sub(idx-6,12).px()
	if(!is_word(map_id(mp,mp->len-mp->len%2-1),";")){
		vec_splice(mp,mp->len-mp->len%2,0,xvec(NULL,";", End)); };
	return mp;
};
char* hello(){ return "Hi"; };
int tok_endl(map* mp,int from){
	for(;from<=mp->len;from+=2){ if(str_chr(is_str(map_id(mp,from-1)),'\n')) {break;}; };
	return from;
};
map* force_curly(map* mp){
	if(!is_map(mp)){ return mp; };
	for(int idx=1; idx<mp->len; idx+=2){
		if(is_map(map_id(mp,idx))){ force_curly(map_id(mp,idx)); };
		char* w=is_str(map_id(mp,idx));
		if(!w){ continue; };
		if(!is_word(w,"if while for each do else each_mem")){
			continue; };
		if(is_word(w,"else") && str_eq(map_id(mp,idx+2),"if")){ continue; };
		int curly_at=9;
		if(is_word(w,"else do")){ curly_at=3; };
		if(str_eq(w,"while") && idx>8 && str_eq(map_id(mp,idx-8),"do")){ continue; };
		if(str_eq(map_id(mp,idx+curly_at-1),"{")){ continue; };
		int oldidx=idx;
		idx+=curly_at;
		int from=idx;
		if(mp->len<from){
			if(!(mp->len%2)){ vec_add(mp," "); };
			xadd(mp,"{",NULL,NULL,NULL,"}",NULL,";", End);
			continue; };
		int upto=idx;
		while(upto<mp->len){
			if(upto>mp->len){
				upto-=2;
				break; };
			if(tok_indent(map_id(mp,upto))){ break; };
			if(str_eq(map_id(mp,upto-1),";")){ break; };
			upto+=2; };
		map* subs=xvec(NULL, End);
		vec_merge(subs,vec_sub(mp,from-1,upto-from+1));
		if(!str_eq(map_id(subs,map_len(subs)-map_len(subs)%2-1),";")){
			if(!(map_len(subs)%2)){ xadd(subs,NULL, End); };
			xadd(subs,";", End); };
		vec_splice(mp,from-1,upto-from+1,xvec("{",NULL,subs,NULL,"}", End));
		idx=oldidx; };
	return mp;
};
map* expand_main(map* mp){
	for(int idx=1;idx<=mp->len;idx+=2){
		if(str_eq(map_id(mp,idx),"run") && str_eq(map_id(mp,idx+2),"(") && str_eq(map_id(mp,idx+8),"{") && !map_len(map_id(mp,idx+4)) && !str_eq(map_id(mp,idx-2),"int")){
//			mp[idx+10].vec_splice(0,0," gc_start(); map* args=argv_map(argv, argc);".x_map())
			vec_compact(vec_splice(mp,idx,7,vec_del(x_map("int run(map* args)"),0,1))); }; };
	return mp;
};
map* add_curly(map* mp,int recursive){
	for(int idx=1;idx<=mp->len;idx+=2){
		if(str_eq(map_id(mp,idx),"=>")){
			int from=idx;
			int upto=tok_endl(mp,idx+2);
			vec_compact(vec_splice(mp,from-1,upto-from,xvec(NULL,"{",NULL,vec_sub(mp,from+1,upto-from-2)," ","}", End))); };
		if(!recursive){ continue; };
		if(str_eq(map_id(mp,idx),"{")){
			add_curly(map_id(mp,idx+2),1);
			idx+=4;
			continue; };
		char* w=is_str(map_id(mp,idx));
		if(!w){ continue; };
		if(!is_word(w,"if while switch for each do else each_mem")){ continue; };
		if(is_word(w,"else") && str_eq(map_id(mp,idx+2),"if") && !block_level(map_id(mp,idx+1))){ continue; };
		int from=0;
		int upto=0;
		if(str_chr(is_str(map_id(mp,idx+1)),' ') && !is_word(w,"else do")){
			from=idx+2;
			upto=mp->len-mp->len%2;
			for(int i=from;i<=mp->len;i+=2){
				if(tok_indent(map_id(mp,i-1)) || str_eq(map_id(mp,i),"=>")){
					upto=i-1;
					break; }; };
			set(mp,idx+1,NULL);
			map* sub=xvec( NULL,"(", NULL,vec_sub(mp,from-1,upto-from+1), NULL,")", End);
			if(is_word(w,"while") && str_eq(map_id(mp,idx-2),"}") && str_eq(map_id(mp,idx-8),"do")){
				xadd(sub,NULL,";", End); };
			vec_compact(vec_splice(mp,from-1,upto-from+1,sub));
		};
		if(idx>=8 && is_word(w,"while") && str_eq(map_id(mp,idx-2),"}") && str_eq(map_id(mp,idx-8),"do")){ continue; };

		//if:1 (:3 x:5 ):7 {:9
		int curly_at=9;
		if(is_word(w,"else do")) {curly_at=3;};
		if(!tok_indent(map_id(mp,idx+curly_at-2)) && str_eq(map_id(mp,idx+curly_at-1),"{")) {continue;};
		if(!tok_indent(map_id(mp,idx+curly_at-2))){ // "else i=3;"
			continue; };
		int curr_indent=0;
		for(int i=idx;i>0;i-=2){
			int ind=tok_indent(map_id(mp,i-1));
			if(!ind) {continue;};
			curr_indent=ind;
			break; };
		if(!curr_indent && str_level(map_id(mp,0))) {curr_indent=str_level(map_id(mp,0))+1;};
		if(!curr_indent) {curr_indent=1;};
		from=idx+curly_at-1;
		upto=mp->len-mp->len%2;
		for(int i=from;i<=mp->len;i+=2){
			int ind=tok_indent(map_id(mp,i-1));
			if(ind && ind<=curr_indent){
				upto=i-1;
				break; }; };
		if(curr_indent) {curr_indent--;};
		char* space=" ";
		char* blankline=is_str(map_id(mp,upto));
		char* tail=NULL;
		if(blankline){
			map* lns=str_split(blankline,"\n",3);
			if(((lns->len==3||(lns->len==2 && mp->len==upto+1)) && !has_chars(map_id(lns,1))) || is_word(w,"do") || (is_word(w,"if") && str_eq(map_id(mp,upto+1),"else"))){
				if(lns->len==3) {tail=xstr("\n",map_id(lns,2), End);}
				else {tail=map_id(lns,2);};
				space=xstr(map_id(lns,0),"\n",str_times("\t",curr_indent), End);
				set(mp,upto,tail); }; };
		vec_compact(vec_splice(mp,from-1,upto-from+1,xvec(NULL,"{",NULL,vec_sub(mp,from-1,upto-from+1),space,"}", End))); };
	return mp;
};
map* dot_key(map* mp){
	return mp;
	for(int idx=1;idx<=map_len(mp);idx+=2){
		if(is_map(map_id(mp,idx))){ dot_key(map_id(mp,idx)); continue; };
		char* str=is_str(map_id(mp,idx));
		if(!str){ continue; };
		if(idx>=2 && *str=='"' && str_eq(map_id(mp,idx-2),".")){
			vec_splice(mp,idx,1,xvec("map_val",NULL,"(",NULL,xvec(NULL,str, End),NULL,")", End)); }; };
	return mp;
};
int str_hasvar(char* in){
	if(!in){ return 0; };
	for(int i=0; i<strlen(in); i++){
		if(in[i]!='$'){ continue; };
		if(is_alpha(in[i+1],"($")){ return 1; }; };
	return 0;
};
char* str_mstr(char* in){
	char* ret=new_str(str_len(in));
	char* rethead=ret;
	char* type=NULL;
	char* str=in;
	map* params=new_vec();
	char* expr=NULL;
	for(;*str;str++){
		if(*str=='%'){	
			if(str[1]=='%'){
				str++;
			}else{
				type=str+1; };
		}else if(type && *str=='{'){
			if(type==str){
				*ret++='s'; };
			expr=str+1;
			type=NULL;
			continue;
		}else if(type && !strchr("cdieEfgGosuxXpnhlL* +-#0123456789.",*str)){
			type=NULL;
		}else if(expr && *str=='}'){
			vec_add(params,sub_str(expr,0,str-expr));
			expr=NULL;
			continue; };
		if(!expr){
			*ret++=*str; }; };
	if(!params->len){ return in; };
	char* args=map_join(params,", ");
	return xstr("mstr(", rethead, ", ", args, ", End)", End);
};
map* xstr_parts(char* in){
	map* ret=new_vec();
	char* tok=new_str(str_len(in));
	char* tokhead=tok;
	int incode=0;
	char* str=in;
	if(strchr("\"'",*str)){ str=sub_str(str,1,-1); };//.str_unquote()
	while(1){
		if(incode){
			if(incode>1 && *str=='('){
				incode++;
			}else if(*str==')'){
				incode--;
				if(incode==1){ incode=0; if(*str){ str++; }; };
			}else if(incode==1 && !is_alphanum(*str,NULL)){
				incode=0; };
			if(!incode || !*str){
				*tok='\0';
				if(str_len(tokhead)){ vec_add(ret,str_dup(tokhead)); };
				tok=tokhead; }; };
		if(!incode){
			if(*str=='$'){
				if(str[1]=='$'){
					str++;
				}else if(is_alpha(str[1],NULL)){
					incode=1;
					str++;
				}else if(str[1]=='('){
					incode=2;
					str+=2;
					if(*str=='('){ incode++; }; }; };
			if(incode || !*str){
				*tok='\0';
				if(str_len(tokhead)){ vec_add(ret,str_quote(tokhead)); };
				tok=tokhead; }; };
		if(!*str){ break; };
		*tok++=*str++; };
	return ret;
};
char* str_xstr(char* in){
	map* parts=heredoc_parts(in);
	if(parts){
		map* nparts=new_vec();
		for(int idx=next(parts,-1,NULL,NULL); has_id(parts,idx); idx++){ void* val=map_id(parts,idx);
			if(idx%2){ continue; };
			if(!str_hasvar(val)){
				vec_add(nparts,idx ? xstr(map_id(parts,idx-1),val, End) : val);
			}else{
				map* xparts=xstr_parts(str_unquote(val));
				if(idx){ set(xparts,0,xstr(map_id(parts,idx-1),map_id(xparts,0), End)); };
				vec_merge(nparts,xparts); }; };
		char* ret=xstr("xstr(", map_join(nparts,", "), ")", End);
		return ret;
	};
	map* ret=xstr_parts(str_unquote(in));
	return map_len(ret)==1 ? map_id(ret,0) : mstr("xstr(%s)",map_join(ret,", "), End);
};
map* str_dollars(map* mp){
	if(!mp){ return mp; };
	assert(is_map(mp));
	for(int idx=1; idx<map_len(mp); idx+=2){
		if(is_map(map_id(mp,idx))){
			str_dollars(map_id(mp,idx));
			continue; };
		char* str=is_str(map_id(mp,idx));
		if(!str){ continue; };
		if((*str=='"' || str_start(str,"---")) && str_hasvar(str)){
			map* subs=vec_sub(dot_key(colon_str(x_map(str_xstr(str)))),1,0);
			vec_splice(mp,idx,1,subs);
			idx+=map_len(subs)-1; }; };
	return mp;
};
map* single_quotes(map* mp){
	for(int idx=1;idx<=map_len(mp);idx+=2){
		if(is_map(map_id(mp,idx))){ single_quotes(map_id(mp,idx)); continue; };
		char* str=is_str(map_id(mp,idx));
		if(!str){ continue; };
		if(str_eq(str,"''")){
			set(mp,idx,"NULL");
		}else if(*str=='\'' && (str_len(str)>4 || (str[1]!='\\' && str_len(str)>3))){
			*str='"';
			str[str_len(str)-1]='"'; }; };
	return mp;
};
map* colon_str(map* mp){
	for(int idx=1;idx<=map_len(mp);idx+=2){
		if(is_map(map_id(mp,idx))){ colon_str(map_id(mp,idx)); continue; };
		char* str=is_str(map_id(mp,idx));
		if(!str){ continue; };
		if(str_len(str)>1 && *str==':'){
			str=str_quote(str_unquote(sub_str(str,1,-2147483648)));
			set(mp,idx,str); };
		if(idx>=2 && *str=='"' && str_eq(map_id(mp,idx-2),".")){
			vec_splice(mp,idx,1,xvec("map_val",NULL,"(",NULL,xvec(NULL,str, End),NULL,")", End)); }; };
	return mp;
};
map* dot_func(map* mp){
	for(int i9=0;i9<map_len(mp);i9+=2){
		if(is_map(map_id(mp,i9+1))){
			dot_func(map_id(mp,i9+1));
			continue; };
		int idx=i9+1;
		if(idx<=2){ continue; };
		if(!str_eq(map_id(mp,idx),".")){ continue; };
		if(!is_code(is_str(map_id(mp,idx+2)))){ continue; };
		if(!str_eq(map_id(mp,idx+4),"(")){ continue; };
		int from=idx-2;
		while(from>0){
			char* s=is_str(map_id(mp,from));
			if(!s){ break; };
			char* s2=is_str(map_id(mp,from+2));
			if(is_word(s,") } ]") && is_word(s2,". -> ( [")) {from-=6;}
			else if(is_word(s,". ->") || *s=='"' || *s=='\'' || str_start(s,"---")) {from-=2;}
			else if((is_code(s)||is_numeric(s)) && is_word(s2,". -> ( { [")) {from-=2;}
			else {break;}; };
		if(str_eq(map_id(mp,from+2),"return")){ from+=2; };
		if(idx-from<=2){ continue; };
		char* prespace=is_str(map_id(mp,idx-1));
		vec_compact(vec_del(mp,idx-1,2));
		from+=2;
		idx-=2;
		map* sub=vec_slice(mp,from,idx-from+2);
		for(int idx2=next(sub,-1,NULL,NULL); has_id(sub,idx2); idx2++){
			if(fox_at(is_str(map_id(sub,idx2)),0)==')' && fox_at(is_str(map_id(sub,idx2+2)),0)=='(') {vec_shift(sub,idx2+2,6,-idx2-2);};
			//if(sub[idx].is_str().fox_at(0)==')' && sub[idx+2].is_str().fox_at(0)=='(') sub.vec_shift(idx+2,6,-idx-2)
			idx2++; };
		map* params=is_map(map_id(mp,idx+6));
		assert(params);
		if(!params){ params=mp->vars[idx+6]=new_vec(); };
		if(params->len){ xadd(sub,",", End); }
		else {vec_compact(vec_del(sub,sub->len-1,1));};
		map* sub1=new_vec();
		vec_add(sub1,prespace);
		vec_merge(sub1,sub);
		vec_splice(params,1-1,0,sub1);
		vec_compact(params);
		dot_func(params);
		vec_compact(mp);
		i9-=4; };
	return mp;
};

map* map_tox(map* mp){
	if(!mp){ return mp; };
	for(int idx=1+next(mp,-1,NULL,NULL);idx;idx=1+next(mp,idx-1,NULL,NULL)){
		if(idx%2-1){
			map* mp1=is_map(map_id(mp,idx-1));
			if(mp1) {map_tox(mp1);};
			continue; };
		idx=func_dot(mp,idx-1); };
	return mp;
};
int stm_start(map* mp,int idx,char* terms){
	idx-=2;
	while(idx>0){
		if(str_chr(terms,fox_at(map_id(mp,idx),0))){ break; };
		idx-=2; };
	return idx+2;
};
map* syn_func(map* syn,int with_body){
	int len=map_len(syn);
	map* body=NULL;
	int params_at=5;
	if(str_eq(map_id(syn,len-3),"}")){
		body=map_id(syn,len-5);
		params_at=11; };
	map* params=new_map();
	map* mp1=toks_split(map_id(syn,len-params_at),",",0);
	for(int i2=next(mp1,-1,NULL,NULL); has_id(mp1,i2); i2++){ void* v=map_id(mp1,i2);
		char* param_name=syn_var_name(v);
		char* param_type=syn_var_type(v);
		void* param_default= is_assign(v) ? param_default=toks_c(vec_sub(v,syn_assign_val(v),0)) : NULL;
		if(!str_eq(param_name,"...") && !param_type){
			fox_error(xstr(map_id(syn,len-params_at-4), "() parameter ", param_name, " type not set", End),0); };
		assert(str_eq(param_name,"...") || param_type);
		assert(param_name);
		add(params,param_name,xmap("type", param_type, "default", param_default, End)); };
	char* name=is_str(map_id(syn,len-params_at-4));
	map* type=vec_sub(syn,1,len-params_at-6);
	char* decltype=toks_c(type);
	for(int i3=next(type,-1,NULL,NULL); has_id(type,i3); i3++){ void* v3=map_id(type,i3);
		if(is_word(v3,"inline")){ vec_del(type,i3,2); }; };
	map* ret=xmap("name", name, "type", toks_c(type), "decltype", decltype, "params", params, End);
	if(with_body){ add(ret,"body",body); };
	return ret;
};
map* file_callmap(char* filename){ return func_depends(file_funcs(filename,1)); };
map* func_depend(map* mp,map* ret){
	for(int i=0; i<map_len(mp); i+=2){
		if(is_map(map_id(mp,i+1))){ func_depend(map_id(mp,i+1),ret); continue; };
		char* name=syn_is_call(mp,i);
		if(name){ add(ret,name,name); }; };
	return ret;
};
map* func_depends(map* mp){
	map* ret=new_map();
	for(int next1=next(mp,-1,NULL,NULL); has_id(mp,next1); next1++){ void* fn=map_id(mp,next1); char* name=map_key(mp, next1);
		px(name,1);
		add(ret,"name",func_depend(map_val(fn,"body"),new_map()));
		map* map_1=map_val(ret,"name"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); px(xstr("\t", val, End),1); }; };
	return ret;
};
map* syn_funcs(map* mp,int with_body){
	map* ret=new_map();
	for(int i=1; i<map_len(mp); i+=2){
		if(is_func_decl(map_id(mp,i))){
			map* fn=syn_func(map_id(mp,i),with_body);
			if(str_eq(map_val(fn,"name"),"main")){ continue; };
			add(ret,map_val(fn,"name"),fn); }; };
	return ret;
};
map* file_vec(char* in){ return str_vec(file_read(in,1)); };
map* file_map(char* in){ return str_map(file_read(in,1),Map); };
map* map_keys(map* mp){
	map* ret=new_vec();
	for(int next1=next(mp,-1,NULL,NULL); has_id(mp,next1); next1++){ char* k=map_key(mp, next1); vec_add(ret,k); };
	return ret;
};
char* drop_right(char* str,char* w){
	if(w && is_str(w) && str_end(w,w)){ return sub_str(str,-str_len(w),-2147483648); };	
	return str;
};
char* drop_left(char* str,char* w){
	if(str && is_str(str) && str_start(str,w)){ return sub_str(str,str_len(w),-2147483648); };	
	return str;
};
map* read_struct(map* toks){
	map* cols=new_map();
	map* map_1=toks_split(toks,";",0); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		if(map_len(v)<2){ break; };
		int hascurly=next_tok(v,0,"{",0);
		if(hascurly){
			void* name=map_id(v,hascurly+6);
			if(name){ add(cols,name,read_struct(map_id(v,hascurly+2))); }
			else {map_merge(cols,read_struct(map_id(v,hascurly+2)));};
		}else{
			add(cols,syn_var_name(v),syn_var_type(v)); }; };
	return cols;		
};
map* c_structs(char* in){
	map* ret=new_map();
	map* syn=toks_syn(
		add_semicolon(
		add_curly(
		x_map(
		in),1),1),0);
	for(int i=1; i<syn->len; i+=2){
		if(!is_word(map_id(map_id(syn,i),1),"typedef struct union class")){ continue; };
		void* toks=map_id(syn,i);
		int hascurly=next_tok(toks,0,"{",0);	
		if(!hascurly){
			add(ret,syn_var_name(toks),syn_var_type(vec_sub(toks,2,0)));
			continue; };
		int istypedef=0;
		int from=1;
		if(str_eq(map_id(toks,1),"typedef")){ from+=2; istypedef=1; };
		char* name=toks_c(vec_sub(toks,from,hascurly-1-from));
		add(ret,name,read_struct(map_id(toks,hascurly+2)));
		if(istypedef){
			add(ret,map_id(toks,hascurly+6),map_val(ret,name)); }; };
	return ret;
};
map* x_funcs(char* in,int with_body){
	return syn_funcs(
		toks_syn(
		add_semicolon(
		add_curly(
		x_map(in),0),0),0),with_body);
};
map* x_toks(char* in,int is_script){
	if(!in){ return NULL; };
	return single_quotes(
		auto_types(
		inline_vectors(
		string_operators(
		heredoc_str(
		toks_syn(
		dot_each(
		dot_func(
		expand_main(
		add_semicolon(
		force_curly(
		add_curly(
		str_dollars(
		dot_key(
		colon_str(
		x_map(in)))),1)),1)))),1)))),"c", is_script,new_map(),new_map(),NULL,0));
};
map* func_params(map* func){ return toks_split(func_param(func),",",0); };
map* func_param(map* func){ return map_id(func,next_tok(func,0,"(",0)+2); };
map* func_add_param(map* func,map* add){
	map* param=func_param(func);
	assert(is_vec(param));
	if(map_len(param)){
		if(!(map_len(param)%2)){ xadd(param,NULL, End); };
		xadd(param,",", End); };
	vec_merge(param,add);
	return func;
};
map* func_rename(map* func,char* name){ return set(func,1,name); };
char* func_name(map* func){ return map_id(func,1); };
map* map_assign(map* exp1,map* exp2){
	map* hasreturn=NULL;
	if(str_eq(map_id(exp1,1),"return")){ hasreturn=vec_sub(exp1,0,2); vec_compact(vec_del(exp1,0,2)); };
	map* params=func_params(exp1);
	char* funcname=str_eq(func_name(exp1),"map_val") ? "add" : "set";
	if(str_eq(map_id(map_id(params,1),1),"NULL")){
		vec_compact(vec_del(params,1,1));
		funcname="vec_add"; };
	syn_set_param(exp1,0,toks_join(params,","));
	func_rename(func_add_param(exp1,exp2),funcname);
	map* mp1=func_param(exp1);
	int isvec=str_eq(funcname,"vec_add") ? 1 : 0;
	while(syn_is_call(mp1,0)){
		if(str_eq(func_name(mp1),"map_val")){ func_rename(mp1,"add_key"); }
		else if(str_eq(func_name(mp1),"map_id")){ func_rename(mp1,"add_id"); }
		else {break;};
		if(isvec){
			func_add_param(mp1,xvec(NULL,"Vector", End));
			isvec=0; };
		mp1=func_param(mp1); };
	if(hasreturn){ vec_splice(exp1,0,0,hasreturn); };
	return exp1;
};
map* call_count(map* toks,map* counter,char* infunc){
	if(!toks){ return counter; };
	for(int i=0; i<toks->len; i+=2){
		char* name=syn_is_call(toks,i);
		if(!str_eq(name,infunc)){
			add(counter,name,int_var((is_int(map_val(counter,name))+1))); };
		if(is_map(map_id(toks,i+1))){
			call_count(map_id(toks,i+1),counter,infunc); }; };
	return counter;
};
map* file_deadcode(char* file){
	map* funcs=x_funcs(file_read(file,1),1);
	map* ret=new_map();
	for(int i=next(funcs,-1,NULL,NULL); has_id(funcs,i); i++){ void* v=map_id(funcs,i); char* k=map_key(funcs, i);
		call_count(map_val(v,"body"),ret,k); };
	for(int i2=next(funcs,-1,NULL,NULL); has_id(funcs,i2); i2++){ void* v2=map_id(funcs,i2); char* k2=map_key(funcs, i2);
		if(!map_val(ret,k2)){ px(k2,1); }; };
	return ret;
};
map* auto_types(map* toks,char* context,int is_script,map* env,map* fns,map* func,int idx){
	int temp=0;
	if(!toks){ return toks; };
	if(str_eq(context,"c")){
		env=xmap("_globals" , "map*", End);
		for(int i=0; i<toks->len; i+=2){
			void* tok=map_id(toks,i+1);
			if(is_func_decl(tok)){
				map* fn=syn_func(tok,1);
				add(fns,map_val(fn,"name"),fn);
				syn_remove_default(tok); }; };
		for(int i=next(fns,-1,NULL,NULL); has_id(fns,i); i++){ void* fn=map_id(fns,i); char* name=map_key(fns, i);
			map* fenv=map_dup(env);	
			map* map_1=map_val(fn,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* op=map_id(map_1,i2); char* param=map_key(map_1, i2);
				add(fenv,param,map_val(op,"type")); };
			auto_types(add_return(map_val(fn,"body")),"body",is_script,fenv,fns,fn,0);
			if(is_script){
				if(map_val(fn,"body")){ add(add_key(add_key(_globals,"cache",Map),"userfuncs",Map),map_val(fn,"name"),map_val(fn,"body")); };
				add(add_key(add_key(add_key(_globals,"cache",Map),"reflect",Map),"funcs",Map),map_val(fn,"name"),map_del_key(fn,"body")); }; };
		return auto_types(toks,"body",is_script,env,fns,NULL,0);
	}else if(str_eq(context,"body")){
		for(int j=0; j<toks->len; j+=2){
			void* tok=map_id(toks,j+1);
			if(is_func_decl(tok)){
				continue;
			}else if(syn_is_macro(tok,0)){
				map* expanded=toks_syn(syn_expand_macro(vec_dup(tok),0),1);
				map* last=map_id(expanded,map_len(expanded)-1);
				auto_types(vec_compact(vec_del(expanded,-2,1)),"body",is_script,env,fns,func,0);
				auto_types(vec_merge(last,vec_sub(tok,8,0)),"syn",is_script,env,fns,func,0);
				if(is_script){
					vec_splice(toks,j+1,1,expanded); };
			}else {auto_types(tok,"syn",is_script,env,fns,func,0);}; };
		return toks;
	}else if(str_eq(context,"syn")){
		if(is_var_decl(toks)){
			char* type=syn_var_type(toks);
			char* name=syn_var_name(toks);
			add(env,name,type);
			auto_types(toks,"expr",is_script,env,fns,func,syn_assign_val(toks));
		}else if(is_flow(toks)){
			char* type=syn_flow_name(toks);
			if(str_eq(type,"for")){
				env=map_dup(env);
				auto_types(map_id(syn_flow_condition(toks,0),1),"syn",is_script,env,fns,NULL,0); };
			int i=0;
			map* body=NULL;
			while((body=syn_flow_condition(toks,i++))){
				auto_types(body,"expr",is_script,map_dup(env),fns,func,0); };
			i=0;
			while((body=syn_flow_body(toks,i++))){
				auto_types(body,"body",is_script,map_dup(env),fns,func,0); };
		}else if(is_assign(toks)){
			auto_types(toks,"expr",is_script,env,fns,func,0);
			if(is_word(syn_is_call(toks,str_eq(map_id(toks,1),"return") ? 2 : 0),"map_val map_id")){
				auto_types(toks,"expr",is_script,env,fns,func,0);
				int from=syn_assign_val(toks);
				int upto=next_tok(toks,syn_assign_val(toks),"; =",0);
				int len=0;
				if(upto){ len=upto-from-1; };
				map* val=vec_slice(toks,from,len);
				vec_compact(vec_del(vec_compact(toks),next_tok(toks,0,"=",0)-1,2));
				auto_types(map_assign(toks,val),"expr",is_script,env,fns,func,0);
				return toks; };
			char* name=syn_var_name(toks);
			char* type=syn_var_type(toks);
			if(*name!='_' && !map_val(env,name) && !str_len(type)){
				type=expr_type(toks,syn_assign_val(toks),0,env,fns);
				if(type){
					vec_splice(toks,1,0,vec_compact(vec_del(x_map(xstr(type, " ", End)),0,1)));
					add(env,name,type);
				}else{
					px(xstr(map_val(func,"name"), "(): unknown var ", name, End),1); }; };
			auto_types(toks,"expr",is_script,env,fns,func,syn_assign_val(toks));
		}else{
			auto_types(toks,"expr",is_script,env,fns,func,0); };
		return toks;
	}else if(str_eq(context,"expr")){
		int head=idx;
		for(int i=idx; i<toks->len; i+=2){
			if(is_map(map_id(toks,i+1))){
				auto_types(map_id(toks,i+1),"expr",is_script,env,fns,func,0);
			}else if(syn_is_call(toks,i)){
				auto_types(map_id(toks,i+5),"expr",is_script,env,fns,func,0);
				char* name=syn_is_call(toks,i);
				if(str_eq(name,"args_map")){
					char* subs="xmap(";
					map* map_1=map_val(func,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ char* k2=map_key(map_1, i2);
						subs=xcat(subs,":",k2,",",k2,",", End); };
					map* mp4=vec_compact(vec_del(colon_str(x_map(xstr(subs, " End)", End))),0,1));
					vec_splice(toks,i+1,7,mp4);
				}else{
					map* params=syn_func_param(toks,i);
					map* fn=map_val(fns,name);
					if(!fn){ fn=map_val(funcs(),name); };
					if(fn){
						params=param_c(params,env,fns,fn); };
					syn_set_param(toks,i,params); };
			}else if(str_eq(map_id(toks,i+1),"[") && is_word(head_type(toks,i-2,i,env,fns),"map* void*") && neq(is_typecast(toks,head),"void*") && 1){
				void* name=map_id(toks,i+3);
				if(!map_len(name)){ name=xvec(NULL,"NULL", End); };
				head=expr_head(toks,i-2,".");
				map* mid=vec_merge(xadd(vec_splice(vec_sub(toks,head+1,i-head-1),0,0,xvec(NULL, End)),NULL,",", End),name);
				char* callfunc=str_eq(expr_type(name,0,0,env,fns),"int") ? "map_id" : "map_val";
				vec_splice(toks,head+1,i+5-head,xvec(callfunc,NULL,"(",NULL,mid,NULL,")", End));
				i=head;
			}else if(str_eq(map_id(toks,i+1),".") && is_name(map_id(toks,i+3)) && expr_head(toks,i-2,".")<i){
				head=expr_head(toks,i-2,".");
				if(head==i){ fox_error(xstr("Can't find header for ", json(toks,0), " @ ",int_str( i), End),0); };
				if(is_word(syn_is_call(toks,i+2),"each")){
					map* mid=vec_splice(vec_sub(toks,head+1,i-head-1),0,0,xvec(NULL, End));
					map* params=toks_split(syn_func_param(toks,i+2),",",0);
					if(syn_is_call(mid,0)){
						temp++;
						char* varname=xstr("map_",int_str(temp), End);
						syn_set_param(toks,i+2,toks_join(vec_splice(params,0,0,xvec(xvec(NULL,varname, End), End)),","));
						vec_splice(toks,head+1,i-head+2,vec_merge(vec_merge(vec_splice(x_map(xstr("map* ", varname, "=", End)),0,1,NULL),mid),xvec(NULL,";"," ", End)));
					}else{
						params=toks_join(vec_splice(params,0,0,xvec(mid, End)),",");
						syn_set_param(toks,i+2,params);
						vec_del(toks,head+1,i-head+2);
						vec_compact(toks); };
					i=head;
				}else if(syn_is_call(toks,i+2)){
					map* mid=vec_splice(vec_sub(toks,head+1,i-head-1),0,0,xvec(NULL, End));
					vec_del(toks,head+1,i-head+2);
					map* params=vec_splice(toks_split(syn_func_param(toks,i+2),",",0),0,0,xvec(mid, End));
 					params=toks_join(params,",");
					syn_set_param(toks,i+2,params);
					vec_compact(toks);
					i=head-2;
				}else if(is_word(expr_type(toks,head,i,env,fns),"map* void*") && neq(is_typecast(toks,head),"void*")){
					void* name=map_id(toks,i+3);
					map* mid=vec_splice(xadd(vec_sub(toks,head+1,i-head-1),NULL,",",NULL,str_quote(str_unquote(name)), End),0,0,xvec(NULL, End));
					vec_splice(toks,head+1,i+3-head,xvec("map_val",NULL,"(",NULL,mid,NULL,")", End));
					i=head; }; }; };
		return toks; };					
	return toks;
};
char* is_typecast(map* toks,int idx){
	if(str_eq(map_id(toks,idx+1),"(") && is_var_decl(map_id(toks,idx+3))){ return str_trim(toks_c(map_id(toks,idx+3))," \t\n\r"); };
	return NULL;
};
char* is_name(char* in){
	if(!is_code(in)){ return 0; };
	if(is_word(in,"return")){ return 0; };
	return in;
};
char* head_type(map* toks, int idx, int upto, map* env,map* fs){
	int head=expr_head(toks,idx,".");
	return expr_type(toks,head, upto, env, fs);
};
int expr_tail(map* toks,int idx,char* expr){
	idx++;
	char* presid="= += -= /= *= %= ^= ||= &&= .= ? : || && == === !== != >= <= > < or and .. + - / * ++ -- ! -> .";
	int expr_presid=is_word(expr,presid);
	while(idx<map_len(toks)){
		char* v=is_str(map_id(toks,idx));
		if(is_name(v) && (str_eq(map_id(toks,idx+2),")")||str_eq(map_id(toks,idx+2),"]"))){
			idx+=8;
		}else if(str_eq(v,"(")||str_eq(v,"[")){
			idx+=6;
		}else if(strchr("\"'`",v[0])){
			idx+=2;
		}else if(is_name(v) || is_numeric(v)){
			idx+=2;
		}else if(expr_presid < is_word(v,presid)){
			idx+=2;
		}else {break;}; };
	return idx-1;
};
int expr_head(map* toks,int idx,char* expr){
	idx++;
	char* presid="= += -= /= *= %= ^= ||= &&= .= ? : || && == === !== != >= <= > < or and .. + - / * ++ -- ! -> .";
	int expr_presid=is_word(expr,presid);
	while(idx>0){
		char* v=is_str(map_id(toks,idx));
		if((str_eq(v,")")||str_eq(v,"]")) && is_name(map_id(toks,idx-6))){
			idx-=8;
		}else if(str_eq(v,")")||str_eq(v,"]")){
			idx-=6;
		}else if(strchr("\"'`",v[0])){
			idx-=2;
		}else if(is_name(v) || is_numeric(v)){
			idx-=2;
		}else if(expr_presid < is_word(v,presid)){
			idx-=2;
		}else {break;}; };
	return idx+1;
};
map* add_return(map* toks){
	return toks;
};
map* wrap_call(map* tok,char* func){
	return xvec(NULL,func,NULL,"(",NULL,tok,NULL,")", End);
};
map* type_convert(map* tok,char* outtype,map* env,map* fs,map* fn){
	if(!outtype){ return tok; };
	char* intype=expr_type(tok,0,0,env,fs);	
	if(!intype){ return tok; };
	if(str_eq(intype,outtype)){ return tok; };
	if(is_word(intype,"int double long long") && str_start(outtype,intype) && outtype[str_len(intype)]=='*' && !outtype[str_len(intype)+1]){
		return vec_splice(tok,1,0,xvec("&",NULL, End)); };
	if(is_word(intype,"int long long size_t")){
		if(str_eq(outtype,"void*")){ return wrap_call(tok,"int_var"); }
		else if(str_eq(outtype,"char*")){ return wrap_call(tok,"int_str"); };
	}else if(str_eq(intype,"void*")){
		if(str_eq(outtype,"double")){ return wrap_call(tok,"to_double"); };
	}else if(str_eq(intype,"double")){
		if(str_eq(outtype,"void*")){ return wrap_call(tok,"double_var"); }
		else if(str_eq(outtype,"char*")){ return wrap_call(tok,"double_str"); };
	}else if(str_eq(intype,"map*")){
		if(str_eq(outtype,"char*")){ return wrap_call(tok,"to_str"); };
	}else if(str_eq(intype,"char*")){
		if(str_eq(outtype,"int")){ return wrap_call(tok,"stoi"); }
		else if(str_eq(outtype,"char**")){
			return vec_splice(tok,1,0,xvec("&",NULL, End)); };
	}else if(str_eq(intype,"char**")){
		if(str_eq(outtype,"char*")){ return vec_splice(tok,1,0,xvec("*",NULL, End)); }; };
	return tok;
};
map* param_c(map* params,map* env,map* fs,map* fn){
	assert(fn);
	if(!params){ return NULL; };
	map* xparam=toks_split(params,",",0);
	map* cparam=new_vec();
	void* sig=map_val(fn,"params");
	for(int i=next(sig,-1,NULL,NULL); has_id(sig,i); i++){ void* p=map_id(sig,i); char* name=map_key(sig, i);
		if(str_eq(name,"...")){
			for(int idx=i; idx<xparam->len; idx++){
				if(str_eq(map_id(map_id(xparam,idx),1),"End")){ break; };
				vec_add(cparam,type_convert(map_id(xparam,idx),map_val(p,"type"),env,fs,fn)); };
			if(neq(map_id(map_id(cparam,map_len(cparam)-1),1),"End")){
				if(!map_id(map_id(cparam,map_len(cparam)-1),1)){ set(add_id(cparam,map_len(cparam)-1),1,"End"); }
				else{
					char* endmark=map_len(cparam) ? " End" : "End";
					vec_add(cparam,x_map(endmark)); }; };
			break;
		}else if(map_len(map_id(xparam,i))<2){
			if(!map_val(p,"default")){ break; };
			vec_add(cparam,vec_compact(vec_del(map_id(x_toks(map_val(p,"default"),0),1),-2,0))); //x_map().colon_str().dot_key().str_dollars()
		}else{ 
//			xparam[i].dx(:IN)
//			xparam[i].expr_type(0,0,env,fs).dx(:INTYPE)
//			p.type.dx(:OUT)
//			xparam[i].type_convert(p.type,env,fs,fn).toks_c().dx(:CONVERTED)
//			"^^^^^^^^^^^^".px()
			vec_add(cparam,type_convert(map_id(xparam,i),map_val(p,"type"),env,fs,fn)); }; };
	map* ret=toks_join(cparam,",");
	return ret;
};
map* syn_expand_macro(map* syn,int idx){
	map* macro=map_val(macros(),map_id(syn,idx+1));
//	"macro: $(syn[idx+1])".px()
	map* params=toks_split(syn_func_param(syn,0),",",0);
	if(map_len(map_val(macro,"params"))!=map_len(params)){ fox_error(xstr("Macro ", map_id(syn,idx), " takes ",int_str( map_len(map_val(macro,"params"))), " parameters, ",int_str( map_len(params)), " provided", End),0); };
	map* subs=new_map();
	map* map_1=map_val(macro,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* v2=map_id(map_1,i2);
		add(subs,v2,vec_compact(vec_del(map_id(params,i2),0,1))); };
	return toks_replace(vec_rdup(map_val(macro,"body")),subs);
	map* expanded=toks_replace(vec_rdup(map_val(macro,"body")),subs);
	return vec_splice(syn,idx+1,7,vec_compact(vec_del(expanded,0,1)));
//	return expanded.map_len()-7
};
map* syn_is_macro(map* syn, int idx){
	return map_val(macros(),syn_is_call(syn,idx));
};
map* syn_set_param(map* syn,int idx,map* params){
	return set(syn,next_tok(syn,idx,"(",0)+2,params);
};
map* syn_func_param(map* syn,int idx){
	return map_id(syn,next_tok(syn,idx,"(",0)+2);
};
map* syn_func_body(map* syn){
	if(!next_tok(syn,0,"{",0)){ return NULL; };
	return map_id(syn,next_tok(syn,0,"{",0)+2);
};
map* syn_remove_default(map* syn){
	map* params=syn_func_param(syn,0);
	if(!params) {return syn;};
	int indel=0;
	int hasdel=0;
	int start=0;
	for(int i=next(params,-1,NULL,NULL); has_id(params,i); i++){ void* v=map_id(params,i);
		if(str_eq(v,"=")){
			indel=1;
			hasdel++;
			vec_del(params,i,1);
		}else if(str_eq(v,"...")){
			if(neq(map_id(params,i-2),",")){
				vec_del(params,start+1,i-start-1); };
		}else if(str_eq(v,",")){
			start=i;
			indel=0;
		}else if(indel) {vec_del(params,i,1);}; };
	if(hasdel) {vec_compact(params);};
	return syn;
};

char* expr_type(map* toks,int idx,int upto,map* env,map* fs){
	void* v=map_id(toks,idx+1);
	if(is_map(v)){ return expr_type(v,0,upto,env,fs); };
	char* w=is_str(v);
	if(!w){ return NULL; };
	assert(is_str(w) && str_len(w));
	if(next_tok(toks,idx,"?",upto)){
		return expr_type(toks,next_tok(toks,idx,"?",0)+1,upto,env,fs);
	}
	else if(str_eq(map_id(toks,idx+3),"(")){
		char* name=is_str(map_id(toks,idx+1));
		if(map_val(fs,name)){ return map_val(map_val(fs,name),"type"); };
		return map_val(map_val(funcs(),name),"type");
	}else if(is_word(w,"++ --")){ return expr_type(toks,idx+2,upto,env,fs); }
	else if(*w=='"'){ return "char*"; }
	else if(*w=='\'' && w[1]=='\''){ return "char*"; }
	else if(*w=='\''){ return "char"; }
	else if(*w>='0' && *w<='9' || *w=='-' || *w=='+'){ return strchr(w,'.') ? "double" : "int"; }
	else if(is_typecast(toks,idx)){ return is_typecast(toks,idx); }
	else if(str_eq(w,"(")){ return expr_type(map_id(toks,idx+3),0,upto,env,fs); }
	else if(str_eq(w,"*")){ return sub_str(expr_type(toks,idx+2,upto,env,fs),0,-1); }
	else if(str_eq(w,"&")){ return xstr(str_dup(expr_type(toks,idx+2,upto,env,fs)),"*", End); }
	else if(str_eq(w,"NULL")){ return "void*"; }
	else if(map_val(env,w)){
		if(next_tok(toks,idx,"-",upto) && is_word(expr_type(toks,next_tok(toks,idx,"-",upto)+1,upto,env,fs),"char* void* map*")){
			return "int"; };
		char* ret=map_val(env,w);	
		if(!upto){ upto=toks->len; };
		map* structtype=NULL;
		for(int i=idx+3; i<upto; i+=2){
			v=map_id(toks,i);
			if(is_word(v,". ->")){
				structtype=map_val(structs(),str_trim(ret,"*"));
			}else if(structtype && is_code(v) && !syn_is_call(toks,i) && map_val(structtype,v)){
				ret=map_val(structtype,v);
			}else if(str_eq(v,"[") && fox_at(ret,-1)=='*'){
				ret=sub_str(ret,0,-1);
				i+=4;
			}else if(str_eq(v,"-")){
				if(str_eq(ret,"int")){ return ret; };
				char* rest=expr_type(toks,i+1,upto,env,fs);
				if(fox_at(rest,-1)=='*'){ return "int"; };
				return ret;
			}else {return ret;}; };
		return ret; };
	return NULL;
};
char* assign_to_func(map* tok){
	return syn_is_call(tok,0);
};
int next_tok(map* toks,int from,char* tok,int upto){
	if(!toks){ return 0; };
	for(int i=from; i<toks->len; i+=2){
		if(is_word(map_id(toks,i+1),tok)){ return !upto || i<upto ? i+1 : 0; }; };
	return 0;
};
int syn_has(map* syn,char* words){ return next_tok(syn,0,words,0); };
char* syn_flow_name(map* syn){ return map_id(syn,1); };
int is_flow(map* syn){
	return is_word(map_id(syn,1),"if while for switch do");
};
map* syn_flow_condition(map* syn,int no){
	char* name=syn_flow_name(syn);
	// do:1 {:3 __:5 }:7 while:9 (:11 __:13 ):15
	// if:1 (:3 __:5 ):7 {:9 __:11 }:13
	// else:15 if:17 (:19 __:21 ):23 {:25 __:27 }:29
	// else:31 if:33 (:35 __:37 ):39 {:41 __:43 }:45
	// else:47 {:49 __:51 }:53
	if(str_eq(name,"do") && !no){ return map_id(syn,13); };
	return is_map(map_id(syn,5+no*16));
};
map* syn_flow_body(map* syn,int no){
	char* name=syn_flow_name(syn);
	// if:1 (:3 __:5 ):7 {:9 __:11 }:13
	// else:15 if:17 (:19 __:21 ):23 {:25 __:27 }:29
	// else:31 if:33 (:35 __:37 ):39 {:41 __:43 }:45
	// else:47 {:49 __:51 }:53
	if(str_eq(name,"do") && !no){ return map_id(syn,5); };
	if(!no){ return map_id(syn,11); };
	map* iselse=is_map(map_id(syn,11+(no-1)*16+8));
	return iselse ? iselse : map_id(syn,11+no*16);
};
char* syn_is_call(map* syn, int idx){
	if(is_code(map_id(syn,idx+1)) && !is_keyword(map_id(syn,idx+1)) && str_eq(map_id(syn,idx+3),"(") && str_eq(map_id(syn,idx+7),")")){
		return map_id(syn,idx+1); };
	return NULL;
};
char* syn_var_name(map* syn){
	int idx=next_tok(syn,0,"= ; ,",0);
	return idx ? map_id(syn,idx-2) : map_id(syn,map_len(syn)-map_len(syn)%2-1);
};
char* syn_var_type(map* syn){
	int idx=next_tok(syn,0,"= ; ,",0);
	return idx ? toks_c(vec_sub(syn,1,idx-4)) : toks_c(vec_sub(syn,1,map_len(syn)-map_len(syn)%2-3));
};
int syn_assign_val(map* syn){
	return next_tok(syn,0,"=",0)+1;
};
int is_assign(map* syn){ return next_tok(syn,0,"=",0); };
int is_var_decl(map* syn){
	return is_word(map_id(syn,1),"int long double float char void struct map static extern unsigned register mempage size_t time_t const FILE inline");
};
int is_func_decl(map* syn){
	int idx=next_tok(syn,0,"(",0);
	if(!idx--){ return 0; };
	if(next_tok(syn,0,"=",0)){ return 0; };
	if(!is_word(map_id(syn,1),"int long double float char void struct map static extern unsigned register mempage size_t time_t const FILE inline")){
		return 0; };
	if(str_eq(map_id(syn,idx+7),";")){ return 1; };
	if(str_eq(map_id(syn,idx+7),"{") && str_eq(map_id(syn,idx+13),";")){ return 1; };
	return 0;
};
char* c_h(char* infile,char* outfile){ return write_file(funcs_cdecl(file_funcs(infile,0),0),outfile,0,1); };
char* fox_h(char* infile,char* outfile){ return write_file((xstr("#include <fox.h>\n",funcs_cdecl(file_funcs(infile,0),0), End)),outfile,0,1); };
char* fox_c(char* infile, char* outfile){ return write_file(x_c(file_read(infile,1)),outfile,0,1); };
map* x_map(char* in){ return c_tokenizer(&in,'\0'); };
char* c_x(char* in){ return toks_c(map_tox(x_map(in))); };

char* x_c(char* in){ return toks_c(x_toks(in,0)); };
char* func_ccall(map* fn){
	char* ret=NULL;
	char* preproc=NULL;
	int isvariadic=0;
	map* map_1=map_val(fn,"params"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); char* k=map_key(map_1, i);
		char* def=map_val(v,"default");
		v=map_val(v,"type");
		v=drop_left(v,"const ");
		v=drop_left(v,"unsigned ");
		char* pre=NULL;
		char* post=NULL;
		char* mid=xstr("v.map_id(",int_str( i), ")", End);
		if(str_eq(v,"char*")){ post=".is_str()"; }
		else if(str_eq(v,"char**")){ preproc=xcat(preproc,xstr("char* p", k, "_", map_val(fn,"name"), "=v.map_id(",int_str( i), ").is_str(); ", End), End); mid=xstr("&p", k, "_", map_val(fn,"name"), End); }
		else if(str_eq(v,"map*")){ post=".is_map()"; }
		else if(is_word(v,"int long size_t time_t") || str_eq(v,"long long")){ post=".to_int()"; }
		else if(str_eq(v,"char")){
			post=xstr(".is_str() ? ", mid, ".is_str()[0] : ", mid, ".to_int())", End);
			pre="(";
		}else if(is_word(v,"double float")){ post=".to_double()"; }
		else if(str_eq(k,"...")){
			char* mtype=NULL;
			if(str_end(map_val(fn,"type"),"*")){ mtype="ptr"; }
			else if(str_eq(map_val(fn,"type"),"int")){ mtype="int"; }
			else if(str_eq(map_val(fn,"type"),"double")){ mtype="double"; }
			else {return NULL;};
			isvariadic=1;
			ret=xstr("call_variadic_", mtype, "(v,", map_val(fn,"name"), ",\"", map_val(fn,"name"), "\")", End);
			break;
		}else if(!str_end(v,"*")){
			return NULL; };
		if(def){ ret=xcat(ret,xstr("v->len>=",int_str( i), " ? ", pre, mid, post, " : ", sub_str(x_c(def),0,-1), ",", End), End); }
		else{ ret=xcat(ret,xstr(pre, mid, post, ",", End), End); }; };
	char* postproc=NULL;
	if(is_word(map_val(fn,"type"),"int long long size_t time_t char")){ postproc=".int_var()"; preproc=xcat(preproc,"return ", End); }
	else if(is_word(map_val(fn,"type"),"double float")){ postproc=".double_var()"; preproc=xcat(preproc,"return ", End); }
	else if(str_eq(map_val(fn,"type"),"void")){
		postproc=xcat(postproc,"; return NULL", End);
	}else if(!is_word(map_val(fn,"type"),"void* map* char*")) {return NULL;}
	else {preproc=xcat(preproc,"return ", End);};
	if(isvariadic){ return x_c(xstr(preproc, ret, postproc, End)); };
	return x_c(xstr(preproc, map_val(fn,"name"), "(", null_str(sub_str(ret,0,-1)), ")", postproc, End));
	//pp1
};
char* map_ccode(void* mp){
	if(!mp){ return "NULL"; };
	if(is_str(mp)){ return str_quote(mp); };
	if(is_int(mp)){ return int_str(is_int(mp)); };
	if(ptr_type(mp)==Map){
		if(!map_len(mp)) {return "new_map()";};
		char* ret="xmap(";
		for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i); char* k=map_key(mp, i);
			ret=xcat(ret,xstr(str_quote(k), ",", map_ccode(v), ",", End), End); };
		return xstr(ret,"End)", End); };
	if(ptr_type(mp)==Vector){
		if(!map_len(mp)) {return "new_vec()";};
		char* ret="xvec(";
		for(int i1=next(mp,-1,NULL,NULL); has_id(mp,i1); i1++){ void* v1=map_id(mp,i1);
			ret=xcat(ret,xstr(map_ccode(v1), ",", End), End); };
		return xstr(ret,"End)", End); };
	return fox_error(xstr("Unknown type of variable ", to_str(mp,"",0), " [", ptr_name(mp), "]", End),0);	
};
char* callfunc_c(map* funcs){
	char* ret="function calls\n";
	map* fdups=new_map();
	for(int i2=next(funcs,-1,NULL,NULL); has_id(funcs,i2); i2++){ void* v=map_id(funcs,i2); char* k=map_key(funcs, i2);
		if(map_val(fdups,map_val(v,"name"))){ continue; };
		add(fdups,map_val(v,"name"),int_var(1));
		if(str_eq(k,"args_map")){ continue; };
		if(is_word(map_val(v,"name"),"main run run_cgi")){ continue; };
		char* str_params=func_ccall(v);
		if(!str_params){
			continue; };
		//pp1
		ret=xcat(ret,mstr("\t\tcase %p: { %s break; }\n",str_hash(map_val(v,"name")),str_params, End), End); };
	return ret;
};
char* file_foxh(char* infile,char* outfile){
	return write_file(funcs_cdecl(file_funcs(infile,0),1),outfile,0,1);
};
char* func_cdecl(map* fn,int show_default){
	char* ret2="";
	map* map_1=map_val(fn,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* param=map_id(map_1,i2); char* name=map_key(map_1, i2);
		char* s=str_join(map_val(param,"type")," ",name);
		if(str_eq(name,"...")){ s=name; };
		if(show_default){ s=str_join(s,"=",map_val(param,"default")); };
		ret2=str_join(ret2,", ",s); };
	return xstr(map_val(fn,"decltype"), " ", map_val(fn,"name"), "(", ret2, ");", End);
};
char* funcs_cdecl(map* fns,int show_default){
	char* ret="";
	for(int i=next(fns,-1,NULL,NULL); has_id(fns,i); i++){ void* v =map_id(fns,i); char* k=map_key(fns, i); ret=str_join(ret,"\n",func_cdecl(v,show_default)); };
	return ret;
};
char* foxh(){
	return ""
	"/* This is a generated file. To change it, edit function foxh() in fox.c */\n"
	"#define _XOPEN_SOURCE\n"
	"#ifndef _GNU_SOURCE\n"
	"#define _GNU_SOURCE\n"
	"#endif\n"
	"#include <setjmp.h>\n"
	"#include <memory.h>\n"
	"#include <stdio.h>\n"
	"#include <stdlib.h>\n"
	"#include <assert.h>\n"
	"#include <stdarg.h>\n"
	"#include <math.h>\n"
	"#include <time.h>\n"
	"#include <assert.h>\n"
	"#include <sys/time.h>\n"
	"#include <regex.h>\n"
	"#ifndef __MINGW32__\n"
	"#include <execinfo.h>\n"
	"#include <sys/wait.h>\n"
	"#include <sys/socket.h>\n"
	"#include <netinet/in.h>\n"
	"#include <arpa/inet.h>\n"
	"#include <netdb.h>\n"
	"#endif\n"
	"#include <unistd.h>\n"
	"#include <ctype.h>\n"
	"#include <dirent.h>\n"
	"#include <sys/types.h>\n"
	"#include <sys/stat.h>\n"
	"\n"
	"#include <signal.h>\n"
	"#include <sqlite3.h>\n"
	"#include <openssl/md5.h>\n"
	"\n"
	"enum Types {\n"
	"	Null,Int,Double,String,Blob,Map,Vector,Index,Keys,Cell,Cell2,Tail\n"
	"};\n"
	"typedef struct Mapcell {\n"
	"	short nextid;\n"
	"	int hkey;\n"
	"	char* id;\n"
	"	void* val;\n"
	"} Mapcell;\n"
	"typedef struct map {\n"
	"	int len;\n"
	"	char type;\n"
	"	union {\n"
	"		struct Mapcell* pairs;\n"
	"		void** vars;\n"
	"	};\n"
	"} map;\n"
	"\n"
	"#define each_mem(pg,i) int i=0; for(mempage* pg=_gcdata.pages;i<_gcdata.total_pages;i++,pg=_gcdata.pages+i)\n"
	"#define gc_start() void* __sp=NULL;init_gc(&__sp)\n"
	"#define max(a,b) ((a)>(b)?(a):(b))\n"
	"#define min(a,b) ((a)<(b)?(a):(b))\n"
	"#ifdef __MINGW32__\n"
	"#define is_i(x) ((int)(x)>>30 & 1)\n"
	"#else\n"
	"#define is_i(x) ((long long)(x)>>61 & 2)\n"
	"#define is_f(x) ((*(long long*)&(x))>>61 & 1)\n"
	"#define is_num(x) ((*(long long*)&(x))>>61 & 3)\n"
	"#endif\n"
	"\n"
	"typedef struct mempage {\n"
	"	int no;\n"
	"	int idx;\n"
	"	int block_size;\n"
	"	int blocks;\n"
	"	int free;\n"
	"	char* types;\n"
	"	char* page;\n"
	"	map chains;\n"
	"	int abandoned;\n"
	"} mempage;\n"
	"struct gcdata {\n"
	"	int total_pages;\n"
	"	void** stack_head;\n"
	"	int page_no;\n"
	"	long long max_used;\n"
	"	long long curr_used;\n"
	"	long long max_mem;\n"
	"	long long curr_mem;\n"
	"	mempage* pages;\n"
	"	int gcruns;\n"
	"	int gcwaste;\n"
	"	int inalloc;\n"
	"	int gctime;\n"
	"	int gcmax;\n"
	"	struct timeval run_time;\n"
	"	struct timeval time;\n"
	"	size_t clockstart;\n"
	"	int total_time;\n"
	"};\n"
	"extern struct gcdata _gcdata;\n"
	"extern map* _globals;\n"
	"\n"
	"extern int _printed;\n"
	"extern int _is_web;\n"
	"\n"
	"extern char* skip;\n"
	"\n"
	"#define End (char*)(0x0FF1B14E059AD3BA)\n"
	"\n"
	"void* php_global(char* name);\n"
	""
	"";
};
char* write_foxh(char* outfile){
	return write_file((xstr(foxh(),funcs_cdecl(source_funcs(),0), End)),outfile,0,1);
};
char* fox_phpc(char* infile,char* outfile){
	map* fns=infile ? file_funcs(infile,0) : source_funcs();
	map* temp=str_split(infile,"/",0);
	temp=str_split(map_id(temp,map_len(temp)-1),".",0);
	void* name=map_id(temp,0);
	char* ret=xstr("", 
	"#ifdef HAVE_CONFIG_H\n", 
	"#include \"config.h\"\n", 
	"#endif\n", 
	"#include <php.h>\n", 
	"#include <fox.h>\n", 
	"#include \"", name, ".h\"\n", 
	"\n", 
	"void* zval_var(zval* z);\n", 
	"zval var_zval(void* v);\n", 
	"zval map_zval(map* mp);\n", 
	"map* zval_map(zval* z);\n", 
	"void* call_php(map* params,char* func);\n", 
	"", 
	"", End);
	char* reg=NULL;
	for(int i=next(fns,-1,NULL,NULL); has_id(fns,i); i++){ void* v=map_id(fns,i);
		if(!is_word(map_val(v,"type"),"void* map* int char*")){ continue; };
		if(is_word(map_val(v,"name"),"main")){ continue; };
		char* decls="";
		char* format="";
		char* post="";
		char* pointers="";
		char* call="";
		int hasdefault=0;
		int skip=0;
		void* foxname=map_val(v,"name");
		if(!infile && !str_start(foxname,"fox_")){ foxname=xstr("fox_", map_val(v,"name"), End); };
		map* map_1=map_val(v,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ char* name=map_key(map_1, i2);
			if(str_eq(name,"...")){ skip=1; break; };
			map* v2=map_val(map_val(v,"params"),name);
			if(!hasdefault && map_val(v2,"default")){
				hasdefault=1;
				format=xcat(format,"|", End); };
			if(str_eq(map_val(v2,"type"),"char*")){
				decls=xcat(decls,"\tchar* ","in_",name,"=NULL;\n", End);
				decls=xcat(decls,"\tsize_t ","in_",name,"_len=-1;\n", End);
				pointers=xcat(pointers,", &in_",name,", &in_",name,"_len", End);
				format=xcat(format,"s", End);
				if(map_val(v2,"default")){
					post=xcat(post,xstr("\tif(in_", name, "_len==-1) in_", name, "=", x_c(map_val(v2,"default")), "\n", End), End);
					post=xcat(post,xstr("\telse in_", name, "=str_dup(in_", name, ");\n", End), End); };
			}
			else if(str_eq(map_val(v2,"type"),"int")){
				decls=xcat(decls,"\tlong ","in_",name,"=(1ll<<62);\n", End);
				pointers=xcat(pointers,", &in_",name, End);
				format=xcat(format,"l", End);
				if(map_val(v2,"default")){ post=xcat(post,xstr("\tif(in_", name, "==(1ll<<62)) in_", name, "=", x_c(map_val(v2,"default")), "\n", End), End); };
			}else if(str_eq(map_val(v2,"type"),"void*")){
				decls=xcat(decls,"\tzval* ","in_",name,"_zval=NULL;\n", End);
				pointers=xcat(pointers,", &in_",name,"_zval", End);
				format=xcat(format,"z", End);
				if(map_val(v2,"default")){
					post=xcat(post,"\tvoid* ","in_",name,"=NULL;\n", End);
					post=xcat(post,xstr("\tif(!in_", name, "_zval) in_", name, "=", x_c(map_val(v2,"default")), "\n", End), End);
					post=xcat(post,"\telse in_",name,xstr("=zval_var(in_", name, "_zval);\n", End), End);
				}else{ post=xcat(post,"\tvoid* ","in_",name,xstr("=zval_var(in_", name, "_zval);\n", End), End); };
			}else if(str_eq(map_val(v2,"type"),"map*")){
				decls=xcat(decls,"\tzval* ","in_",name,"_zval=NULL;\n", End);
				pointers=xcat(pointers,", &in_",name,"_zval", End);
				format=xcat(format,"z", End);
				if(map_val(v2,"default")){
					post=xcat(post,"\tvoid* ","in_",name,"=NULL;\n", End);
					post=xcat(post,xstr("\tif(!in_", name, "_zval) in_", name, "=", x_c(map_val(v2,"default")), "\n", End), End);
					post=xcat(post,"\telse in_",name,xstr("=zval_var(in_", name, "_zval);\n", End), End);
				}else{ post=xcat(post,"\tmap* ","in_",name,xstr("=zval_var(in_", name, "_zval);\n", End), End); };
			}else{ skip=1; break; };
			call=xcat(call,xstr("in_", name, ",", End), End);
		};
		if(skip){ continue; };
		reg=xcat(reg,xstr("\tPHP_FE(", foxname, ", NULL)\n", End), End);
		call=sub_str(call,0,-1);
		ret=xcat(ret,xstr("\nPHP_FUNCTION(", foxname, "){\n", End), End);
		if(map_len(map_val(v,"params"))){
			ret=xstr(ret,xstr("", 
			decls, "\tif(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,\"", format, "\"", pointers, ")==FAILURE){ RETURN_NULL(); }\n", 
			post, "\n", 
			"", 
			"", End), End); };
		if(str_eq(map_val(v,"type"),"void")){
			ret=xcat(ret,xstr("\t", map_val(v,"name"), "(", call, ");\n", End), End);
			ret=xcat(ret,"\tRETURN_NULL();\n", End);
		}else{
			ret=xcat(ret,xstr("\t", map_val(v,"type"), " ret=", map_val(v,"name"), "(", call, ");\n", End), End);
			if(str_eq(map_val(v,"type"),"int")){ ret=xcat(ret,"\tRETURN_LONG(ret);\n", End); }
			else if(str_eq(map_val(v,"type"),"char*")){
				ret=xcat(ret,"\tif(!ret) RETURN_NULL();\n", End);
				ret=xcat(ret,"\tRETVAL_STRING(ret);\n", End);
			}else if(str_eq(map_val(v,"type"),"map*")){
				ret=xcat(ret,""
				"	zval zret;\n"
				"	if(!ret){\n"
				"		array_init(&zret);\n"
				"	}\n"
				"	else zret=var_zval(ret);\n"
				"	RETURN_ZVAL(&zret,0,0);\n"
				""
				"", End);
			}else if(str_eq(map_val(v,"type"),"void*")){
				ret=xcat(ret,"\tzval zret=var_zval(ret);\n", End);
				ret=xcat(ret,"\tRETURN_ZVAL(&zret,0,0);\n", End); }; };
		ret=xcat(ret,"}\n", End);
	};
	ret=xcat(ret,xstr("", 
	"\n", 
	"static zend_function_entry ", name, "_functions[] = {\n", 
	reg, "\t{NULL, NULL, NULL}\n", 
	"};\n", 
	"", 
	"", End), End);
	ret=xcat(ret,xstr("", 
	"PHP_RINIT_FUNCTION(", name, "){\n", 
	"	gc_start();\n", 
	"	return SUCCESS;\n", 
	"}\n", 
	"PHP_RSHUTDOWN_FUNCTION(", name, "){\n", 
	"	gc_end();	\n", 
	"	return SUCCESS;\n", 
	"}\n", 
	"\n", 
	"zend_module_entry ", name, "_module_entry = {\n", 
	"#if ZEND_MODULE_API_NO >= 20010901\n", 
	"	STANDARD_MODULE_HEADER,\n", 
	"#endif\n", 
	"\t\"", name, "\",\n", 
	"\t", name, "_functions,\n", 
	"	NULL,\n", 
	"	NULL,\n", 
	"\tPHP_RINIT(", name, "),\n", 
	"\tPHP_RSHUTDOWN(", name, "),\n", 
	"	NULL,\n", 
	"#if ZEND_MODULE_API_NO >= 20010901\n", 
	"	\"0.70\", //Version Number\n", 
	"#endif\n", 
	"	STANDARD_MODULE_PROPERTIES\n", 
	"};\n", 
	"\n", 
	"#ifdef COMPILE_DL_", str_upper(str_dup(name)), "\n", 
	"ZEND_GET_MODULE(", name, ")\n", 
	"#endif\n", 
	"\n", 
	"void xexit(int val){\n", 
	"	zend_error(E_ERROR,\"%s\",\"Exiting Abnormally\");\n", 
	"}\n", 
	"void* fox_error(char* msg,int dump){\n", 
	"	if(dump) php_printf(\"<pre>%s</pre>\",stack_str());\n", 
	"	zend_error(E_ERROR,\"%s\",msg);\n", 
	"	return msg;\n", 
	"}\n", 
	"void* px(void* str,int newline){\n", 
	"	php_printf(\"%s\",str);\n", 
	"	return str;\n", 
	"}\n", 
	"void* zval_var(zval* z){\n", 
	"	if(!z) return NULL;\n", 
	"	int type=Z_TYPE_P(z);\n", 
	"	if(type==IS_NULL) return NULL;\n", 
	"	else if(type==IS_ARRAY) return zval_map(z);\n", 
	"	else if(type==IS_STRING) return str_dup(Z_STRVAL_P(z));\n", 
	"	else if(type==IS_TRUE||type==IS_FALSE||type==IS_LONG) return int_var(Z_LVAL_P(z));\n", 
	"	else if(type==IS_DOUBLE) return int_var((int)Z_DVAL_P(z));\n", 
	"	else if(type==IS_RESOURCE) return \"<RES>\";\n", 
	"	else if(type==IS_OBJECT) return \"<OBJ>\";\n", 
	"	else if(type==IS_REFERENCE) return \"<REF>\";\n", 
	"	else if(type==IS_UNDEF) return NULL;\n", 
	"	else if(type==IS_CONSTANT) return \"<CONST>\";\n", 
	"	else if(type==IS_CONSTANT_AST) return \"<AST>\";\n", 
	"	else if(type==IS_INDIRECT) return \"<INDIRECT>\";\n", 
	"	else if(type==IS_PTR) return \"<PTR>\";\n", 
	"	printf(\"utype=%d\\n\",type);\n", 
	"	return \"<UNKNOWN>\";\n", 
	"}\n", 
	"zval var_zval(void* v){\n", 
	"	zval ret={0};\n", 
	"	if(!v) { ZVAL_NULL(&ret); }\n", 
	"	else if(is_map(v)) return map_zval(v);\n", 
	"	else if(is_str(v) && v){ ZVAL_STRING(&ret,v); }\n", 
	"	else if(is_i(v)){ ZVAL_LONG(&ret,is_int(v)); }\n", 
	"	else if(is_f(v)){ ZVAL_DOUBLE(&ret,is_double(v)); }\n", 
	"	return ret;\n", 
	"}\n", 
	"zval map_zval(map* mp){\n", 
	"	zval ret={0};\n", 
	"	array_init(&ret);\n", 
	"	for(int i=next(mp,-1,NULL,NULL);has_id(mp,i);i++){\n", 
	"		char* k=map_key(mp,i);\n", 
	"		zval z=var_zval(map_id(mp,i));\n", 
	"		if(is_i(k)) add_index_zval(&ret,is_int(k)-1,&z);\n", 
	"		else add_assoc_zval(&ret,k,&z);\n", 
	"	}\n", 
	"	return ret;\n", 
	"}\n", 
	"map* zval_map(zval* z){\n", 
	"	if(!z) return NULL;\n", 
	"	map* mp=new_map();\n", 
	"	zval *arr, *data;\n", 
	"	HashTable *arr_hash=Z_ARRVAL_P(z);\n", 
	"	HashPosition pointer;\n", 
	"	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); (data=zend_hash_get_current_data_ex(arr_hash, &pointer)); zend_hash_move_forward_ex(arr_hash, &pointer)){\n", 
	"		zend_string *zkey;\n", 
	"		zend_ulong index;\n", 
	"		char* key;\n", 
	"		if(zend_hash_get_current_key_ex(arr_hash,&zkey,&index,&pointer)==HASH_KEY_IS_LONG) key=int_var(index+1);\n", 
	"		else key=ZSTR_VAL(zkey);\n", 
	"		map_add(mp,key,zval_var(data));\n", 
	"	}\n", 
	"	return mp;\n", 
	"}\n", 
	"void* php_global(char* name){\n", 
	"	zend_string* key=zend_string_init(name,strlen(name),0);\n", 
	"	zval* zret=zend_hash_find(&EG(symbol_table), key);\n", 
	"	zend_string_release(key);\n", 
	"	if(!zret) return NULL;\n", 
	"	if(Z_TYPE_P(zret)==IS_INDIRECT) zret = Z_INDIRECT_P(zret);\n", 
	"	if(Z_TYPE_P(zret)==IS_REFERENCE) ZVAL_DEREF(zret);\n", 
	"	return zval_var(zret);\n", 
	"}\n", 
	"void* call_php(map* params,char* func){\n", 
	"	zval z={0};\n", 
	"	zval php_func={0};\n", 
	"	ZVAL_STRING(&php_func,func);\n", 
	"	int no=params && params->len ? params->len : 0;\n", 
	"	zval* php_args=NULL;\n", 
	"	void* ret={0};\n", 
	"	if(no){\n", 
	"		php_args=emalloc(sizeof(zval)*no);\n", 
	"		for(int i=0;i<no;i++) php_args[i]=var_zval(map_id(params,i));\n", 
	"	}\n", 
	"	if(call_user_function(CG(function_table),NULL,&php_func,&z,no,php_args)!=SUCCESS){\n", 
	"		zend_error(E_ERROR,\"Call to %s failed\\n\",func);\n", 
	"	}\n", 
	"	else ret=zval_var(&z);\n", 
	"	if(no){\n", 
	"		for(int i=0;i<no;i++) zval_dtor(&php_args[i]);\n", 
	"		efree(php_args);\n", 
	"	}\n", 
	"	zval_dtor(&z);\n", 
	"	return ret;\n", 
	"}\n", 
	"", 
	"", End), End);
	return write_file(ret,outfile,0,1);
};
char* write_phpconfig(){
	return write_file(""
	"PHP_ARG_ENABLE(foxphp, whether to enable FoxPHP library support,\n"
	"[ --enable-foxphp   Enable FoxPHP library support])\n"
	"if test \"$PHP_FOXPHP\" = \"yes\"; then\n"
	"  PHP_SUBST(CFLAGS)\n"
	"  AC_DEFINE(HAVE_FOXPHP, 1, [Whether you have FoxPHP Library])\n"
	"  PHP_NEW_EXTENSION(foxphp, foxphp.c fox.c sql.c extern.c callfunc.c, $ext_shared,,-Wno-logical-op-parentheses -DPHP_MOD)\n"
	"fi\n"
	""
	"","config.m4",0,1);
};
char* write_meta(char* outfile){
	map* funcs=source_funcs();
	char* ret=write_file(x_c(xstr("", 
	"/* This is a generated file. To change it, edit function write_meta() in fox.c */\n", 
	"#include \"sqlite3.h\"\n", 
	"#include \"fox.h\"\n", 
	"\n", 
	"char* version(){\n", 
	"\treturn \"Fox: build: ", increase_version(), ", date: ", time_str(0), " [%s old]\".mstr(\"", time_str(0), "\".time_ago());\n", 
	"}\n", 
	"void* invoke(map* v,char* name){\n", 
	"	unsigned long long idn=str_hash((unsigned char*)name)\n", 
	"	switch(idn){\n", 
	"//", callfunc_c(funcs), "\n", 
	"	}\n", 
	"\treturn \"invoke(): Function $name not defined\".fox_error()\n", 
	"}\n", 
	"map* reflect(){\n", 
	"	return {\n", 
	"\t\tfuncs: ", map_ccode(funcs), ",\n", 
	"\t\tmacros: ", map_ccode(source_macros()), ",\n", 
	"\t\tstructs: ", map_ccode(source_structs()), "\n", 
	"	}\n", 
	"}\n", 
	"", 
	"", End)),outfile,0,1);
	px(mem_usage(),1);
	return ret;
};
map* eval_params(map* sent,char* name,map* env){
	assert(name);
	map* ret=new_map();
	void* fn=map_val(funcs(),name);
	if(!fn){ fox_error(xstr("Function ", name, "() not found", End),0); };
	int named=0;
	map* map_1=map_val(fn,"params"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); char* k=map_key(map_1, i);
		if(str_eq(k,"...")){
			for(int i2=i; i2<=sent->len; i2++){
				vec_add(ret,map_id(sent,i2)); };
			break;
		}else if(map_has_key(sent,k)){
			named=1;
			add(ret,k,map_val(sent,k));
		}else if(!named && map_len(sent)>i && is_i(map_key(sent,i))){
			add(ret,k,map_id(sent,i));
		}else if(map_has_key(env,k)){
			named=1;
			add(ret,k,map_val(env,k));
		}else if(str_eq(k,"env")){
			add(ret,k,env);
		}else if(map_val(v,"default")){
			add(ret,k,eval(map_val(v,"default"),NULL));
		}else{
			fox_error(xstr("Parameter missing in ", map_val(fn,"name"), "(", k, "=?) in ", func_cdecl(fn,1), "\nargs=", json(sent,0), End),0);
			assert(0); }; };
	return ret;
};
char* to_c(void* val){
	if(!val){ return "NULL"; };
	if(is_i(val)){ return int_str(is_int(val)); };
	if(is_f(val)){ return double_str(is_double(val)); };
	if(is_str(val)){ return str_quote(val); };
	if(is_map(val)){ return json(val,0); };
	assert(0);
	return "fox_error";
};
char* str_shorten(char* in,int max){
	if(str_len(in)>max){ return xstr(sub_str(in,0,max),"...", End); };
	return in;
};
char* call_c(map* params,char* name){
	map* ret=new_vec();
	for(int i=next(params,-1,NULL,NULL); has_id(params,i); i++){ void* v=map_id(params,i);
		vec_add(ret,str_shorten(to_c(v),40)); };
	return xstr(name, "(", map_join(ret,", "), ")", End);
};
void* call_func(map* params,char* name,map* env){
	params=eval_params(params,name,env);
	if(str_start(name,"php_")){ return call_php(params,sub_str(name,5,-2147483648)); };
	map* user=map_val(map_val(map_val(_globals,"cache"),"userfuncs"),name);
//	_globals.add_key(:callstack,Vector)[]=name //params.call_c(name)
	void* ret=NULL;
	if(user){
		int halt=0;
		ret=fox_eval(user,params,&halt);
	}else{
		ret=invoke(params,name); };
//	_globals.callstack.vec_del(-1)
	return ret;
};
void* data_exec(void* data,map* env){
	if(!data){ return NULL; };
	if(is_str(data)){ return data; };	
	if(!is_map(data)){ return to_str(data,"",0); };
	char* func=map_id(data,0);
	return call_func(vec_compact(map_del(data,0,1)),func,env);
};
static void* eval_toks(map* mp,map* env){
	int idx=1;
	return eval_expr(mp,&idx,env,0);
};
static void* eval_expr(map* mp,int* idx,map* env,int level){
	void* last=NULL;
	*idx=eval_expr_cont(mp,*idx,env,&last,level);
	return last;
};
void* eval(char* in,map* env){
	return eval_toks(x_toks(in,0),env);
};
int eval_expr_cont(map* mp,int idx,map* env,void** last,int level){
	if(!mp){ return idx; };
	void* ret=*last;
	if(!level) {level=100;};
	for(;idx<mp->len;idx+=2){
		void* val=map_id(mp,idx);	
		if(!val){ continue; }; //abnormal
		if(is_map(val)){ ret=eval_toks(val,env); continue; }
		else if(is_num(val)){ px("int",1); ret=val; continue; }
		else if(is_numeric(val)){
			if(strchr(val,'.')){
				double v2=0.0;
				sscanf(val,"%lf",&v2);
				ret=double_var(v2);
			}else{
				long long v2=0;
				sscanf(val,"%lld",&v2);
				ret=int_var(v2); };
//				ret=atoi(val).int_var()
			continue;
		}else if(fox_at(val,0)=='"'){
			ret=str_unquote(val);
			while(fox_at(is_str(map_id(mp,idx+2)),0)=='"'){
				ret=xcat(ret,str_unquote(map_id(mp,idx+2)), End);
				idx+=2; };
			continue;
		}else if(str_eq(val,"&")){
			void* name=map_id(mp,idx+2);
			if(!is_code(name)){ fox_error(xstr("operator& on a non variable ", name, End),1); };
			int idx1=map_has_key(env,name);
			if(!idx1){ fox_error(xstr("variable ", name, " doesn't exist", End),1); };
			ret=&(env->pairs[idx1-1].val);
			idx+=2;
		}else if(str_eq(val,"(")){ ret=eval_toks(map_id(mp,idx+2),env); idx+=4; continue; }
		else if(str_eq(val,"End")){
			ret=End;
			idx+=2;
		}else if(is_code(val)){
			if(str_eq(val,"NULL")){
				ret=NULL;
			}else if(str_eq(val,"_globals")){
				ret=_globals;
			}else if(str_eq(map_id(mp,idx+2),"(")){
				map* params=map_split(map_id(mp,idx+4),",",0);
				for(int i=next(params,-1,NULL,NULL); has_id(params,i); i++){ void* v=map_id(params,i); set(params,i,eval_toks(v,env)); };
				ret=call_func(params,val,env);
				idx+=6;
			}else if(map_has_key(env,"_") && map_has_key(map_val(env,"_"),val)){
				ret=map_val(map_val(env,"_"),val);
			}else if(str_eq(map_id(mp,idx+2),"=")){
				idx+=4;
				ret=eval_expr(mp,&idx,env,6);
				add(env,val,ret);
			}else if(str_eq(map_id(mp,idx+2),"++")){
				idx+=2;
				add(env,val,binary_op(map_val(env,val),'+',int_var(1)));
			}else if(str_eq(map_id(mp,idx+2),"--")){
				idx+=2;
				add(env,val,binary_op(map_val(env,val),'-',int_var(1)));
			}else if(str_eq(map_id(mp,idx+2),"+=")){
				idx+=4;
				add(env,val,binary_op(map_val(env,val),'+',eval_expr(mp,&idx,env,6)));
			}else if(str_eq(map_id(mp,idx+2),"-=")){
				idx+=4;
				add(env,val,binary_op(map_val(env,val),'-',eval_expr(mp,&idx,env,6)));
			}else if(str_eq(map_id(mp,idx+2),"*=")){
				idx+=4;
				add(env,val,binary_op(map_val(env,val),'*',eval_expr(mp,&idx,env,6)));
			}else if(str_eq(map_id(mp,idx+2),"/=")){
				idx+=4;
				add(env,val,binary_op(map_val(env,val),'/',eval_expr(mp,&idx,env,6)));
			}else{
				ret=map_val(env,val); };
			continue;
		}else if(str_eq(val,".")){ if(is_map(ret)) {ret=map_val(ret,map_id(mp,idx+=2-1));} continue; }
		else if(str_eq(val,"!")){
			idx+=2;
			ret=eval_expr(mp,&idx,env,0);
			ret = int_var((is_i(ret) ? !is_int(ret) : !ret));
			continue;
		};
		int clevel=1;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"/")){ idx+=2; ret=binary_op(ret,'/',eval_expr(mp,&idx,env,clevel)); continue; }
		else if(str_eq(val,"*")){ idx+=2; ret=binary_op(ret,'*',eval_expr(mp,&idx,env,clevel)); continue; };

		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"-")){ idx+=2; ret=binary_op(ret,'-',eval_expr(mp,&idx,env,clevel)); continue; }
		else if(str_eq(val,"+")){ idx+=2; ret=binary_op(ret,'+',eval_expr(mp,&idx,env,clevel)); continue; };


		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(is_word(val,"== !=")){
			idx+=2;
			void* rest=eval_expr(mp,&idx,env,clevel);
			if(is_i(ret)){ ret=int_var((is_int(ret)==to_int(rest))); }
			else if(is_str(ret)){ ret=int_var((str_eq(ret,rest))); }
			else {ret=NULL;};
			if(str_eq(val,"!=")){ ret=int_var((!is_int(ret))); };
			continue;
		}else if(is_word(val,"> < >= <=")){
			idx+=2;
			int val1=is_int(ret);
			int val2=is_int(eval_expr(mp,&idx,env,clevel));
			if(str_eq(val,">")){ val1=val1>val2; }
			else if(str_eq(val,"<")){ val1=val1<val2; }
			else if(str_eq(val,"<=")){ val1=val1<=val2; }
			else if(str_eq(val,">=")){ val1=val1>=val2; };
			ret=int_var(val1);
			continue;
		};
		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"&&")){
			idx+=2;
			if(is_true(ret)){ ret=eval_expr(mp,&idx,env,clevel); }
			else{
				while(idx<mp->len-2){
					if(is_word(map_id(mp,idx),"&& || ? :")){ idx-=2; break; };
					idx+=2; }; };
			continue;
		};
		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"||")){
			idx+=2;
			if(is_true(ret)){ break; }
			else {ret=eval_expr(mp,&idx,env,clevel);};
			continue;
		};
		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"?")){
			idx+=2;
			if(is_true(ret)){
				ret=eval_expr(mp,&idx,env,clevel);
				break;
			}else{
				while(idx<mp->len && !str_eq(map_id(mp,idx),":")){ idx+=2; };
				continue; }; };
		if(str_eq(val,":")){
			break;
		};
		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"=")){
			idx+=2;
			if(!is_str(ret)){ fox_error("eval() in name=value name should be a valid name",0); };
			void* val=eval_expr(mp,&idx,env,clevel);
			add(env,ret,val);
			ret=val;
			continue; };
	};
	*last=ret;
	return idx;
};
void* binary_op(void* left, char oper, void* right){
	if(is_f(left)||is_f(right)){
		double a=is_double(left);
		double b=is_double(right);
		if(oper=='+'){ return double_var((a+b)); };
		if(oper=='-'){ return double_var((a-b)); };
		if(oper=='/'){ return double_var((a/b)); };
		if(oper=='*'){ return double_var((a*b)); };
		return double_var(0);
		fox_error(xstr("Unknown operator ", oper, End),0); };
	long long a=is_int(left);
	long long b=is_int(right);
	if(oper=='+'){ return int_var((a+b)); };
	if(oper=='-'){ return int_var((a-b)); };
	if(oper=='/'){ return int_var((a/b)); };
	if(oper=='*'){ return int_var((a*b)); };
	fox_error(xstr("Unknown operator ", oper, End),0);
	return NULL;
};
int is_true(void * val){
	if(!val){ return 0; };
	if(is_i(val)){ return is_int(val); };
	if(is_f(val)){ return is_double(val); };
	if(is_map(val)){ return map_len(val); };
	if(is_str(val)){ return str_len(val); };
	return 1;
};
char* read_line(FILE* fp){
	char buff[1024];
	char* ret=NULL;
	while(fgets(buff,sizeof(buff),fp)){
		ret=xcat(ret,buff, End); };		
	return ret;
};
char* skip_paren(char* str){
	char open=*str;
	char close=closing_paren(open);
	int level=1;
	str++;
	while(*str && level){
		if(str_chr("\"'`",*str)){ str=skip_quote(str); continue; }
		else if(*str==open){ level++; }
		else if(*str==close){ level--; }
		else if(*str=='\\'){ str++; if(!*str){ break; }; };
		str++; };
	return str;
};
char* skip_quote(char* str){
	if(!str || !*str){ return str; };
	char end=*str;
	str++;
	while(*str && *str!=end){
		if(*str=='\\'){
			str++;
			if(!*str){ break; }; };
		str++; };
	if(*str==end){ str++; };
	return str;
};
char* str_toupper(char* str){
	if(!str) {return NULL;};
	char* ret=str;
	for(;*str;str++) {*str=toupper(*str);};
	return ret;
};
char* str_tolower(char* str){
	if(!str) {return NULL;};
	char* ret=str;
	for(;*str;str++) {*str=tolower(*str);};
	return ret;
};
char* str_title(char* str){
	if(!str) {return NULL;};
	map* words=str_split(str,"_",0);
	for(int i=next(words,-1,NULL,NULL); has_id(words,i); i++){ void* v=map_id(words,i);
		char* s=v;
		if(!str_len(s)) {map_del(words,i,1);}
		else if(is_word(s,"map_id")){
			str_toupper(s);
		}else {s[0]=toupper(s[0]);}; };
	return map_join(words," ");
};
char hex_char(char* in){
	char temp[3]={0};
	if(!in[0]||!in[1]) {return '\0';};
	temp[0]=in[0];
	temp[1]=in[1];
	return (char)strtol(temp,NULL,16);
};
char* rand_str(int len){
	char* ret=NULL;
	while(str_len(ret)<len){ ret=xcat(ret,int_str(rand()), End); };
	return sub_str(ret,0,len);
};

int str_char_count(char* str,char c){
	int ret=0;
	if(!str) {return ret;};
	while(*str) {if(*str++==c) {ret++;};};
	return ret;
};
char* write_c(char* infile,char* outfile){
	source_funcs();
	return write_file(x_c(file_read(infile,1)),outfile,1,1);
};
map* command_line(char* in,int argc,char** argv){
	if(argc==1 || (argc==2 && str_eq(argv[1],"-h"))){ px(in,1); return NULL; };
	map* map_1=str_split(in,"\n",0); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		if(fox_at(v,0)=='\t'){ continue; };
		map* toks=str_split(str_trim(v," \t\n\r")," ",0);
		if(str_eq(map_id(toks,0),argv[1])){
			if(argc!=toks->len+1){
				px("Invalid number of arguments",1);
				px(xstr("Usage: fox ", v, End),1);
				return NULL; };
			map* ret=xvec(str_trim(argv[1],"-"), End);
			for(int i=2;i<argc;i++){ vec_add(ret,argv[i]); };
			return ret; }; };
	px("Unrecognized argument",1);
	px(in,1);
	return NULL;
};
map* param_test(char* one,char* two){
	return xmap("one",one,"two",two, End);
};
map* cmdline_params(map* args,char* func){
	map* params=map_val(map_val(funcs(),func),"params");
	if(!params){ fox_error(xstr("Function: ", func, "() not found", End),0); };
	map* ret=new_map();
	int curr=1;
	int variadic=str_eq(map_key(params,map_len(params)-1),"...") ? map_len(params) : 0;
	char* named_param=NULL;
	for(int i=next(args,-1,NULL,NULL); has_id(args,i); i++){ void* v=map_id(args,i); char* k=map_key(args, i);
		if(str_start(v,"-") && str_len(v)>1){
			char* val=NULL;
			if(named_param){
				add(ret,named_param,int_var(1)); };
			if(str_start(v,"--")){
				named_param=sub_str(v,2,-2147483648);
				if(str_has(v,"=")){
					map* mp1=str_split(named_param,"=",2);
					named_param=map_id(mp1,0);
					val=map_id(mp1,1);
				}else {named_param=sub_str(v,2,-2147483648);};
			}else{
				named_param=sub_str(v,1,-2147483648);
				if(!map_val(params,named_param)){
					for(int i2=next(params,-1,NULL,NULL); has_id(params,i2); i2++){ void* v2=map_id(params,i2); char* k2=map_key(params, i2);
						if(map_has_key(ret,k2)){ continue; };
						if(str_start(k2,named_param)){ named_param=k2; break; }; }; }; };
//				if v.str_len()>2
//					val=v.sub_str(2)
			if(!map_val(params,named_param)){ fox_error(xstr("Invalid parameter ", named_param, " in call to...\n", func_cdecl(map_val(funcs(),func),1), End),0); };
			if(val){
				add(ret,named_param,val);
				named_param=NULL; };
		}else if(named_param){
			add(ret,named_param,v);
			named_param=NULL;
		}else if(variadic && curr>=variadic){
			vec_add(ret,v);
			curr++;
		}else{
			if(curr>map_len(params)){ fox_error(xstr("Excess number of arguments. Function has only ",int_str( map_len(params)), "\narguments ", func_cdecl(map_val(funcs(),func),1), End),0); };
			add(ret,map_key(params,curr-1),v);
			curr++; }; };
	if(named_param){
		add(ret,named_param,int_var(1)); };
	for(int i2=next(params,-1,NULL,NULL); has_id(params,i2); i2++){ void* v2=map_id(params,i2); char* k2=map_key(params, i2);
		if(str_eq(map_val(v2,"type"),"map*")) {}; };
	ret=eval_params(ret,func,NULL);
	for(int i3=next(ret,-1,NULL,NULL); has_id(ret,i3); i3++){ void* v3=map_id(ret,i3); char* k3=map_key(ret, i3);
		if(str_eq(map_val(map_val(params,k3),"type"),"map*")){
			add(ret,k3,str_map(v3,Map)); }; };
	return ret;
};
int test_add(int a,int b){ return a+b; };
map* test_map(map* out){ return out; };

int utests(char* test,char* file){
	map* mp=file_vec(file);
	int errs=0;
	int runs=0;
	for(int i=0; i<map_len(mp); i+=3){
		if(test && !str_has(map_id(mp,i),test)){ continue; };
		runs++;
		printf("Running: %20s\r",map_id(mp,i));
		fflush(stdout);
		errs+=utest(eval(map_id(mp,i+1),NULL),map_id(mp,i+2),map_id(mp,i+1),map_id(mp,i)); };
	int passed=runs-errs;
	px(xstr("\n",int_str( passed), "/",int_str( runs), " tests passed.", End),1);
	return errs;
};
char* file_path(char* file){
	int i=0;
	for(i=str_len(file); i>0; i--){ if(file[i-1]=='/'){ break; }; };
	if(!i){ return "./"; };
	return sub_str(file,0,i);
};
map* load_global(char* file){
	map* ret=str_map(file_read(file,1),Map);
	map_merge(_globals,ret);
	return ret;
};
int err_msg(char* msg,char** ptr){
	if(!ptr){ return 0; };
	*ptr=msg;
	return 0;
};
char* tutorial(){
	return ""
	"# Fox Language\n"
	"Fox language. Transcompiles source into into human readable C.\n"
	"Maintaining your original format, comment and indention and line number.\n"
	"Builtin Fox & C interpreter. Bootstrapped compiler.\n"
	"\n"
	"Source in Fox:\n"
	"```\n"
	"#include \"hello.h\"\n"
	"\n"
	"main(){\n"
	"	\"\n"
	"	Total $(args.map_len()) command line arguments\n"
	"	those are $args\n"
	"	\".px()	\n"
	"\n"
	"	name=:Habib\n"
	"	msg=\"hello, $name!\"\n"
	"	msg.px()\n"
	"\n"
	"	myvector=[:hello, :world]\n"
	"	myvector.px()\n"
	"\n"
	"	mymap={name: :Habib, age: 23}\n"
	"	mymap.name=:Ibrahim\n"
	"	mymap.px()\n"
	"\n"
	"	---\n"
	"	Drawing a box, $name!\n"
	"	|---------|\n"
	"	|         |\n"
	"	|         |\n"
	"	|---------|\n"
	"	---.px()\n"
	"\n"
	"	return 0\n"
	"}\n"
	"\n"
	"```\n"
	"Generated C code:\n"
	"```\n"
	"#include \"hello.h\"\n"
	"\n"
	"int main(int argc, char** argv, char** env){ gc_start(); map* args=argv_map(argv, argc);\n"
	"	px(xstr(\"\", \n"
	"	\"Total \",int_str( map_len(args)), \" command line arguments\\n\", \n"
	"	\"those are \", args, \n"
	"	\"\", End),1);	\n"
	"\n"
	"	char* name=\"Habib\";\n"
	"	char* msg=xstr(\"hello, \", name, \"!\", End);\n"
	"	px(msg,1);\n"
	"\n"
	"	map* myvector=xvec(\"hello\", \"world\", End);\n"
	"	px(myvector,1);\n"
	"\n"
	"	map* mymap=xmap(\"name\", \"Habib\", \"age\",int_var( 23), End);\n"
	"	add(mymap,\"name\",\"Ibrahim\");\n"
	"	px(mymap,1);\n"
	"\n"
	"	px(xstr(\"\", \n"
	"	\"Drawing a box, \", name, \"!\\n\", \n"
	"	\"|---------|\\n\", \n"
	"	\"|         |\\n\", \n"
	"	\"|         |\\n\", \n"
	"	\"|---------|\", \n"
	"	\"\", End),1);\n"
	"\n"
	"	return 0;\n"
	"};\n"
	"```\n"
	"Run and output\n"
	"```\n"
	"$ fox cc hello.fox\n"
	"-> hello.c\n"
	"-> hello.h\n"
	"gcc -g -Os hello.c -o hello -std=gnu99 -Wno-logical-op-parentheses -lm -lfox 2>&1\n"
	"\n"
	"$ ./hello\n"
	"Total 1 command line arguments were passed\n"
	"and those are [\"./hello\"]\n"
	"hello, Habib!\n"
	"[\"hello\", \"world\"]\n"
	"{\"name\":\"Habib\", \"age\":23}\n"
	"{\"name\":\"Ibrahim\", \"age\":23}\n"
	"Drawing a box, Habib!\n"
	"|---------|\n"
	"|         |\n"
	"|         |\n"
	"|---------|\n"
	"\n"
	"$\n"
	"```\n"
	"\n"
	"## Features\n"
	"\n"
	"### Small code base\n"
	"1K lines for the runtime. The compiler with interpreter is 4K lines. Creates small 30KB binary for staticaly linked hello world app.\n"
	"\n"
	"### GCed\n"
	"Mark and sweep tracing GC. 50ms max delay in average use cases.\n"
	"\n"
	"### Minimal boiler code\n"
	"Primitive types are not boxed. Use C native int, double, float, char* as usual. Variants are void*.\n"
	"It isn't until you wish to use vectors and maps when you need structures.\n"
	"Vector and map structure\n"
	"```\n"
	"typedef struct map {\n"
	"	int len;\n"
	"	char type;\n"
	"	union {\n"
	"		struct Mapcell* pairs;\n"
	"		void** vars;\n"
	"	};\n"
	"} map;\n"
	"\n"
	"typedef struct Mapcell {\n"
	"	short nextid;\n"
	"	int hkey;\n"
	"	char* id;\n"
	"	void* val;\n"
	"} Mapcell;\n"
	"\n"
	"```\n"
	"\n"
	"### Inline maps and vectors. Inline JSON.\n"
	"```\n"
	"data={name: Habib, age: 31}\n"
	"data.name=:Ibrahim\n"
	"days=[sun, mon, tue]\n"
	"days[1].px()\n"
	"```\n"
	"\n"
	"### Reflection\n"
	"```\n"
	"funcs()\n"
	"[\"hello\",\"Abdul\"].call_func(:hello_world)\n"
	"```\n"
	"\n"
	"### Multiline string\n"
	"Three dashes with variable substitution.\n"
	"```\n"
	"---\n"
	"Thee dash multiline comment\n"
	"With $variable. \n"
	"Put code like this: $(1+1)\n"
	"Or call functions $(name.str_upper())\n"
	"---\n"
	"```\n"
	"Or double quotes. Same as triple dashes.\n"
	"```\n"
	"\"\n"
	"Escape dollar sign using double dollars, like $$this.\n"
	"\"\n"
	"```\n"
	"Or single quote. No variable substitution.\n"
	"```\n"
	"'\n"
	"Single quote, without variable substitution.\n"
	"No $variable substitution will occure.\n"
	"'\n"
	"```\n"
	"You can add string terminators with single and double quote version\n"
	"Or extend the tripple dashes with more dashes and a matched closing.\n"
	"```\n"
	"\".end1\n"
	"Using an unique string terminator.\n"
	".end1\"\n"
	"```\n"
	"### Function chaining\n"
	"```\n"
	"\"myfile.txt\".file_open().parse().print()\n"
	"```\n"
	"\n"
	"### Default parameters\n"
	"```\n"
	"int fn(char* p1=\"hello\", p2=\"world\")\n"
	"fn(,\"hi\")\n"
	"```\n"
	"\n"
	"### Indented blocks\n"
	"```\n"
	"if a==1\n"
	"	:hello.px()\n"
	"```\n"
	"Use => in single line blocks\n"
	"```\n"
	"if a==1 || b==2 => done=1; return 1\n"
	"```\n"
	"\n"
	"### No semicolons, unless needed\n"
	"```\n"
	"a=1+1\n"
	"a.px()\n"
	"```	\n"
	"### Inherited types\n"
	"```\n"
	"i=0\n"
	"s=\"\"\n"
	"vals={a: b, c: d}\n"
	"```\n"
	"\n"
	"### Easily call functions from command line\n"
	"```\n"
	"./fox sub_str hello 3 -1\n"
	"```\n"
	"\n"
	"### Execute code from CL\n"
	"```\n"
	"./fox \":todo.table('todo.db').cols.px()\"\n"
	"```\n"
	"\n"
	"### Execute a file from CL\n"
	"```\n"
	"./fox example/script.fox\n"
	"```\n"
	"\n"
	"### Compile\n"
	"```\n"
	"make\n"
	"make install\n"
	"make tests\n"
	"```\n"
	"\n"
	"### Unit Tests\n"
	"```\n"
	"cd tests\n"
	"fox utests\n"
	"```\n"
	"\n"
	"### Bind with PHP. Every function prefixed with :fox_\n"
	"```\n"
	"<?=fox_hello()?>\n"
	"```\n"
	"\n"
	"### Embeded variables in string\n"
	"```\n"
	"\"name=$val\"\n"
	"\"name=$(val+1)\"\n"
	"```\n"
	"\n"
	"### eval() most of any code\n"
	"```\n"
	"\"(1+2).px()\".eval()\n"
	"```\n"
	"\n"
	"### String operations\n"
	"```\n"
	"cat .. cat .. cat\n"
	"compare === compare\n"
	"not !== not\n"
	"merge .= string\n"
	"```\n"
	"\n"
	"## Syntax\n"
	"```\n"
	"int i=1;\n"
	"mp=[];\n"
	"\n"
	"int i=1\n"
	"map* mp=[]\n"
	"```\n"
	"\n"
	"### Function Chaining\n"
	"```\n"
	"px(json(str_split(str_trim(\"habib,ayman\"),\",\")));\n"
	"\"habib,ayman\".str_trim().str_split(\",\").json().px()\n"
	"\"habib,ayman\"\n"
	"	.str_trim()\n"
	"	.str_split(\",\")\n"
	"	.json()\n"
	"	.px()\n"
	"```\n"
	"\n"
	"### Code Blocks\n"
	"```\n"
	"if(n<2){\n"
	"	n*=2;\n"
	"	return n;\n"
	"}\n"
	"if(n<2)\n"
	"	n*=2\n"
	"	return n\n"
	"if n<2\n"
	"	n*=2\n"
	"	return n\n"
	"if n<2 => n*=2; return n\n"
	"```\n"
	"\n"
	"### Strings\n"
	"```\n"
	"char* a=\"hello, world\"\n"
	"char* a=:hello\\,\\ world\n"
	"char* a=\"\n"
	"	hello, world\n"
	"	\"\n"
	"char* a=---\n"
	"	hello, world\n"
	"	---\n"
	"```\n"
	"### Maps\n"
	"```\n"
	"map* mp={}\n"
	"map* mp=[]\n"
	"```\n"
	"\n"
	"### Vectors\n"
	"```\n"
	"map* mp=[]\n"
	"```\n"
	"\n"
	"### Populate Map\n"
	"```\n"
	"mp={name: :Sanjir}[:age]=21\n"
	"mp={name: :Sanjir,age: 21}\n"
	"mp=\"\n"
	"	name=Sanjir\n"
	"	age=#21\n"
	"	\".str_map()\n"
	"mp=\"name=Sanjir,age=#21\".str_map()\n"
	"```\n"
	"\n"
	"### Initialize Vector\n"
	"```\n"
	"mp=[:Habib,:Anas,:Ayman]\n"
	"mp=\"\n"
	"	Habib\n"
	"	Anas\n"
	"	Ayman\n"
	"	\".str_vec()\n"
	"mp=\"Habib,Anas,Ayman\".str_vec()\n"
	"```\n"
	"\n"
	"### Access by key	\n"
	"```\n"
	"map_val(map_val(:address,mp),:city)\n"
	"mp[:address][:city]\n"
	"mp.:address.:city\n"
	"mp.address.city\n"
	"```\n"
	"\n"
	"### Iterate\n"
	"```\n"
	"for(int i=1;i<=mp->len;i++)\n"
	"	mp[i-1].px()\n"
	"\n"
	"mp.each ,,i\n"
	"	mp[i-1].px()\n"
	"\n"
	"mp.each v,,i\n"
	"	v.px()\n"
	"```\n"
	"\n"
	"```\n"
	"mp.each v,,i => => v.px()\n"
	"mp.each v,,i => v.px()\n"
	"mp.each ,,i => mp[i-1].px()\n"
	"mp.each ,k,i => mp[k].px()\n"
	"mp.each v ,,i=> v.px()\n"
	"mp.each v ,k,i=> v.px()\n"
	"```\n"
	"\n"
	"```\n"
	"input.str_split(\" \").each v,,i\n"
	"	v.px()\n"
	"```\n"
	"\n"
	"### Functions\n"
	"```\n"
	"int add(int a,int b){\n"
	"	return a+b;\n"
	"}\n"
	"int add(int a,int b)\n"
	"	return a+b\n"
	"int add(int a,int b) => return a+b\n"
	"```\n"
	"\n"
	"### Default Parameters\n"
	"```\n"
	"int http_out(\n"
	"	char* body,\n"
	"	int status=200,\n"
	"	char* mime=\"text/html\",\n"
	"	char* template=_globals.default.template\n"
	"){\n"
	"	body.px()\n"
	"}\n"
	"```\n"
	"\n"
	"### Invoke Functions\n"
	"```\n"
	"int increase(int num,int inc_by=2) => return num * inc_by\n"
	"\n"
	"increase(7)\n"
	"increase(7,2)\n"
	"[7,2].call_func(:increase)\n"
	"[7].call_func(:increase)\n"
	"[NULL,:increase,:num,7,:inc_by,2].data_exec()\n"
	"\"\n"
	"	increase\n"
	"	num=7\n"
	"	inc_by=2\n"
	"	\".str_map().data_exec()\n"
	"```\n"
	"\n"
	"### Eval\n"
	"```\n"
	"\"1+1\".eval()==2\n"
	"\"1 && 2\".eval()==2\n"
	"\"7 || 4\".eval()==7\n"
	"\"info.name\".eval(\"\n"
	"	info\n"
	"		name=Habib\n"
	"	\".str_map())\n"
	"```\n"
	"\n"
	"\n"
	"### Template\n"
	"```\n"
	"\"\n"
	"	name=Habib\n"
	"	age=32\n"
	"\".render(\"\n"
	"	Hi #{name}! Your age is #{age}\n"
	"\")\n"
	"\n"
	"\"\n"
	"	name=Habib\n"
	"	age=32\n"
	"\".render(\"\n"
	"	We know the following information about you!\n"
	"	--body\n"
	"	#{_key}: #{_val}\n"
	"	--foot\n"
	"	That was all!\n"
	"\")\n"
	"```\n"
	"\n"
	"### String Operation	\n"
	"```\n"
	"str=\"hello world\"\n"
	"str=:hello\\ world\n"
	"str===:hello\n"
	"str.is_word(\"hello helo hilo\")\n"
	"```\n"
	""
	"";
};
char* h(char* in){
	return str_replace(in,xmap(
	"&", "&amp;",
	"<", "&gt;",
	">", "&lt;",
	"\"", "&quot;",
	"'", "&apos;"
	, End),NULL);
};
char* type_name(int type){
	char* names[]={"Free","Int","Double","String","Blob","Map","Vector","Index","Keys","Cell","Cell2"};
	return names[type];
};
char* ptr_name(void* var){ return type_name(ptr_type(var)); };
int is_file(char* filename){
	struct stat buff={0};
	return stat(filename,&buff)==0;
};
char* fork_exec(char* cmd,map* params){
	pid_t pid=fork();
	if(pid==0){
		switch(map_len(params)){
			case 0: execlp(cmd,cmd,NULL); break;
			case 1: execlp(cmd,cmd,map_id(params,0),NULL); break;
			case 2: execlp(cmd,cmd,map_id(params,0),map_id(params,1),NULL); break;
			case 3: execlp(cmd,cmd,map_id(params,0),map_id(params,1),map_id(params,2),NULL); break;
			case 4: execlp(cmd,cmd,map_id(params,0),map_id(params,1),map_id(params,2),map_id(params,3),NULL); break;
			case 5: execlp(cmd,cmd,map_id(params,0),map_id(params,1),map_id(params,2),map_id(params,3),map_id(params,4),NULL); break;
			case 6: execlp(cmd,cmd,map_id(params,0),map_id(params,1),map_id(params,2),map_id(params,3),map_id(params,4),map_id(params,5),NULL); break;
			default: return "6 max params supported in fork/exec"; break; };
	}else if(pid<0){ return xstr("Can't exec ", cmd, End); }
	else{
		int status=0;
		pid_t ws=waitpid(pid,&status,WNOHANG);
		if(ws == -1){ return xstr("Exec fox_error while running ", cmd, End); }
		else if(WIFEXITED(status)){ return WEXITSTATUS(status) ? mstr("command %s failed",cmd, End) : NULL; }
		else if(WIFSIGNALED(status)){ mstr("command %s killed",cmd, End); }; };
	return NULL;
};
map* source_files(){ return xvec("core.fox","fox.fox","sql.fox","cmd.fox","cgi.fox","astro/astro.h","astrostr.fox", End); };
map* source_funcs(){
	if(!map_val(map_val(_globals,"cache"),"funcs")){
		map* mp=new_map();	
		map* map_1=source_files(); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
			map* mp3=file_funcs(v,0);
			map_merge(mp,mp3); };
		add(add_key(_globals,"cache",Map),"funcs",mp); };
	return map_val(map_val(_globals,"cache"),"funcs");
};
map* file_funcs(char* filename,int withbody){ return x_funcs(file_read(filename,1),withbody); };
char* help(){ return funcs_cdecl(funcs(),1); };
map* funcs(){
	if(!map_val(map_val(_globals,"cache"),"reflect")){add(add_key(_globals,"cache",Map),"reflect",reflect()); };
	return map_val(map_val(_globals,"cache"),"funcs") ? map_val(map_val(_globals,"cache"),"funcs") : map_val(map_val(map_val(_globals,"cache"),"reflect"),"funcs");
};
void* fox_run(char* in){
	int halt=0;
	return fox_eval(x_toks(in,1),xmap("args", map_val(_globals,"args"), End),&halt);
};
void* fox_eval(map* mp,map* env,int* halt){
	if(!mp){ return NULL; };
	assert(ptr_type(mp)==Vector);
	enum {HBreak=1, HReturn, HContinue};
	void* v=map_id(mp,1);
	void* ret=NULL;
	if(is_map(v)){
		for(int i3=next(mp,-1,NULL,NULL); has_id(mp,i3); i3++){ void* v3=map_id(mp,i3); char* k3=map_key(mp, i3);
			ret=fox_eval(v3,env,halt);
			if(*halt){ return ret; }; };
		return ret; };
	if(str_eq(v,"if")){
		int idx=5;
		while(1){
			if(is_true(eval_toks(map_id(mp,idx),env))){
				return fox_eval(map_id(mp,idx+6),env,halt);
			}else if(str_eq(map_id(mp,idx+10),"else")){
				if(str_eq(map_id(mp,idx+12),"if")){ idx+=16; }
				else {return fox_eval(map_id(mp,idx+14),env,halt);};
			}else{
				return NULL; }; };
	}else if(str_eq(v,"while")){
		while(is_true(eval_toks(map_id(mp,5),env))){
			ret=fox_eval(map_id(mp,11),env,halt);
			if(*halt==HContinue){ *halt=0; }
			else if(*halt==HBreak){ *halt=0; break; }
			else if(*halt){ break; }; };
		return ret;
	}else if(str_eq(v,"for")){
		map* conds=map_id(mp,5);
		for(eval_toks(map_id(conds,1),env); is_true(eval_toks(map_id(conds,3),env)); eval_toks(map_id(conds,5),env)){
			ret=fox_eval(map_id(mp,11),env,halt);
			if(*halt==HContinue){ *halt=0; }
			else if(*halt==HBreak){ *halt=0; break; }
			else if(*halt){ break; }; };
		return ret;
	}else if(str_eq(v,"return")){
		*halt=HReturn;
		return eval_toks(mp,env);
	}else if(str_eq(v,"break")){
		*halt=HBreak;
	}else if(str_eq(v,"continue")){
		*halt=HContinue;
	}else if(is_func_decl(mp)){
		return ret;
	}else{
		return eval_toks(mp,env); };
	return ret;
};
void src(map* mp,int from,int len,char* msg){
	if(!len){ len=mp->len-from; };
	px(json(vec_sub(mp,from,len),0),1);
	if(msg){ px(xstr(msg, " ___________________________", End),1); };
};
map* structs(){
	return map_val(_globals,"structs") ? map_val(_globals,"structs") : map_val(map_val(map_val(_globals,"cache"),"reflect"),"structs");
};
map* macros(){
	return map_val(_globals,"macros") ? map_val(_globals,"macros") : map_val(map_val(map_val(_globals,"cache"),"reflect"),"macros");
};
map* source_structs(){
	if(!map_val(_globals,"structs")){ add(_globals,"structs",c_structs(write_foxh(NULL))); };
	return map_val(_globals,"structs");
};
map* source_macros(){
	if(!map_val(_globals,"macros")){ add(_globals,"macros",c_macros(write_foxh(NULL))); };
	return map_val(_globals,"macros");
};
int fib(int i){
	if(i < 2){ return i; };
	return fib(i-1)+fib(i-2);
};
map* toks_syn(map* toks,int recurse){
	map* ret=new_vec();
	map* line=new_vec();
	for(int i=0; i<map_len(toks)-1; i+=2){
		void* v=map_id(toks,i+1);
		xadd(line,map_id(toks,i), v, End);
		if(str_eq(v,";")){
			xadd(ret,NULL,line, End);
			line=new_vec(); };
		if(recurse && is_word(v,") else do") && str_eq(map_id(toks,i+3),"{")){
			set(toks,i+5, toks_syn(map_id(toks,i+5),1));
		}else if(str_eq(v,"for")){ set(toks,i+5, toks_syn(map_id(toks,i+5),1)); }; };
	if(map_len(line)){ xadd(ret,NULL,line, End); };
	if(map_len(toks)%2){ xadd(ret,NULL,xvec(map_id(toks,map_len(toks)-1),NULL, End), End); };
	return ret;
};
time_t str_time(char* in){
	struct tm tm={0};
	tm.tm_year=atoi(sub_str(in,0,4))-1900;
	tm.tm_mon=atoi(sub_str(in,5,2))-1;
	tm.tm_mday=atoi(sub_str(in,8,2));
	tm.tm_hour=atoi(sub_str(in,11,2));
	tm.tm_min=atoi(sub_str(in,14,2));
	tm.tm_sec=atoi(sub_str(in,17,2));
	return mktime(&tm);
};
char* time_str(time_t timer){
	char buffer[20];
	if(!timer){ timer=time(0); };
	strftime(buffer,20, "%Y-%m-%d %H:%M:%S",localtime(&timer));
	return str_dup(buffer);
};
char* increase_version(){ return write_file(int_str((atoi(file_read(".version.txt",1))+1)),".version.txt",0,1); };
int call_variadic_int(map* mp,void* fp,char* name){
	int(*ptr)(void* param1,...)=fp;
	if(!mp){ return ptr(End); };
	int len=map_len(mp);
	if(map_id(mp,mp->len-1)==(void*)End){ len--; };
	if(len==0){ return ptr(End); };
	if(len==1){ return ptr(map_id(mp,0),End); };
	if(len==2){ return ptr(map_id(mp,0),map_id(mp,1),End); };
	if(len==3){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),End); };
	if(len==4){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),End); };
	if(len==5){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),End); };
	if(len==6){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),End); };
	if(len==7){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),End); };
	if(len==8){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),End); };
	if(len==9){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),End); };
	if(len==10){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),End); };
	if(len==11){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),End); };
	if(len==12){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),End); };
	if(len==13){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),End); };
	if(len==14){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),End); };
	if(len==15){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),End); };
	if(len==16){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),End); };
	if(len==17){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),End); };
	if(len==18){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),End); };
	if(len==19){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),End); };
	if(len==20){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),End); };
	if(len==21){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),End); };
	if(len==22){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),End); };
	if(len==23){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),End); };
	if(len==24){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),End); };
	if(len==25){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),End); };
	if(len==26){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),End); };
	if(len==27){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),End); };
	if(len==28){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),End); };
	if(len==29){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),map_id(mp,28),End); };
	if(len==30){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),map_id(mp,28),map_id(mp,29),End); };
	if(len==31){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),map_id(mp,28),map_id(mp,29),map_id(mp,30),End); };
	if(len==32){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),map_id(mp,28),map_id(mp,29),map_id(mp,30),map_id(mp,31),End); };
	fox_error(xstr("Only 32 parameters supported in call to function ", name, " sent=", json(mp,0), End),0);
	return 0;
};
void* call_variadic_ptr(map* mp,void* fp,char* name){
	void*(*ptr)(void* param1,...)=fp;
	if(!mp){ return ptr(End); };
	int len=map_len(mp);
	if(map_id(mp,mp->len-1)==(void*)End){ len--; };
	if(len==0){ return ptr(End); };
	if(len==1){ return ptr(map_id(mp,0),End); };
	if(len==2){ return ptr(map_id(mp,0),map_id(mp,1),End); };
	if(len==3){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),End); };
	if(len==4){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),End); };
	if(len==5){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),End); };
	if(len==6){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),End); };
	if(len==7){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),End); };
	if(len==8){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),End); };
	if(len==9){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),End); };
	if(len==10){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),End); };
	if(len==11){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),End); };
	if(len==12){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),End); };
	if(len==13){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),End); };
	if(len==14){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),End); };
	if(len==15){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),End); };
	if(len==16){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),End); };
	if(len==17){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),End); };
	if(len==18){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),End); };
	if(len==19){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),End); };
	if(len==20){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),End); };
	if(len==21){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),End); };
	if(len==22){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),End); };
	if(len==23){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),End); };
	if(len==24){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),End); };
	if(len==25){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),End); };
	if(len==26){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),End); };
	if(len==27){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),End); };
	if(len==28){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),End); };
	if(len==29){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),map_id(mp,28),End); };
	if(len==30){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),map_id(mp,28),map_id(mp,29),End); };
	if(len==31){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),map_id(mp,28),map_id(mp,29),map_id(mp,30),End); };
	if(len==32){ return ptr(map_id(mp,0),map_id(mp,1),map_id(mp,2),map_id(mp,3),map_id(mp,4),map_id(mp,5),map_id(mp,6),map_id(mp,7),map_id(mp,8),map_id(mp,9),map_id(mp,10),map_id(mp,11),map_id(mp,12),map_id(mp,13),map_id(mp,14),map_id(mp,15),map_id(mp,16),map_id(mp,17),map_id(mp,18),map_id(mp,19),map_id(mp,20),map_id(mp,21),map_id(mp,22),map_id(mp,23),map_id(mp,24),map_id(mp,25),map_id(mp,26),map_id(mp,27),map_id(mp,28),map_id(mp,29),map_id(mp,30),map_id(mp,31),End); };
	fox_error(xstr("Only 32 parameters supported in call to function ", name, " sent=", json(mp,0), End),0);
	return 0;
};
map* vec_rdup(map* mp){
	assert(is_vec(mp));
	map* ret=vec_dup(mp);
	for(int i=next(ret,-1,NULL,NULL); has_id(ret,i); i++){ void* v=map_id(ret,i); char* k=map_key(ret, i);
		if(is_vec(v)){ set(ret,i,vec_rdup(v)); }; };
	return ret;
};
map* toks_replace(map* in,map* replace){
	for(int i=next(in,-1,NULL,NULL); has_id(in,i); i++){ void* v=map_id(in,i); char* k=map_key(in, i);
		if(is_map(v)){
			toks_replace(v,replace);
		}else if(is_str(v) && map_val(replace,v)){
			vec_compact(vec_splice(in,i,1,map_val(replace,v)));
			i+=map_len(map_val(replace,v))+1; }; };
	return in;
};
map* toks_join(map* in, char* by){
	if(!in){ return NULL; };
	map* ret=vec_dup(map_id(in,0));
	assert(is_vec(ret));
	for(int i=1; i<in->len; i++){
		if(!(map_len(ret)%2)){ xadd(ret,NULL, End); };
		xadd(ret,by, End);
		vec_merge(ret,map_id(in,i)); };
	return ret;
};
map* toks_align(map* in){
	int last=map_len(in);
	if(!(last%2)){ return in; };
	if(map_id(in,last-1)==NULL){
		vec_compact(vec_del(in,-1,1));
	}else {vec_add(in,NULL);};
	return in;
};
map* toks_split(map* in,char* by,int limit){
	map* ret=map_split(in,by,limit);
	for(int i=1;i<ret->len;i++){ toks_align(map_id(ret,i-1)); };
	return ret;
};
map* expand_macros(map* mp,map* macros){
	if(!mp){ return NULL; };
	assert(ptr_type(mp)==Vector);
	int old=0;
	for(int i=1; i<mp->len; i+=2){
		assert(i!=old);
		old=i;
		void* v=map_id(mp,i);
		if(is_map(v)){
			expand_macros(v,macros);
		}else if(str_eq(map_id(mp,i+2),"(") && is_str(v) && map_val(macros,v) && !is_word(v,"End")){
			map* macro=map_val(macros,map_id(mp,i));
			map* params=toks_split(map_id(mp,i+4),",",0);
			if(map_len(map_val(macro,"params"))!=map_len(params)){ fox_error(xstr("Macro ", map_id(mp,i), " takes ",int_str( map_len(map_val(macro,"params"))), " parameters, ",int_str( map_len(params)), " provided", End),0); };
			map* subs=new_map();
			map* map_1=map_val(macro,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* v2=map_id(map_1,i2);
				add(subs,v2,vec_compact(vec_del(map_id(params,i2),0,1))); };
			map* expanded=toks_replace(vec_rdup(map_val(macro,"body")),subs);
			vec_splice(mp,i,7,vec_compact(vec_del(expanded,0,1)));
			i+=map_len(expanded)-7; }; };
	return mp;
};
map* c_macros(char* in){ return toks_macros(x_map(in)); };
map* toks_macros(map* mp){
	map* ret=new_map();
	for(int i=1; i<map_len(mp); i+=2){
		if(!str_has(map_id(mp,i-1),"#define")){ continue; };
		map* map_1=str_split(map_id(mp,i-1),"\n",0); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* line=map_id(map_1,i2);
			if(!str_start(line,"#define")){ continue; };
			map* toks=x_map(drop_left(line,"#define "));
			if(!str_eq(map_id(toks,3),"(")){ continue; };
			map* params=new_vec();
			map* map_1=map_split(map_id(toks,5),",",0); for(int i3=next(map_1,-1,NULL,NULL); has_id(map_1,i3); i3++){ void* v=map_id(map_1,i3); char* k=map_key(map_1, i3);
				vec_add(params,map_id(v,1)); };
			int upto=9;
			for(;upto<toks->len; upto+=2){ if(str_has(is_str(map_id(toks,upto-1)),"\n")){ break; }; };
			add(ret,map_id(toks,1), xmap("name", map_id(toks,1), "params", params, "body", vec_sub(toks,8,upto-9), End)); }; };
	return ret;
};
int is_inline_vector(map* toks,int idx){
	if(!is_word(map_id(toks,idx),"[ { ")){ return 0; };
	char* pre=is_str(map_id(toks,idx-2));
	if(str_eq(pre,":")){
		for(int i=idx-4; i>=0; i-=2){
			if(str_eq(map_id(toks,i),"case")){ return 0; }; }; };
	if(is_word(pre,") ]")){ return 0; };
	if(is_code(pre) && !is_word(pre,"return")){ return 0; };
	if(str_eq(pre,"=")){
		char* type=old_decl_type(toks,idx-4);
		if(type && !is_word(type,"map* void*")){ return 0; }; };
	return str_eq(map_id(toks,idx),"[") ? Vector : Map;
};
map* string_operators(map* toks){
	for(int i=0;i<=map_len(toks);i+=2){
		if(is_map(map_id(toks,i+1))){
			string_operators(map_id(toks,i+1));
		}else if(str_eq(map_id(toks,i+1),".=")){
			int head=expr_head(toks,i-2,":");
			int tail=expr_tail(toks,i+2,".=");
			map* mid=vec_sub(toks,head+1,i-head-1);
			vec_merge(vec_splice(mid,0,0,xvec(NULL, End)),xvec(NULL,",", End));
			vec_merge(vec_merge(mid,vec_sub(toks,i+2,tail-i-2)),xvec(NULL,","," ","End", End));
			for(int i2=next(mid,-1,NULL,NULL); has_id(mid,i2); i2++){ void* v2=map_id(mid,i2); char* k2=map_key(mid, i2);
				if(str_eq(v2,"..")){
					add(mid,k2,","); }; };
			mid=xvec("xcat",NULL,"(",NULL,mid,NULL,")", End);
			vec_splice(toks,i+3,tail-i-3,mid);
			set(toks,i+1,"=");
			i=head;
		}else if(str_eq(map_id(toks,i+1),"and")){
			int head=expr_head(toks,i-2,"and");
			int tail=expr_tail(toks,i+2,"and");
			map* left=vec_sub(toks,head+1,i-head-1);
			map* right=string_operators(vec_sub(toks,i+2,tail-i-2));
			map* mid=xvec("(",NULL,vec_merge(vec_merge(vec_merge(vec_merge(xvec(NULL, End),left),xvec(map_id(toks,i),"?", End)),right),xvec(" ",":"," ","NULL", End)),NULL,")", End);
			vec_splice(toks,head+1,tail-head-1,mid);
		}else if(str_eq(map_id(toks,i+1),"or")){
			int head=expr_head(toks,i-2,"or");
			int tail=expr_tail(toks,i+2,"or");
			map* left=vec_sub(toks,head+1,i-head-1);
			map* right=string_operators(vec_sub(toks,i+2,tail-i-2));
			map* mid=xvec("(",NULL,vec_merge(vec_merge(vec_merge(vec_merge(vec_merge(xvec(NULL, End),left),xvec(map_id(toks,i),"?"," ", End)),left),xvec(" ",":", End)),right),NULL,")", End);
			vec_splice(toks,head+1,tail-head-1,mid);
		}else if(str_eq(map_id(toks,i+1),"||=")){
			int head=expr_head(toks,i-2,":");
			int tail=expr_tail(toks,i+2,"||=");
			map* left=vec_sub(toks,head+1,i-head-1);
			map* right=vec_sub(toks,i+2,tail-i-2);
			map* mid=xvec("if",NULL,"(",NULL,xvec(NULL,"!",NULL,"(",NULL,vec_merge(xvec(NULL, End),left),NULL,")", End),NULL,")",NULL,"{",map_id(toks,i),xvec(NULL,vec_merge(vec_merge(vec_merge(vec_merge(xvec(NULL, End),left),xvec(NULL,"=", End)),right),xvec(NULL,";", End)), End),NULL,"}", End);
			vec_splice(toks,head+1,tail-head-1,mid);
		}else if(str_eq(map_id(toks,i+1),"&&=")){
			int head=expr_head(toks,i-2,":");
			int tail=expr_tail(toks,i+2,"&&=");
			map* left=vec_sub(toks,head+1,i-head-1);
			map* right=vec_sub(toks,i+2,tail-i-2);
			map* mid=xvec("if",NULL,"(",NULL,vec_merge(xvec(NULL, End),left),NULL,")",NULL,"{",map_id(toks,i),xvec(NULL,vec_merge(vec_merge(vec_merge(vec_merge(xvec(NULL, End),left),xvec(NULL,"=", End)),right),xvec(NULL,";", End)), End),NULL,"}", End);
			vec_splice(toks,head+1,tail-head-1,mid);
		}else if(str_eq(map_id(toks,i+1),"..")){
			int head=expr_head(toks,i-2,"<");
			int tail=expr_tail(toks,i+2,"<");
			map* mid=vec_merge(vec_splice(vec_sub(toks,head+1,tail-head-1),0,0,xvec(NULL, End)),xvec(NULL,","," ","End", End));
			for(int i2=next(mid,-1,NULL,NULL); has_id(mid,i2); i2++){ void* v2=map_id(mid,i2); char* k2=map_key(mid, i2);
				if(str_eq(v2,"..")){
					add(mid,k2,","); }; };
			mid=xvec("xstr",NULL,"(",NULL,mid,NULL,")", End);
			vec_splice(toks,head+1,tail-head-1,mid);
			i=head;
		}else if(str_eq(map_id(toks,i+1),"===")||str_eq(map_id(toks,i+1),"!==")){
			int head=expr_head(toks,i-2,"==");
			int tail=expr_tail(toks,i+2,"==");
			map* mid=vec_splice(vec_merge(xadd(vec_sub(toks,head+1,i-head-1),NULL,",", End),vec_sub(toks,i+2,tail-i-2)),0,0,xvec(NULL, End));
			if(str_eq(map_id(toks,i+1),"!==")){ mid=xvec("!",NULL,"str_eq",NULL,"(",NULL,mid,NULL,")", End); }
			else {mid=xvec("str_eq",NULL,"(",NULL,mid,NULL,")", End);};
			vec_splice(toks,head+1,tail-head-1,mid);
			i=head; }; };
	return toks;
};
map* inline_vectors(map* toks){
	if(!map_len(toks)){ return toks; };
	for(int idx=1;idx<=toks->len;idx+=2){
		if(is_map(map_id(toks,idx))){
			inline_vectors(map_id(toks,idx));
			continue; };
		int type=is_inline_vector(toks,idx);
		if(!type){ continue; };
		map* body=is_map(map_id(toks,idx+2));
		if(!body){ continue; };
		if(type==Map && str_eq(map_id(body,1),"0")){ continue; };
		inline_vectors(body);
		set(toks,idx,"(");
		set(toks,idx+4,")");
		char* funcname="xmap";
		if(type==Vector){ funcname="xvec"; };
		if(body->len<2){ funcname= type==Vector ? "new_vec" : "new_map"; };
		vec_splice(toks,idx,0,xvec(funcname,NULL, End));
		int skip=0;
		for(int idx2=1;idx2<=body->len;idx2+=2){
			if(str_eq(map_id(body,idx2),"?")){ skip++; };
			if(str_eq(map_id(body,idx2),":")){
				if(!skip){
					if(is_code(map_id(body,idx2-2))){
						if(is_word(map_id(body,idx2-2),"NULL null")){
							set(body,idx2-2,"NULL");
						}else{
							set(body,idx2-2,str_quote(map_id(body,idx2-2))); }; };
					if(idx2>4 && !str_eq(map_id(body,idx2-2),")") && !str_eq(map_id(body,idx2-4),",")){
						if(str_eq(map_id(body,idx2-4),";")){ set(body,idx2-4,","); }
						else{
							vec_splice(body,idx2-3,0,xvec(NULL,",", End));
							idx2+=2; };
					}else if(idx2>8 && str_eq(map_id(body,idx2-2),")") && !str_eq(map_id(body,idx2-8),",")){
						if(str_eq(map_id(body,idx2-8),";")){ set(body,idx2-8,","); }
						else{
							vec_splice(body,idx2-7,0,xvec(NULL,",", End));
							idx2+=2; }; };
					set(body,idx2,","); };
				if(skip){ skip--; }; }; };
		if(str_eq(map_id(body,body->len-body->len%2-1),";")){
			vec_compact(vec_del(body,body->len-body->len%2-2,2)); };
		idx+=6;
		if(requires_semicolon(toks,idx+1)){ vec_splice(toks,idx+2-1,0,xvec(NULL,";", End)); }; };
	return toks;
};
int msleep(int msecs){
	return usleep(msecs*1000);
};
char* time_ago(char* stime){
	int secs=time(0)-str_time(stime);
	if(secs < 0){ secs*=-1; };
	if(secs<120) {return "a minute";};
	secs/=60;
	if(secs<60) {return xstr(int_str(secs), " minutes", End);};
	secs/=60;
	if(secs<2) {return "an hour";};
	if(secs<24) {return xstr(int_str(secs), " hours", End);};
	secs/=24;
	if(secs<2) {return "a day";};
	if(secs<30) {return xstr(int_str(secs), " days", End);};
	if(secs<60) {return "a month";};
	if(secs<365) {return xstr(int_str(secs/30), " months", End);};
	secs/=365;
	if(secs<2) {return "a year";};
	return xstr(int_str(secs), " years", End);
};
char* ptr_id(void* ptr){
	static char temp[128];
	mempage* pg=ptr_page(ptr);
	if(!pg){ return ptr; };
	int block=ptr_block(ptr,pg);
	int type=ptr_type(ptr);
	if(type){
		int len=block_len(block,pg);
		int head=block_head(block,pg);
		int pre=block-head;
		sprintf(temp,"%d#%d+%d-%d*%d[%s]",pg->no,block,len,pre,pg->block_size,ptr_name(ptr));
		return temp; };
	sprintf(temp,"%d#%d+%d [%s]",pg->no,block,pg->block_size,ptr_name(ptr));
	return temp;
};
char* today(){ return sub_str(time_str(0),0,10); };
char* now(){ return sub_str(time_str(0),11,8); };
int this_year(){ return str_int(sub_str(time_str(0),0,4)); };
char* datetime(){ return time_str(0); };
int str_ascii(char* in){ return in[0]; };
void benchmark_gc(){
	map* ret=new_vec();
	for(int i=0;i<1000000;i++){
		set(ret,i%200000,new_str(1023)); };
	dx(int_var(map_len(ret)),NULL,0);
	dx(mem_usage(),NULL,0);
};
char* ping(char* str){ return str; };
map* ping_map(map* in){ return in; };
char* str_join(char* str1,char* joiner,char* str2){
	if(!str_len(str1)){ return str2; };
	if(!str_len(str2)){ return str1; };
	char* ret=NULL;
	ret=xcat(ret,str1, End);
	ret=xcat(ret,joiner, End);
	ret=xcat(ret,str2, End);
	return ret;
};
map* read_paren(map* mp,char** line,map*(*func)(char**)){
	char* str=*line;
	vec_add(mp,char_str(*str));
	vec_add(mp,func(line));
	vec_add(mp,char_str(closing_paren(*str)));
	return mp;
};
map* set_map(void* val,map* mp,int idx){ return set(mp,idx,val); };
char* var_bits(void* var){
	char* ret=new_str(71);
	unsigned char *ptr = (unsigned char*)&var;
	for(int idx=64,i=0;idx--;i++){
		if(i && !(i%8)){ ret[i+i/8-1]='-'; };
		ret[i+i/8]=ptr[idx/8] & (1u << (idx%8) ) ? '1' : '0'; };
	return ret;
};
char* rtrim_upto(char* in,char upto,int keep){
	if(!in){ return in; };
	char* at=strrchr(in,upto);
	if(!at){ return in; };
	return sub_str(in,0,at-in + (keep ? 1 : 0));
};
map* data_toks(char* in){ in=sub_str(in,2,-2); return xvec("xmap",NULL,"(",NULL,data_tokenizer(&in,0),NULL,")", End); };
map* data_tokenizer(char** in,int level){
	if(!in||!*in||!**in){ return NULL; };
	char* str=*in;
	map* mp=new_vec();
	char* last=NULL;
	char* space1=NULL;
	char* space2=NULL;
	map* key=NULL;
	map* val=NULL;
	while(*str){
		assert(last!=str);
		last=str;

		char* temp=str;
		if(str_start(str,"//")){ space1=xcat(space1,read_theline(&str), End); str++; }
		else if(str_chr(" \t\n\r",*str)){ space1=xcat(space1,read_space(&str,"\t\n\r "), End); str++; };
		int clevel=0;
		for(int i=str_len(space1)-1; i>=0; i--){
			if(str_chr(" \t",space1[i])){ clevel++; }; };
		if(clevel<level){
			space1=NULL;
			str=temp;
			break;
		};
		if(str_chr("\"'`",*str)){ key=xvec(read_quote(&str), End); str++; }
		else if(*str=='('){
			str++;
			key=xvec("(",NULL,c_tokenizer(&str,')'),NULL,")", End);
			if(*str){ str++; };
		}else{
			key=data_quote(read_upto(&str,"\n\r \t"));
		};
		if(str_chr(" \t",*str)){ space2=read_over(&str,"\t "); str++; };

		if(str_start(str,"---")){ val=xvec(read_heredoc(&str), End); str++; }
		else if(*str=='-' && str_chr(" \t\n\r\0",str[1])){ val=prop_toks(read_theline(&str),key); str++; }
		else if(!str_chr("\n\r",*str)){ val=data_quote(read_theline(&str)); str++; };
		
		if(key && !val){
			temp=str;
			map* ret=data_tokenizer(&str,clevel+1);	
			if(map_len(ret)){
				val=xvec("xmap",NULL,"(",NULL,ret,NULL,")", End);
			}else{
				val=NULL;
	  			str=temp; }; };
		if(key || val){
			if(map_len(mp)){
				vec_add(mp,NULL);
				vec_add(mp,","); };
			vec_add(mp,space1);
			vec_merge(mp,(key ? key : xvec("NULL", End)));
			vec_add(mp,NULL);
			vec_add(mp,",");
			vec_add(mp,space2);
			vec_merge(mp,(val ? val : xvec("NULL", End)));
			space1=space2=NULL;
			key=val=NULL;
		}else{
			space1=xcat(space1,space2, End);
			space2=NULL; }; };
	vec_add(mp,space1);
	*in=str;
	return mp;
};
char* read_over(char** in,char* chars){
	char* str=*in;
	while(*str && str_chr(chars,*str)){
		str++; };
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
map* data_quote(char* in){
	if(!in || !str_len(in)){ return NULL; };
	if(str_chr("\"'`",*in)){ return xvec(in, End); };
	if(*in=='='){ return vec_compact(vec_del(x_map(sub_str(in,1,-2147483648)),0,1)); };
	if(*in=='{'){ return prop_toks(sub_str(in,1,-1),NULL); };
	if(*in=='['){ return prop_vec_toks(sub_str(in,1,-1)); };
	return xvec(str_quote(in), End);
};
map* prop_vec_toks(char* in){
	map* ret=new_vec();
	map* toks=split_by(in,',',0);
	for(int next1=next(ret,-1,NULL,NULL); has_id(ret,next1); next1++){ void* val=map_id(ret,next1); char* idx=map_key(ret, next1);
		if(idx){ vec_merge(ret,xvec(NULL,","," ", End)); }
		else {vec_add(ret,NULL);};
		vec_merge(ret,data_quote(val)); };
	return xvec("xvec",NULL,"(",NULL,ret,NULL,")", End);
};
map* prop_toks(char* in,map* name){
	map* ret=new_vec();
	map* toks=split_by(in,',',0);
	for(int  idx=next(toks,-1,NULL,NULL); has_id(toks, idx);  idx++){ void* val=map_id(toks, idx);
		val=split_by(val,' ',2);
		if(!map_len(val)){ continue; };
		if(idx==0 && str_eq(map_id(val,0),"-")){
			if(name){
				vec_add(ret,NULL);
				vec_merge(ret,data_quote("name"));
				vec_merge(ret,xvec(NULL,","," ", End));
				vec_merge(ret,name); };
			set(val,0,"type"); };
		if(map_len(ret)){
			vec_merge(ret,xvec(NULL,","," ", End));
		}else{
			vec_add(ret,NULL); };
		vec_merge(ret,data_quote(map_id(val,0)));
		vec_merge(ret,xvec(NULL,","," ", End));
		vec_merge(ret,map_id(val,1) ? data_quote(map_id(val,1)) : xvec("NULL", End)); };
	return xvec("xmap",NULL,"(",NULL,ret,NULL,")", End);
};
map* split_by(char* str, char term, int max){
//	"spliting by $(term.char_str())".px()
	if(!*str){ return NULL; };
	map* ret=new_vec();
	char* head=str;
	if(!str_chr(" \t\n\r",term)){ head=skip_over(head," \t\n\r"); };
	while(*str && (!max || map_len(ret)<max-1)){
		if(strchr("\"'`",*str)){ str=skip_quote(str); continue; }
		else if(strchr("([{",*str)){ str=skip_paren(str); continue; }
		else if(*str==term){
			vec_add(ret,sub_str(head,0,str-head));
			head=str+1;
			if(!str_chr(" \t\n\r",term)){ head=skip_over(head," \t\n\r"); }; };
		str++; };
	while(*str){ str++; };
	if(str>head){ vec_add(ret,sub_str(head,0,str-head)); };
	return ret;
};
char* str_hex(char* in){
	char* ret=new_str(str_len(in)*2);
	for(int i=0; i<str_len(in); i++){
		sprintf(ret+i*2,"%02x",((unsigned char*)in)[i]); };
	return ret;
};
char* md5(char* in){
	char* ret=new_blob(16);
	MD5((unsigned char*)in,str_len(in),(unsigned char*)ret);
	return str_hex(ret);
};
map* data_map(char* in){ return data_map2(&in,0); };
map* data_map2(char** in,int level){
	if(!in||!*in||!**in){ return NULL; };
	char* str=*in;
	map* ret=new_map();
	char* last=NULL;
	while(*str){
		assert(last!=str);
		last=str;
		char* tabstart=skip_over(str,"\n\r");
		if(str_start(str,"//")){ str=skip_upto(str,"\n\r"); continue; };
		char* keystart=skip_over(tabstart," \t");
		char* keyend=skip_code(keystart," \r\n\t");
		str=keyend;
		if(keyend==keystart){ continue; };
		if(keystart-tabstart<level){ break; };
		void* key=data_unquote(sub_str(keystart,0,keyend-keystart));
		char* valstart=skip_over(keyend," \t");
		char* valend=skip_upto(valstart,"\n\r");
		str=valend;
		if(valend==valstart){
			map* submap=data_map2(&str,level+1);
			if(submap){ add(ret,key,submap); }
			else {add(ret,key,NULL);};
		}else{
			char* val=sub_str(valstart,0,valend-valstart);
			if(val[0]=='-' && str_chr(" \t",val[1])){ add(ret,key,prop_map(val,key)); }
			else {add(ret,key,data_unquote(val));}; }; };
	if(map_len(ret)){
		*in=str;
		return ret; };
	return NULL;	
};
char* skip_code(char* in,char* terminator){
	while(*in){
		if(str_chr("({[",*in)){ in=skip_paren(in); continue; };
		if(str_chr("\"'`",*in)){ in=skip_quote(in); continue; };
		if(str_chr(terminator,*in)){ break; };
		in++; };
	return in;
};
char* skip_upto(char* in,char* chars){
	while(*in && !strchr(chars,*in)) {in++;};
	return in;
};
void* data_unquote(char* in){
	if(!in || !str_len(in)){ return NULL; };
	if(str_chr("\"'`",*in)){ return str_unquote(in); };
	if(*in=='='){ return eval(sub_str(in,1,-2147483648),NULL); };
	if(str_start(in,"---")){ return sub_str(in,3,-3); };
	if(*in=='{'){ return prop_map(sub_str(in,1,-1),NULL); };
	if(*in=='['){ return prop_vec(sub_str(in,1,-1)); };
	return in;
};
map* prop_vec(char* in){
	map* ret=split_by(in,',',0);
	for(int next1=next(ret,-1,NULL,NULL); has_id(ret,next1); next1++){ void* val=map_id(ret,next1); char* idx=map_key(ret, next1);
		add(ret,idx,data_unquote(val)); };
	return ret;
};
map* prop_map(char* in,char* name){
	map* ret=new_map();
	map* toks=split_by(in,',',0);
	for(int  idx=next(toks,-1,NULL,NULL); has_id(toks, idx);  idx++){ void* val=map_id(toks, idx);
		val=split_by(val,' ',2);
		if(!map_len(val)){ continue; };
		if(idx==0 && str_eq(map_id(val,0),"-")){
			if(name){ add(ret,"name",name); };
			set(val,0,"type"); };
		add(ret,data_unquote(map_id(val,0)),data_unquote(map_id(val,1))); };
	return ret;
};
char* num_lang(char* in,char* lang){
	if(!in || !*in){ return NULL; };
	char buff[5]={0};
	map* langid=xmap("en",int_var( 0), "bg",int_var( 1), "ar",int_var( 2), End);
	int id=to_int(map_val(langid,lang));
	int points[]={ 48, 2534, 1632 };
	char* ret=NULL;
	int code=0;
	while((code=utf_unicode(in))){
		in+=utf_len(in);
		for(int i=0; i<3; i++){
			if(i!=id && code>=points[i] && code<points[i]+10){
				code-=(points[i]-points[id]);
				break; }; };
		ret=xcat(ret,unicode_utf(code,buff), End); };
	return ret;
};
char* str_tr(char* in, map* replace){
	if(!in){ return NULL; };
	char* ret=NULL;
	char* saved=in;
	int maxlen=0;
	for(int next1=next(replace,-1,NULL,NULL); has_id(replace,next1); next1++){ char* key=map_key(replace, next1); maxlen=max(maxlen,str_len(key)); };
	char* buff=new_str(maxlen);
	while(*in){
		while(*in && !is_alphanum(*in,NULL)) {in+=utf_len(in);};
		char* head=in;
		while(*in && is_alphanum(*in,NULL)) {in+=utf_len(in);};
		if(!(in-head)){ break; };
		if(in-head>maxlen){ continue; };
		memcpy(buff,head,in-head);
		buff[in-head]='\0';
		if(!map_val(replace,buff)){ continue; };
		ret=xcat(ret,sub_str(saved,0,head-saved), End);
		ret=xcat(ret,map_val(replace,buff), End);
		saved=in;
	};
	if(saved<in){
		ret=xcat(ret,sub_str(saved,0,in-saved), End);
	};
	return ret;	
};
