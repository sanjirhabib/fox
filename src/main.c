#include "fox.h"

int main(int argc,char** argv,char** env){
	gc_start();
	return run(argv_map(argv,argc));
};
