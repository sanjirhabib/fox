#line 2 "src/generator.fox"
#include <core.h>
#include <generator.h>
#include <foxtime.h>
#include <playground.h>
#include <cmd.h>
#include <dir.h>
#include <tsv.h>
#include <map.h>
#include <foxparser.h>
#include <cmdline.h>

//extern void* (*invoke)(map* v,char* name);
//extern map* (*reflect)();
void* (*invoke)(map* v,char* name)=NULL;
map* (*reflect)()=NULL;

#define MAXMEM 40*1024*1024
#define MIN_CHAIN 1
int _iscmd=0;

int cgi(char* infile, char* xfiles, char* profile, char* outfile,char* opts,int keepfiles){
	return cc(infile,xfiles, profile, outfile, opts, keepfiles);
};
static char* fox_meta(char* infile, char* name, map* depends, char* outfile){
	map* funcs=funcs_nonstatic(file_funcs(infile,0));
	map* macros=c_macros(infile);
	map* structs=c_structs(infile);
	for(int next1=next(depends,-1,NULL,NULL); has_id(depends,next1); next1++){ void* file=map_id(depends,next1);
		funcs=map_merge(funcs_nonstatic(file_funcs((xstr(file,".fox", End)),0)),funcs);
		macros=map_merge(c_macros((xstr(file,".fox", End))),macros);
		structs=map_merge(c_structs((xstr(file,".fox", End))),structs);
	};
	char* prefix=xstr(name,"_", End);
	return write_file(x_c(xstr("", 
	"/* This is a generated file. To change it, edit function fox_meta() in generator.fox */\n", 
	"#include \"", name, ".h\"\n", 
	"void* ", prefix, "invoke(map* v,char* name){\n", 
	"	unsigned long long idn=str_hash((unsigned char*)name)\n", 
	"	switch(idn){\n", 
	"//", callfunc_c(funcs), "\n", 
	"	}\n", 
	"	return End\n", 
	"}\n", 
	"map* ", prefix, "reflect(){\n", 
	"	return {\n", 
	"\t\tfuncs: ", map_ccode(funcs), ",\n", 
	"\t\tmacros: ", map_ccode(macros), ",\n", 
	"\t\tstructs: ", map_ccode(structs), "\n", 
	"	}\n", 
	"}\n", 
	"", 
	"", End)),outfile,0,1);
};
map* file_deps(char* filename, map* deps){
	map* pragmas=map_val(map_val(_globals,"reflect"),"pragmas");
	map* map_1=fox_includes(filename); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* f=map_id(map_1,next1);
		deps=map_merge(deps,map_val(pragmas,f)); };
	char* val=fox_pragma(filename);
	if(val){ add(deps,val,val); };
	return deps; //.map_vec().vec_reverse()
};
int vec_has(map* in, char* key){
	for(int next1=next(in,-1,NULL,NULL); has_id(in,next1); next1++){ void* val=map_id(in,next1);
		if(str_eq(val,key)){ return 1; }; };
	return 0;
};
int cc(char* infile, char* xfiles, char* profile, char* outfile, char* opts, int keepfiles){
	map* libs=new_map();
	map* names=new_vec();
	map* map_1=str_split(xfiles,",",0); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* file=map_id(map_1,next1);
		vec_add(names,fox_cs(file_rename(file,NULL,".fox",NULL,NULL,NULL),NULL,0));
		libs=file_deps(file,libs); };
	char* name=file_rename(infile,NULL,".fox",NULL,NULL,NULL);
	int hasmeta=vec_has(fox_includes(infile),xstr(name,"_meta", End));
	fox_cs(name,names, hasmeta);
	libs=file_deps(infile,libs);
	char* xlibs=map_join(libs," ");
	if(!outfile){ outfile=name; };
	// -lfoxstatic
	char* cflags="-m64 -std=gnu99 -Werror=implicit-function-declaration -L/usr/local/lib -lm -I/usr/local/include";
	map* switches=xmap(
		"debug", "-g -lfoxcmdstatic",
		"release", "-O2 -lfoxcmdstatic",
		"cgi", "-O2 -lfcgi",
		"cgidebug", "-g -lfcgi"
	, End);
	profile = (map_val(switches,profile) ? map_val(switches,profile) : map_val(switches,"debug"));
	char* extras=NULL;
	if(map_len(names)){ extras=xstr(map_join(names,".c "),".c", End); };
	if(hasmeta){ extras=xcat(extras,xstr(" ", name, "_meta.c", End), End); };
	int ret=exec(
		px(
		xstr("gcc ", name, ".c ", extras, " -o ", outfile, " ", opts, " ", cflags, " ", xlibs, " ", profile, End),1),NULL);
	return ret;
};
char* write_user_meta(char* name, map* depends){
	map_merge(map_val(map_val(_globals,"reflect"),"funcs"),x_funcs(meta_h(name),stoi(xstr(name,"_meta.c", End)),NULL));
	write_file(meta_h(name),xstr(name,"_meta.h", End),0,1);
	char* meta=NULL;
	meta=xcat(meta,xstr("", 
	"extern int _iscmd;\n", 
	"int cmdline(){\n", 
	"	args=_globals.args\n", 
	"	if !args[1].is_code() => return 0\n", 
	"	_iscmd=1\n", 
	"\treflect=", name, "_reflect\n", 
	"\tinvoke=", name, "_invoke\n", 
	"\targs.cmdline_params(", name, "_reflect().funcs).", name, "_invoke(args[1]).ret_print()\n", 
	"	return 1\n", 
	"}\n", 
	"", 
	"", End), End);
	meta=x_c(meta);
	write_file((xstr(fox_meta((xstr(name,".fox", End)),name,depends,NULL),meta, End)),xstr(name,"_meta.c", End),0,1);
};
static char* fox_cs(char* name,map* depends,int addmeta){
	char* fox=xstr(name,".fox", End);
	map* func=file_funcs(fox,0);
	map_merge(map_val(map_val(_globals,"reflect"),"funcs"),func);
	if(addmeta){ write_user_meta(name,depends); };
	write_file(exclude_defines(fox_c(fox,NULL)),xstr(name,".c", End),0,1);
	fox_h(fox,xstr(name,".h", End));
	return name;
};
void write_source(char* infile,...){
	map* files=xvec(infile, End);
	va_list args;
	va_start(args,infile);
	while(1){
		char* name=va_arg(args,char*);
		if(name==End){ break; };
		vec_add(files,name); };
	va_end(args);
	source_funcs(files);
	char* includes=NULL;
	for(int next1=next(files,-1,NULL,NULL); has_id(files,next1); next1++){ void* infile=map_id(files,next1);
		includes=xcat(includes,"#include <",file_nodir(file_noext(infile)),".h>\n", End);
		if(str_end(infile,".h")){ continue; };
		write_file(exclude_defines(fox_c(infile,NULL)),xstr(file_rename(infile,"src",".fox",NULL,NULL,NULL),".c", End),0,1);
		fox_h(infile,xstr(file_rename(infile,"include",".fox",NULL,NULL,NULL),".h", End)); };
	map* data=new_map();
	add(data,"funcs",funcs_nonstatic(source_funcs(NULL)));
	add(data,"macros",c_macros(files_section(files,"#define")));
	add(data,"structs",c_structs(files_section(files,"typedef struct union class enum")));
	add(data,"pragmas",source_pragmas(files,new_map()));
	map* map_1=map_val(data,"funcs"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* name=map_key(map_1, next1);
		add(add_key(data,"depends",Map),name,func_depend(map_val(val,"body"),new_map()));
		map_del_key(val,"body"); };
	add(add_key(data,"version",Map),"build",increase_version());
	add(add_key(data,"version",Map),"date",time_str(0));
	write_file((xstr("#include <meta.h>\n",includes,funcs_meta(data,"fox"), End)),file_rename("meta.c","src",NULL,NULL,NULL,NULL),0,1);
	write_file(""
	"#pragma once\n"
	"#include <core.h>\n"
	"void* fox_invoke(map* v,char* name);\n"
	"map* fox_reflect();\n"
	""
	"","include/fox_meta.h",0,1);
	px(mem_usage(),1);
};
map* source_pragmas(map* infiles, map* ret){
	map* deps=new_map();
	ret=new_map();
	map* modules=new_map();
	for(int next1=next(infiles,-1,NULL,NULL); has_id(infiles,next1); next1++){ void* file=map_id(infiles,next1);
		if(!str_eq(file_ext(file,NULL),"fox")){ continue; };
		char* module=file_nodir(file_noext(file));
		add(add_key(ret,module,Map),xstr("-lfox", module, "static", End),xstr("-lfox", module, "static", End));
		char* val=fox_pragma(file);
		if(val){ add(add_key(ret,module,Map),val,val); };
		add(deps,module,fox_includes(file)); };
	int found=1;
	while(found){
		found=0;
		for(int next1=next(deps,-1,NULL,NULL); has_id(deps,next1); next1++){ void* ds=map_id(deps,next1); char*  module=map_key(deps, next1);
			for(int next1=next(ds,-1,NULL,NULL); has_id(ds,next1); next1++){ void* mod=map_id(ds,next1);
				map* map_1=map_val(ret,mod); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* cmd=map_id(map_1,next1);	
					if(!map_val(map_val(ret,module),cmd)){
						add(add_key(ret,module,Map),cmd,cmd);
						found++; }; }; }; }; };
	return ret;
};
map* fox_includes(char* infile){
	map* ret=new_vec();
	map* toks=str_tokenize(str_ltrim(file_read(infile,1,1),"\n"),"\n",0);
	for(int i=next(toks,-1,NULL,NULL); has_id(toks,i); i+=2){ void* tok=map_id(toks,i);
		if(!str_start(tok,"#include")){
			continue; };
		map* words=str_tokenize(tok," \t",0);
		vec_add(ret,file_nodir(file_noext(sub_str(map_id(words,2),1,-1)))); };
	return ret;
};
char* fox_pragma(char* infile){
	map* toks=str_split(files_section(xvec(infile, End),"#pragma"),"\n",0);
	for(int next1=next(toks,-1,NULL,NULL); has_id(toks,next1); next1++){ void* tok=map_id(toks,next1);
		map* words=str_tokenize(tok," \t",0);
		if(!str_eq(map_id(words,2),"fox")){
			continue; };
		if(!str_eq(map_id(words,4),"cc")){
			continue; };
		return map_join(vec_sub(words,6,0),""); };
	return NULL;
};
char* source_defines(map* infiles){
	char* ret=NULL;
	for(int i=next(infiles,-1,NULL,NULL); has_id(infiles,i); i++){ void* v=map_id(infiles,i);
		ret=xcat(ret,fox_defines(v), End); };
	return ret;
};
char* exclude_defines(char* in){
	if(!in){ return NULL; };
	map* toks=str_tokenize(in,"\n",0);
	int start=0;
	char* ret=NULL;
	if(str_chr(map_id(toks,0),'\n')){
		start++;
		ret=xcat(ret,map_id(toks,0), End); };
	for(int i=next(toks,start-1,NULL,NULL); has_id(toks,i); i+=2){ void* tok=map_id(toks,i);
		if(tok_isdefine(tok)){
			ret=xcat(ret,"//",str_replace(tok,"\n","\n//"), End);
		}else{
			ret=xcat(ret,tok, End); };
		ret=xcat(ret,map_id(toks,i+1), End); };
	return ret;
};
int tok_isdefine(char* in){
	char* type=next_word(in," \t\n");
	if(!is_word(type,"typedef struct enum union class extern")){
		return 0; };
	if(str_eq(type,"struct")){
		map* words=str_tokenize(in," \t{}",0);
		if(!str_chr(map_id(words,3),'{')){ return 0; }; };
	return 1;
};
char* fox_defines(char* infile){
	map* toks=str_tokenize(str_ltrim(file_read(infile,1,1),"\n"),"\n",0);
	char* basename=file_noext(file_nodir(infile));
	char* ret=NULL;
	for(int i=next(toks,-1,NULL,NULL); has_id(toks,i); i+=2){ void* tok=map_id(toks,i);
		char* type=next_word(tok," \t\n");
		if(!is_word(type,"typedef struct enum union class #include #pragma extern #define #if #ifdef #undef #else #endif #ifndef")){
			continue; };
		if(str_eq(type,"struct")){
			map* words=str_tokenize(tok," \t{}",0);
			if(!str_chr(map_id(words,3),'{')){
				continue; }; };
		if(str_eq(type,"#include")){
			map* words=str_tokenize(tok," \t<>",0);
			char* filename=file_noext(file_nodir(map_id(words,2)));
			if(str_eq(filename,basename)){
				ret=xcat(ret,xstr("#pragma fox module ", filename, "\n", End), End);
				continue; }; };
		ret=xcat(ret,tok,"\n", End); };
	return ret;
};
static map* funcs_nonstatic(map* funcs){
	map* ret=new_map();
	for(int next1=next(funcs,-1,NULL,NULL); has_id(funcs,next1); next1++){ void* val=map_id(funcs,next1); char*  key=map_key(funcs, next1);
		if(map_val(val,"isstatic")){ continue; };
		add(ret,key,val); };
	return ret;
};
static char* meta_h(char* prefix){
	if(prefix){prefix=xstr(prefix,"_", End);};
	return xstr("", 
	"#pragma once\n", 
	"void* ", prefix, "invoke(map* v,char* name);\n", 
	"map* ", prefix, "reflect();\n", 
	"int cmdline();\n", 
	"int exec_cmdline(map* args);\n", 
	"void* user_invoke(map* params, char* name);\n", 
	"map* user_funcs();\n", 
	"", 
	"", End);
};
static char* funcs_meta(map* data, char* prefix){
	if(prefix){prefix=xstr(prefix,"_", End);};
	return xstr("", 
	"/* This is a generated file. To change it, edit function funcs_meta() in generator.fox */\n", 
	"void* ", prefix, "invoke(map* v,char* name){\n", 
	"	unsigned long long idn=str_hash((unsigned char*)name);\n", 
	"	switch(idn){\n", 
	"//", callfunc_c(map_val(data,"funcs")), "\n", 
	"	}\n", 
	"	return fox_error(xstr(\"invoke(): Function %s not defined\", name, End), 0);\n", 
	"}\n", 
	"map* ", prefix, "reflect(){\n", 
	"	return xmap(\n", 
	"\t\t\"funcs\", ", map_ccode(map_val(data,"funcs")), ",\n", 
	"\t\t\"macros\", ", map_ccode(map_val(data,"macros")), ",\n", 
	"\t\t\"structs\", ", map_ccode(map_val(data,"structs")), ",\n", 
	"\t\t\"depends\", ", map_ccode(map_val(data,"depends")), ",\n", 
	"\t\t\"pragmas\", ", map_ccode(map_val(data,"pragmas")), ",\n", 
	"\t\t\"version\", ", map_ccode(map_val(data,"version")), ", End\n", 
	"	);\n", 
	"}\n", 
	"", 
	"", End);
};
static char* gen_htaccess(char* outfile){
	return write_file(""
	"RewriteEngine On\n"
	"RewriteCond %{REQUEST_FILENAME} !-f\n"
	"RewriteCond %{REQUEST_FILENAME} !-d\n"
	"RewriteRule ^(.+)$ index.cgi [L,QSA]\n"
	"Options -Indexes\n"
	"<FilesMatch \"\\.(db|txt|sql)$\">\n"
	"Order allow,deny\n"
	"Deny from all\n"
	"</FilesMatch>\n"
	""
	"",outfile,0,1);
};
static char* gen_cgi_makefile(char* name, char* outfile){
	return write_file(xstr("", 
	"all:\n", 
	"\tfox cgi ", name, ".fox\n", 
	"debug:\n", 
	"\tfox cgi ", name, ".fox --profile=cgidebug\n", 
	"", 
	"", End),outfile,0,1);
};
static char* gen_fox_cgi(char* name,char* outfile){
	return write_file(xstr("", 
	"#line 2 \"", name, ".fox\"\n", 
	"\n", 
	"#include <fcgi_stdio.h> /* fcgi library; put it first*/\n", 
	"#include <core.h>\n", 
	"#include <syslog.h>\n", 
	"#include \"", name, ".h\"\n", 
	"\n", 
	"int run(map* args){\n", 
	"\t_globals.dbs.", name, "=\"", name, ".db\"\n", 
	"	if cmdline() => return 0\n", 
	"	LOG_INFO.syslog(\"fox.fcgi started\")\n", 
	"	while FCGI_Accept() >= 0\n", 
	"		ret=http_req().dispatch()\n", 
	"		//close_conns()\n", 
	"		_queries=0\n", 
	"		\"%s\".printf(ret or \"content-type: text/html\\r\\n\\r\\nBody is Blank!\")\n", 
	"		//gc_reset()\n", 
	"	LOG_INFO.syslog(\"fox.fcgi exit\")\n", 
	"	return 0\n", 
	"}\n", 
	"\n", 
	"char* dispatch(map* req){\n", 
	"	path=_globals.paths\n", 
	"	if \"/\".get().end\n", 
	"\t\treturn \"Hello World! - from ", name, ".\".show_body{{\n", 
	"\t\t\ttitle ", str_title(name), "'s Application\n", 
	"		}}\n", 
	"\n", 
	"	return not_found()\n", 
	"}\n", 
	"", 
	"", End),outfile,0,1);
};
int init_fox(char* name){
	if(!is_file("Makefile")){
		write_file(xstr("", 
		"all:\n", 
		"\tfox cc ", name, ".fox\n", 
		"\t./", name, "\n", 
		"debug:\n", 
		"\tfox cc ", name, ".fox --profile=debug\n", 
		"\t./", name, "\n", 
		"fox:\n", 
		"	pushd . && cd /web/fox2 && make install && popd\n", 
		"", 
		"", End),"Makefile",0,1); };
	char* filename=xstr(name, ".fox", End);
	if(!is_file(filename)){
		write_file(xstr("", 
		"#line 2 \"", name, ".fox\"\n", 
		"#include <core.h>\n", 
		"#include \"", name, ".h\"\n", 
		"\n", 
		"int run(map* args){\n", 
		"	return 0\n", 
		"}\n", 
		"", 
		"", End),filename,0,1); };
	return 0;
};
int init_cgi(char* name){
	if(!is_file((xstr(name,".fox", End)))){ gen_fox_cgi(name,xstr(name,".fox", End)); };
	if(!is_file(".htaccess")){ gen_htaccess(".htaccess"); };
	if(!is_file("Makefile")){ gen_cgi_makefile(name,"Makefile"); };
	return 0;
};
char* c_h(char* infile,char* outfile){ return write_file(funcs_cdecl(file_funcs(infile,0),0),outfile,0,1); };
char* fox_h(char* infile,char* outfile){
	return write_file((xstr("#pragma once\n",fox_defines(infile),funcs_cdecl(file_funcs(infile,0),0), End)),outfile,0,1);
};
//static
char* c_x(char* in){ return toks_c(map_tox(x_map(in))); };

static char* map_ccode(void* mp){
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
	return fox_error(xstr("Unknown type of variable ", to_str(mp,"",0), End),0);	
};
static char* file_foxh(char* infile,char* outfile){
	return write_file(funcs_cdecl(file_funcs(infile,0),1),outfile,0,1);
};
//static
char* funcs_cdecl(map* fns,int show_default){
	char* ret="";
	for(int  i=next(fns,-1,NULL,NULL); has_id(fns, i);  i++){ void* v=map_id(fns, i); char*  k=map_key(fns,  i);
		if(is_word(k,"main")){ continue; };
		ret=xcat(ret,func_cdecl(v,show_default), End);
		ret=xcat(ret,"\n", End); };
	return ret;
};
static char* old_decl_type(map* mp,int idx){
	int from=stm_start(mp,idx,"{};,().>:=><!&-+/?");
	for(int i=from;i<idx;i+=2){
		if(is_word(map_id(mp,i),"int long double float char void struct map static extern unsigned register mempage size_t time_t const var")){
			return toks_c(vec_sub(mp,from,idx-from-1)); }; };
	return NULL;
};
static char* toks_str(map* mp){
	char* ret=NULL;
	for(int next1=next(mp,-1,NULL,NULL); has_id(mp,next1); next1++){ void* val=map_id(mp,next1); ret=xcat(ret,is_map(val) ? toks_str(val) : val, End); };
	return ret;
};
static map* toks_keywords(map* mp,char* keywords){
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
static int is_keyword(char* str){ return is_word(str,"abstract and as break callable case catch class clone const continue declare default do echo else elseif enddeclare endfor endforeach endif endswitch endwhile extends final for foreach def function global goto if implements include include_once instanceof insteadof interface namespace new or private protected public require require_once return static switch throw trait try use while xor yield false true null"); };
static char* toks_c(map* mp){
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
static int func_dot(map* mp,int idx){
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
static map* multiline_parts(char* str){
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
static map* heredoc_parts(char* str){
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
static map* heredoc_str(map* mp){
	for(int idx=1; idx<map_len(mp); idx+=2){
		if(is_map(map_id(mp,idx))){ heredoc_str(map_id(mp,idx)); };
		map* parts=heredoc_parts(map_id(mp,idx));
		if(!parts){ continue; };
		vec_splice(mp,idx,1,parts); };
	return mp;
};
static map* dot_each(map* mp){
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
static int tok_indent(char* str){
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
static int requires_semicolon(map* mp,int idx){
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
static map* add_semicolon(map* mp,int recurse){
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
static int tok_endl(map* mp,int from){
	for(;from<=mp->len;from+=2){ if(str_chr(is_str(map_id(mp,from-1)),'\n')) {break;}; };
	return from;
};
static map* force_curly(map* mp){
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
static map* expand_main(map* mp){
	for(int idx=1;idx<=mp->len;idx+=2){
		if(str_eq(map_id(mp,idx),"run") && str_eq(map_id(mp,idx+2),"(") && str_eq(map_id(mp,idx+8),"{") && !map_len(map_id(mp,idx+4)) && !str_eq(map_id(mp,idx-2),"int")){
			vec_compact(vec_splice(mp,idx,7,vec_del(x_map("void* run(map* args)"),0,1))); }; };
	return mp;
};
static map* add_curly(map* mp,int recursive){
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
static map* dot_key(map* mp){
	return mp;
	for(int idx=1;idx<=map_len(mp);idx+=2){
		if(is_map(map_id(mp,idx))){ dot_key(map_id(mp,idx)); continue; };
		char* str=is_str(map_id(mp,idx));
		if(!str){ continue; };
		if(idx>=2 && *str=='"' && str_eq(map_id(mp,idx-2),".")){
			vec_splice(mp,idx,1,xvec("map_val",NULL,"(",NULL,xvec(NULL,str, End),NULL,")", End)); }; };
	return mp;
};
static int str_hasvar(char* in){
	if(!in){ return 0; };
	for(int i=0; i<strlen(in); i++){
		if(in[i]!='$'){ continue; };
		if(is_alpha(in[i+1],"($")){ return 1; }; };
	return 0;
};
static char* str_mstr(char* in){
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
static map* xstr_parts(char* in){
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
static char* str_xstr(char* in){
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
static map* str_dollars(map* mp){
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
static map* single_quotes(map* mp){
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
static map* colon_str(map* mp){
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
static map* dot_func(map* mp){
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

static map* map_tox(map* mp){
	if(!mp){ return mp; };
	for(int idx=1+next(mp,-1,NULL,NULL);idx;idx=1+next(mp,idx-1,NULL,NULL)){
		if(idx%2-1){
			map* mp1=is_map(map_id(mp,idx-1));
			if(mp1) {map_tox(mp1);};
			continue; };
		idx=func_dot(mp,idx-1); };
	return mp;
};
static int stm_start(map* mp,int idx,char* terms){
	idx-=2;
	while(idx>0){
		if(str_chr(terms,fox_at(map_id(mp,idx),0))){ break; };
		idx-=2; };
	return idx+2;
};
static map* syn_func(map* syn,int with_body,char* filename){
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
	char* isstatic=NULL;
	for(int i3=next(type,-1,NULL,NULL); has_id(type,i3); i3++){ void* v3=map_id(type,i3);
		if(is_word(v3,"inline")){ vec_compact(vec_del(type,i3,2)); };
		if(is_word(v3,"static")){ vec_compact(vec_del(type,i3,2)); isstatic="static"; }; };
	map* ret=xmap("name", name, "type", toks_c(type), "decltype", decltype, "params", params, End);
	if(with_body){ add(ret,"body",body); };
	if(filename){ add(ret,"file",file_nodir(filename)); };
	if(isstatic){ add(ret,"isstatic",isstatic); };
	return ret;
};
static map* syn_funcs(map* mp,int with_body,char* filename){
	map* ret=new_map();
	for(int i=1; i<map_len(mp); i+=2){
		if(is_func_decl(map_id(mp,i))){
			map* fn=syn_func(map_id(mp,i),with_body,filename);
			if(str_eq(map_val(fn,"name"),"main")){ continue; };
			add(ret,map_val(fn,"name"),fn); }; };
	return ret;
};
static map* read_struct(map* toks){
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
		if(!is_word(map_id(map_id(syn,i),1),"typedef struct union class enum")){ continue; };
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
static map* x_funcs(char* in,int with_body,char* filename){
	return syn_funcs(
		toks_syn(
		add_semicolon(
		add_curly(
		x_map(in),0),0),0),with_body,filename);
};
static map* func_params(map* func){ return toks_split(func_param(func),",",0); };
static map* func_param(map* func){ return map_id(func,next_tok(func,0,"(",0)+2); };
static map* func_add_param(map* func,map* add){
	map* param=func_param(func);
	assert(is_vec(param));
	if(map_len(param)){
		if(!(map_len(param)%2)){ xadd(param,NULL, End); };
		xadd(param,",", End); };
	vec_merge(param,add);
	return func;
};
static map* func_rename(map* func,char* name){ return set(func,1,name); };
static char* func_name(map* func){ return map_id(func,1); };
static map* map_assign(map* exp1,map* exp2){
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
static map* call_count(map* toks,map* counter,char* infunc){
	if(!toks){ return counter; };
	for(int i=0; i<toks->len; i+=2){
		char* name=syn_is_call(toks,i);
		if(!str_eq(name,infunc)){
			add(counter,name,int_var((is_int(map_val(counter,name))+1))); };
		if(is_map(map_id(toks,i+1))){
			call_count(map_id(toks,i+1),counter,infunc); }; };
	return counter;
};
static map* file_deadcode(char* file){
	map* funcs=x_funcs(file_read(file,1,1),1,NULL);
	map* ret=new_map();
	for(int i=next(funcs,-1,NULL,NULL); has_id(funcs,i); i++){ void* v=map_id(funcs,i); char* k=map_key(funcs, i);
		call_count(map_val(v,"body"),ret,k); };
	for(int i2=next(funcs,-1,NULL,NULL); has_id(funcs,i2); i2++){ void* v2=map_id(funcs,i2); char* k2=map_key(funcs, i2);
		if(!map_val(ret,k2)){ px(k2,1); }; };
	return ret;
};
static char* is_typecast(map* toks,int idx){
	if(str_eq(map_id(toks,idx+1),"(") && is_var_decl(map_id(toks,idx+3))){ return str_trim(toks_c(map_id(toks,idx+3))," \t\n\r"); };
	return NULL;
};
static char* is_name(char* in){
	if(!is_code(in)){ return 0; };
	if(is_word(in,"return")){ return 0; };
	return in;
};
static int expr_tail(map* toks,int idx,char* expr){
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
static int expr_head(map* toks,int idx,char* expr){
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
static map* add_return(map* toks){
	return toks;
};
static map* wrap_call(map* tok,char* func){
	return xvec(NULL,func,NULL,"(",NULL,tok,NULL,")", End);
};
static map* syn_expand_macro(map* syn,int idx){
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
static map* syn_is_macro(map* syn, int idx){
	return map_val(macros(),syn_is_call(syn,idx));
};
static map* syn_set_param(map* syn,int idx,map* params){
	return set(syn,next_tok(syn,idx,"(",0)+2,params);
};
static map* syn_func_param(map* syn,int idx){
	return map_id(syn,next_tok(syn,idx,"(",0)+2);
};
static map* syn_func_body(map* syn){
	if(!next_tok(syn,0,"{",0)){ return NULL; };
	return map_id(syn,next_tok(syn,0,"{",0)+2);
};
static map* syn_remove_default(map* syn){
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

static char* assign_to_func(map* tok){
	return syn_is_call(tok,0);
};
static int next_tok(map* toks,int from,char* tok,int upto){
	if(!toks){ return 0; };
	for(int i=from; i<toks->len; i+=2){
		if(is_word(map_id(toks,i+1),tok)){ return !upto || i<upto ? i+1 : 0; }; };
	return 0;
};
static int syn_has(map* syn,char* words){ return next_tok(syn,0,words,0); };
static char* syn_flow_name(map* syn){ return map_id(syn,1); };
static int is_flow(map* syn){
	return is_word(map_id(syn,1),"if while for switch do");
};
static map* syn_flow_condition(map* syn,int no){
	char* name=syn_flow_name(syn);
	// do:1 {:3 __:5 }:7 while:9 (:11 __:13 ):15
	// if:1 (:3 __:5 ):7 {:9 __:11 }:13
	// else:15 if:17 (:19 __:21 ):23 {:25 __:27 }:29
	// else:31 if:33 (:35 __:37 ):39 {:41 __:43 }:45
	// else:47 {:49 __:51 }:53
	if(str_eq(name,"do") && !no){ return map_id(syn,13); };
	return is_map(map_id(syn,5+no*16));
};
static map* syn_flow_body(map* syn,int no){
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
static char* syn_is_call(map* syn, int idx){
	if(is_code(map_id(syn,idx+1)) && !is_keyword(map_id(syn,idx+1)) && str_eq(map_id(syn,idx+3),"(") && str_eq(map_id(syn,idx+7),")")){
		return map_id(syn,idx+1); };
	return NULL;
};
static char* syn_var_name(map* syn){
	int idx=next_tok(syn,0,"= ; ,",0);
	return idx ? map_id(syn,idx-2) : map_id(syn,map_len(syn)-map_len(syn)%2-1);
};
static char* syn_var_type(map* syn){
	int idx=next_tok(syn,0,"= ; ,",0);
	return idx ? toks_c(vec_sub(syn,1,idx-4)) : toks_c(vec_sub(syn,1,map_len(syn)-map_len(syn)%2-3));
};
static int syn_assign_val(map* syn){
	return next_tok(syn,0,"=",0)+1;
};
static int is_assign(map* syn){ return next_tok(syn,0,"=",0); };
static int is_var_decl(map* syn){
	return is_word(map_id(syn,1),"int long double float char void struct map static extern unsigned register mempage size_t time_t const FILE inline var");
};
static int is_func_decl(map* syn){
	int idx=next_tok(syn,0,"(",0);
	if(!idx--){ return 0; };
	if(next_tok(syn,0,"=",0)){ return 0; };
	if(!is_word(map_id(syn,1),"int long double float char void struct map static extern unsigned register mempage size_t time_t const FILE inline var")){
		return 0; };
	if(str_eq(map_id(syn,idx+7),";")){ return 1; };
	if(str_eq(map_id(syn,idx+7),"{") && str_eq(map_id(syn,idx+13),";")){ return 1; };
	return 0;
};
static char* to_c(void* val){
	if(!val){ return "NULL"; };
	if(is_i(val)){ return int_str(is_int(val)); };
	if(is_f(val)){ return double_str(is_double(val)); };
	if(is_str(val)){ return str_quote(val); };
	if(is_map(val)){ return json(val,0); };
	assert(0);
	return "fox_error";
};
static char* str_shorten(char* in,int max){
	if(str_len(in)>max){ return xstr(sub_str(in,0,max),"...", End); };
	return in;
};
static char* call_c(map* params,char* name){
	map* ret=new_vec();
	for(int i=next(params,-1,NULL,NULL); has_id(params,i); i++){ void* v=map_id(params,i);
		vec_add(ret,str_shorten(to_c(v),40)); };
	return xstr(name, "(", map_join(ret,", "), ")", End);
};
map* source_funcs(map* infiles){
	if(!infiles){ return map_val(_globals,"funcs"); };
	map* mp=new_map();	
	for(int i=next(infiles,-1,NULL,NULL); has_id(infiles,i); i++){ void* v=map_id(infiles,i);
		map* mp3=NULL;
		if(str_end(v,".fox")){ mp3=file_funcs(v,1); }
		else {mp3=file_funcs(v,0);};
		px(xstr("+ ",int_str( map_len(mp3)), " <- ", v, End),1);
		map_merge(mp,mp3); };
	add(_globals,"funcs",mp);
	return map_val(_globals,"funcs");
};
map* file_funcs(char* filename,int withbody){ return x_funcs(file_read(filename,1,1),withbody,filename); };
static void src(map* mp,int from,int len,char* msg){
	if(!len){ len=mp->len-from; };
	px(json(vec_sub(mp,from,len),0),1);
	if(msg){ px(xstr(msg, " ___________________________", End),1); };
};
static map* structs(){
	return (map_val(_globals,"structs") ? map_val(_globals,"structs") : map_val(map_val(_globals,"reflect"),"structs"));
};
static map* macros(){
	return (map_val(_globals,"macros") ? map_val(_globals,"macros") : map_val(map_val(_globals,"reflect"),"macros"));
};
map* file_blocks(char* infile){
	map* toks=str_tokenize(file_read(infile,1,1),"\n",0);
	map* ret=new_vec();
	for(int i=next(toks,-1,NULL,NULL); has_id(toks,i); i+=2){ char* tok=map_id(toks,i);
		if(!tok){ continue; };
		if(tok[0]=='/' && (tok[1]=='/' || tok[1]=='*')){
			vec_add(ret,xmap("type", "comment", "code", tok, "space", map_id(toks,i+1) , End));
			continue; };
		char* head=next_word(tok," \t\n");
		if(is_word(head,"typedef struct enum union class #include #pragma extern #define #if #ifdef #undef #else #endif #ifndef #line")){
			vec_add(ret,xmap("type", head, "code", tok, "space", map_id(toks,i+1) , End));
			continue; };
		if(str_chr(tok,'{') || str_has(tok,"=>")){
			vec_add(ret,xmap("type", "func", "code", tok, "space", map_id(toks,i+1) , End));
			continue; };
		if(str_has(tok,"=>")){
			vec_add(ret,xmap("type", "linefunc", "code", tok, "space", map_id(toks,i+1) , End));
			continue; };
		vec_add(ret,xmap("type", "global", "code", tok, "space", map_id(toks,i+1) , End)); };
	return ret;
};
char* file_section(char* file, char* section){
	char* ret=NULL;
	map* map_1=file_blocks(file); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* rec=map_id(map_1,next1);
		if(is_word(map_val(rec,"type"),section)){
			ret=xcat(ret,map_val(rec,"code"),map_val(rec,"space"), End); }; };
	return ret;
};
char* files_section(map* files, char* section){
	char* ret=NULL;
	for(int next1=next(files,-1,NULL,NULL); has_id(files,next1); next1++){ void* file=map_id(files,next1); ret=xcat(ret,file_section(file,section), End); };
	return ret;
};
static map* toks_syn(map* toks,int recurse){
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
char* increase_version(){
	if(!is_file(".version.txt")){ write_file("0",".version.txt",0,1); };
	return write_file(int_str((atoi(file_read(".version.txt",1,1))+1)),".version.txt",0,1);
};
static map* toks_replace(map* in,map* replace){
	for(int i=next(in,-1,NULL,NULL); has_id(in,i); i++){ void* v=map_id(in,i); char* k=map_key(in, i);
		if(is_map(v)){
			toks_replace(v,replace);
		}else if(is_str(v) && map_val(replace,v)){
			vec_compact(vec_splice(in,i,1,map_val(replace,v)));
			i+=map_len(map_val(replace,v))+1; }; };
	return in;
};
static map* toks_join(map* in, char* by){
	if(!in){ return NULL; };
	map* ret=vec_dup(map_id(in,0));
	assert(is_vec(ret));
	for(int i=1; i<in->len; i++){
		if(!(map_len(ret)%2)){ xadd(ret,NULL, End); };
		xadd(ret,by, End);
		vec_merge(ret,map_id(in,i)); };
	return ret;
};
static map* expand_macros(map* mp,map* macros){
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
static map* toks_macros(map* mp){
	map* ret=new_map();
	for(int i=0; i<map_len(mp); i+=2){
		if(!str_has(map_id(mp,i),"#define")){ continue; };
		map* map_1=str_split(map_id(mp,i),"\n",0); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* line=map_id(map_1,i2);
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
static int is_inline_vector(map* toks,int idx){
	if(!is_word(map_id(toks,idx),"[ { ")){ return 0; };
	char* pre=is_str(map_id(toks,idx-2));
	if(str_eq(pre,":")){
		for(int i=idx-4; i>=0; i-=2){
			if(str_eq(map_id(toks,i),"case")){ return 0; }; }; };
	if(is_word(pre,") ]")){ return 0; };
	if(is_code(pre) && !is_word(pre,"return")){ return 0; };
	if(str_eq(pre,"=")){
		char* type=old_decl_type(toks,idx-4);
		if(type && !is_word(type,"map* void* var")){ return 0; }; };
	return str_eq(map_id(toks,idx),"[") ? Vector : Map;
};
static map* string_operators(map* toks){
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
static map* inline_vectors(map* toks){
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
static map* file_map(char* filename){ return data_map(file_read(filename,1,1)); };
map* data_map(char* in){ return data_map2(&in,0); };
static map* data_map2(char** in,int level){
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
		if(keyend==keystart){ str=keyend; continue; };
		if(keystart-tabstart<level){ break; };
		str=keyend;
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
static char* skip_code(char* in,char* terminator){
	while(*in){
		if(str_chr("({[",*in)){ in=skip_paren(in); continue; };
		if(str_chr("\"'`",*in)){ in=skip_quote(in); continue; };
		if(str_chr(terminator,*in)){ break; };
		in++; };
	return in;
};
static char* skip_upto(char* in,char* chars){
	while(*in && !strchr(chars,*in)) {in++;};
	return in;
};
static void* data_unquote(char* in){
	if(!in || !str_len(in)){ return NULL; };
	if(str_chr("\"'`",*in)){ return str_unquote(in); };
	if(*in=='='){ return in; }; //return in.sub_str(1).eval()
	if(str_start(in,"---")){ return sub_str(in,3,-3); };
	if(*in=='{'){ return prop_map(sub_str(in,1,-1),NULL); };
	if(*in=='['){ return prop_vec(sub_str(in,1,-1)); };
	return in;
};
static map* prop_vec(char* in){
	map* ret=split_by(in,',',0);
	for(int next1=next(ret,-1,NULL,NULL); has_id(ret,next1); next1++){ void* val=map_id(ret,next1); char* idx=map_key(ret, next1);
		add(ret,idx,data_unquote(val)); };
	return ret;
};
static map* prop_map(char* in,char* name){
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
char* callfunc_c(map* funcs){
	char* ret="function calls\n";
	map* fdups=new_map();
	for(int i2=next(funcs,-1,NULL,NULL); has_id(funcs,i2); i2++){ void* v=map_id(funcs,i2); char* k=map_key(funcs, i2);
		if(map_val(fdups,map_val(v,"name"))){ continue; };
		add(fdups,map_val(v,"name"),int_var(1));
		if(str_eq(k,"args_map")){ continue; };
		if(is_word(map_val(v,"name"),"main run")){ continue; };
		char* str_params=func_ccall(v);
		if(!str_params){
			continue; };
		//pp1
		ret=xcat(ret,mstr("\t\tcase %p: { %s break; }\n",str_hash(map_val(v,"name")),str_params, End), End); };
	return ret;
};
static char* func_ccall(map* fn){
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
		if(str_eq(k,"...")){
			char* mtype=NULL;
			if(str_eq(map_val(fn,"type"),"void") || str_end(map_val(fn,"type"),"*")){ mtype="ptr"; }
			else if(str_eq(map_val(fn,"type"),"int")){ mtype="int"; }
			else if(str_eq(map_val(fn,"type"),"double")){ mtype="double"; }
			else {return NULL;};
			isvariadic=1;
			ret=xstr("call_variadic_", mtype, "(v,", map_val(fn,"name"), ",\"", map_val(fn,"name"), "\")", End);
			break;
		}else if(str_eq(v,"char*")){ post=".is_str()"; }
		else if(str_eq(v,"char**")){ preproc=xcat(preproc,xstr("char* p", k, "_", map_val(fn,"name"), "=v.map_id(",int_str( i), ").is_str(); ", End), End); mid=xstr("&p", k, "_", map_val(fn,"name"), End); }
		else if(str_eq(v,"map*")){ post=".is_map()"; }
		else if(is_word(v,"int long size_t time_t") || str_eq(v,"long long")){ post=".to_int()"; }
		else if(str_eq(v,"char")){
			post=xstr(".is_str() ? ", mid, ".is_str()[0] : ", mid, ".to_int())", End);
			pre="(";
		}else if(is_word(v,"double float")){ post=".to_double()"; }
		else if(!str_end(v,"*")){
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
char* fox_c(char* infile, char* outfile){ return write_file(x_c(file_read(infile,1,1)),outfile,0,1); };
char* x_c(char* in){ return toks_c(x_toks(in,0)); };
static map* type_convert(map* tok,char* outtype,map* env,map* fs,map* fn){
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
static map* param_c(map* params,map* env,map* fs,map* fn){
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
static char* head_type(map* toks, int idx, int upto, map* env,map* fs){
	int head=expr_head(toks,idx,".");
	return expr_type(toks,head, upto, env, fs);
};
char* help(char* func){
	if(func){
		return func_cdecl(map_val(funcs(),func),1); };
	return funcs_cdecl(funcs(),1);
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
static map* auto_types(map* toks,char* context,int is_script,map* env,map* fns,map* func,int idx){
	int temp=0;
	if(!toks){ return toks; };
	if(str_eq(context,"c")){
		env=xmap("_globals" , "map*", End);
		for(int i=0; i<toks->len; i+=2){
			void* tok=map_id(toks,i+1);
			if(is_func_decl(tok)){
				map* fn=syn_func(tok,1,NULL);
				add(fns,map_val(fn,"name"),fn);
				syn_remove_default(tok); }; };
		for(int i=next(fns,-1,NULL,NULL); has_id(fns,i); i++){ void* fn=map_id(fns,i); char* name=map_key(fns, i);
			map* fenv=map_dup(env);	
			map* map_1=map_val(fn,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* op=map_id(map_1,i2); char* param=map_key(map_1, i2);
				add(fenv,param,map_val(op,"type")); };
			auto_types(add_return(map_val(fn,"body")),"body",is_script,fenv,fns,fn,0);
			if(is_script){
				if(map_val(fn,"body")){ add(add_key(_globals,"userfuncs",Map),map_val(fn,"name"),map_val(fn,"body")); };
				add(add_key(add_key(_globals,"reflect",Map),"funcs",Map),map_val(fn,"name"),map_del_key(fn,"body")); }; };
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
					px(xstr(map_val(func,"name"), "(): unknown void* ", name, End),1); }; };
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
			}else if(str_eq(map_id(toks,i+1),"[") && is_word(head_type(toks,i-2,i,env,fns),"map* void* var") && neq(is_typecast(toks,head),"void*") && 1){
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
				}else if(is_word(expr_type(toks,head,i,env,fns),"map* void* var") && neq(is_typecast(toks,head),"void*")){
					void* name=map_id(toks,i+3);
					map* mid=vec_splice(xadd(vec_sub(toks,head+1,i-head-1),NULL,",",NULL,str_quote(str_unquote(name)), End),0,0,xvec(NULL, End));
					vec_splice(toks,head+1,i+3-head,xvec("map_val",NULL,"(",NULL,mid,NULL,")", End));
					i=head; }; }; };
		return toks; };					
	return toks;
};
map* file_callmap(char* filename){ return func_depends(file_funcs(filename,1),filename); };
static map* func_depend(map* mp,map* ret){
	for(int i=0; i<map_len(mp); i+=2){
		if(is_map(map_id(mp,i+1))){ func_depend(map_id(mp,i+1),ret); continue; };
		char* name=syn_is_call(mp,i);
		if(name){ add(ret,name,map_val(map_val(funcs(),name),"file")); }; };
	return ret;
};
map* depends(char* filename){
	map* ret=new_map();
	map* ref=map_val(_globals,"reflect");
	map* map_1=map_val(ref,"depends"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* funcs=map_id(map_1,next1); char*  name=map_key(map_1, next1);
//		funcs.px()
		if(str_eq(map_val(map_val(map_val(ref,"funcs"),name),"file"),filename)){
			continue; };
		for(int next1=next(funcs,-1,NULL,NULL); has_id(funcs,next1); next1++){ void* file=map_id(funcs,next1); char*  name1=map_key(funcs, next1);
			if(!str_eq(file,filename)){ continue; };
			px(xstr(map_val(map_val(map_val(ref,"funcs"),name),"file"), " ", name, " ", name1, " ", filename, End),1);
			add(ret,map_val(map_val(map_val(ref,"funcs"),name),"file"),map_val(map_val(map_val(ref,"funcs"),name),"file")); }; };
	return ret;
};
map* file_depends(char* filename,...){
	map* files=new_map();
	add(files,filename,file_funcs(filename,1));
	map* filenames=new_vec();
	va_list args;
	va_start(args,filename);
	while(1){
		void* val=va_arg(args,void*);
		if(val==End){ break; };
		vec_add(filenames,val); };
	va_end(args);
	for(int next1=next(filenames,-1,NULL,NULL); has_id(filenames,next1); next1++){ void* name=map_id(filenames,next1);
		add(files,name,file_funcs(name,1)); };
	map* funcs=new_map();
	map* map_1=map_val(files,filename); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ char* name=map_key(map_1, next1);
		func_depend_recursive(name,filename,funcs,files); };
	return funcs;
};
static map* func_depends(map* mp,char* filename){
	map* ret=new_map();
	for(int next1=next(mp,-1,NULL,NULL); has_id(mp,next1); next1++){ void* fn=map_id(mp,next1); char* name=map_key(mp, next1);
		map* map_1=func_depend(map_val(fn,"body"),new_map()); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* key=map_key(map_1, next1); px(xstr(filename, " ", name, " ", key, " ", val, End),1); }; };
	return ret;
};
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
map* funcs(){
	return (map_val(_globals,"funcs") ? map_val(_globals,"funcs") : map_val(map_val(_globals,"reflect"),"funcs"));
};
void* fox_run(char* in){
	int halt=0;
	return fox_eval(x_toks(in,1),xmap("args", map_val(_globals,"args"), End),&halt);
};
static void* fox_eval(map* mp,map* env,int* halt){
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
static void* eval_toks(map* mp,map* env){
	int idx=1;
	return eval_expr(mp,&idx,env,0);
};
static void* eval_expr(map* mp,int* idx,map* env,int level){
	void* last=NULL;
	*idx=eval_expr_cont(mp,*idx,env,&last,level);
	return last;
};
static void* binary_op(void* left, char oper, void* right){
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
static int is_true(void* val){
	if(!val){ return 0; };
	if(is_i(val)){ return is_int(val); };
	if(is_f(val)){ return is_double(val); };
	if(is_map(val)){ return map_len(val); };
	if(is_str(val)){ return str_len(val); };
	return 1;
};
char* func_cdecl(map* fn,int show_default){
	char* ret2="";
	map* map_1=map_val(fn,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* param=map_id(map_1,i2); char* name=map_key(map_1, i2);
		char* s=str_join(map_val(param,"type")," ",name);
		if(str_eq(name,"...")){ s=name; };
		if(show_default){ s=str_join(s,"=",map_val(param,"default")); };
		ret2=str_join(ret2,", ",s); };
	char* file=NULL;
	if(map_val(fn,"file")){ file=xstr("//", map_val(fn,"file"), End); };
	return xstr(map_val(fn,"decltype"), " ", map_val(fn,"name"), "(", ret2, "); ", file, End);
};
void* eval(char* in,map* env){
	return eval_toks(x_toks(in,0),env);
};
static int eval_expr_cont(map* mp,int idx,map* env,void** last,int level){
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
void* data_exec(void* data,map* env){
	if(!data){ return NULL; };
	if(is_str(data)){ return data; };	
	if(!is_map(data)){ return to_str(data,"",0); };
	char* func=map_id(data,0);
	return call_func(vec_compact(map_del(data,0,1)),func,env);
};
void* call_func(map* params,char* name,map* env){
	params=eval_params(params,name,env,funcs());
	if(str_start(name,"php_")){ return call_php(params,sub_str(name,5,-2147483648)); };
	map* user=map_val(map_val(_globals,"userfuncs"),name);
	void* ret=NULL;
	if(user){
		int halt=0;
		ret=fox_eval(user,params,&halt);
	}else{
		ret=invoke(params,name); };
	return ret;
};
static map* ping_map(map* in){ return in; };
map* func_depend_recursive(char* func, char* file, map* funcs, map* files){
	if(map_val(funcs,func)){ return funcs; };
	add(funcs,func,file);
	if(!file){
		px(xstr("-- ", func, " -- --", End),1);
		return funcs; };
	if(!map_val(files,file)){ add(files,file,file_funcs(file,1)); };
	void* mp=map_val(files,file);
	mp=func_depend(map_val(map_val(mp,func),"body"),new_map());
	for(int next1=next(mp,-1,NULL,NULL); has_id(mp,next1); next1++){ void* ffile=map_id(mp,next1); char*  fname=map_key(mp, next1);
		if(map_val(funcs,fname)){ continue; };
		if(!ffile){
			for(int next1=next(files,-1,NULL,NULL); has_id(files,next1); next1++){ void* fncs=map_id(files,next1); char*  fs=map_key(files, next1);
				if(map_val(fncs,fname)){ ffile=fs; break; }; }; };
		px(xstr(file, " ", func, " ", fname, " ", ffile, End),1);
		func_depend_recursive(fname,ffile, funcs, files); };
	return funcs;
};
//TODO: merge with _ptr defined bellow.
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
