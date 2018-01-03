/* This is a generated file. To change it, edit function fox_meta() in generator.fox */
#include "foxpath.h"
void* foxpath_invoke(map* v,char* name){
	unsigned long long idn=str_hash((unsigned char*)name);
	switch(idn){
//function calls
		case 0x34fa715c0886a20b: { return execute_xpath_expression(is_str(map_id(v,0)),is_str(map_id(v,1))); break; };
		case 0xaa49c2a859d575ce: { return print_xpath_nodes(map_id(v,0)); break; };

	};
	return End;
};
map* foxpath_reflect(){
	return xmap(
		"funcs", xmap("execute_xpath_expression",xmap("name","execute_xpath_expression","type","map*","decltype","map*","params",xmap("in",xmap("type","char*","default",NULL, End),"xpathExpr",xmap("type","char*","default",NULL, End), End),"file","foxpath.fox", End),"print_xpath_nodes",xmap("name","print_xpath_nodes","type","map*","decltype","map*","params",xmap("nodes1",xmap("type","void*","default",NULL, End), End),"file","foxpath.fox", End), End),
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
	if((ret=foxpath_invoke(params,name))!=End){ return ret; };
	return invoke(params,name);
};
map* user_funcs(){
	//if _globals.cache.reflect => return _globals.cache.reflect
	map* ret=funcs();
	map_merge(ret,map_val(foxpath_reflect(),"funcs"));
	//_globals.cache.reflect=ret
	return ret;
};
