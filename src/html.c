#include <fox.h>

char* fkey_sql(char* sql,char* db){
	map* cols=sql_select_cols(sql,db,NULL);
	void* pkey=map_id(cols_pkeys(cols),0);
	void* skey=map_id(cols_skeys(cols),0);
	map* ret=sql_map(sql);
	add(ret,"select",xmap("id", map_val(map_val(cols,pkey),"expr"), "name", map_val(map_val(cols,skey),"expr"), End));
	return sql_add_order(map_sql(ret),db,NULL);
};
char* callback_php(map* param,char* name){ return call_php(param,name); };
char* fkey_show(char* sql,char* db,char* id){
	char* sign=xstr(sql, "/", id, End);
	char* ret=NULL;
	if((ret=cache(sign,"str_show",NULL))){ return ret; };
	return cache(sign,"str_show",map_val(sql_row(sql_add_where(fkey_sql(sql,db),xmap("id", ":id", End)),db,xmap("id", id, End)),"name"));
};
int fkey_valid(char* sql,char* db,char* id){
	return fkey_show(sql,db,id) ? 1 : 0;
};
char* str_show(char* value,char* type,map* op,int width){
	value=to_str(value,"",0);
	if(!value){ return ""; };
	if(!type){ return value; };
	if(map_val(op,"list")){ return map_val(map_val(op,"list"),value) ? str_title(map_val(map_val(op,"list"),value)) : value; };
	if(map_val(op,"sql") && map_val(op,"db")){ return fkey_show(map_val(op,"sql"),map_val(op,"db"),value); };
	type=map_type(xmap(
		"text", "text",
		"para", "para",
		"source", "source",
		"password", "password",
		"html", "html",
		"guid", "guid",
		"amount", "number",
		"mins", "mins",
		"debit", "debit",
		"credit", "credit",
		"bool", "bool",
		"email", "email",
		"date", "date",
		"quarter", "quarter",
		"file", "file",
		"jpeg", "image",
		"duration", "duration"
	, End),type);
	if(width && is_word(type,"source text para") && str_len(value)>width*256){ value=sub_str(value,0,width*256); };
	if(str_eq(type,"bool")){ return is_int(value) ? "Yes" : "No"; };
	if(str_eq(type,"text")){ return str_html(value); };
	if(str_eq(type,"file")){ return int_kb(str_len(value),"B"); };
	if(str_eq(type,"para")){ return str_replace(str_html(value),"\n","<br>"); };
	if(str_eq(type,"source")){ return xstr("<pre>",str_html(value),"</pre>", End); };
	if(str_eq(type,"html")){ return value; };
	if(str_eq(type,"number")){ return int_human(stoi(value),map_val(op,"unit"),""); };
	if(str_eq(type,"debit")){ return int_human(stoi(value),map_val(op,"unit"),""); };
	if(str_eq(type,"credit")){ return int_human((-to_int(value)),map_val(op,"unit"),""); };
	if(str_eq(type,"password")){ return "****"; };
	if(str_eq(type,"email")){ return xstr("<a href='mailto:",str_html(value),"'>",str_html(value),"</a>", End); };
	if(str_eq(type,"guid")){ return str_html("<ID>"); };
	if(str_eq(type,"mins")){ long long n=to_int(value); return mstr("%d:%02d",n/60,n%60, End); };
	if(str_eq(type,"duration")){ return "Duration/Pending"; };
	if(str_eq(type,"date")){ return datetime_human(value); };
	if(str_eq(type,"quarter")){ return "Pending-Qurter"; };
	if(str_eq(type,"image")){ return value ? xstr("<img src=",map_val(_globals,"base_url"),"/",thumb_name(value),"></img>", End) : "--"; };
	return value;
};
char* cols_show(map* cols,map* row,char* name,int width){
	return str_show(map_val(row,name),map_val(map_val(cols,name),"type"),map_val(cols,name),width);
};
char* datetime_human(char* in){
	if(!in){ return NULL; };
	char buffer[64];
	time_t time=str_time(in);
	strftime(buffer,sizeof(buffer), "%a %e-%b-%Y %l:%M %p",localtime(&time));
	return str_dup(buffer);
};
char* thumb_name(char* name){
	map* parts=str_split(name,"/",0);
	void* ext=vec_pop(parts);
	vec_add(parts,"thumbnail");
	vec_add(parts,ext);
	return map_join(parts,"/");
};
map* row_show(map* row,map* cols,int width){
	map* ret=new_map();
	for(int next1=next(row,-1,NULL,NULL); has_id(row,next1); next1++){ void* v=map_id(row,next1); char* f=map_key(row, next1);
		if(map_val(cols,f)){ add(ret,f,cols_show(cols,row,f,width)); }
		else {add(ret,f,v);}; };
	return ret;
};
map* rows_show(map* rows,map* cols,int width){
	map* ret=new_vec();
	for(int i=next(rows,-1,NULL,NULL); has_id(rows,i); i++){ void* row=map_id(rows,i); char* k=map_key(rows, i);
		map* r=new_map();
		for(int i2=next(row,-1,NULL,NULL); has_id(row,i2); i2++){ void* v=map_id(row,i2); char* f=map_key(row, i2);
			if(map_val(cols,f)){ add(r,f,cols_show(cols,row,f,width)); }
			else {add(r,f,v);}; };
		vec_add(ret,r); };
	return ret;
};

char* month_name(int month){ return map_val(xmap("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec", End),abs(month-1)%12); };
char* http_moved(char* url){ return http_out(NULL,"301 Moved Permanently","text/html",xvec(xstr("Location: ", url, End), End)); };
char* http_redirect(char* url,char* msg){
	if(msg){ add(add_key(_globals,"sess",Map),"msg",msg); };
	return http_out(NULL,"302 Moved Temporarily","text/html",xvec(xstr("Location: ", url, End), End));
};
void* http_error(char* msg,char* status){ http_out(msg,status,"text/html; charset=utf-8",NULL); xexit(0); return NULL; };
char* file_mime(char* path){
	map* map_1=map_val(_globals,"mime"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); char* k=map_key(map_1, i);
		if(str_end(path,k)) {return v;}; };
	return NULL;
};
char* str_html(char* in){
	if(!str_len(in)){ return ""; };
	char* chars="<>\"'&";
	map* into=xvec("&lt;","&gt;","&quot;","&#039;","&amp;", End);
	char* ret=NULL;
	for(;*in;in++){
		char* match=strchr(chars,*in);
		if(!match){ ret=cat_char(ret,*in); }
		else {ret=xcat(ret,map_id(into,match-chars), End);}; };
	return ret;
};
void header(char* str){ print(str,stdout); print("\r\n",stdout); };
char* http_out(char* str,char* status,char* mime,map* headers){
	static int callonce=0;
	if(callonce){ return str; };
	callonce=1;
	char* sess=json(map_val(_globals,"sess"),0);
	if(str_eq(sess,"{}")){ sess=NULL; };
	if(!str_eq(map_val(_globals,"sess_str"),sess)){
		add(_globals,"sess",NULL);
		add(_globals,"sess_str",NULL);
		void* sid=map_val(_globals,"sess_id");
		if(!sid && sess){
			sid=sess_newid();
			cookie_set("sessid",sid,"/",NULL); };
		if(!sess && sid){
			if(is_file(xstr("/tmp/sess.", sid, End))){ unlink(xstr("/tmp/sess.", sid, End)); };
		}else if(sess && sid){
			write_file(sess,xstr("/tmp/sess.", sid, End),0,0); }; };
	char* out=xstr(map_val(_globals,"out"),str, End);
	header(xstr("Status: ", status, End));
	header(xstr("Content-Type: ", mime, End));
	header(xstr("Content-Length: ",int_str( str_len(out)), End));
	for(int i=next(headers,-1,NULL,NULL); has_id(headers,i); i++){ void* v=map_id(headers,i); header(v); };
	map* map_1=map_val(_globals,"cookie"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* v2=map_id(map_1,next1);
		header(xstr("Set-Cookie: ", v2, End)); };
	header("");
	print(out,stdout);
	xexit(0);
	return str;
};
char* static_file(char* path){
	if(has_word(path,"? ../ /.. \\")){ return NULL; };
	if(!str_start(path,"/res/")){ return NULL; };
	char* ret=file_read(cat("/web/",str_trim(path,"/"),-1020),1,0);
	if(ret){ return http_out(ret,"200 OK",file_mime(path),NULL); };
	return NULL;
};

char* str_url(char* in){
	if(!in||!*in){ return in; };
	int bad=0;
	char* head=in;
	for(;*in;in++){
		if(!is_alphanum(*in,"\"'-.[]*$%{}()@!~")){ bad++; }; };
	if(!bad){ return head; };
	char* ret=new_str(str_len(head)+bad*2);		
	int off=0;
	in=head;
	for(;*in;in++){
		if(!is_alphanum(*in,"\"'-.[]*$%{}()@!~")){
			sprintf(ret+off,"%%%02X",(unsigned char)*in);
			off+=3;
		}else{
			ret[off]=*in;
			off++; }; };
	return ret;
};
char* url_str(char* in){
	if(!in){ return NULL; };
	int len=0;
	int fix=0;
	char* str=in;
	for(;*in;in++){
		if(*in=='+'){ fix=1; }
		else if(*in=='%'){ fix=1; len-=2; }; };
	if(!fix){ return str; };
	char* ret=new_str(str_len(str)+len);
	int i=0;
	for(in=str;*in;in++,i++){
		if(*in=='+'){ ret[i]=' '; }
		else if(*in=='%'){ ret[i]=hex_char((++in)); in++; }
		else {ret[i]=*in;}; };
	return ret;
};
char* map_amps(void* val,char* name){
	if(!val){ return NULL; };
	if(!is_map(val)){
		val=to_str(val,"",0);
		return name ? xstr(str_url(name),"=",str_url(val), End) : NULL; };
	char* ret=NULL;
	if(name){ name=xcat(name,".", End); };
	for(int next1=next(val,-1,NULL,NULL); has_id(val,next1); next1++){ void* v=map_id(val,next1); char* n=map_key(val, next1);
		ret=str_join(ret,"&",map_amps(v,xstr(name,n, End))); };
	return ret;
};
map* amps_map(char* in){
	map* ret=new_map();
	map* map_1=str_split(in,"&",0); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		map* pr=str_split(v,"=",2);
		char* key=url_str(map_id(pr,0));
		if(!strchr(key,'.')){
			add(ret,key,url_str(map_id(pr,1)));
			continue; };
		map* keys=str_split(key,".",0);
		void* lastkey=map_id(keys,map_len(keys)-1);
		vec_del(keys,-1,1);
		map* val=ret;
		for(int next1=next(keys,-1,NULL,NULL); has_id(keys,next1); next1++){ void* k=map_id(keys,next1);
			void* nval=map_val(val,k);
			if(!is_map(nval)){ nval=new_map(); add(val,k,nval); };
			val=nval; };
		add(val,lastkey,url_str(map_id(pr,1))); };
	return ret;
};
map* parse_url(char* path){
	map* ret=xmap("url", path, End);
	map* two=str_split(path,"?",2);
	add(ret,"path",map_id(two,0));
	add(ret,"get",amps_map(map_id(two,1)));
	return ret;
};
char* url_host(char* url){ return map_id(regexp(url,"://([^:/]+)"),1); };
map* sess_init(){
	void* sid=map_val(header_map(map_val(env_vars(),"HTTP_COOKIE")),"sessid");
	if(!sid){ return NULL; };
	char* sess=file_read(xstr("/tmp/sess.", sid, End),1,0);
	add(_globals,"sess_id",sid);
	add(_globals,"sess_str",sess);
	add(_globals,"sess",xjson_map(sess,Map));
	return map_val(_globals,"sess");
};
void	sess_add(char* name, char* value){ add(add_key(_globals,"sess",Map),name,value); };
char*	sess_id(){ return map_val(_globals,"sess_id"); };
char*	sess_file(){ return sess_id() ? xstr("/tmp/sess.", sess_id(), End) : NULL; };
char*	sess_newid(){ return rand_str(24); };
void cookie_set(char* name,char* value,char* path,char* expire){
	char* xexpire=NULL;
	if(expire){ xexpire=xstr("; expires=", expire, End); };
	char* ss=xstr(name, "=", value, "; path=", path, xexpire, End);
	vec_add(add_key(_globals,"cookie",Vector),ss);
};
void sess_delete(){
	if(!sess_file()){ return; };
	remove(sess_file());
	cookie_set("sessid","NULL","/","Thu, 01 Jan 1970 00:00:00 GMT");
};
map* link_relative(map* links,char* url){
	if(!links||!url) {return links;};
	char* prepad=str_times("../",str_char_count(url,'/')+1);
	for(int i=next(links,-1,NULL,NULL); has_id(links,i); i++){ void* v=map_id(links,i);
		if(is_map(v)){
			link_relative(v,url);
			continue; };
		if(fox_at(v,0)=='/') {continue;};
		set(links,i,xstr(prepad,v, End)); };
	return links;
};
map* header_map(char* val){
	if(!val){ return NULL; };
	map* ret=new_map();
	map* map_1=str_split(val,";",0); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		if(!strchr(v,'=')){ vec_add(ret,v); continue; };
		map* pair=str_split(v,"=",2);
		add(ret,str_trim(map_id(pair,0)," \t\n\r"),str_unquote(str_trim(map_id(pair,1)," \t\n\r"))); };
	return ret;
};
map* http_req(){
	map* ret=new_map();
	map* env=env_vars();
	sess_init();
	add(_globals,"css",new_map());
	add(_globals,"js",new_map());
	if(!map_val(env,"REQUEST_METHOD")){
		void* path=map_id(map_val(_globals,"args"),1);
		char* home=xstr("https://sanjir.com",sub_str(cwd(),char_at((cwd()+1),"/")+1,-2147483648),"/", End);
		ret=parse_url((xstr(home,path, End)));
		add(ret,"method","get");
		add(ret,"remote","localhost");
		add(ret,"server","localhost");
		add(ret,"protocol","http");
		add(ret,"port","80");
		add(ret,"path",xmap(
			"full", map_val(ret,"path"),
			"home", home,
			"next", (sub_str(map_val(ret,"path"),str_len(home),-2147483648) ? sub_str(map_val(ret,"path"),str_len(home),-2147483648) : "/")
		, End));
		add(_globals,"req",ret);
		return ret; };
	ret=parse_url(map_val(env,"REQUEST_URI"));
	add(ret,"remote",map_val(env,"REMOTE_ADDR"));
	add(ret,"server",map_val(env,"HTTP_HOST"));
	add(ret,"protocol",map_val(env,"REQUEST_SCHEME"));
	add(ret,"port",map_val(env,"SERVER_PORT"));
	char* home=rtrim_upto(map_val(env,"SCRIPT_NAME"),'/',1);
	add(ret,"path",xmap(
		"full", map_val(ret,"path"),
		"home", home,
		"next", (sub_str(map_val(ret,"path"),str_len(home),-2147483648) ? sub_str(map_val(ret,"path"),str_len(home),-2147483648) : "/")
	, End));
	if(str_eq(map_val(env,"REQUEST_METHOD"),"GET")){
		add(ret,"method","get");
		add(_globals,"req",ret);
		return ret; };
	if(!str_eq(map_val(env,"REQUEST_METHOD"),"POST")){
		http_error(xstr("Method ", map_val(env,"REQUEST_METHOD"), " not supported", End),"405 Method not supported"); };
	add(ret,"method","post");
	long long size=str_int(map_val(env,"CONTENT_LENGTH"));
	if(!size){ http_error("Content-Lenght was not provided","411 Length Required"); };
	if(size>20000000){ http_error("Request should be with less than 20MB data","413 Request Entity Too Large"); };
	char line[1024];
	char* header=new_blob(0);
	int done=0;
	int done1=0;
	int toread=min(size-done,1024);
	while(done<size && (done1=read(STDIN_FILENO,line,toread))){
		if(done1<=0){ http_error(xstr("POST data read error at ",int_str( done), "/",int_str( size), " bytes", End),"500 Internal Server Error"); };
		header=cat(header,line,done1);
		done+=done1;
		toread=min(size-done,1024); };
	add(ret,"post",header);
	add(_globals,"req",ret);
	return ret;
};
char* full_url(char* url){ return xstr(((map_val(map_val(_globals,"req"),"protocol") ? map_val(map_val(_globals,"req"),"protocol") : "http")),"://",((map_val(map_val(_globals,"req"),"server") ? map_val(map_val(_globals,"req"),"server") : "localhost")),show_port(),"/",str_ltrim(url,"/"), End); };
//char* full_path(char* url=NULL) => return "/"..url.str_ltrim("/")
char* url_abs(char* abs, char* rel){
	if(!rel){ return abs; };
	if(str_start(rel,"./")){ return xstr(abs,sub_str(rel,2,-2147483648), End); };
	if(rel[0]!='.'){ return xstr(abs,rel, End); };
	char* srel=rel;
	int level=0;
	while(str_start(srel,"../")){ srel+=3; level++; };
	map* base=str_split(abs,"/",0);
	if(strstr(abs,"://")){ level=max(map_len(base)-level-1,3); }
	else {level=max(map_len(base)-level-1,0);};
	char* ret=map_join(vec_sub(base,0,level),"/");
	return xstr(ret,"/",srel, End);
};
char* base_url(char* path){ return url_abs(map_key(map_val(_globals,"tabs"),0),path); };
char* home_url(char* path){ return url_abs(map_val(map_val(map_val(_globals,"req"),"path"),"home"),path); };
char* show_port(){
	if(!map_val(map_val(_globals,"req"),"port")){ return NULL; };
	if(str_eq(map_val(map_val(_globals,"req"),"protocol"),"http") && str_eq(map_val(map_val(_globals,"req"),"port"),"80")){ return NULL; };
	if(str_eq(map_val(map_val(_globals,"req"),"protocol"),"https") && str_eq(map_val(map_val(_globals,"req"),"port"),"443")){ return NULL; };
	return xstr(":",map_val(map_val(_globals,"req"),"port"), End);
};
char* h(char* in){
	return str_replace(in,xmap(
	"&", "&amp;",
	"<", "&gt;",
	">", "&lt;",
	"\"", "&quot;",
	"'", "&apos;"
	, End),NULL);
};
