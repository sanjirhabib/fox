#include <fox.h>
int run(map* args);
int main(int argc,char** argv){
	map* globals=NULL;
	return run(argv_map(argv,argc,&globals));
};
