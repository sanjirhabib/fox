#line 2 "src/main.fox"

#include <core.h>
#include <main.h>
#include <meta.h>
#include <cmdline.h>
#include <fox_meta.h>

map* fox_reflect();

int run(map* args){
	invoke=fox_invoke;
	reflect=fox_reflect;
	add(_globals,"reflect",reflect());
//	_globals.reflect.json(1).px()	

	if(args->len==1){
		px(xstr("", 
		"Fox: build: ", map_val(map_val(fox_reflect(),"version"),"build"), ", date: ", map_val(map_val(fox_reflect(),"version"),"date"), " [", time_ago(map_val(map_val(fox_reflect(),"version"),"date")), " old]\n", 
		"Usage: fox <function> [<arg1> <arg2> ...]\n", 
		"	Run \"fox help\" for list of functions\n", 
		"	Run \"fox tutorial\" for a tutorial", 
		"", End),1);
		return 0; };
	char* name=map_id(args,1);
	void* ret=NULL;
	if(str_end(name,".fox")){
		add(_globals,"args",vec_sub(map_val(_globals,"args"),1,0));
		ret=fox_run(file_read(name,1,1));
	}else if(!is_code(name) && !str_start(name,"--")){
		ret=fox_run(name);
	}else{
		if(str_start(name,"--")){ name=sub_str(name,2,-2147483648); };
		ret=invoke(cmdline_params(args,funcs()),name); };
	return ret_print(ret);
};
