#include "text.h"
int run(map* args){
	if(exec_cmdline(args)){ return 0; };
	void* in=map_id(args,1);
	px(int_var(sizeof(unsigned short)),1);
	return 0;
};
char* ucs_str(int* in,int len){
	if(!len){
		while(in[len++]) {}; };
	int slen=0;
	int i=0;
	while(i<len){
		slen+=unicode_len(in[i]);
		i++; };
	char* ret=new_str(slen);
	char* ptr=ret;
	i=0;
	while(i<len){
		unicode_utf(in[i],ptr);
		ptr+=str_len(ptr);
		i++; };
	return ret;
};
int* str_ucs(char* in){
	int len=str_len(in);
	int* ret=new_blob(((utf_strlen(in)+1)*sizeof(int)));
	int code=0;
	int i=0;
	while((code=utf_unicode(in))){
		ret[i++]=code;
		in+=utf_len(in); };
	ret[i]=0;
	return ret;
};
int is_letter(int code){
	if(code>='a' && code<='z' || code>='A' && code<='Z' || code>='0' && code<='9' || code>0x0980 && code<0x09F7){
		return 1; };
	return 0;
};
int* word_end(int* in){
	while(*in){
		if(is_letter((*in))){ in++; continue; };
		if(*in=='\'' && is_letter(in[1])){ in++; continue; };
		break; };
	return in;
};
int* word_start(int* in){
	while(*in && !is_letter((*in))){ in++; };
	return in;
};
int* ucs_substr(int* in, int from, int upto){
	return sub_str(in,from*sizeof(int),upto*sizeof(int));
};
int ucs_len(int* in){
	return str_len(in)/sizeof(4);
};
map* ucs_words(int* uin){
	map* ret=new_vec();
	while(*uin){
		int* head=word_start(uin);
		if(!*head){ break; };
		int* tail=word_end(head);
		vec_add(ret,ucs_substr(head,0, tail-head));
		uin=tail; };
	return ret;
};
map* str_words(char* in){
	int* uin=str_ucs(in);
	map* ret=new_vec();
	while(*uin){
		int* head=word_start(uin);
		if(!*head){ break; };
		int* tail=word_end(head);
		vec_add(ret,ucs_str(head,tail-head));
		uin=tail; };
	return ret;
};
void go(char* in){
	int* codes=str_ucs(in);
	int i=0;
	while(codes[i++]){
		px(int_var(codes[i-1]),1); };
	px(ucs_str(codes,0),1);
};
char* stem_bangla(char* in){
	map* map_1=xmap(
		"ই",NULL,
		"ও",NULL,
		"তো",NULL,
		"কে",NULL,
		"তে",NULL,
		"রা",NULL,
		"চ্ছি",NULL,
		"চ্ছিল",NULL,
		"চ্ছে",NULL,
		"চ্ছিস",NULL,
		"চ্ছিলেন",NULL,
		"চ্ছ",NULL,
		"য়েছে",NULL,
		"ছ",NULL,
		"ছে",NULL,
		"ছেন",NULL,
		"রছ", "র",
		"রব", "র",
		"ল",NULL,
		"লো",NULL,
		"ওয়া",NULL,
		"য়ে",  NULL,
		"য়",NULL,
		"য়ে",NULL,
		"য়েছিল",NULL,
		"য়েছিল",  NULL,
		"ছিল",NULL,
		"য়েছিলেন", NULL,
		".েছিলেন",  "া.",
		"ছিলেন",NULL,
		"লেন",NULL,
		"দের",NULL,
		".ে", ".",
		"র",NULL,
		"র",NULL,
		"ন",NULL,
		"বেন",NULL,
		"স",NULL,
		"ছিস",NULL,
		"ছিলি",NULL,
		"ছি",NULL,
		"ছে",NULL,
		"লি",NULL,
		"বি",NULL,
		"টি",NULL,
		"টির",NULL,
		"রটা",NULL,
		"রটার",NULL,
		"টা",NULL,
		"টার",NULL,
		"গুলো",NULL,
		"গুলোর",NULL,
		"রগুলো",NULL,
		"রগুলোর",NULL
	, End); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char*  key=map_key(map_1, next1);
		if(str_end(in,key)){
			return xstr(sub_str(in,0, -str_len(key)),val, End); }; };
	return in;
};
