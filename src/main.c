#include "fox.h"
#include "foxcmd.h"
int main(int argc,char** argv,char** env){
	gc_start();
	add(add_key(_globals,"cache",Map),"reflect",reflect());
	add(_globals,"args",ptrs_vec((void**)argv,argc));
	return run_cmdline(map_val(_globals,"args"));
};
