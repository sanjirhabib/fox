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
	fputs("ERROR: ",stderr);
	fputs(msg,stderr);
	fputs("\n",stderr);
	if(dump){ fputs(stack_str(),stderr); };
	xexit(-1);
	return NULL;
};
void* call_php(map* params,char* func){ return "DUMMY"; };
