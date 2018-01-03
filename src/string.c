#include <fox.h>

char* mstr(char* format, ...){
	char* ret=NULL;
	va_list args;
	va_start(args,format);
	ret=va_str(format,args);
	va_end(args);
	return ret;
};
char* cat_char(char* str, char c){
	if(!str){ return char_str(c); };
	int len=str_len(str); //lima
	str=fox_realloc(str,len+2,String);	
	str[len]=c;
	assert(!str[len+1]);
	return str;
};
char* va_str(char* format,va_list args){
	if(!format){ return NULL; };
	va_list copy;
	va_copy(copy,args);
	int len=vsnprintf(NULL,0,format,args);
	char* ret=new_str(len);
	vsnprintf(ret,len+1,format,copy);
	va_end(copy);
	return ret;
};
void* dx(void* data,char* name,int panic){
	if(name){
		px(xstr(name, "=", End),0); };
	px(to_str(data,"NULL",1),1);
	if(panic==1){ xexit(0); }
	else if(panic==2){ fox_stack_dump(); };
	return data;
};
int neq(char* str,char* str1){ return !str_eq(str,str1); };
int eq(char* str,char* str1){ return !str||!str1||!is_str(str) ? 0 : strcmp(str,str1)==0; };
char* cwd(){
	char* ret=getcwd(NULL,0);
	char* ret2=str_dup(ret);
	free(ret);
	return ret2;
};
char* str_unquote(char* str){
	if(!str||!*str){ return NULL; };
	if(str_eq(str,"\"\"") || str_eq(str,"''")){ return ""; };
	if(*str=='\'' || *str=='"'){ str=sub_str(str,1,-1); };
	if(!str_chr(str,'\\')){ return str; };
	char* temp=str_dup(str);
	char* ret=temp;
	while(*str){
		int other=0;
		if(*str=='\\'){
			str++;
			if(!*str){ break; };
			switch(*str){
				case 'n': *temp='\n'; break;
				case 'r': *temp='\r'; break;
				case 't': *temp='\t'; break;
				case '0': *temp='\0'; break;
				default: other=1; break; };
			other=!other; };
		if(!other){ *temp=*str; };
		temp++;
		str++; };
	*temp=0;
	return ret;
};
char* str_escape(char* head){
	if(!head){ return ""; };
	int len=str_len(head);
	assert(len<4000);
	int extra=0;
	char c;
	char* str=head;
	char* quotable="\\\"";
	char* quoteto="\\\"";
	while((c=*str++)) {if(strchr(quotable,c)){ extra++; };};
	assert(extra<=len);
	char* ret=new_str(str_len(head)+extra);
	str=head;
	int i=0;
	char* fox_at=NULL;
	while((c=*str++)){
		if(!(fox_at=strchr(quotable,c))){ ret[i++]=c; continue; };
		ret[i++]='\\';
		ret[i++]=quoteto[fox_at-quotable];
		assert(i<len+extra+2);
	};
	assert(!ret[i]);
	return ret;
};
char* str_end(char* str,char* end){
	if(str_len(str)<str_len(end)){ return NULL; };
	return strcmp(str+str_len(str)-str_len(end),end)==0 ? str : NULL;
};
int str_starti(char* str,char* start){
	if(!str || !start || !*str || !*start){ return 0; };
	for(int i=0; i<strlen(start); i++){
	while(*str && *start){
		if(*str!=*start){
			char c=*str; 
			if(c>='A' && c<='Z'){ c+='a'-'A'; };
			if(c!=*start){ return 0; }; };
		start++;
		str++; };
	if(*start){ return 0; };
	return 1;
 };
	while(*str && *start){
		if(*str!=*start){
			char c=*str; 
			if(c>='A' && c<='Z'){ c+='a'-'A'; };
			if(c!=*start){ return 0; }; };
		start++;
		str++; };
	if(*start){ return 0; };
	return 1;
};
int str_start(char* str,char* start){ return str && start && is_str(str) && strncmp(str,start,strlen(start))==0; };
char* str_chr(char* str,char c){ return !str||!c||!is_str(str) ? NULL : strchr(str,c); };
int str_chrs(char* str,char* chars){
	for(int i=0; i<strlen(chars); i++){
		if(str_chr(str,chars[i])){ return 1; }; };
	return 0;
};
char* str_ltrim(char* str,char* chars){
	if(!str || !*str){ return NULL; };
	return str+lchars(str,chars);
};
char* str_rtrim(char* str,char* chars){
	if(!str || !*str){ return NULL; };
	int r=rchars(str,chars,0);
	if(!r){ return str; };
	return sub_str(str,0,str_len(str)-r);
};
char* str_trim(char* str,char* chars){
	int l=lchars(str,chars);
	int r=rchars(str,chars,0);
	if(!l && !r){ return str; };
	return sub_str(str,l,str_len(str)-l-r);
};
int line_isempty(char* line){
	while(*line && *line!='\n' && *line!='\r'){ if(*line!=' ' && *line!='\t') {return 0;} line++; };
	return 1;
};
int line_len(char* line){
	int ret=0;
	while(*line && *line!='\n' && *line!='\r'){ ret++; line++; };
	if(*line=='\r'){ ret++; line++; };
	if(*line=='\n'){ ret++; };
	return ret;
};
int lchars(char* str,char* chars){
	if(!str||!chars){ return 0; };
	int ret=0;
	while(str[ret]){
		if(strchr(chars,str[ret])){ ret++; }
		else {break;};
	};
	return ret;
};
int rchars(char* str,char* chars,int off){
	if(!str||!chars){ return 0; };
	int ret=0;
	int len=str_len(str)+off;
	while(ret<len){
		if(strchr(chars,str[len-ret-1])){ ret++; }
		else {break;};
	};
	return ret;
};
int has_str(char* str,char* substr){
	if(!str||!substr){ return 0; };
	int ret=0;
	while(*str){
		if(str_start(str,substr)){ ret++; };
		str++; };
	return ret;
};
char* str_replace(char* str,void* find,void* replace){
	if(!str || !find || !str_len(str)){ return str; };
	assert(is_str(str));
	if(is_str(find)){
		if(!str_has(str,find)){ return str; };
		assert(is_str(replace));
		find=xmap((find) , replace, End); };
	assert(is_map(find));
	int dels=0;
	int adds=0;
	for(int i2=next(find,-1,NULL,NULL); has_id(find,i2); i2++){ void* v=map_id(find,i2); char* k=map_key(find, i2);
		assert(is_str(k));
		if(is_i(v)){ set(find,i2,(v=int_str(is_int(v)))); };
		assert(!v || is_str(v));
		int i=has_str(str,k);
		if(!i){ continue; };
		dels+=i*str_len(k);
		adds+=i*str_len(v); };
	if(!dels){ return str; };
	char* ret=new_str(str_len(str)+adds-dels);
	char* temp=str;
	char* temp2=ret;
	for(;*temp;temp++,temp2++){
		int found=0;
		for(int  i3=next(find,-1,NULL,NULL); has_id(find, i3);  i3++){ void* v3=map_id(find, i3); char*  k3=map_key(find,  i3);
			if(str_start(temp,k3)){
				if(v3){
					strcat(temp2,v3);
					temp2+=strlen(v3)-1;
				}else{
					temp2--; };
				temp+=strlen(k3)-1;
				found=1;
				break; }; };
		if(!found){ *temp2=*temp; }; };
	return ret;
};
char* str_str(char* in, char* sub){
	int len=str_len(in);
	int sublen=str_len(sub);
	for(int i=0; i<=len-sublen; i++){
		int ok=1;
		for(int j=0; j<sublen; j++){
			if(sub[j]!=in[i+j]){
				ok=0;
				break; }; };
		if(ok){ return in+i; }; };
	return NULL;
};
char* str_upper(char* s){
	char* ret=s;
	for(;*s;++s) {*s=toupper(*s);};
	return ret;
};
char* str_lower(char* s){
	char* ret=s;
	for(;*s;++s) {*s=tolower(*s);};
	return ret;
};
char fox_at(char* str,int idx){
	if(!str || !is_str(str)){ return '\0'; };
	if(idx<0) {idx+=str_len(str);};
	return idx<str_len(str) ? str[idx] : '\0';
};
char* indent_str(int i){
	char* ret=new_str(i+1);
	memset(ret,'\t',i+1);
	ret[0]='\n';
	return ret;
};
int str_chars(char* str,char* chars){
	if(!str || !chars || !is_str(str)){ return 0; };
	int len1=strlen(str);
	int len2=strlen(chars);
	for(int i=0;i<len1;i++){
		for(int j=0;j<len2;j++){
			if(str[i]==chars[j]){ return 1; }; }; };
	return 0;
};
char* end_word(char* str,char* words){
	if(!str||!words){ return NULL; };
	char* word=str_dup(words);
	int total=terminate_words(word);
	for(int i=0;i<total;i++){
		char* ret=str_end(str,word);
		if(ret){ return ret; };
		word+=str_len(word)+1;
	};
	return NULL;
};
char* has_word(char* str,char* words){
	if(!str||!words){ return NULL; };
	char* word=str_dup(words);
	int total=terminate_words(word);
	for(int i=0;i<total;i++){
		char* ret=strstr(str,word);
		if(ret){ return ret; };
		word+=str_len(word)+1;
	};
	return NULL;
};
int is_word(char* word,char* list){
	assert(list);
	assert(is_str(list));
	if(!is_str(word)){ return 0; };
	char* ret=list;
	while((ret=str_has(ret,word))){
		if(ret && (ret==list||ret[-1]==' ') && (ret[strlen(word)]==' '||!ret[strlen(word)])){ return ret-list+1; };
		ret+=strlen(word); };
	return 0;
};
int terminate_words(char* in){//Takes a string and adds NULL between eash words. Returns total number of words terminated with NULL
	if(!in){ return 0; };
	char* s=in;
	int ret=1;
	for(;*s;s++) {if(*s==' '){ ret++; *s='\0'; };};
	return ret;
};
int is_number(char c){ return (c>='0' && c<='9') || c=='.' || c=='-' || c=='+'; };
int is_alphanum(char c,char* others){
	if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_'){ return 1; };
	if(others && strchr(others,c)){ return 1; };
	return 0;
};
int is_alpha(char c,char* others){
	if(c>='a' && c<='z' || c>='A' && c<='Z' || c=='_'){ return 1; };
	if(others && strchr(others,c)){ return 1; };
	return 0;
};
int is_oper(char c){ return strchr("~!@#$%^&*-=+|<>?/\\}]).",c) ? 1 : 0; };
int str_is_oper(char* str){
	if(!str_len(str)){ return 0; };
	while(*str++) {if(!strchr("~!@#$%^&*-=+|:<>?/\\{[(}]),.",str[-1])){ return 0; };};
	return 1;
};
int str_is_num(char* str){
	if(!str_len(str)){ return 0; };
	while(*str++) {if(!is_number(str[-1])){ return 0; };};
	return 1;
};
int is_code(char* str){
	if(!str_len(str)){ return 0; };
	if(!is_alpha(*str,NULL)){ return 0; };
	while(*++str) {if(!is_alphanum(*str,NULL)){ return 0; };};
	return 1;
};
int char_at(char* str,char* terms){
	if(!str || !is_str(str)){ return 0; };
	char* head=str;
	while(*str && !strchr(terms,*str)){ str++; };
	return str-head;
};
int rchar_at(char* str,char* terms){
	if(!str || !is_str(str)){ return 0; };
	char* tail=str+str_len(str)-1;
	while(tail>=str && !strchr(terms,*tail)){ tail--; };
	return tail-str;
};
char* char_str(char c){
	char ret[2]={0};
	ret[0]=c;
	ret[1]='\0';
	char* str=str_dup(ret);
	assert(str_len(str)<=1);
	return str;
};
int has_chars(char* line){
	if(!line){ return 0; };
	for(;*line;line++){ if(!strchr("\n\r \t",*line)){ return 1; }; };
	return 0;	
};
char* drop_right(char* str,char* w){
	if(w && is_str(w) && str_end(w,w)){ return sub_str(str,-str_len(w),-2147483648); };	
	return str;
};
char* drop_left(char* str,char* w){
	if(str && is_str(str) && str_start(str,w)){ return sub_str(str,str_len(w),-2147483648); };	
	return str;
};
char* str_toupper(char* str){
	if(!str) {return NULL;};
	char* ret=str;
	for(;*str;str++) {*str=toupper(*str);};
	return ret;
};
char* str_tolower(char* str){
	if(!str) {return NULL;};
	char* ret=str;
	for(;*str;str++) {*str=tolower(*str);};
	return ret;
};
char* str_title(char* str){
	if(!str) {return NULL;};
	map* words=str_split(str,"_",0);
	for(int i=next(words,-1,NULL,NULL); has_id(words,i); i++){ void* v=map_id(words,i);
		char* s=v;
		if(!str_len(s)) {map_del(words,i,1);}
		else if(is_word(s,"lft rgt id slno")){
			str_toupper(s);
		}else {s[0]=toupper(s[0]);}; };
	return map_join(words," ");
};
char hex_char(char* in){
	char temp[3]={0};
	if(!in[0]||!in[1]) {return '\0';};
	temp[0]=in[0];
	temp[1]=in[1];
	return (char)strtol(temp,NULL,16);
};
char* rand_str(int len){
	char* ret=NULL;
	while(str_len(ret)<len){ ret=xcat(ret,int_str(rand()), End); };
	return sub_str(ret,0,len);
};

int str_char_count(char* str,char c){
	int ret=0;
	if(!str) {return ret;};
	while(*str) {if(*str++==c) {ret++;};};
	return ret;
};
char* str_join(char* str1,char* joiner,char* str2){
	if(!str_len(str1)){ return str2; };
	if(!str_len(str2)){ return str1; };
	return xstr(str1,joiner,str2, End);
};
char* var_bits(void* var){
	char* ret=new_str(71);
	unsigned char *ptr = (unsigned char*)&var;
	for(int idx=64,i=0;idx--;i++){
		if(i && !(i%8)){ ret[i+i/8-1]='-'; };
		ret[i+i/8]=ptr[idx/8] & (1u << (idx%8) ) ? '1' : '0'; };
	return ret;
};
char* rtrim_upto(char* in,char upto,int keep){
	if(!in){ return in; };
	char* at=strrchr(in,upto);
	if(!at){ return in; };
	return sub_str(in,0,at-in + (keep ? 1 : 0));
};
map* split_by(char* str, char term, int max){
//	"spliting by $(term.char_str())".px()
	if(!*str){ return NULL; };
	map* ret=new_vec();
	char* head=str;
	if(!str_chr(" \t\n\r",term)){ head=skip_over(head," \t\n\r"); };
	while(*str && (!max || map_len(ret)<max-1)){
		if(strchr("\"'`",*str)){ str=skip_quote(str); continue; }
		else if(strchr("([{",*str)){ str=skip_paren(str); continue; }
		else if(*str==term){
			vec_add(ret,sub_str(head,0,str-head));
			head=str+1;
			if(!str_chr(" \t\n\r",term)){ head=skip_over(head," \t\n\r"); }; };
		str++; };
	while(*str){ str++; };
	if(str>head){ vec_add(ret,sub_str(head,0,str-head)); };
	return ret;
};
char* str_hex(char* in){
	char* ret=new_str(str_len(in)*2);
	for(int i=0; i<str_len(in); i++){
		sprintf(ret+i*2,"%02x",((unsigned char*)in)[i]); };
	return ret;
};
char* str_tr(char* in, map* replace){
	if(!in){ return NULL; };
	char* ret=NULL;
	char* saved=in;
	int maxlen=0;
	for(int next1=next(replace,-1,NULL,NULL); has_id(replace,next1); next1++){ char* key=map_key(replace, next1); maxlen=max(maxlen,str_len(key)); };
	char* buff=new_str(maxlen);
	while(*in){
		while(*in && !is_alphanum(*in,NULL)) {in+=utf_len(in);};
		char* head=in;
		while(*in && is_alphanum(*in,NULL)) {in+=utf_len(in);};
		if(!(in-head)){ break; };
		if(in-head>maxlen){ continue; };
		memcpy(buff,head,in-head);
		buff[in-head]='\0';
		if(!map_val(replace,buff)){ continue; };
		ret=xcat(ret,sub_str(saved,0,head-saved), End);
		ret=xcat(ret,map_val(replace,buff), End);
		saved=in;
	};
	if(saved<in){
		ret=xcat(ret,sub_str(saved,0,in-saved), End);
	};
	return ret;	
};
char* skip_over(char* in,char* chars){
	while(*in && strchr(chars,*in)) {in++;};
	return in;
};
char* skip_quote(char* str){
	if(!str || !*str){ return str; };
	char end=*str;
	str++;
	while(*str && *str!=end){
		if(*str=='\\'){
			str++;
			if(!*str){ break; }; };
		str++; };
	if(*str==end){ str++; };
	return str;
};
char* skip_paren(char* str){
	char open=*str;
	char close=closing_paren(open);
	int level=1;
	str++;
	while(*str && level){
		if(str_chr("\"'`",*str)){ str=skip_quote(str); continue; }
		else if(*str==open){ level++; }
		else if(*str==close){ level--; }
		else if(*str=='\\'){ str++; if(!*str){ break; }; };
		str++; };
	return str;
};
char closing_paren(char c){ switch(c){ case '{': return '}'; case '[': return ']'; case '(': return ')'; default: return '\0'; }; };
