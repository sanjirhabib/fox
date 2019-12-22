#line 2 "src/tsv.fox"

#include <core.h>
#include <tsv.h>

map* tsv_load(char* filename,int key){ return str_rows(file_read(filename,1,1),key); };
map* str_rows(char* in,int key){
	map* ret=new_map();
	map* cols=NULL;
	map* map_1=split_by(str_rtrim(in," \t\n\r"),'\n',0); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* ln=map_id(map_1,i);
		map* vals=split_by(str_rtrim(ln," \t\n\r"),'\t',0);
		if(!i){
			cols=vals;
			continue; };
		map* row=new_map();
		for(int j=next(cols,-1,NULL,NULL); has_id(cols,j); j++){ void* name=map_id(cols,j);
			add(row,name,map_id(vals,j)); };
		add(ret,key ? map_id(vals,0) : NULL,row); };
	return ret;
};
int type_size(char* type){
	return to_int(map_type(xmap(
		"text", "128",
		"code", "50",
		"guid", "36",
		"int", "18",
		"bool", "1",
		"para", "256",
		"daymonth", "5",
		"month", "7"
	, End),type));
};
char* map_type(map* mp,char* type){
	while(type){
		if(map_val(mp,type)){ return to_str(map_val(mp,type),"",0); };
		type=map_val(types(),type); };
	return type;
};
map* types(){
	return xmap(
		"text",NULL,
		"tinytext", "text",
		"code", "text",
		"name", "text",
		"fold", "text",
		"html", "text",
		"link", "html",
		"para", "text",
		"source", "para",
		"password", "code",
		"email", "text",
		"phone", "text",
		"imagelink", "text",
		"guid", "code",
		"hidden", "code",
		"list", "code",
		"radio", "list",
		"multicheck", "list",
		"grid", "multicheck",
		"fkey", "list",
		"combo", "fkey",
		"user", "code",
		"ip", "code",
		"flist", "fkey",
		"inline", "fkey",
		"int", "code",
		"smallint", "int",
		"bigint", "int",
		"integer", "int",
		"number", "int",
		"intrange", "int",
		"amount", "number",
		"currency", "amount",
		"debit", "amount",
		"credit", "amount",
		"float", "int",
		"duration", "amount",
		"mins", "amount",
		"date", "int",
		"pastdate", "date",
		"futuredate", "date",
		"time", "date",
		"hour", "time",
		"datetime", "date",
		"timestamp", "datetime",
		"daterange", "date",
		"datefrom", "date",
		"dateupto", "date",
		"daymonth", "date",
		"yearmonth", "date",
		"month", "yearmonth",
		"quarter", "yearmonth",
		"bool", "int",
		"check", "bool",
		"checkbox", "check",
		"file", "para",
		"jpeg", "file",
		"imagefile", "jpeg",
		"bmp", "jpeg",
		"wsq", "jpeg",
		"clob", "para",
		"blob", "file",
		"tinyblob", "blob",
		"longtext", "blob",
		"raw", "blob",
		"largefile", "file"
	, End);
};
map* subtypes(char* type){
	map* ret=new_vec();
	void* parent=map_val(types(),type);
	while(parent){
		vec_add(ret,parent);
		parent=map_val(types(),parent); };
	return vec_reverse(ret);
};
map* str_cols(char* in){
	if(!in){ return NULL; };
	if(is_map(in)){ return (map*)in; };
	map* ret=new_map();
	map* toks=toks_recs(toks_trim(str_tokenize(in," ,\n\r\t=:",0)," ,\n\r\t=:"),"\n\r,");
	for(int next1=next(toks,-1,NULL,NULL); has_id(toks,next1); next1++){ void* tks=map_id(toks,next1);
		int i=0;
		map* row=new_map();
		while(i<map_len(tks)){
			if(!i){
				add(row,"name",map_id(tks,i));
				i+=2;
				continue; };
			if(map_len(tks)<i+3){
				add(row,map_id(tks,i),"yes");
				i+=2;
				continue; };
			add(row,map_id(tks,i),str_unquote(map_id(tks,i+2)));
			i+=4; };
		if(!map_val(row,"type")){ add(row,"type",data_type(map_val(row,"name"),NULL)); };
		add(ret,map_val(row,"name"),row); };
	return ret;
};
char* skip_c_comment(char* in){
	if(!*in){ return in; };
	if(*in=='/' && in[1]=='*'){
		while(*in && in[0]!='*' || in[1]!='/'){
			in++; };
		if(in){ in+=1; };
		return in; };
	if(*in=='/' && in[1]=='/'){
		while(*in && *in!='\n'){
			in++; };
		in--;
		return in; };
	return in;
};
char* skip_c_code(char* in){
	if(!*in){ return in; };
	if(strchr("({[",*in)){
		char open=*in;
		char close=')';
		if(open=='['){ close=']'; }
		else if(open=='{'){ close='}'; };
		in++;
		while(*in && *in!=close){
			in=skip_c_code(in);
			in++; };
		return in; };
	char* temp=skip_c_comment(in);
	if(temp!=in){ return temp; };

	if(*in=='-' && in[1]=='-' && in[2]=='-'){ 
		char* head=in;
		while(*in && *in!='\n'){ in++; };
		if(!*in){ return in; };
		while(*in){
			int bad=0;
			int j=0; 
			for(j=0; head[j]!='\n'; j++){
				if(in[j] && in[j]!=head[j]){
					bad=1;
					break; }; };
			if(!bad){
				in+=j-1;
				return in; };
			in++; };
		return in; };
	if(*in=='\'' && in[1]=='-' && in[2]=='-' && in[3]=='-'){
		char* head=in+1;
		while(*in && *in!='\n'){ in++; };
		if(!*in){ return in; };
		while(*in){
			int bad=0;
			int j=0; 
			for(j=0; in[j] && head[j]!='\n'; j++){
				if(in[j] && in[j]!=head[j]){
					bad=1;
					break; }; };
			if(!bad && in[j]=='\''){
				in+=j;
				return in; };
			in++; };
		return in; };
	if(strchr("\"'`",*in)){
		char quote=*in;
		in++;
		while(*in && *in!=quote && (quote!='\'' || *in!='\n')){
			if(*in=='\\'){ in++; };
			in++; };
		return in; };
	return in;
};
map* str_tokenize(char* in,char* terminators, int max){
	map* ret=new_vec();
	if(!in){ return ret; };
	char* head=in;
	max--;
	while(*in && max--){
		while(*in && !strchr(terminators,*in)){
			in=skip_c_code(in);
			in++; };
		vec_add(ret,sub_str(head,0,in-head));
		head=in;
		if(!*in || !max--){ break; };
		while(*in && strchr(terminators,*in)){
			in=skip_c_comment(in);
			in++; };
		vec_add(ret,sub_str(head,0,in-head));
		head=in; };
	if(*head){ vec_add(ret,str_dup(head)); };
	return ret;
};
map* toks_trim(map* toks, char* terminators){
	if(!toks || !map_len(toks) || !str_chr(terminators,is_str(map_id(toks,0))[0])){ return toks; };
	return vec_splice(toks,1,1,NULL);
};
map* toks_recs(map* in, char* terminator){
	map* ret=new_vec();
	map* row=new_vec();
	for(int next1=next(in,-1,NULL,NULL); has_id(in,next1); next1++){ void* rec=map_id(in,next1);
		vec_add(row,rec);
		if(str_chr(terminator,is_str(rec)[0])){
			vec_add(ret,row);
			row=new_vec(); }; };
	if(map_len(row)){
		vec_add(ret,row); };
	return ret;
};
char* data_type(char* name, void* val){
	map* vals=xmap(
		"debit", "currency",
		"due", "currency",
		"amount", "currency",
		"bill", "currency",
		"paid", "currency",
		"balance", "currency",
		"credit", "currency",
		"note", "para",
		"time", "daysec",
		"datetime", "datetime",
		"date", "date",
		"month", "month",
		"year", "year",
		"password", "password",
		"cell", "phone",
		"phone", "phone",
		"date", "date",
		"action", "action",
		"flat", "flat",
		"arabic", "arabic"
	, End);
	if(map_val(vals,name)){ return map_val(vals,name); };
	if(!val){ return "text"; };
	if(is_i(val) || is_f(val)){ return "int"; };
	if(!is_str(val)){ return "text"; };
	if(is_numeric(val)){ return "int"; };
	if(is_str(val)[0]=='<'){ return "html"; };
	return "text";
};
char* month_en3(int val){
	map* ret=xvec("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec", End);
	if(!val) {return NULL;};
	return map_id(ret,val-1);
};
double frac(double val){ return val>0 ? val-(int)val : -(val-(int)(val)); };
