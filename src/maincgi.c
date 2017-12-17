#include <fox.h>
int run(map* req);
int main(int argc,char** argv){
	map* globals=NULL;
	argv_map(argv,argc,&globals);
	return run(http_req());
};
