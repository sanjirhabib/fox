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
int main(int argc,char** argv){
	gc_start();
	return run(argv_map(argv,argc));
};
void xexit(int val){
	gc_end();
	exit(val);
};
void* fox_error(char* msg,int dump){
	fputs("ERROR: ",stderr);
	fputs(msg,stderr);
	//"\n_________________________________________ [ERROR]\n".fputs(stderr)
	fputs("\n",stderr);
	if(dump){ fputs(stack_str(),stderr); };
	xexit(-1);
	return NULL;
};
void* call_php(map* params,char* func){ return "DUMMY"; };
