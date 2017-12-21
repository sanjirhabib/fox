#line 2 "/web/fox/generator.fox"
#include <fox.h>

int cgi(char* infile, char* xfiles, char* profile, char* outfile,char* opts,int keepfiles){
	return cc(infile,profile, outfile, opts,int_str( keepfiles),0);
};
int _iscmd=0;
char* fox_meta(char* infile, char* name,char* outfile){
	return write_file(x_c(xstr("", 
	"#include \"", name, ".h\"\n", 
	funcs_meta(file_funcs(infile,0),c_macros(infile), c_structs(infile), name), "\n", 
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
		"\tfns=", name, "_reflect().funcs\n", 
		"	if fns[args[1]]\n", 
		"\t\targs.cmdline_params(fns).", name, "_invoke(args[1]).ret_print()\n", 
		"		return 1\n", 
		"", 
		"", End), End);
		for(int next1=next(depends,-1,NULL,NULL); has_id(depends,next1); next1++){ void* file=map_id(depends,next1);
			meta=xcat(meta,xstr("", 
			"\tfns=", file, "_reflect().funcs\n", 
			"	if fns[args[1]]\n", 
			"\t\targs.cmdline_params(fns).", file, "_invoke(args[1]).ret_print()\n", 
			"		return 1\n", 
			"", 
			"", End), End); };
		meta=xcat(meta,""
		"	args.cmdline_params(funcs()).invoke(args[1]).ret_print()\n"
		"	return 1\n"
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
	char* xlibs="-lmarkdown -lcurl -lsqlite3";
	// -fdata-sections -ffunction-sections -Wl,-dead_strip -Wl,-emain
	map* switches=xmap(
		"debug", xstr("-g -O0 -lfox ", cflags, " ", xlibs, " -lfoxmain", End),
		"speed", xstr("-O3 -lfox ", cflags, " ", xlibs, " -lfoxmain", End),
		"size", xstr("-Os -lfox ", cflags, " -lfoxmain", End),
		"static", xstr("-lfoxstatic -lfoxcmdstatic ", cflags, " -fdata-sections -ffunction-sections -Wl,-dead_strip -lfoxmain", End),
		"cgi", xstr("-Os -lfoxstatic -lfoxcgistatic ", cflags, " ", xlibs, " -lfoxmaincgi", End),
		"scgi", xstr("-Os -lfoxstatic -lfoxcgistatic ", cflags, " ", xlibs, " -lfoxmaincgi -fdata-sections -ffunction-sections -Wl,-dead_strip ", End)
	, End);
	profile = (map_val(switches,profile) ? map_val(switches,profile) : map_val(switches,"debug"));
	char* extras=NULL;
	if(names){ extras=xstr(map_join(names,".c "),".c ",map_join(names,"_meta.c "),"_meta.c", End); };
	int ret=exec(
		px(
		xstr("gcc ", name, ".c ", name, "_meta.c ", extras, " -o ", outfile, " -L/usr/local/lib ", profile, " ", opts, " -std=gnu99 -Wno-logical-op-parentheses -lm 2>&1", End),1),NULL);
	if(!keepfiles){
		remove((xstr(infile,".c", End)));	
		remove((xstr(infile,".h", End))); };	
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
void write_source(){
	source_funcs();
	write_foxh(file_rename("fox.h","include",NULL,NULL,NULL,NULL));
	map* map_1=source_files(); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* infile=map_id(map_1,next1);
		if(str_end(infile,".h")){ continue; };
		fox_c(infile,xstr(file_rename(infile,"src",".fox",NULL,NULL,NULL),".c", End)); };
		//infile.fox_h(infile.file_rename(:include,".fox")..".h")
	write_file((xstr("#include <fox.h>\n\n",funcs_meta(source_funcs(),source_macros(), source_structs(),NULL), End)),file_rename("meta.c","src",NULL,NULL,NULL,NULL),0,1);
	px(mem_usage(),1);
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
char* meta_h(char* prefix){
	if(prefix){prefix=xstr(prefix,"_", End);};
	return xstr("", 
	"char* ", prefix, "version();\n", 
	"void* ", prefix, "invoke(map* v,char* name);\n", 
	"map* ", prefix, "reflect();\n", 
	"int exec_cmdline(map* args);\n", 
	"", 
	"", End);
};
char* funcs_meta(map* funcs, map* macros, map* structs, char* prefix){
	if(prefix){prefix=xstr(prefix,"_", End);};
	return x_c(xstr("", 
	"/* This is a generated file. To change it, edit function funcs_meta() in fox.c */\n", 
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
