#line 2 "/web/fox/generator.fox"
#include <fox.h>

int cgi(char* infile, char* xfiles, char* profile, char* outfile,char* opts,int keepfiles){
	return cc(infile,xfiles, profile, outfile, opts, keepfiles);
};
int _iscmd=0;
char* fox_meta(char* infile, char* name,char* outfile){
	map* funcs=file_funcs(infile,0);
	map* macros=c_macros(infile);
	map* structs=c_structs(infile);

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
char* fox_cs(char* name,map* depends){
	char* fox=xstr(name,".fox", End);
	map* func=file_funcs(fox,0);
	map_merge(func,x_funcs(meta_h(name),stoi(xstr(name,"_meta.c", End)),NULL));
	map_merge(map_val(map_val(map_val(_globals,"cache"),"reflect"),"funcs"),func);
	fox_c(fox,xstr(name,".c", End));
	write_file((xstr(""
	"#include <fox.h>\n"
	"#pragma once\n"
	""
	"",funcs_cdecl(func,0), End)),xstr(name,".h", End),0,1);
	char* meta=NULL;
	if(depends){
		for(int next1=next(depends,-1,NULL,NULL); has_id(depends,next1); next1++){ void* file=map_id(depends,next1);
			meta=xcat(meta,meta_h(file), End); };
		meta=xcat(meta,xstr("", 
		"extern int _iscmd;\n", 
		"int exec_cmdline(map* args){\n", 
		"	if !args[1].is_code() => return 0\n", 
		"	_iscmd=1\n", 
		"	args.cmdline_params(user_funcs()).user_invoke(args[1]).ret_print()\n", 
		"	return 1\n", 
		"}\n", 
		"void* user_invoke(map* params, char* name){\n", 
		"	void* ret=''\n", 
		"\tif (ret=params.", name, "_invoke(name))!=End => return ret\n", 
		"", 
		"", End), End);
		for(int next1=next(depends,-1,NULL,NULL); has_id(depends,next1); next1++){ void* file=map_id(depends,next1);
			meta=xcat(meta,xstr("", 
			"\tif (ret=params.", file, "_invoke(name))!=End => return ret\n", 
			"", 
			"", End), End); };
		meta=xcat(meta,xstr("", 
		"	return params.invoke(name)\n", 
		"}\n", 
		"map* user_funcs(){\n", 
		"	//if _globals.cache.reflect => return _globals.cache.reflect\n", 
		"	ret=funcs()\n", 
		"\tret.map_merge(", name, "_reflect().funcs)\n", 
		"", 
		"", End), End);
		for(int next1=next(depends,-1,NULL,NULL); has_id(depends,next1); next1++){ void* file=map_id(depends,next1);
			meta=xcat(meta,xstr("", 
			"\tret.map_merge(", file, "_reflect().funcs)\n", 
			"", 
			"", End), End); };
		meta=xcat(meta,""
		"	//_globals.cache.reflect=ret\n"
		"	return ret\n"
		"}\n"
		""
		"", End);
		meta=x_c(meta); };
	write_file((xstr(fox_meta(fox,name,NULL),meta, End)),xstr(name,"_meta.c", End),0,1);
	return name;
};
int cc(char* infile, char* xfiles, char* profile, char* outfile, char* opts, int keepfiles){
	map* names=new_vec();
	map* map_1=str_split(xfiles,",",0); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* file=map_id(map_1,next1);
		vec_add(names,fox_cs(file_rename(file,NULL,".fox",NULL,NULL,NULL),NULL)); };
	char* name=fox_cs(file_rename(infile,NULL,".fox",NULL,NULL,NULL),names);
	if(!outfile){ outfile=name; };
	char* cflags="-m64 -std=gnu99 -Wno-unused-command-line-argument -g";
	char* xlibs="-lmarkdown -lcurl -lsqlite3 -lstemmer -lfoxastro -lxml2";
	// -fdata-sections -ffunction-sections -Wl,-dead_strip -Wl,-emain
	map* switches=xmap(
		"debug", xstr("-g -O0 -lfox ", cflags, " ", xlibs, " -lfoxmain", End),
		"speed", xstr("-O3 -lfox ", cflags, " ", xlibs, " -lfoxmain", End),
		"size", xstr("-Os -lfox ", cflags, " -lfoxmain", End),
		"static", xstr("-Os -lfoxstatic -lfoxcmdstatic ", cflags, " -fdata-sections -ffunction-sections -Wl,-dead_strip -lfoxmain", End),
		"cgi", xstr("-Os -lfoxstatic -lfoxcgistatic ", cflags, " ", xlibs, " -lfoxmaincgi", End),
		"scgi", xstr("-Os -lfoxstatic -lfoxcgistatic ", cflags, " ", xlibs, " -lfoxmaincgi -fdata-sections -ffunction-sections -Wl,-dead_strip ", End)
	, End);
	profile = (map_val(switches,profile) ? map_val(switches,profile) : map_val(switches,"debug"));
	char* extras=NULL;
	if(map_len(names)){ extras=xstr(map_join(names,".c "),".c ",map_join(names,"_meta.c "),"_meta.c", End); };
	int ret=exec(
		px(
		xstr("gcc ", name, ".c ", name, "_meta.c ", extras, " -o ", outfile, " -L/usr/local/lib ", profile, " ", opts, " -std=gnu99 -Wno-logical-op-parentheses -lm 2>&1", End),1),NULL);
//	if !keepfiles
//		(infile..".c").remove()	
//		(infile..".h").remove()	
	return ret;
};
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
void write_source(char* infile,...){
	map* files=xvec(infile, End);
	va_list args;
	va_start(args,infile);
	while(1){
		char* name=va_arg(args,char*);
		if(name==End){ break; };
		vec_add(files,name); };
	va_end(args);
//	("Compiling "..files.map_join(", ")).px()
//	files=["src/astrostr.fox", "src/cgi.fox", "src/cmd.fox", "src/core.fox", "src/fox.fox", "src/generator.fox", "src/main.fox", "src/maincgi.fox", "src/run.fox", "src/sql.fox", "src/text.fox", "astro/astro.h", "src/eval.fox", "src/dbmeta.fox"]
	source_funcs(files);
	write_foxh(file_rename("fox.h","include",NULL,NULL,NULL,NULL));
	for(int next1=next(files,-1,NULL,NULL); has_id(files,next1); next1++){ void* infile=map_id(files,next1);
		if(str_end(infile,".h")){ continue; };
		fox_c(infile,xstr(file_rename(infile,"src",".fox",NULL,NULL,NULL),".c", End)); };
		//infile.fox_h(infile.file_rename(:include,".fox")..".h")
	write_file((xstr("#include <fox.h>\n\n",funcs_meta(source_funcs(NULL),source_macros(), source_structs(),NULL), End)),file_rename("meta.c","src",NULL,NULL,NULL,NULL),0,1);
	px(mem_usage(),1);
};
char* write_foxh(char* outfile){
	return write_file((xstr(foxh(),funcs_cdecl(source_funcs(NULL),0), End)),outfile,0,1);
};
char* fox_phpc(char* infile,char* outfile){
	map* fns=infile ? file_funcs(infile,0) : source_funcs(NULL);
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
char* meta_h(char* prefix){
	if(prefix){prefix=xstr(prefix,"_", End);};
	return xstr("", 
	"char* ", prefix, "version();\n", 
	"void* ", prefix, "invoke(map* v,char* name);\n", 
	"map* ", prefix, "reflect();\n", 
	"int exec_cmdline(map* args);\n", 
	"void* user_invoke(map* params, char* name);\n", 
	"map* user_funcs();\n", 
	"", 
	"", End);
};
char* funcs_meta(map* funcs, map* macros, map* structs, char* prefix){
	if(prefix){prefix=xstr(prefix,"_", End);};
	return x_c(xstr("", 
	"/* This is a generated file. To change it, edit function funcs_meta() in generator.fox */\n", 
	"char* ", prefix, "version(){\n", 
	"\treturn \"Fox: build: ", increase_version(), ", date: ", time_str(0), " [%s old]\".mstr(\"", time_str(0), "\".time_ago());\n", 
	"}\n", 
	"void* ", prefix, "invoke(map* v,char* name){\n", 
	"	unsigned long long idn=str_hash((unsigned char*)name)\n", 
	"	switch(idn){\n", 
	"//", callfunc_c(funcs), "\n", 
	"	}\n", 
	"\treturn \"invoke(): Function $name not defined\".fox_error()\n", 
	"}\n", 
	"map* ", prefix, "reflect(){\n", 
	"	return {\n", 
	"\t\tfuncs: ", map_ccode(funcs), ",\n", 
	"\t\tmacros: ", map_ccode(macros), ",\n", 
	"\t\tstructs: ", map_ccode(structs), "\n", 
	"	}\n", 
	"}\n", 
	"", 
	"", End));
};
char* gen_htaccess(char* outfile){
	return write_file(""
	"RewriteEngine On\n"
	"#RewriteCond %{HTTPS} off\n"
	"#RewriteRule (.*) https://%{HTTP_HOST}%{REQUEST_URI} [R=301,L]\n"
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
char* gen_cgi_makefile(char* name, char* outfile){
	return write_file(xstr("", 
	"all:\n", 
	"\tfox cgi ", name, ".fox\n", 
	"fox:\n", 
	"	cd /web/fox && make install && cd -\n", 
	"c:\n", 
	"	fox cgi salat.fox --keepfiles=1\n", 
	"", 
	"", End),outfile,0,1);
};
char* gen_fox_cgi(char* name,char* outfile){
	return write_file(xstr("", 
	"#line 2 \"", name, ".fox\"\n", 
	"#include \"", name, ".h\"\n", 
	"\n", 
	"int run(map* req){\n", 
	"\t_globals.dbs.", name, "=\"", name, ".db\"\n", 
	"	path=_globals.paths\n", 
	"\n", 
	"	if \"/\".get().end\n", 
	"		{{\n", 
	"\t\t\ttitle ", str_title(name), "'s Application\n", 
	"		}}.show_page()\n", 
	"\n", 
	"	not_found()\n", 
	"	return 0\n", 
	"}\n", 
	"", 
	"", End),outfile,0,1);
};
int cgi_init(char* name){
	if(!is_file((xstr(name,".fox", End)))){ gen_fox_cgi(name,xstr(name,".fox", End)); };
	if(!is_file(".htaccess")){ gen_htaccess(".htaccess"); };
	if(!is_file("Makefile")){ gen_cgi_makefile(name,"Makefile"); };
	return 0;
};
char* c_h(char* infile,char* outfile){ return write_file(funcs_cdecl(file_funcs(infile,0),0),outfile,0,1); };
char* fox_h(char* infile,char* outfile){ return write_file((xstr("#include <fox.h>\n",funcs_cdecl(file_funcs(infile,0),0), End)),outfile,0,1); };
map* x_map(char* in){ return c_tokenizer(&in,'\0'); };
char* c_x(char* in){ return toks_c(map_tox(x_map(in))); };

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
	for(int  i=next(fns,-1,NULL,NULL); has_id(fns, i);  i++){ void* v=map_id(fns, i); char*  k=map_key(fns,  i);
		if(is_word(k,"main")){ continue; };
		ret=xcat(ret,func_cdecl(v,show_default), End);
		ret=xcat(ret,"\n", End); };
	return ret;
};
char* foxh(){
	return ""
	"/* This is a generated file. To change it, edit function foxh() in fox.c */\n"
	"#pragma once\n"
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
	"//	#include <openssl/md5.h>\n"
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
	"	int max_roots;\n"
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
	"#define None 0x0F9AD3BA\n"
	"#define End (char*)(0x0FF1B14E059AD3BA)\n"
	"\n"
	"void* invoke(map* v,char* name);\n"
	"char* strstr(const char* str1,const char* str2);\n"
	"int chdir(const char* path);\n"
	"int max_mem();\n"
	"int curr_mem();\n"
	"map* reflect();\n"
	"char* version();\n"
	""
	"";
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
char* old_decl_type(map* mp,int idx){
	int from=stm_start(mp,idx,"{};,().>:=><!&-+/?");
	for(int i=from;i<idx;i+=2){
		if(is_word(map_id(mp,i),"int long double float char void struct map static extern unsigned register mempage size_t time_t const")){
			return toks_c(vec_sub(mp,from,idx-from-1)); }; };
	return NULL;
};
char* toks_str(map* mp){
	char* ret=NULL;
	for(int next1=next(mp,-1,NULL,NULL); has_id(mp,next1); next1++){ void* val=map_id(mp,next1); ret=xcat(ret,is_map(val) ? toks_str(val) : val, End); };
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
map* syn_func(map* syn,int with_body,char* filename){
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
	if(filename){ add(ret,"file",filename); };
	return ret;
};
map* syn_funcs(map* mp,int with_body,char* filename){
	map* ret=new_map();
	for(int i=1; i<map_len(mp); i+=2){
		if(is_func_decl(map_id(mp,i))){
			map* fn=syn_func(map_id(mp,i),with_body,filename);
			if(str_eq(map_val(fn,"name"),"main")){ continue; };
			add(ret,map_val(fn,"name"),fn); }; };
	return ret;
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
map* x_funcs(char* in,int with_body,char* filename){
	return syn_funcs(
		toks_syn(
		add_semicolon(
		add_curly(
		x_map(in),0),0),0),with_body,filename);
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
	map* funcs=x_funcs(file_read(file,1,1),1,NULL);
	map* ret=new_map();
	for(int i=next(funcs,-1,NULL,NULL); has_id(funcs,i); i++){ void* v=map_id(funcs,i); char* k=map_key(funcs, i);
		call_count(map_val(v,"body"),ret,k); };
	for(int i2=next(funcs,-1,NULL,NULL); has_id(funcs,i2); i2++){ void* v2=map_id(funcs,i2); char* k2=map_key(funcs, i2);
		if(!map_val(ret,k2)){ px(k2,1); }; };
	return ret;
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
char* read_line(FILE* fp){
	char buff[1024];
	char* ret=NULL;
	while(fgets(buff,sizeof(buff),fp)){
		ret=xcat(ret,buff, End); };		
	return ret;
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
map* source_funcs(map* infiles){
	if(!infiles){ return map_val(map_val(_globals,"cache"),"funcs"); };
	map* mp=new_map();	
	for(int i=next(infiles,-1,NULL,NULL); has_id(infiles,i); i++){ void* v=map_id(infiles,i);
		map* mp3=file_funcs(v,0);
		map_merge(mp,mp3); };
	add(add_key(_globals,"cache",Map),"funcs",mp);
	return map_val(map_val(_globals,"cache"),"funcs");
};
map* file_funcs(char* filename,int withbody){ return x_funcs(file_read(filename,1,1),withbody,filename); };
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
char* increase_version(){
	if(!is_file(".version.txt")){ write_file("0",".version.txt",0,1); };
	return write_file(int_str((atoi(file_read(".version.txt",1,1))+1)),".version.txt",0,1);
};
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
	if(map_len(mp) || space1){ vec_add(mp,space1); };
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
map* read_paren(map* mp,char** line,map*(*func)(char**)){
	char* str=*line;
	vec_add(mp,char_str(*str));
	vec_add(mp,func(line));
	vec_add(mp,char_str(closing_paren(*str)));
	return mp;
};
map* file_map(char* filename){ return data_map(file_read(filename,1,1)); };
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
	if(*in=='='){ return in; }; //return in.sub_str(1).eval()
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
