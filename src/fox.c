#line 2 "/web/fox/fox.fox"
/*
NOTE:
	Haven't used LLVM. That would force the whole application to be JITed.
BUGS:
	Dec-2017
	"'$k'=" doesn't compile correctly.
	Dec-2017
	---; <-- requires ending semicolon.
TODO:
	eval case:
	eval << >>
CONSIDER:
	sym(char*);
*/
#include <fox.h>

#ifdef PHP_MOD
#undef assert
#define assert(x) if(!(x)){ fox_error("Assert failed!",1); }
#endif


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
char* xlog(char* str){
	FILE* fp=fopen("/tmp/log.txt","a+");
	fprintf(fp,"%s\n",str);
	fclose(fp);
	return str;
};
char* hello(){ return "Hi"; };
map* file_vec(char* in){ return str_vec(file_read(in,1,1)); };
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
		else if(is_word(s,"lft rgt id slno")){
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
int test_add(int a,int b){ return a+b; };
map* test_map(map* out){ return out; };

char* file_path(char* file){
	int i=0;
	for(i=str_len(file); i>0; i--){ if(file[i-1]=='/'){ break; }; };
	if(!i){ return "./"; };
	return sub_str(file,0,i);
};
map* load_global(char* file){
	map* ret=str_map(file_read(file,1,1),Map);
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
int str_ascii(char* in){ return in[0]; };
void benchmark_gc(){
	map* ret=new_vec();
	for(int i=0;i<1000000;i++){
		set(ret,i%200000,new_str(1023)); };
	dx(int_var(map_len(ret)),NULL,0);
	dx(mem_usage(),NULL,0);
};
char* ping(char* str){ return str; };
char* str_join(char* str1,char* joiner,char* str2){
	if(!str_len(str1)){ return str2; };
	if(!str_len(str2)){ return str1; };
	return xstr(str1,joiner,str2, End);
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
char* c_md5(char* in, size_t initial_len, char* out);
char* md5(char* in){ char ret[33]; return str_dup(c_md5(in,str_len(in),ret)); };
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
int ret_print(void* ret){
	if(_printed){ return is_i(ret) ? is_int(ret) : 0; };
	if(is_map(ret)){ px(json(ret,1),1); return 0; };
	if(is_blob(ret)){ print(ret,stdout); }
	else if(is_str(ret) || is_num(ret)){ px(to_str(ret,"",0),1); };
	return is_i(ret) ? is_int(ret) : 0;
};
