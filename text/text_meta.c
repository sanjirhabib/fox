/* This is a generated file. To change it, edit function fox_meta() in generator.fox */
#include "text.h"
void* text_invoke(map* v,char* name){
	unsigned long long idn=str_hash((unsigned char*)name);
	switch(idn){
//function calls
		case 0xd0b721632a68: { return ucs_str(map_id(v,0),v->len>=1 ? to_int(map_id(v,1)) : 0); break; };
		case 0x377cc446ee87670: { return int_var(is_letter(to_int(map_id(v,0)))); break; };
		case 0xd0b721630aae: { return int_var(ucs_len(map_id(v,0))); break; };
		case 0x377db05071d15de: { return ucs_words(map_id(v,0)); break; };
		case 0x377d91e81891a0c: { return str_words(is_str(map_id(v,0))); break; };
		case 0x5977fb: { go(is_str(map_id(v,0))); return NULL; break; };
		case 0xc0d28aa7b288a882: { return stem_bangla(is_str(map_id(v,0))); break; };
		case 0x377d91ab7cc797d: { return int_var(stem_test()); break; };

	};
	return End;
};
map* text_reflect(){
	return xmap(
		"funcs", xmap("ucs_str",xmap("name","ucs_str","type","char*","decltype","char*","params",xmap("in",xmap("type","int*","default",NULL, End),"len",xmap("type","int","default","0", End), End),"file","text.fox", End),"str_ucs",xmap("name","str_ucs","type","int*","decltype","int*","params",xmap("in",xmap("type","char*","default",NULL, End), End),"file","text.fox", End),"is_letter",xmap("name","is_letter","type","int","decltype","int","params",xmap("code",xmap("type","int","default",NULL, End), End),"file","text.fox", End),"word_end",xmap("name","word_end","type","int*","decltype","int*","params",xmap("in",xmap("type","int*","default",NULL, End), End),"file","text.fox", End),"word_start",xmap("name","word_start","type","int*","decltype","int*","params",xmap("in",xmap("type","int*","default",NULL, End), End),"file","text.fox", End),"ucs_substr",xmap("name","ucs_substr","type","int*","decltype","int*","params",xmap("in",xmap("type","int*","default",NULL, End),"from",xmap("type","int","default",NULL, End),"upto",xmap("type","int","default",NULL, End), End),"file","text.fox", End),"ucs_len",xmap("name","ucs_len","type","int","decltype","int","params",xmap("in",xmap("type","int*","default",NULL, End), End),"file","text.fox", End),"ucs_words",xmap("name","ucs_words","type","map*","decltype","map*","params",xmap("uin",xmap("type","int*","default",NULL, End), End),"file","text.fox", End),"str_words",xmap("name","str_words","type","map*","decltype","map*","params",xmap("in",xmap("type","char*","default",NULL, End), End),"file","text.fox", End),"go",xmap("name","go","type","void","decltype","void","params",xmap("in",xmap("type","char*","default",NULL, End), End),"file","text.fox", End),"stem_bangla",xmap("name","stem_bangla","type","char*","decltype","char*","params",xmap("in",xmap("type","char*","default",NULL, End), End),"file","text.fox", End),"stem_test",xmap("name","stem_test","type","int","decltype","int","params",new_map(),"file","text.fox", End), End),
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
	if((ret=text_invoke(params,name))!=End){ return ret; };
	return invoke(params,name);
};
map* user_funcs(){
	//if _globals.cache.reflect => return _globals.cache.reflect
	map* ret=funcs();
	map_merge(ret,map_val(text_reflect(),"funcs"));
	//_globals.cache.reflect=ret
	return ret;
};
