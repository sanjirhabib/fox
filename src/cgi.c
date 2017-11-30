#include <fox.h>
__attribute__((destructor)) static void maindtor(void){ http_out(map_val(_globals,"out"),"200 OK","text/html; charset=utf-8",NULL); gc_end(); };
void* px(void* str,int newline){
	void* ret=str;
	str=to_str(str,"",0);
	if(!str_len(str) && !newline){ return ret; };
	add(_globals,"out",xcat(map_val(_globals,"out"),str, End));
	return ret;
};
void xexit(int val){
	http_out(NULL,"200 OK","text/html; charset=utf-8",NULL);
	gc_end();
	exit(val);
};
void* fox_error(char* msg,int dump){
	http_error(xstr("", 
	msg, "\n", 
	"System Stack Dump\n", 
	str_replace(stack_str(),"\n","<br/>"), "\n", 
	"Eval Stack Dump", 
	"", End),"500 Internal Server Error");
	return NULL;
};
void* call_php(map* params,char* func){ return "DUMMY"; };
