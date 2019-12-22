#line 2 "src/foxparser.fox"

#include <core.h>
#include <foxparser.h>

char* fox_read_oper(char** in,char term){
	char* str=*in;
	char* from=*in;
	while(*str && is_oper(*str) && *str!=term) {str++;};
	char* ret=sub_str(from,0,str-from);
	int less=0;
	while(strlen(ret) && !is_word(ret,"~ ! @ # $ % ^ & * - = + | < > ? / \\ } ] ) . != >= <= += -= /= *= || && << >> => ++ -- ** *** **** |= &= >>= <<= ^^ ... -> == === !== .= .. ||= &&=")){
		ret[strlen(ret)-1]='\0';
		less++; };
	*in=str-1-less;
	return ret;
};
char* read_upto_word(char** in,char* upto){
	char* str=*in;
	while(*str && !str_start(str,upto)) {str++;};
	char* from=*in;
	if(*str){
		*in=str+strlen(upto)-1;
		return sub_str(from,0,str-from+strlen(upto));
	}else{
		*in=str-1;
		return sub_str(from,0,str-from);
	};};
char* read_theline(char** in){
	if(!in||!*in||!**in){ return NULL; };
	char* str=*in;
	while(*str && !strchr("\n\r",*str)) {str++;};
	if(*str=='\r' && str[1]=='\n'){ str++; };
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
char* read_newline(char** in){
	char* str=*in;
	while(*str && strchr("\n\r",*str)) {str++;};
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
char* read_space(char** in,char* spaces){
	char* str=*in;
	while(*str && strchr(spaces,*str)) {str++;};
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
map* read_data(char** in){
	char* str=*in;
	char* from=*in;
	while(*str && !str_start(str,"}}")){
		str+=line_len(str);
		str+=str_level(str); };
	if(*str){ str+=2; };
	*in=str-1;
	return data_toks(sub_str(from,0,str-from));
};
char* read_heredoc(char** in){
	char* str=*in;
	int len=line_len(str);
	char* hd=str_trim(sub_str(str,0,len)," \t\n\r");
	char* from=*in;
	int lno=0;
	str+=len;
	int tabs=str_level(str);
	str+=tabs;
	while(*str && !str_start(str,hd)){
		len=line_len(str);
		tabs=str_level((str+len));
		str+=len+tabs; };
	if(*str){ str+=str_len(hd)-1; };
	*in=str;
	return sub_str(from,0,str-from+1);
};
char* read_multistr(char** in){
	char* str=*in;
	char end=*str;
	char* hd=NULL;
	char* from=*in;
	int lno=0;
	char* word_start=NULL;
	str++;
	while(*str){
		if(*str=='\\'){ if(!hd) {str++;}; }
		else if(*str==end){ if(!hd) {break;}; }
		else if(*str=='\n'||*str=='\r'){
			if(!lno && word_start){
				hd=sub_str(word_start,0,str-word_start); };
			lno++;
			word_start=NULL;
		}else if(*str!=' ' && *str!='\t'){
			if(!word_start){
				word_start=str;
				if(hd && lno && str_start(str,hd) && str[str_len(hd)]==end){
					str+=str_len(hd);
					break; }; }; };
		str++; };
	*in=str;
	return sub_str(from,0,str-from+1);
};
char* read_quote(char** in){
	char* str=*in;
	char end=*str;
	str++;
	while(*str){
		if(*str=='\\'){ str++; }
		else if(*str==end){ break; };
		str++;
	};
	char* from=*in;
	*in=str;
	return sub_str(from,0,str-from+1);
};
char* fox_read_symbol(char** in){
	char* str=*in;
	str++;
	while(*str){
		if(*str=='\\'){
			str+=2;
			continue; };
		if(!is_alphanum(*str,NULL)){ break; };
		str++; };
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
map* data_toks(char* in){ in=sub_str(in,2,-2); return xvec("xmap",NULL,"(",NULL,data_tokenizer(&in,0),NULL,")", End); };
map* data_tokenizer(char** in,int level){
	if(!in||!*in||!**in){ return NULL; };
	char* str=*in;
	map* mp=new_vec();
	char* last=NULL;
	char* space1=NULL;
	char* space2=NULL;
	map* key=NULL;
	map* val=NULL;
	while(*str){
		assert(last!=str);
		last=str;

		char* temp=str;
		if(str_start(str,"//")){ space1=xcat(space1,read_theline(&str), End); str++; }
		else if(str_chr(" \t\n\r",*str)){ space1=xcat(space1,read_space(&str,"\t\n\r "), End); str++; };
		int clevel=0;
		for(int i=str_len(space1)-1; i>=0; i--){
			if(str_chr(" \t",space1[i])){ clevel++; }; };
		if(clevel<level){
			space1=NULL;
			str=temp;
			break;
		};
		if(str_chr("\"'`",*str)){ key=xvec(read_quote(&str), End); str++; }
		else if(*str=='('){
			str++;
			key=xvec("(",NULL,c_tokenizer(&str,')'),NULL,")", End);
			if(*str){ str++; };
		}else{
			key=data_quote(read_upto(&str,"\n\r \t"));
		};
		if(str_chr(" \t",*str)){ space2=read_over(&str,"\t "); str++; };

		if(str_start(str,"---")){ val=xvec(read_heredoc(&str), End); str++; }
		else if(*str=='-' && str_chr(" \t\n\r\0",str[1])){ val=prop_toks(read_theline(&str),key); str++; }
		else if(!str_chr("\n\r",*str)){ val=data_quote(read_theline(&str)); str++; };
		
		if(key && !val){
			temp=str;
			map* ret=data_tokenizer(&str,clevel+1);	
			if(map_len(ret)){
				val=xvec("xmap",NULL,"(",NULL,ret,NULL,")", End);
			}else{
				val=NULL;
	  			str=temp; }; };
		if(key || val){
			if(map_len(mp)){
				vec_add(mp,NULL);
				vec_add(mp,","); };
			vec_add(mp,space1);
			vec_merge(mp,(key ? key : xvec("NULL", End)));
			vec_add(mp,NULL);
			vec_add(mp,",");
			vec_add(mp,space2);
			vec_merge(mp,(val ? val : xvec("NULL", End)));
			space1=space2=NULL;
			key=val=NULL;
		}else{
			space1=xcat(space1,space2, End);
			space2=NULL; }; };
	if(map_len(mp) || space1){ vec_add(mp,space1); };
	*in=str;
	return mp;
};
char* read_over(char** in,char* chars){
	char* str=*in;
	while(*str && str_chr(chars,*str)){
		str++; };
	char* from=*in;
	*in=str-1;
	return sub_str(from,0,str-from);
};
map* prop_vec_toks(char* in){
	map* ret=new_vec();
	map* toks=split_by(in,',',0);
	for(int next1=next(ret,-1,NULL,NULL); has_id(ret,next1); next1++){ void* val=map_id(ret,next1); char* idx=map_key(ret, next1);
		if(idx){ vec_merge(ret,xvec(NULL,","," ", End)); }
		else {vec_add(ret,NULL);};
		vec_merge(ret,data_quote(val)); };
	return xvec("xvec",NULL,"(",NULL,ret,NULL,")", End);
};
map* prop_toks(char* in,map* name){
	map* ret=new_vec();
	map* toks=split_by(in,',',0);
	for(int  idx=next(toks,-1,NULL,NULL); has_id(toks, idx);  idx++){ void* val=map_id(toks, idx);
		val=split_by(val,' ',2);
		if(!map_len(val)){ continue; };
		if(idx==0 && str_eq(map_id(val,0),"-")){
			if(name){
				vec_add(ret,NULL);
				vec_merge(ret,data_quote("name"));
				vec_merge(ret,xvec(NULL,","," ", End));
				vec_merge(ret,name); };
			set(val,0,"type"); };
		if(map_len(ret)){
			vec_merge(ret,xvec(NULL,","," ", End));
		}else{
			vec_add(ret,NULL); };
		vec_merge(ret,data_quote(map_id(val,0)));
		vec_merge(ret,xvec(NULL,","," ", End));
		vec_merge(ret,map_id(val,1) ? data_quote(map_id(val,1)) : xvec("NULL", End)); };
	return xvec("xmap",NULL,"(",NULL,ret,NULL,")", End);
};
map* read_paren(map* mp,char** line,map*(*func)(char**)){
	char* str=*line;
	vec_add(mp,char_str(*str));
	vec_add(mp,func(line));
	vec_add(mp,char_str(closing_paren(*str)));
	return mp;
};
map* data_quote(char* in){
	if(!in || !str_len(in)){ return NULL; };
	if(str_chr("\"'`",*in)){ return xvec(in, End); };
	if(*in=='='){ return vec_compact(vec_del(x_map(sub_str(in,1,-2147483648)),0,1)); };
	if(*in=='{'){ return prop_toks(sub_str(in,1,-1),NULL); };
	if(*in=='['){ return prop_vec_toks(sub_str(in,1,-1)); };
	return xvec(str_quote(in), End);
};
map* c_tokenizer(char** line,char term){
	if(!line||!*line||!**line){ return NULL; };
	char* head=*line;
	int headlen=str_len(head);
	char* expected_end=head+str_len(head);
	char* str=*line;
	map* mp=new_vec();
	char* space=NULL;
	int code=0;
	char* last=NULL;
	while(*str && *str!=term){
		assert(last!=str);
		assert(str_len(head)==headlen);
		last=str;
		if(str_start(str,"//")){ add_ctok(read_theline(&str),mp,0); }
		else if(str_start(str,"/*")){ add_ctok(read_upto_word(&str,"*/"),mp,0); }
		else if(str_start(str,"#")){ add_ctok(read_theline(&str),mp,0); }
		else if(strchr(" \t",*str)){ add_ctok(read_space(&str," \t"),mp,0); }
		else if(strchr("\n\r",*str)){ add_ctok(read_newline(&str),mp,0); }
		else if(strchr("\"`",*str)){ add_ctok(read_quote(&str),mp,1); }
		else if(*str=='\''){ add_ctok(read_multistr(&str),mp,1); }
		else if(str_start(str,"{{")){
			if(!(mp->len%2)){ vec_add(mp,NULL); };
			vec_merge(mp,read_data(&str));
		}else if(str_start(str,"---")){ add_ctok(read_heredoc(&str),mp,1); }
		else if(*str==':'){ add_ctok(fox_read_symbol(&str),mp,1); }
		else if(is_oper(*str)){ add_ctok(fox_read_oper(&str,term),mp,1); }
		else if(*str>='0' && *str<='9'){ add_ctok(read_num(&str),mp,1); }
		else if(is_alpha(*str,NULL)){ add_ctok(read_alpha(&str),mp,1); }
		else if(strchr("([{",*str)){
			char c=*str;
			str++;
			add_ctok(char_str(c),mp,1);
			add_ctok(c_tokenizer(&str,closing_paren(c)),mp,1);
			if(*str){ add_ctok(char_str(closing_paren(c)),mp,1); };
		}else if(','==*str){ add_ctok(sub_str(str,0,1),mp,1); }
		else if(';'==*str){ add_ctok(sub_str(str,0,1),mp,1); };
		if(*str){ str++; }; };
	assert(term || str==expected_end);
	*line=str;
	return mp;
};
static map* add_ctok(void* data,map* mp,int iscode){
	if(!data){ return mp; };
	assert(data);
	assert(ptr_type(mp)==Vector);
	int incode=!(mp->len%2);
	if(incode && iscode){
		vec_add(mp,NULL);
		vec_add(mp,data);
	}else if(incode && !iscode){ vec_add(mp,data); }
	else if(!incode && !iscode){
		if(mp->len){
			char* old=is_str(map_id(mp,mp->len-1));
			mp->vars[mp->len-1]=xstr(old,data, End);
		}else {vec_add(mp,data);};
	}else if(!incode && iscode){ vec_add(mp,data); };
	return mp;
};
map* x_map(char* in){ return c_tokenizer(&in,'\0'); };
int map_has_word(map* mp,char* str){
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){ void* v=map_id(mp,idx); if(str_eq(v,str)){ return idx+1; }; };
	return 0;
};
map* toks_split(map* in,char* by,int limit){
	map* ret=map_split(in,by,limit);
	for(int i=1;i<ret->len;i++){ toks_align(map_id(ret,i-1)); };
	return ret;
};
map* toks_align(map* in){
	int last=map_len(in);
	if(!(last%2)){ return in; };
	if(map_id(in,last-1)==NULL){
		vec_compact(vec_del(in,-1,1));
	}else {vec_add(in,NULL);};
	return in;
};
