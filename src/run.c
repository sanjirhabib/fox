#include <fox.h>
int run(map* args){
	add(add_key(_globals,"cache",Map),"reflect",reflect());
	if(args->len==1){
		px(version(),1);
		px(""
		"Usage: fox <function> [<arg1> <arg2> ...]\n"
		"	Run \"fox help\" for list of functions\n"
		"	Run \"fox tutorial\" for a tutorial"
		"",1);
		return 0;
	};
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
