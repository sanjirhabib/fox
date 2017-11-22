#include <sqlite3.h>
#include <fox.h>
#include <foxcmd.h>
#include <http.h>
#include <sql.h>
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
