#include "cgi.h"
/* This is a generated file. To change it, edit function funcs_meta() in fox.c */
char* cgi_version(){
	return mstr("Fox: build: 224, date: 2017-12-21 20:06:31 [%s old]",time_ago("2017-12-21 20:06:31"), End);
};
void* cgi_invoke(map* v,char* name){
	unsigned long long idn=str_hash((unsigned char*)name);
	switch(idn){
//function calls
		case 0x59792d: { return px(map_id(v,0),v->len>=1 ? to_int(map_id(v,1)) : 1); break; };
		case 0x3110b3eb57: { xexit(v->len>=0 ? to_int(map_id(v,0)) : 0); return NULL; break; };
		case 0x377c84dc6878e9b: { return fox_error(is_str(map_id(v,0)),v->len>=1 ? to_int(map_id(v,1)) : 0); break; };
		case 0x1ae6e807bf1ec8: { return call_php(is_map(map_id(v,0)),is_str(map_id(v,1))); break; };

	};
	return fox_error(xstr("invoke(): Function ", name, " not defined", End),0);
};
map* cgi_reflect(){
	return xmap(
		"funcs", xmap("px",xmap("name","px","type","void*","decltype","void*","params",xmap("str",xmap("type","void*","default",NULL, End),"newline",xmap("type","int","default","1", End), End),"file","cgi.fox", End),"xexit",xmap("name","xexit","type","void","decltype","void","params",xmap("val",xmap("type","int","default","0", End), End),"file","cgi.fox", End),"fox_error",xmap("name","fox_error","type","void*","decltype","void*","params",xmap("msg",xmap("type","char*","default",NULL, End),"dump",xmap("type","int","default","0", End), End),"file","cgi.fox", End),"call_php",xmap("name","call_php","type","void*","decltype","void*","params",xmap("params",xmap("type","map*","default",NULL, End),"func",xmap("type","char*","default",NULL, End), End),"file","cgi.fox", End), End),
		"macros", new_map(),
		"structs", new_map()
	, End);
};

