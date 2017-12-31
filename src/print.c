#include <fox.h>

char* outx(char* str){
	px(str,1);
	xexit(0);
	return NULL;
};
char* flush_out(){ char* ret=map_val(_globals,"out"); add(_globals,"out",NULL); return ret; };
