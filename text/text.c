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
	return str_len(in)/sizeof(int);
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
		"েরগুলোর",NULL,
		"েরগুলো",NULL,
		"গুলোর",NULL,
		"গুলো",NULL,
		"েয়েছিলেন", "া",
		".েছিলেন", ".",
		"েয়েছিল", "া",
		"েছিলেন",NULL,
		"য়েছিল",NULL,
		"েছিল",NULL,
		"চ্ছিলেন",NULL,
		"েরটা",NULL,
		"েরটার",NULL,
		"রছ", "র",
		"রব", "র",
		"েল",NULL,
		"েলো",NULL,
		"ওয়া",NULL,
		"েয়ে", "া",
		"চ্ছিল",NULL,
		"চ্ছিস",NULL,
		"েছেন",NULL,
		"য়েছে",NULL,
		"বেন",NULL,
		"লেন",NULL,
		"দের",NULL,
		"ছিস",NULL,
		"ছিলি",NULL,
		"টির",NULL,
		"টার",NULL,
		"চ্ছি",NULL,
		"চ্ছে",NULL,
		"চ্ছ",NULL,
		"েছ",NULL,
		"েছে",NULL,
		"ের",NULL,
		"ার",NULL,
		"েন",NULL,
		"তো",NULL,
		"কে",NULL,
		"তে",NULL,
		"রা",NULL,
		"িস",NULL,
		"ছি",NULL,
		"ছে",NULL,
		"লি",NULL,
		"বি",NULL,
		"টি",NULL,
		"টা",NULL,
		"য়ে",NULL,
		".ে", ".",
		"ে",NULL,
		"ই",NULL,
		"য়",NULL,
		"ও",NULL

	, End); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char*  key=map_key(map_1, next1);
		if(str_eq(val,".")){
			key=key+1;
			if(str_end(in,key) && str_eq(sub_str(in,-str_len(key)-6,3),"ে")){
				return xstr(sub_str(in,0,-str_len(key)-6),"া",sub_str(in,-str_len(key)-3,3), End); };
			continue; };
		if(str_end(in,key)){
			return xstr(sub_str(in,0, -str_len(key)),val, End); }; };
	return in;
};
int stem_test(){
	int errs=0;
	map* map_1=xmap(
		"এটাই", "এটা",
		"সেটাই", "সেটা",
		"এটাও", "এটা",
		"সেটাও", "সেটা",
		"হয়তো", "হয়",
		"করলতো", "করল",
		"এটাকে", "এটা",
		"আমাকে", "আমা",
		"হাসতে", "হাস",
		"গাইতে", "গাই",
		"রহিমরা", "রহিম",
		"করিমরা", "করিম",
		"খাচ্ছি", "খা",
		"যাচ্ছি", "যা",
		"হচ্ছিল", "হ",
		"হচ্ছে", "হ",
		"যাচ্ছে", "যা",
		"যাচ্ছিস", "যা",
		"খাচ্ছিস", "খা",
		"যাচ্ছিলেন", "যা",
		"খাচ্ছিলেন", "খা",
		"যাচ্ছ", "যা",
		"খাচ্ছ", "খা",
		"হয়েছে", "হ",
		"খেয়েছে", "খে",
		"দেখেছ", "দেখ",
		"পেরেছ", "পের",
		"করেছে", "কর",
		"পেরেছে", "পের",
		"করেছেন", "কর",
		"পেরেছেন", "পের",
		"করছ", "কর",
		"পারছ", "পার",
		"করব", "কর",
		"পারব", "পার",
		"গেল", "গ",
		"পেল", "প",
		"খেলো", "খ",
		"যাওয়া", "যা",
		"খাওয়া", "খা",
		"গেয়ে", "গা",
		"পেয়ে", "পা",
		"হয়", "হ",
		"খায়", "খা",
		"হয়ে", "হ",
		"ধুয়ে", "ধু",
		"হয়েছিল", "হ",
		"পেয়েছিল", "পা",
		"গেয়েছিল", "গা",
		"করেছিল", "কর",
		"বলেছিল", "বল",
		"পেয়েছিলেন", "পা",
		"গেয়েছিলেন", "গা",
		"হেসেছিলেন", "হাস",
		"নেচেছিলেন", "নাচ",
		"করেছিলেন", "কর",
		"বলেছিলেন", "বল",
		"করলেন", "কর",
		"বললেন", "বল",
		"রহিমদের", "রহিম",
		"করিমদের", "করিম",
		"হেসে", "হাস",
		"নেচে", "নাচ",
		"রেখে", "রাখ",
		"রহিমের", "রহিম",
		"করিমের", "করিম",
		"চলার", "চল",
		"বলার", "বল",
		"দেখেন", "দেখ",
		"করেন", "কর",
		"দেখবেন", "দেখ",
		"করবেন", "কর",
		"দেখিস", "দেখ",
		"করিস", "কর",
		"দেখছিস", "দেখ",
		"করছিস", "কর",
		"দেখছিলি", "দেখ",
		"করছিলি", "কর",
		"করছি", "কর",
		"ভাবছি", "ভাব",
		"করছে", "কর",
		"পারছে", "পার",
		"দেখলি", "দেখ",
		"করলি", "কর",
		"দেখবি", "দেখ",
		"করবি", "কর",
		"বলে", "বল",
		"বসে", "বস",
		"একটি", "এক",
		"দশটি", "দশ",
		"একটির", "এক",
		"দশটির", "দশ",
		"রহিমেরটা", "রহিম",
		"করিমেরটা", "করিম",
		"রহিমেরটার", "রহিম",
		"করিমেরটার", "করিম",
		"এটা", "এ",
		"ওটা", "ও",
		"এটার", "এ",
		"ওটার", "ও",
		"এগুলো", "এ",
		"তারগুলো", "তার",
		"এগুলোর", "এ",
		"তারগুলোর", "তার",
		"রহিমেরগুলো", "রহিম",
		"করিমেরগুলো", "করিম",
		"রহিমেরগুলোর", "রহিম",
		"করিমেরগুলোর", "করিম"
	, End); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char*  key=map_key(map_1, next1);
		if(!str_eq(stem_bangla(key),val)){
			px(xstr(key, "!=", stem_bangla(key), " -- correct is ", val, End),1);
			errs++; }; };
	return errs;
};
