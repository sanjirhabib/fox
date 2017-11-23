#include <fox.h>
#include <foxcmd.h>

int main(int argc,char** argv){
	gc_start();
	px("hello, world",1);
	px(xvec("hello", "world", End),1);
	void* data=px(xmap("name", "Habib", "age",int_var( 23), End),1);
	add(data,"name","Ibrahim");
	px(data,1);
	map* map_1=xvec("hi","there", End); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* v=map_id(map_1,next1); px(v,1); };
	return 0;
};
