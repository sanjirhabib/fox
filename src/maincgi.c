#include <fox.h>
int run(map* req);
int main(int argc,char** argv){
	gc_start();
	argv_map(argv,argc);
	return run(http_req());
};