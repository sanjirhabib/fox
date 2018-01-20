/* This is a generated file. To change it, edit function fox_meta() in generator.fox */
#include "teststruct.h"
void* teststruct_invoke(map* v,char* name){
	unsigned long long idn=str_hash((unsigned char*)name);
	switch(idn){;
//function calls

	};
	return End;
};
map* teststruct_reflect(){
	return xmap(
		"funcs", new_map(),
		"macros", new_map(),
		"structs", new_map()
	, End);
};
extern int _iscmd;
int cmdline(){
	void* args=map_val(_globals,"args");
	if(!is_code(map_id(args,1))){ return 0; };
	_iscmd=1;
	ret_print(user_invoke(cmdline_params(args,user_funcs()),map_id(args,1)));
	return 1;
};
void* user_invoke(map* params, char* name){
	void* ret=NULL;
	if((ret=teststruct_invoke(params,name))!=End){ return ret; };
	return invoke(params,name);
};
map* user_funcs(){
	//if _globals.cache.reflect => return _globals.cache.reflect
	map* ret=funcs();
	map_merge(ret,map_val(teststruct_reflect(),"funcs"));
	//_globals.cache.reflect=ret
	return ret;
};
