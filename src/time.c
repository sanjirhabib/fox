#include <fox.h>

char* time_ago(char* stime){
	int secs=time(0)-str_time(stime);
	if(secs < 0){ secs*=-1; };
	if(secs<120) {return "a minute";};
	secs/=60;
	if(secs<60) {return xstr(int_str(secs), " minutes", End);};
	secs/=60;
	if(secs<2) {return "an hour";};
	if(secs<24) {return xstr(int_str(secs), " hours", End);};
	secs/=24;
	if(secs<2) {return "a day";};
	if(secs<30) {return xstr(int_str(secs), " days", End);};
	if(secs<60) {return "a month";};
	if(secs<365) {return xstr(int_str(secs/30), " months", End);};
	secs/=365;
	if(secs<2) {return "a year";};
	return xstr(int_str(secs), " years", End);
};
time_t str_time(char* in){
	if(!in){ return time(0); };
	struct tm tm={0};
	tm.tm_year=atoi(sub_str(in,0,4))-1900;
	tm.tm_mon=atoi(sub_str(in,5,2))-1;
	tm.tm_mday=atoi(sub_str(in,8,2));
	if(str_len(in)==19){
		tm.tm_hour=atoi(sub_str(in,11,2));
		tm.tm_min=atoi(sub_str(in,14,2));
		tm.tm_sec=atoi(sub_str(in,17,2)); };
	return mktime(&tm);
};
char* time_str(time_t timer){
	char buffer[20];
	if(!timer){ timer=time(0); };
	strftime(buffer,20, "%Y-%m-%d %H:%M:%S",localtime(&timer));
	return str_dup(buffer);
};
char* today(){ return sub_str(time_str(0),0,10); };
char* now(){ return sub_str(time_str(0),11,8); };
int this_year(){ return str_int(sub_str(time_str(0),0,4)); };
char* datetime(){ return time_str(0); };
i;
