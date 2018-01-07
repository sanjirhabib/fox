#line 2 "/web/fox/fox.fox"
/*
NOTE:
	Haven't used LLVM. That would force the whole application to be JITed.
BUGS:
	Dec-2017
	"'$k'=" doesn't compile correctly.
	a[hello][world] bug
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
int fib(int i){
	if(i < 2){ return i; };
	return fib(i-1)+fib(i-2);
};
int msleep(int msecs){
	return usleep(msecs*1000);
};
int str_ascii(char* in){ return in[0]; };
char* ping(char* str){ return str; };
