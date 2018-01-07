#include <fox.h>
#include <regex.h>
map* regexp(char* in, char* pattern){
	int status=0;
	regex_t	re={0};
	regmatch_t match[10]={0};
	if(regcomp(&re, pattern, REG_EXTENDED)){ return NULL; };
	if(regexec(&re, in, 10, match, 0)){ regfree(&re); return NULL; };
	map* ret=new_vec();
	for(int i=0; i<10; i++){
		if(match[i].rm_so<0){ break; };
		vec_add(ret,sub_str(in,match[i].rm_so, match[i].rm_eo-match[i].rm_so)); };
	regfree(&re);
	return ret;
};
