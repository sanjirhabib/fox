#include <fox.h>

char* outx(char* str){
	px(str,1);
	xexit(0);
	return NULL;
};
char* flush_out(){ char* ret=map_val(_globals,"out"); add(_globals,"out",NULL); return ret; };
int ret_print(void* ret){
	if(_printed){ return is_i(ret) ? is_int(ret) : 0; };
	if(is_map(ret)){ px(json(ret,1),1); return 0; };
	if(is_blob(ret)){ print(ret,stdout); }
	else if(is_str(ret) || is_num(ret)){ px(to_str(ret,"",0),1); };
	return is_i(ret) ? is_int(ret) : 0;
};
