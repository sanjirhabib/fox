#include <fox.h>
int run(map* args);
int main(int argc,char** argv){
	gc_start();
	return run(argv_map(argv,argc));
};