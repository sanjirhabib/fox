#include "fox.h"

int run_cgi(map* args, map* req);
int main(int argc,char** argv){
	gc_start();
	return run_cgi(argv_map(argv,argc),http_req());
};
