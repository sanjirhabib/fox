#include <fox.h>

int main(int argc,char** argv){
	gc_start();

	char* name="Habib";
	char* msg=xstr("hello, ", name, "!", End);
	px(msg,1);

	map* myvector=xvec("hello", "world", End);
	px(myvector,1);

	void* mymap=px(xmap("name", "Habib", "age",int_var( 23), End),1);
	add(mymap,"name","Ibrahim");
	px(mymap,1);

	px(xstr("", 
	"Drawing a box, ", name, "!\n", 
	"|---------|\n", 
	"|         |\n", 
	"|         |\n", 
	"|---------|", 
	"", End),1);

	return 0;
};
