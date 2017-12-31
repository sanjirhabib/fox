#include <fox.h>

char* utf_letter(char* in, char* out){
	int len=utf_len(in);
	if(!len){ return NULL; };
	out[len]=0;
	return memcpy(out,in,len);
};
int utf_strlen(char* in){
	int i=0;
	int j=0;
	while(in[i]){
		if((in[i] & 0xc0) != 0x80){ j++; };
		i++; };
	return j;
};
int unicode_len(int ucs2){
	if(ucs2 < 0x80){
		return 1; };
	if(ucs2 >= 0x80  && ucs2 < 0x800){
		return 2; };
	if(ucs2 >= 0x800 && ucs2 < 0xFFFF){
		return 3; };
	if(ucs2 >= 0x10000 && ucs2 < 0x10FFFF){
		return 4; };
	return 1;
};
char* unicode_utf(int ucs2,char* ret){
//	if !ucs2
//		ret[0]='\0'
//		return ret
	if(ucs2 < 0x80){
		ret[0] = ucs2;
		ret[1] = '\0';
		return ret; };
	if(ucs2 >= 0x80  && ucs2 < 0x800){
		ret[0] = (ucs2 >> 6)   | 0xC0;
		ret[1] = (ucs2 & 0x3F) | 0x80;
		ret[2] = '\0';
		return ret; };
	if(ucs2 >= 0x800 && ucs2 < 0xFFFF){
		if(ucs2 >= 0xD800 && ucs2 <= 0xDFFF){ return 0; };
		ret[0] = ((ucs2 >> 12)	   ) | 0xE0;
		ret[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
		ret[2] = ((ucs2	  ) & 0x3F) | 0x80;
		ret[3] = '\0';
		return ret; };
	if(ucs2 >= 0x10000 && ucs2 < 0x10FFFF){
		ret[0] = 0xF0 | (ucs2 >> 18);
		ret[1] = 0x80 | ((ucs2 >> 12) & 0x3F);
		ret[2] = 0x80 | ((ucs2 >> 6) & 0x3F);
		ret[3] = 0x80 | (ucs2 & 0x3F);
		ret[4] = '\0';
		return ret; };
	ret[0] = '\0';
	return ret;
};
int utf_len(char* in){
	unsigned char* input=(unsigned char*) in;
	if(!input[0]){ return 0; };
	if(input[0] < 0x80){ return 1; };
	if((input[0] & 0xE0) == 0xE0){ return 3; };
	if((input[0] & 0xC0) == 0xC0){ return 2; };
	return 1;
};
int utf_unicode(char* ptr){
	unsigned char* input=(unsigned char*)ptr;
	if(!input[0]){ return 0; };
	if(input[0] < 0x80){ return input[0]; };
	if((input[0] & 0xE0) == 0xE0){
//Error checks commented
		if(input[1] < 0x80 || input[1] > 0xBF || input[2] < 0x80 || input[2] > 0xBF){ return 0; };
		return (input[0] & 0x0F)<<12 | (input[1] & 0x3F)<<6  | (input[2] & 0x3F); };
	if((input[0] & 0xC0) == 0xC0){
		if(input[1] < 0x80 || input[1] > 0xBF){ return 0; };
		return (input[0] & 0x1F)<<6  | (input[1] & 0x3F); };
	return '?';
};
