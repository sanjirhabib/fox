/* This is a generated file. To change it, edit function fox_meta() in generator.fox */
#include "htmltext.h"
void* htmltext_invoke(map* v,char* name){
	unsigned long long idn=str_hash((unsigned char*)name);
	switch(idn){
//function calls
		case 0x6531c9383d2: { return int_var(str_at(is_str(map_id(v,0)),is_str(map_id(v,1)))); break; };
		case 0x377cb0b1bf05c7e: { return html_text(is_str(map_id(v,0))); break; };

	};
	return End;
};
map* htmltext_reflect(){
	return xmap(
		"funcs", xmap("str_at",xmap("name","str_at","type","int","decltype","int","params",xmap("in",xmap("type","char*","default",NULL, End),"find",xmap("type","char*","default",NULL, End), End),"file","htmltext.fox", End),"html_text",xmap("name","html_text","type","char*","decltype","char*","params",xmap("in",xmap("type","char*","default",NULL, End), End),"file","htmltext.fox", End), End),
		"macros", new_map(),
		"structs", new_map()
	, End);
};
extern int _iscmd;
int exec_cmdline(map* args){
	if(!is_code(map_id(args,1))){ return 0; };
	_iscmd=1;
	ret_print(user_invoke(cmdline_params(args,user_funcs()),map_id(args,1)));
	return 1;
};
void* user_invoke(map* params, char* name){
	void* ret=NULL;
	if((ret=htmltext_invoke(params,name))!=End){ return ret; };
	return invoke(params,name);
};
map* user_funcs(){
	//if _globals.cache.reflect => return _globals.cache.reflect
	map* ret=funcs();
	map_merge(ret,map_val(htmltext_reflect(),"funcs"));
	//_globals.cache.reflect=ret
	return ret;
};
