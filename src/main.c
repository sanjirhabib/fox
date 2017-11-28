#include "fox.h"

int main(int argc,char** argv,char** env){
	gc_start();
	return run_cmdline(argv_map(argv,argc));
};
