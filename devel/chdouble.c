#include <fox.h>

int run(map* args){
	px(map_id(args,1),1);
	px(double_str(str_double(map_id(args,1))),1);
	double ret=str_double(map_id(args,1));
	sscanf(map_id(args,1),"%lf",&ret);
	printf("%f\n",str_double(map_id(args,1)));
	return ret;
};
