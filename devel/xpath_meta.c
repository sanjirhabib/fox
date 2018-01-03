/* This is a generated file. To change it, edit function fox_meta() in generator.fox */
#include "xpath.h"
void* xpath_invoke(map* v,char* name){
	unsigned long long idn=str_hash((unsigned char*)name);
	switch(idn){
//function calls
		case 0x311084ecfa: { usage(is_str(map_id(v,0))); return NULL; break; };

	};
	return End;
};
map* xpath_reflect(){
	return xmap(
		"funcs", xmap("usage",xmap("name","usage","type","void","decltype","void","params",xmap("name",xmap("type","char*","default",NULL, End), End),"file","xpath.fox", End), End),
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
	if((ret=xpath_invoke(params,name))!=End){ return ret; };
	return invoke(params,name);
};
map* user_funcs(){
	//if _globals.cache.reflect => return _globals.cache.reflect
	map* ret=funcs();
	map_merge(ret,map_val(xpath_reflect(),"funcs"));
	//_globals.cache.reflect=ret
	return ret;
};
