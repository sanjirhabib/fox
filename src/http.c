#line 2 "src/http.fox"

#include <core.h>
#include <http.h>

void go_back(char* msg){ http_redirect(url_back(),msg); };
char* http_moved(char* url){ return http_out(NULL,"301 Moved Permanently","text/html",xvec(xstr("Location: ", url, End), End)); };
char* http_redirect(char* url,char* msg){
	if(msg){ add(add_key(_globals,"sess",Map),"msg",msg); };
	return http_out(NULL,"302 Moved Temporarily","text/html",xvec(xstr("Location: ", url, End), End));
};
char* http_error(char* msg,char* status){ return http_out(msg,status,"text/html; charset=utf-8",NULL); };
char* file_mime(char* path){
	map* map_1=map_val(_globals,"mime"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); char* k=map_key(map_1, i);
		if(str_end(path,k)) {return v;}; };
	return NULL;
};
char* header(char* str){ return xstr(str,"\r\n", End); };
char* http_out(char* str,char* status,char* mime,map* headers){
	if(map_val(_globals,"error")){
		status="500 Internal Server Error";
		mime="text/html; charset=utf-8";
		str=map_val(_globals,"error");
	}else{
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
				write_file(sess,xstr("/tmp/sess.", sid, End),0,0); }; }; };
	char* out=xstr(map_val(_globals,"out"),str, End);
	char* ret=header(xstr("Status: ", status, End));
	ret=xcat(ret,header(xstr("Content-Type: ", mime, End)), End);
	ret=xcat(ret,header(xstr("Content-Length: ",int_str( str_len(out)), End)), End);
	for(int i=next(headers,-1,NULL,NULL); has_id(headers,i); i++){ void* v=map_id(headers,i); ret=xcat(ret,header(v), End); };
	map* map_1=map_val(_globals,"cookie"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* v2=map_id(map_1,next1);
		ret=xcat(ret,header(xstr("Set-Cookie: ", v2, End)), End); };
	ret=xcat(ret,header(""), End);
	ret=xcat(ret,out, End);
	return ret;
};
char* static_file(char* path){
	if(has_word(path,"? ../ /.. \\")){ return NULL; };
	if(!str_start(path,"/res/")){ return NULL; };
	char* ret=file_read(cat("/web/",str_trim(path,"/"),-1020),1,0);
	if(ret){ return http_out(ret,"200 OK",file_mime(path),NULL); };
	return NULL;
};

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
map* http_req(char* home){
	map* ret=new_map();
	add(_globals,"env",NULL);
	map* env=env_vars();
	sess_init();
	add(_globals,"css",new_map());
	add(_globals,"js",new_map());
	if(!map_val(env,"REQUEST_METHOD")){
		void* path=map_id(map_val(_globals,"args"),1);
		ret=parse_url((xstr(home,path, End)));
		add(ret,"method","get");
		add(ret,"remote","localhost");
		add(ret,"server","localhost");
		add(ret,"protocol","http");
		add(ret,"port","80");
		add(ret,"fullpath",map_val(ret,"path"));
		add(ret,"home",home);
		add(ret,"path",(sub_str(map_val(ret,"fullpath"),str_len(home),-2147483648) ? sub_str(map_val(ret,"fullpath"),str_len(home),-2147483648) : ""));
		add(_globals,"req",ret);
		return ret; };
	ret=parse_url(map_val(env,"REQUEST_URI"));
	add(ret,"remote",map_val(env,"REMOTE_ADDR"));
	add(ret,"server",map_val(env,"HTTP_HOST"));
	add(ret,"protocol",map_val(env,"REQUEST_SCHEME"));
	add(ret,"port",map_val(env,"SERVER_PORT"));
	add(ret,"fullpath",map_val(ret,"path"));
	add(ret,"home",home);
	add(ret,"path",(sub_str(map_val(ret,"fullpath"),str_len(home),-2147483648) ? sub_str(map_val(ret,"fullpath"),str_len(home),-2147483648) : ""));
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
char* home_url(char* path){ return url_abs(map_val(map_val(_globals,"req"),"home"),path); };
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
//char* url_host(char* url) => return url.regexp("://([^:/]+)")[1]
char* param(map* change){
	return map_amps(merge_soft(change,map_val(map_val(_globals,"req"),"get")),NULL);
};
void save_caller(){
	char* refer=url_back();
	void* req=map_val(_globals,"req");
	if(!str_eq(map_val(req,"method"),"get") || !refer || map_val(map_val(req,"get"),"_refer") || str_end(refer,map_val(req,"path"))){ return; };
	add(add_key(req,"get",Map),"_refer",refer);
	http_redirect(xstr("./?", map_amps(map_val(req,"get"),NULL), End),NULL);
};
char* url_back(){
	return (map_val(map_val(map_val(_globals,"req"),"get"),"_refer") ? map_val(map_val(map_val(_globals,"req"),"get"),"_refer") : (map_val(parse_url(map_val(env_vars(),"HTTP_REFERER")),"path") ? map_val(parse_url(map_val(env_vars(),"HTTP_REFERER")),"path") : home_url(NULL)));
};
