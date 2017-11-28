#include "hello.h"

int main(int argc, char** argv, char** env){ gc_start(); map* args=argv_map(argv, argc);
	px(xstr("", 
	"Total ",int_str( map_len(args)), " command line arguments\n", 
	"those are ", args, 
	"", End),1);	

	char* name="Habib";
	char* msg=xstr("hello, ", name, "!", End);
	px(msg,1);

	map* myvector=xvec("hello", "world", End);
	px(myvector,1);

	map* mymap=xmap("name", "Habib", "age",int_var( 23), End);
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
