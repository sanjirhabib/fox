#include <fox.h>

int main(int argc,char** argv){
	gc_start();
	px("hello, world",1);
	px(xvec("hello", "world", End),1);
	void* data=px(xmap("name", "Habib", "age",int_var( 23), End),1);
	add(data,"name","Ibrahim");
	px(data,1);
	return 0;
};
