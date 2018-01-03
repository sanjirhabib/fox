#include <libstemmer.h>
#include <fox.h>
enum {English, Bangla, Arabic};

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
int utf_lang(char* in){
	if(!in || !*in){ return English; };
	if(in[0]==-32){ return Bangla; };
	if(in[0]==-39||in[0]==-40){ return Arabic; };
	return English;
};
int is_letter(int code){
	if(code>='a' && code<='z' || code>='A' && code<='Z' || code>='0' && code<='9' || code>0x980 && code<0x9F7 || code>=0x620 && code<=0x669){
		return 1; };
	return 0;
};
map* str_stem(char* in){
	return words_stem(str_words(in));
};
map* words_stem(map* in){
	for(int next1=next(in,-1,NULL,NULL); has_id(in,next1); next1++){ void* val=map_id(in,next1); add(val,"stem",word_stem(map_val(val,"word"))); };	
	return in;
};
char* word_stem(char* in){
	int lang=utf_lang(in);
	if(lang==Bangla){ return bangla_norm(stem_bangla(in)); };
	if(lang==Arabic){ return stem_arabic(in); };
	return stem_english(in);
};
char* utf_word_end(char* in){
	while(*in){
		int code=utf_unicode(in);
		if(is_letter(code)){ in+=utf_len(in); continue; };
		if(code=='\'' && is_letter(utf_unicode((in+utf_len(in))))){ in+=utf_len((in+1))+1; continue; };
		break; };
	return in;
};
char* utf_word_start(char* in){
	while(*in && !is_letter(utf_unicode(in))){ in+=utf_len(in); };
	return in;
};
int* ucs_substr(int* in, int from, int upto){
	return sub_str(in,from*sizeof(int),upto*sizeof(int));
};
int ucs_len(int* in){
	return str_len(in)/sizeof(int);
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
map* str_words(char* in){
	char* start=in;
	map* ret=new_vec();
	if(!in){ return ret; };
	while(*in){
		char* head=utf_word_start(in);
		if(!*head){ break; };
		char* tail=utf_word_end(head);
		vec_add(ret,xmap(
			"word", sub_str(head,0, tail-head),
			"from",int_var( head-start),
			"len",int_var( tail-head
		), End));
		in=tail; };
	return ret;
};
char* stem_arabic(char* in){
	if(!map_val(_globals,"stemmer")){ add(_globals,"stemmer",sb_stemmer_new("arabic","UTF_8")); };
	return str_dup(sb_stemmer_stem(map_val(_globals,"stemmer"), in, str_len(in)));
};
char* stem_english(char* in){
	if(!map_val(_globals,"stemmer")){ add(_globals,"stemmer",sb_stemmer_new("english","ISO_8859_1")); };
	return str_dup(sb_stemmer_stem(map_val(_globals,"stemmer"), str_lower(in), str_len(in)));
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
"//",		".ে .",
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
		if(str_end(in,key) && !str_eq(sub_str(in,-str_len(key)-3,3),"্")){
			return xstr(sub_str(in,0, -str_len(key)),val, End); }; };
	return in;
};
int stem_test(){
	int errs=0;
	map* map_1=str_split(file_read("banglatest.txt",1,1),"\n",0); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* line=map_id(map_1,next1);
		map* mp=str_split(line," ",0);
		void* key=map_id(mp,0);
		void* val=map_id(mp,1);
		if(!str_eq(stem_bangla(key),val)){
			px(xstr(key, "!=", stem_bangla(key), " -- correct is ", val, End),1);
			errs++;
		}else{
			px(xstr("ok ", key, "=", stem_bangla(key), "\r", End),0); }; };
	px(NULL,1);
	px("done",1);
	return errs;
};
char* bangla_norm(char* in){
	return str_replace(in,xmap(
		"ী", "ি",
		"ূ", "ু",
		"ঁ",NULL,
		"শ", "স",
		"ষ", "স",
		"ণ", "ন",
		"ঙ", "ং",
		"ঃ",NULL,
		"ব়", "র",
		"ড়", "ড়",
		"য়", "য়",
		"ঢ়", "ঢ়",
		"্ব",NULL,
		"্রি", "ৃ",
		"্রী", "ৃ",
		"অা", "আ",
		"ৎ", "ত"
	, End),NULL);
};

fts5_tokenizer xtokenizer={0};
int xTokenize(Fts5Tokenizer* unused1, void *pCtx, int flags, const char *pText, int nText, void* callback){
	int (*xToken)(void *, int, const char *, int, int, int)=callback;
	map* map_1=words_stem(str_words(sub_str(pText,0,nText))); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1);
		xToken(pCtx, 0, map_val(val,"stem"), str_len(map_val(val,"stem")), is_int(map_val(val,"from")), is_int(map_val(val,"from"))+to_int(map_val(val,"len")));
		if(!str_eq(map_val(val,"stem"),map_val(val,"word"))){
			xToken(pCtx, 0, map_val(val,"word"), str_len(map_val(val,"word")), is_int(map_val(val,"from")), is_int(map_val(val,"from"))+to_int(map_val(val,"len"))); };
		int lang=utf_lang(map_val(val,"word"));
		if(lang==Bangla){
			map* phonetic=bangla_english(map_val(val,"word"));
			xToken(pCtx, FTS5_TOKEN_COLOCATED, phonetic, str_len(to_str(phonetic,"",0)), is_int(map_val(val,"from")), is_int(map_val(val,"from"))+to_int(map_val(val,"len"))); }; };
//			sound=phonetic.soundex()
//			xToken(pCtx, FTS5_TOKEN_COLOCATED, sound, sound.str_len(), val.from.is_int(), val.from.is_int()+val.len.to_int())
//		else if lang==English
//			sound=val.word.soundex()
//			xToken(pCtx, FTS5_TOKEN_COLOCATED, sound, sound.str_len(), val.from.is_int(), val.from.is_int()+val.len.to_int())
		
	return SQLITE_OK;
};
int xCreate(void* unused, const char **azArg, int nArg, Fts5Tokenizer **ppOut){
	*ppOut=1;
	return SQLITE_OK;
};
static int xDelete(Fts5Tokenizer* unused){
	return SQLITE_OK;
};

char* db_init_tokenizer(char* db){
	void* conn=lite_conn(db);
	fts5_api *pRet=NULL;
	sqlite3_stmt *pStmt=NULL;
	if( SQLITE_OK==sqlite3_prepare(conn, "SELECT fts5(?1)", -1, &pStmt, 0) ){
		sqlite3_bind_pointer(pStmt, 1, (void*)&pRet, "fts5_api_ptr", NULL);
		sqlite3_step(pStmt);
	};
	sqlite3_finalize(pStmt);
	xtokenizer.xCreate=xCreate;
	xtokenizer.xDelete=xDelete;
	xtokenizer.xTokenize=xTokenize;
	pRet->xCreateTokenizer(pRet, "habib", NULL, &xtokenizer, NULL);
	return db;
};
map* bangla_joins(){
	return str_split("ক্ক ক্ট ক্ট্র ক্ত ক্ত্র ক্ব ক্ম ক্য ক্র ক্ল ক্ষ ক্ষ্ণ ক্ষ্ব ক্ষ্ম ক্ষ্ম্য ক্ষ্য ক্স খ্য খ্র গ্‌ণ গ্ধ গ্ধ্য গ্ধ্র গ্ন গ্ন্য গ্ব গ্ম গ্য গ্র গ্র্য গ্ল ঘ্ন ঘ্য ঘ্র ঙ্ক ঙ্‌ক্ত ঙ্ক্য ঙ্ক্ষ ঙ্খ ঙ্গ ঙ্গ্য ঙ্ঘ ঙ্ঘ্য ঙ্ঘ্র ঙ্ম চ্চ চ্ছ চ্ছ্ব চ্ছ্র চ্ঞ চ্ব চ্য জ্জ জ্জ্ব জ্ঝ জ্ঞ জ্ব জ্য জ্র ঞ্চ ঞ্ছ ঞ্জ ঞ্ঝ ট্ট ট্ব ট্ম ট্য ট্র ড্ড ড্ব ড্য ড্র ড়্গ ঢ্য ঢ্র ণ্ট ণ্ঠ ণ্ঠ্য ণ্ড ণ্ড্য ণ্ড্র ণ্ঢ ণ্ণ ণ্ব ণ্ম ণ্য ৎক ত্ত ত্ত্ব ত্ত্য ত্থ ত্ন ত্ব ত্ম ত্ম্য ত্য ত্র ত্র্য ৎল ৎস থ্ব থ্য থ্র দ্গ দ্ঘ দ্দ দ্দ্ব দ্ধ দ্ব দ্ভ দ্ভ্র দ্ম দ্য দ্র দ্র্য ধ্ন ধ্ব ধ্ম ধ্য ধ্র ন্ট ন্ট্র ন্ঠ ন্ড ন্ড্র ন্ত ন্ত্ব ন্ত্য ন্ত্র ন্ত্র্য ন্থ ন্থ্র ন্দ ন্দ্য ন্দ্ব ন্দ্র ন্ধ ন্ধ্য ন্ধ্র ন্ন ন্ব ন্ম ন্য প্ট প্ত প্ন প্প প্য প্র প্র্য প্ল প্স ফ্র ফ্ল ব্জ ব্দ ব্ধ ব্ব ব্য ব্র ব্ল ভ্ব ভ্য ভ্র ম্ন ম্প ম্প্র ম্ফ ম্ব ম্ব্র ম্ভ ম্ভ্র ম্ম ম্য ম্র ম্ল য্য র্ক র্ক্য র্গ্য র্ঘ্য র্চ্য র্জ্য র্ণ্য র্ত্য র্থ্য র্ব্য র্ম্য র্শ্য র্ষ্য র্হ্য র্খ র্গ র্গ্র র্ঘ র্চ র্ছ র্জ র্ঝ র্ট র্ড র্ণ র্ত র্ত্র র্থ র্দ র্দ্ব র্দ্র র্ধ র্ধ্ব র্ন র্প র্ফ র্ভ র্ম র্য র্ল র্শ র্শ্ব র্ষ র্স র্হ র্ঢ্য ল্ক ল্ক্য ল্গ ল্ট ল্ড ল্প ল্‌ফ ল্ব ল্‌ভ ল্ম ল্য ল্ল শ্চ শ্ছ শ্ন শ্ব শ্ম শ্য শ্র শ্ল ষ্ক ষ্ক্র ষ্ট ষ্ট্য ষ্ট্র ষ্ঠ ষ্ঠ্য ষ্ণ ষ্প ষ্প্র ষ্ফ ষ্ব ষ্ম ষ্য স্ক স্ক্র স্খ স্ট স্ট্র স্ত স্ত্ব স্ত্য স্ত্র স্থ স্থ্য স্ন স্প স্প্র স্প্‌ল স্ফ স্ব স্ম স্য স্র স্ল হ্ণ হ্ন হ্ব হ্ম হ্য হ্র হ্ল হৃ"," ",0);
};
map* bangla_english(char* in){
	map* letters=xmap(	
		"ক", "k",
		"খ", "kh",
		"গ", "g",
		"ঘ", "gh",
		"ঙ", "ng",
		"চ", "c",
		"ছ", "ch",
		"জ", "j",
		"ঝ", "jh",
		"ট", "t",
		"ঠ", "th",
		"ড", "d",
		"ঢ", "dh",
		"ণ", "n",
		"ত", "t",
		"থ", "th",
		"দ", "d",
		"ধ", "dh",
		"ন", "n",
		"প", "p",
		"ফ", "f",
		"ব", "b",
		"ভ", "v",
		"ম", "m",
		"য", "j",
		"র", "r",
		"ল", "l",
		"শ", "sh",
		"ষ", "sh",
		"স", "sh",
		"হ", "h",
		"ড়", "r",
		"ঢ়", "r",
		"য়", "y",
		"ৎ",   "t",
		"ং",   "ng",
		"া", "a",
		"ি", "i",
		"ী", "i",
		"ু", "u",
		"ূ", "u",
		"ৃ", "ri",
		"ে", "e",
		"ৈ", "oi",
		"ো", "o",
		"ৌ", "ou",
		"অ", "a",
		"আ", "a",
		"ই", "i",
		"ঈ", "i",
		"উ", "u",
		"ঊ", "u",
		"ঋ", "r",
		"এ", "e",
		"ঐ", "oi",
		"ও", "o",
		"ঔ", "ou",
		"১", "1",
		"২", "2",
		"৩", "3",
		"৪", "4",
		"৫", "5",
		"৬", "6",
		"৭", "7",
		"৮", "8",
		"৯", "9",
		"০", "0"

	, End);
	char letter[4];
	char* ret=NULL;
	while(utf_letter(in,letter)){
		ret=xcat(ret,map_val(letters,letter), End);
		in+=strlen(letter); };
	return ret;
};
char* soundex(char *s) {
	static const char *cls[] = { "AEIOU", "", "BFPV", "CGJKQSXZ", "DT", "L", "MN", "R", 0};
	static char code[128] = {0};
	if(!code[80]){
		for(int i = 0; cls[i]; i++){
			char* s=cls[i];
			int c=i-1;
			while(*s){
				code[(int)*s] = code[0x20 ^ (int)*s] = c;
				s++; }; };
	};
	static char out[5];
	int c;
	int prev;
	int i;

	out[0] = out[4] = 0;
	if(!s || !*s){ return NULL; };

	out[0] = *s++;

	prev = code[(int)out[0]];
	for(i = 1; *s && i < 4; s++){
		if((c = code[(int)*s]) == prev){ continue; };

		if(c == -1){ prev = 0; }
		else if(c > 0){
			out[i++] = c + '0';
			prev = c; }; };
	while(i < 4){ out[i++] = '0'; };
	return str_dup(out);
};
char* num_lang(char* in,int langid){
	if(!in || !*in){ return NULL; };
	char buff[5]={0};
	int points[]={ 48, 2534, 1632 };
	char* ret=NULL;
	int code=0;
	while((code=utf_unicode(in))){
		in+=utf_len(in);
		for(int i=0; i<3; i++){
			if(i!=langid && code>=points[i] && code<points[i]+10){
				code-=(points[i]-points[langid]);
				break; }; };
		ret=xcat(ret,unicode_utf(code,buff), End); };
	return ret;
};
