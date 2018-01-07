/* This is a generated file. To change it, edit function fox_meta() in generator.fox */
#include "draw.h"
void* draw_invoke(map* v,char* name){
	unsigned long long idn=str_hash((unsigned char*)name);
	switch(idn){
//function calls
		case 0x72714d7127a95653: { return img_resize(is_str(map_id(v,0)),v->len>=1 ? to_double(map_id(v,1)) : 0,v->len>=2 ? to_int(map_id(v,2)) : 0,v->len>=3 ? to_int(map_id(v,3)) : 0); break; };

	};
	return End;
};
map* draw_reflect(){
	return xmap(
		"funcs", xmap("img_resize",xmap("name","img_resize","type","char*","decltype","char*","params",xmap("img",xmap("type","char*","default",NULL, End),"enlarge",xmap("type","double","default","0", End),"width",xmap("type","int","default","0", End),"height",xmap("type","int","default","0", End), End),"file","draw.fox", End), End),
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
	if((ret=draw_invoke(params,name))!=End){ return ret; };
	return invoke(params,name);
};
map* user_funcs(){
	//if _globals.cache.reflect => return _globals.cache.reflect
	map* ret=funcs();
	map_merge(ret,map_val(draw_reflect(),"funcs"));
	//_globals.cache.reflect=ret
	return ret;
};
