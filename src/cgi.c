#include <fox.h>
extern int _iscmd;
__attribute__((destructor)) static void maindtor(void){
	close_conns();
	if(!_iscmd){
		http_out(NULL,"200 OK","text/html; charset=utf-8",NULL); };
	gc_end();
};
void* px(void* str,int newline){
	void* ret=str;
	str=to_str(str,"",0);
	if(!str_len(str) && !newline){ return ret; };
	if(_iscmd){
		print(str,stdout);
		if(newline){ print("\n",stdout); };
	}else{
		add(_globals,"out",xcat(map_val(_globals,"out"),str, End)); };
	return ret;
};
void xexit(int val){
	if(!_iscmd){
		http_out(NULL,"200 OK","text/html; charset=utf-8",NULL); };
	gc_end();
	exit(val);
};
void* fox_error(char* msg,int dump){
	if(!_iscmd){
		http_error(xstr("", 
		msg, "\n", 
		stack_str(), 
		"", End),"500 Internal Server Error");
	}else{
		fputs(msg,stderr);
		fputs(stack_str(),stderr);
		xexit(-1); };
	return NULL;
};
void* call_php(map* params,char* func){ return "DUMMY"; };
