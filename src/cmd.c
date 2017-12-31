#include <fox.h>

__attribute__((destructor)) static void maindtor(void){ gc_end(); };
void* px(void* str,int newline){
	void* ret=str;
	str=to_str(str,"",0);
	if(!str_len(str) && !newline){ return ret; };
	print(str,stdout);
	if(newline){ print("\n",stdout); };
	return ret;
};
void xexit(int val){
	gc_end();
	exit(val);
};
void* fox_error(char* msg,int dump){
	fprintf(stderr,"%s",msg);
	if(dump){ fprintf(stderr,"%s",stack_str()); };
	xexit(-1);
	return NULL;
};
void* call_php(map* params,char* func){ return "DUMMY"; };
