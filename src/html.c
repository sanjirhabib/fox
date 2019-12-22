#line 2 "src/html.fox"

#include <core.h>
#include <foxtime.h>
#include <regexp.h>
#include <http.h>
#include <html.h>
#include <tsv.h>
//#include "astrostr.h"

#pragma fox cc -lfoxdeps

//extern char*(*page_html)(map* data);
//extern map*(*page_data)(map* data);

char*(*page_html)(map* data)=default_page_html;
map*(*page_data)(map* data)=default_page_data;

char* str_show(char* value,char* type,map* op,int width){
	value=to_str(value,"",0);
	if(!value){ return ""; };
	if(!type){ return value; };
	if(map_val(op,"list")){ return map_val(map_val(op,"list"),value) ? str_title(map_val(map_val(op,"list"),value)) : value; };
//	if op.sql && op.db => return op.sql.fkey_show(op.db,value)
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
//extern int _queries;
char* default_page_html(map* data){
	char* head=xstr("", 
	"<!DOCTYPE html>\n", 
	"<html><head><meta charset='utf-8'>\n", 
	"<title>", map_val(data,"title"), "</title>\n", 
	"<script type=\"application/ld+json\">\n", 
	"{\n", 
	"	\"@context\": \"http://schema.org\",\n", 
	"	\"@type\": \"WebSite\",\n", 
	"	\"name\": \"Habib's Site\",\n", 
	"	\"alternateName\": \"Sanjir Habib\",\n", 
	"\t\"url\": \"", full_url(map_val(map_val(_globals,"req"),"fullpath")), "\"\n", 
	"}\n", 
	"</script>\n", 
	"<link rel=\"apple-touch-icon\" href=\"/apple-touch-icon.png\">\n", 
	"<meta name=\"application-name\" content=\"habib\"/>\n", 
	"<meta name=\"msapplication-square70x70logo\" content=\"/small.jpg\"/>\n", 
	"<meta name=\"msapplication-square150x150logo\" content=\"/medium.jpg\"/>\n", 
	"<meta name=\"msapplication-wide310x150logo\" content=\"/wide.jpg\"/>\n", 
	"<meta name=\"msapplication-square310x310logo\" content=\"/large.jpg\"/>\n", 
	"<meta name=\"msapplication-TileColor\" content=\"#fff\"/>\n", 
	"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n", 
	map_val(data,"header"), "\n", 
	"<link rel='icon' href='", map_val(data,"logo"), "'></link>\n", 
	"</head><body>\n", 
	"", 
	"", End);
	if(map_val(map_val(map_val(_globals,"req"),"get"),"_print")){
		return xstr("", 
		head, "\n", 
		map_val(data,"body"), "\n", 
		"</body></html>", 
		"", End); };
	return xstr("", 
	head, "\n", 
	"<div class='row-fluid alert-success'><span class='span8' style='padding-left:2em;'>", map_val(data,"msg"), "</span>", map_val(data,"login"), "</div>\n", 
	"<div class=row-fluid>\n", 
	"<div class='span3' style='text-align:center;'><a href='", map_val(data,"home"), "'><img style='width:auto;height:100px;padding:.3em;' src='", map_val(data,"logo"), "'></img></a></div>\n", 
	"<div class='span7'>\n", 
	"<h2 style='text-align:center;'>", map_val(data,"title"), "</h2>\n", 
	"<div style='border-top:1px solid gray;border-bottom:1px solid gray;min-height:0.2em;text-align:center;'>", map_val(data,"desc"), "</div>\n", 
	"</div>\n", 
	"</div>\n", 
	map_val(data,"tabs"), map_val(data,"ldmenu"), "\n", 
	"<div class=container-fluid>\n", 
	"<div class=row-fluid>", map_val(data,"body"), "</div>\n", 
	"<hr noshade=noshade style=\"width:70%;margin:auto;height:1px;margin-bottom:.5em;margin-top:8em;color:gray;\">\n", 
	"<div class=footer style='color:#333;font-size:small;text-align:center;'>\n", 
	"&copy; Habib &lt;habib@habibur.com&gt;. ", map_val(data,"hits"), " ", map_val(data,"footer"), "\n", 
	"</div>\n", 
	"</div>\n", 
	"</body></html>\n", 
	"", 
	"", End);
};
map* default_page_data(map* data){
	if(!(map_val(data,"title"))){add(data,"title","Habib's Site");};
	if(!(map_val(data,"tab"))){add(data,"tab",map_val(data,"title"));};
	if(!(map_val(data,"logo"))){add(data,"logo",home_url("logo.jpg"));};
	if(!(map_val(data,"home"))){add(data,"home",home_url(NULL));};
	if(map_val(data,"css")){add(data,"css",xstr("<style>\n", map_val(data,"css"), "</style>\n", End));};
	if(!(map_val(data,"msg"))){add(data,"msg",map_val(map_val(_globals,"sess"),"msg"));};
	map_compact(map_del_key(map_val(_globals,"sess"),"msg"));
	char* head=NULL;
	map* map_1=
	xvec("/res/bootstrap.css","/res/bootstrap-responsive.css","/res/jquery.js","/res/bootstrap.js", End); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* v=map_id(map_1,next1);
		if(str_eq(sub_str(v,-4,-2147483648),".css")){ head=xcat(head,xstr("<link rel='stylesheet' href='", v, "'></link>\n", End), End); }
		else {head=xcat(head,xstr("<script src='", v, "'></script>\n", End), End);}; };
	map* map_2=map_val(_globals,"jsfile"); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* v=map_id(map_2,next1);
		head=xcat(head,xstr("<script src='", v, "'></script>\n", End), End); };
	add(data,"header",xstr(head,map_val(data,"css")
		,((map_len(map_val(_globals,"css")) ? xstr("<style>\n",map_join(map_val(_globals,"css"),"\n"),"</style>\n", End) : NULL)), End));
	if(!(map_val(data,"desc"))){add(data,"desc",map_val(map_val(data,"meta"),"description"));};
	if(!(map_val(map_val(data,"meta"),"description"))){add(add_key(data,"meta",Map),"description",map_val(data,"desc"));};
	if(!(map_val(map_val(data,"meta"),"description"))){add(add_key(data,"meta",Map),"description",map_val(data,"title"));};
	add(add_key(data,"meta",Map),"og:description",map_val(map_val(data,"meta"),"description"));
	add(add_key(data,"meta",Map),"og:title",map_val(data,"title"));
	if(!(map_val(map_val(data,"meta"),"og:image"))){add(add_key(data,"meta",Map),"og:image",full_url(map_val(data,"logo")));};
	map* map_3=map_val(data,"meta"); for(int next1=next(map_3,-1,NULL,NULL); has_id(map_3,next1); next1++){ void* val=map_id(map_3,next1); char*  key=map_key(map_3, next1);
		add(data,"header",xcat(map_val(data,"header"),xstr("<meta property=\"", key, "\" content=\"", val, "\">\n", End), End)); };
	if(map_val(map_val(map_val(_globals,"req"),"get"),"_print")){ return data; };
	add(data,"footer",xstr("", 
	"Total: ", int_kb(max_mem(),"B"), ". ", mem_usage(), " Queries: ", int_str(_queries), ".", 
	"", End));
	add(data,"footer",xcat(map_val(data,"footer"),((map_len(map_val(_globals,"js")) ? xstr("<script>\n",map_join(map_val(_globals,"js"),"\n"),"</script>", End) : NULL)), End));
	char* tabs=NULL;
	map* lddata=xmap("@context", "http://schema.org", "@type", "BreadcrumbList", End);
	int i=1;
	map* map_4=map_val(_globals,"tabs"); for(int  idx=next(map_4,-1,NULL,NULL); has_id(map_4, idx);  idx++){ void* name=map_id(map_4, idx); char*  url=map_key(map_4,  idx);
		char* active=NULL;
		if(idx==map_len(map_val(_globals,"tabs"))-1){
			active=" class='active'"; };			
		tabs=xcat(tabs,xstr("<li", active, "><a href='", url, "'>", name, "</a></li>\n", End), End);
			
		vec_add(add_key(lddata,"itemListElement",Vector),xmap(
			"@type", "ListItem",
			"position",int_var( i++), "item", xmap( "@id", url, "name", name , End)
		, End)); };
	if(map_len(map_val(_globals,"tabs"))>1){
		add(data,"tabs",xstr("<div><ul class='nav nav-tabs'>", tabs, "</ul></div>", End)); };
	add(data,"ldmenu",xstr("<script type='application/ld+json'>",to_str( lddata,"",0), "</script>", End));
	char* body=NULL;
	vec_add(add_key(_globals,"css",Vector),""
	"a.menu {overflow:hidden;text-overflow:ellipsis;white-space:nowrap;}\n"
	"ul.menu2 {margin-top:5em;}"
	"");
	map* map_5=map_val(data,"menu"); for(int next1=next(map_5,-1,NULL,NULL); has_id(map_5,next1); next1++){ void* name=map_id(map_5,next1); char*  url=map_key(map_5, next1);
		if(str_start(url,"onclick:")){
			body=xcat(body,xstr("<li><a class='menu' href='#' onclick='", sub_str(url,8,-2147483648), "'>", name, "</a></li>\n", End), End);
		}else{
			body=xcat(body,xstr("<li><a class='menu' href='", url, "'>", name, "</a></li>\n", End), End); }; };
	if(map_val(data,"menus")){
		map* map_1=map_val(data,"menus"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* menu=map_id(map_1,next1); char*  title=map_key(map_1, next1);
			if(!map_len(menu)){ continue; };
			body=xcat(body,xstr("</ul><h5 style='text-align:center;'>", title, "</h5><ul class='nav nav-tabs nav-stacked menu2'>", End), End);
			for(int next1=next(menu,-1,NULL,NULL); has_id(menu,next1); next1++){ void* name=map_id(menu,next1); char*  url=map_key(menu, next1);
				body=xcat(body,xstr("<li><a class='menu' href='", url, "'>", name, "</a></li>\n", End), End); }; }; };
	if(is_int(map_val(data,"width"))==4){
		body=xstr("<div class='span11 offset1'><ul class='nav nav-pills'>", body, "</ul></div>", End);
	}else{
		body=xstr("<div class=span2><ul class='nav nav-tabs nav-stacked'>", body, "</ul></div>", End);
	};
	switch(is_int(map_val(data,"width"))){
		case 1:
			body=xcat(body,xstr("<div class='span4'>", map_val(data,"body"), "</div><div class=span6>", map_val(data,"help"), "</div>", End), End);
		break;
		case 3:
			body=xcat(body,xstr("<div class='span10'>", map_val(data,"help"), map_val(data,"body"), "</div>", End), End);
		break;
		case 4:
			body=xcat(body,xstr("</div><div class=row-fluid>", map_val(data,"help"), map_val(data,"body"), "</div>", End), End);
		break;
		default: //=2
			body=xcat(body,xstr("<div class='span7'>", map_val(data,"body"), "</div><div class=span3>", map_val(data,"help"), "</div>", End), End); };
	add(data,"body",body);
	char* login=NULL;
	if(map_val(map_val(_globals,"sess"),"user")){
		login=xstr("<span class='pull-right'>", map_val(map_val(_globals,"sess"),"user"), " as ", map_val(map_val(_globals,"sess"),"role"), " | <a href='", base_url(NULL), "logout/'>Logout</a> &nbsp; </span>", End);
	}else{
		login=xstr("<span class='pull-right' style='margin-right:3em;'><a href='", base_url(NULL), "login/'>Login</a> | <a href='", base_url(NULL), "register/'>Register</a></span>", End); };
	add(data,"login",login);
	return data;
};
char* not_found(char* path){ return http_out(xstr("The requested content ", (path ? path : map_val(map_val(_globals,"req"),"fullpath")), " was not found on the server.", End),"404 Not Found","text/html; charset=utf-8",NULL); };
char* show_page(map* data){ return http_out(page_html(page_data(data)),"200 OK","text/html; charset=utf-8",NULL); };
int is_post(){ return str_eq(map_val(map_val(_globals,"req"),"method"),"post"); };
map* post_data_multipart(){
	if(map_val(_globals,"post")){ return map_val(_globals,"post"); };
	map* mime=header_map(map_val(env_vars(),"CONTENT_TYPE"));
	if(!str_eq(map_id(mime,0),"multipart/form-data")){
		return amps_map(map_val(map_val(_globals,"req"),"post")); };
	map* ret=new_map();
	char* seperator=map_val(mime,"boundary");
	map* map_1=str_split(map_val(map_val(_globals,"req"),"post"),seperator,0); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		map* mp3=str_split(v,"\r\n\r\n",2);
		if(map_len(mp3)!=2){ continue; };
		map* map_1=str_split(map_id(mp3,0),"\r\n",0); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* v2=map_id(map_1,i2); char* k2=map_key(map_1, i2);
			map* header=header_map(v2);
			if(map_val(header,"name")){
				add(add_key(ret,map_val(header,"name"),Map),"filename",map_val(header,"filename"));
				add(add_key(ret,map_val(header,"name"),Map),"data",sub_str(map_id(mp3,1),0,-4));
				break; }; }; };
	add(_globals,"post",ret);
	add(add_key(_globals,"req",Map),"post",NULL);
	return ret;
};
char* name_tab(char* name){
	add(add_key(_globals,"tabs",Map),home_url(map_val(map_val(_globals,"paths"),"matched")),name);
	return name;
};
map* get(char* path, map* names){
	map* ret=regexp(map_val(map_val(_globals,"req"),"path"),xstr("^",path, End));
	if(!ret){ return NULL; };
	void* ret2=map_val(_globals,"paths");
	if(!ret2){ add(_globals,"paths",new_map()); ret2=map_val(_globals,"paths"); };
	if(names){
		for(int i=1; i<map_len(ret) && i<=map_len(names); i++){
			add(ret2,map_id(names,i-1),map_id(ret,i)); }; };
	add(ret2,"matched",map_id(ret,0));
	add(ret2,"end",regexp(map_val(map_val(_globals,"req"),"path"),xstr("^",path,"$", End)) ? "yes" : NULL);
	return ret2;
};
char* show_body(char* body,map* page){
	add(page,"body",body);
	return show_page(page);
};
int days_in_feb(int year){
	if(!(year%400)){ return 29; };
	if(!(year%100)){ return 28; };
	if(!(year%4)){ return 29; };
	return 28;
};
int days_in_month(int month,int year){
	if(month==4||month==6||month==9||month==11){ return 30; };
	if(month==2){ return days_in_feb(year); };
	return 31;
};
char* add_tab(char* path,char* name){
	if(path && path[str_len(path)-1]!='/'){ path=xcat(path,"/", End); };
	path = path ? home_url(path) : home_url(map_val(map_val(_globals,"paths"),"matched"));
	if(!name){
		if(!path || str_eq(path,"/")){ name="Home"; }
		else{
			int i=str_len(path)-1;
			while(i && path[--i]!='/'){;};
			name=str_title(sub_str(path,i+1,-1)); }; };
	add(add_key(_globals,"tabs",Map),path,name);
	return name;
};
char* tz_dst(char* tz, char* date){
	time_t ret=str_time(date);
	if(tz){ setenv("TZ", tz, 1); };
	struct tm* tm=localtime(&ret);
	return tm->tm_isdst==1 ? "DST" : NULL;
};
double tz_offset(char* tz, char* date){
//in hours
	time_t ret=str_time(date);
	struct tm tm={0};
	gmtime_r(&ret,&tm);
	if(tz){ setenv("TZ", tz, 1); };
	struct tm tml={0};
	localtime_r(&ret,&tml);
	tm.tm_isdst = tml.tm_isdst;
	time_t gm=mktime(&tm);
	return (ret-gm)/3600.0;
};
char* tz_utc(char* tz, char* date){ return offset_utc(tz_offset(tz,date)); };
char* offset_utc(double off){
	if(!off){ return "UTC"; };
	char sign=off<0 ? '-' : '+';
	off=fabs(off);
	if(off-floor(off)<1/100.){ return mstr("UTC%c%d",sign,(int)off, End); };
	return mstr("UTC%c%d:%02d",sign,(int)off,(int)((off-(int)off)*60+0.5), End);
};
char* date_ymd(char* in,int* year, int* month,int* day){
	*year=*month=*day=0;
	if(!in){ return NULL; };
	sscanf(in,"%04d-%02d-%02d",year,month,day);
	return in;
};
char* date_human(char* in){
	int year=0;
	int month=0;
	int day=0;
	date_ymd(in,&year,&month,&day);
	return xstr(int_str(day),"-",month_en3(month),"-",int_str(year), End);
};
char* tz_human(double tz){
	if(!tz){ return "UTC"; };
	int mins=(int)(frac(tz)*60);
	if(!mins){ return mstr("UTC%c%d", tz<0 ? '-' : '+', abs((int)(tz)), mins, End); };
	return mstr("UTC%c%d:%d", tz<0 ? '-' : '+', (int)(tz), mins, End);
};
map* tz_data(char* name,char* date){
	return xmap(
		"name", name,
		"offset",double_var( tz_offset(name,date)),
		"dst", tz_dst(name,date),
		"utc", tz_utc(name,date)
	, End);
};
char* form_html(map* mp){
	map* map_1=map_val(mp,"vals"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* key=map_key(map_1, next1);
		if(map_val(map_val(mp,"cols"),key)){ add(add_key(add_key(mp,"cols",Map),key,Map),"value",val); }; };
	char* ret=xstr("", 
	"<fieldset><legend>", map_val(mp,"title"), "</legend>\n", 
	"<p class='text-", map_val(mp,"error") ? "error" : "info", "'>", map_val(mp,"msg"), "</p>\n", 
	"<form method=", (map_val(mp,"method") ? map_val(mp,"method") : "post"), ">", 
	"", End);
	map* map_2=map_val(mp,"cols"); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* val=map_id(map_2,next1); char*  name=map_key(map_2, next1);
		ret=xcat(ret,xstr("", 
		"<div class=\"control-group\">\n", 
		"<label>", (map_val(val,"label") ? map_val(val,"label") : str_title(map_val(val,"name"))), "</label>", ctrl_html(val), "\n", 
		"<span class='help-block'>", map_val(map_val(mp,"error"),name), "</span>\n", 
		"</div>", 
		"", End), End); };
	ret=xcat(ret,"<div class='form-actions'><div class='btn-group'>", End);
	map* map_3=map_val(mp,"buttons"); for(int  idx=next(map_3,-1,NULL,NULL); has_id(map_3, idx);  idx++){ void* val=map_id(map_3, idx); char*  name=map_key(map_3,  idx);
		ret=xcat(ret,xstr("", 
		"<input class='btn", !idx ? " btn-primary" : "", "' type='submit' name='", map_val(val,"name"), "' value='", str_title(map_val(val,"name")), "'>", 
		"", End), End); };
	ret=xcat(ret,"</div></div>", End);
	ret=xcat(ret,"</form>", End);
	return ret;
};
map* param_map(char* in){
	map* ret=new_map();
	map* map_1=str_split(in,"&",0); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1);
		map* two=str_split(val,"=",2);
		add(ret,map_id(two,0),map_id(two,1)); };
	return ret;
};
map* post_data(){ return amps_map(map_val(map_val(_globals,"req"),"post")); };
map* form_posted(map* form){
	save_caller();
	if(!str_eq(map_val(map_val(_globals,"req"),"method"),"post")){ return NULL; };
	map* param=post_data();
	map* map_1=map_val(form,"buttons"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ char* key=map_key(map_1, next1);
		if(map_val(param,key)){
			map* map_1=map_val(form,"cols"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* key=map_key(map_1, next1);
				add(val,"value",map_val(param,key)); };
			return param; }; };
	return NULL;
};
char* ctrl_html(map* ctrl){
	void* id=(map_val(ctrl,"id") ? map_val(ctrl,"id") : name_id(map_val(ctrl,"name")));
	if(map_val(ctrl,"list")){
		char* ret="<option value=''>--</option>";
		map* map_1=map_val(ctrl,"list"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char*  key=map_key(map_1, next1);
			ret=xcat(ret,xstr("<option value='", h(key), "'", str_eq(map_val(ctrl,"value"),key) ? " selected" :NULL, ">", val ? h(val) : h(str_title(key)), "</option>", End), End); };
		return xstr("<select name='", map_val(ctrl,"name"), "' id='", id, "' class='input-medium'>", ret, "</select>", End); };
	char* type=map_type(xmap(
		"text", "text",
		"code", "code",
		"para", "para",
		"source", "source",
		"password", "password",
		"hidden", "hidden"
	, End),map_val(ctrl,"type"));
	if(str_eq(type,"code")){
		return xstr("", 
		"<input type=text name=", map_val(ctrl,"name"), " value=\"", map_val(ctrl,"value"), "\" id=", id, " size=\"input-", (map_val(ctrl,"size") ? map_val(ctrl,"size") : "medium"), "\"></input>", 
		"", End); };
	if(str_eq(type,"text")){
		return xstr("", 
		"<input type=text name=", map_val(ctrl,"name"), " value=\"", map_val(ctrl,"value"), "\" id=", id, " size=\"input-", (map_val(ctrl,"size") ? map_val(ctrl,"size") : "large"), "\"></input>", 
		"", End); };
	if(str_eq(type,"para")){
		return xstr("", 
		"<textarea class=\"para\" style=\"width:20em; height:10em;\" id=", id, " name='", map_val(ctrl,"name"), "'>", map_val(ctrl,"value"), "</textarea>", 
		"", End); };
	if(str_eq(type,"hidden")){
		return xstr("", 
		"<input type=hidden name=", map_val(ctrl,"name"), " value=\"", map_val(ctrl,"value"), "\" id=", id, "></input>", 
		"", End);
	};
	if(str_eq(type,"source")){
		static int codeincluded=0;
		if(!codeincluded){
			vec_add(add_key(_globals,"jsfile",Vector),"/res/jquery.textarea.js");
			vec_add(add_key(_globals,"css",Vector),".source {font-family:courier;overflow:auto;width:100%;height:30em;}");
			vec_add(add_key(_globals,"js",Vector),xstr("$(\"#", id, "\").tabby()", End));
			codeincluded=1; };
		return xstr("", 
		"<textarea id=", id, " class='source' wrap='soft' name='", map_val(ctrl,"name"), "'>", map_val(ctrl,"value"), "</textarea>", 
		"", End); };
	if(str_eq(type,"password")){
		return xstr("", 
		"<input type=password name=", map_val(ctrl,"name"), " value=\"", map_val(ctrl,"value"), "\" id=", id, "></input>", 
		"", End); };
	return xstr("control type ", map_val(ctrl,"type"), " unspported. please provide support in ctrl_html()", End);
};
char* name_id(char* name){
	return name;
};
map* form_gets(map* form){
	map* map_1=map_val(map_val(map_val(_globals,"req"),"get"),"vals"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* key=map_key(map_1, next1);
		if(map_val(map_val(form,"cols"),key)){ add(add_key(add_key(form,"cols",Map),key,Map),"value",val); }; };
	return form;
};
char* show_form(map* data){
	add(data,"body",form_html(data));
	return show_page(data);
};
int is_user(char* who){ return !str_len(who) || str_eq(map_val(map_val(_globals,"sess"),"user"),who) || str_eq(map_val(map_val(_globals,"sess"),"role"),who); };
char* authorized(char* who){
	if(!is_user(who)){ return not_found(map_val(map_val(_globals,"req"),"path")); };
	return NULL;
};
char* str_bare(char* in,char* accept){
	if(!in){ return in; };
	char* ret=in;
	while(*in){
		if(*in>0 && !is_alphanum((*in),accept)){ *in=' '; };
		in+=utf_len(in); };
	return ret;
};
char* word_end(char* in,int len){
	if(str_len(in)<=len){ return in; };
	char* ptr=in+len;
	while(*ptr && !str_chr(" \t\n\r",*ptr)){ ptr++; };
	if(!*ptr){ return in; };
	return sub_str(in,0,ptr-in);
};
char* confirm(char* msg){
	map* form=xmap(
		"title", msg,
		"buttons",xmap(
			"yes", xmap("name", "yes", "type", NULL, End),
			"cancel", xmap("name", "cancel", "type", NULL, End), End)
	, End);
	map* data=form_posted(form);
	if(map_val(data,"yes")){
		return "yes"; };
	if(data){ return NULL; };
	return show_form(form);
};
char* html_text(char* in){
	int len=0;
	char* head=in;
	while(*in){
		if(*in=='<'){
			if(str_starti(in,"<script")){
				while(*in && !str_starti(in,"</script")){ in++; }; };
			if(str_starti(in,"<style")){
				while(*in && !str_starti(in,"</style")){ in++; }; };
			while(*in && *in!='>'){ in++; };
			if(*in=='>'){ in++; };
			continue; };
		in++;
		len++; };	
	if(len==str_len(in)){ return in; };
	char* ret=new_str(len);
	len=0;
	in=head;
	while(*in){
		if(*in=='<'){
			if(str_starti(in,"<script")){
				while(*in && !str_starti(in,"</script")){ in++; }; };
			if(str_starti(in,"<style")){
				while(*in && !str_starti(in,"</style")){ in++; }; };
			while(*in && *in!='>'){ in++; };
			if(*in=='>'){ in++; };
			continue; };
		ret[len++]=strchr("\n\r",*in) ? ' ' : *in;
		in++; };
	return ret;
};
char* url_filename(char* url){
	int question=rchar_at(url,"?");
	if(question>=0){ url=sub_str(url,0,question); };
	int slash=rchar_at(url,"/");
	if(slash<0){ return url; };
	return sub_str(url,slash+1,-2147483648);
};
int init_html(char* name){
	write_file(page_html(page_data(xmap(
		"title", str_title(name),
		"body", name,
		"tabs",xmap(
			"../", "Home", End)
	, End))),xstr(name,".html", End),0,1);
	return 0;
};
map* str_list(char* in){
	if(!in){ return NULL; };
	if(!is_str(in)){ return (map*)in; };
	map* toks=str_tokenize(in," ,\n\r\t=:",0);
	char* name=NULL;
	char* val=NULL;
	map* ret=new_map();
	for(int i=next(toks,-1,NULL,NULL); has_id(toks,i); i++){ void* tok=map_id(toks,i);
		int isword=!str_chr(" ,\n\r\t=:",is_str(tok)[0]);
		if(!isword){
			if(name && str_chr(tok,',')){
				add(ret,name,str_title(name));
				name=NULL; };
			continue; };
		if(!name){
			name=tok;
			continue; };
		add(ret,name,tok);
		name=NULL; };
	if(name){
		add(ret,name,str_title(name)); };
	return ret;
};
map* row_cols(map* row){
	map* ret=new_map();
	for(int next1=next(row,-1,NULL,NULL); has_id(row,next1); next1++){ void* v=map_id(row,next1); char* n=map_key(row, next1);
		add(ret,n,xmap(
			"type", data_type(n,v),
			"title", str_title(n),
			"name", n
		, End)); };
	return ret;
};
map* rows_cols(map* rows){ return rows ? row_cols(map_id(rows,0)) : new_vec(); };
char* val_format(char* val,char* type,map* op){
	if(map_val(op,"list")){
		map* list=str_list(sub_str(map_val(op,"list"),1,-1));
		return (map_val(list,val) ? map_val(list,val) : val);
	}
//		return val.taka()
	else if(str_eq(type,"html")){
		return val;
	}else if(str_eq(type,"action")){
		return val;
	}
//		return val.hm12()
	else if(str_eq(type,"password")){
		return str_len(val) ? "****" : NULL;
	}else if(str_eq(type,"para")){
		return str_replace(val,"\n","<br/>");
	}else if(str_eq(type,"date")){
		return date_human(val);
	}
//		return val.month_human()
//	else if type===:int
//		return val ? val.number_format() : ''
	else if(str_eq(type,"percent")){
		return val ? xstr(val,"%", End) : NULL;
	}else if(str_eq(type,"flat")){
		return val && is_numeric(val) ? xstr("F-", val, End) : val;
	}
//		return val.balance();
	else if(str_eq(type,"phone")){
		return xstr(sub_str(val,0,-6),"-",sub_str(val,-6,-2147483648), End);
	}else if(str_eq(type,"gb")){
		return xstr(int_str(((int)(floor((to_int(val)/1000000.0+0.5)))))," GB", End); };
	return html(val);
};
char* rows_sum(map* rows, map* cols){
	map* sum=new_map();
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* col=map_id(cols,next1); char* n=map_key(cols, next1);
		if(map_val(col,"sum")){ add(sum,n,int_var(0)); }; };	
	if(!map_len(sum)){ return NULL; };
	for(int next1=next(rows,-1,NULL,NULL); has_id(rows,next1); next1++){ void* row=map_id(rows,next1);
		for(int next1=next(row,-1,NULL,NULL); has_id(row,next1); next1++){ void* val=map_id(row,next1); char* col=map_key(row, next1);
			add(sum,col,map_val(sum,col)+str_int(map_val(row,col))); }; };
	char* ret="<tr class='total'>";
	int blanks=0;
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* col=map_id(cols,next1); char* n=map_key(cols, next1);
		if(map_val(sum,n)){ break; };
		blanks++; };
	if(blanks){ ret=xcat(ret,xstr("<td colspan=",int_str( blanks), " class=total>Total</td>", End), End); };
	for(int n=next(cols,-1,NULL,NULL); has_id(cols,n); n++){ char* col=map_key(cols, n);
		if(n<blanks){ continue; };
		if(!map_val(sum,col)){ ret=xcat(ret,xstr("<td class=", map_val(map_val(cols,col),"type"), ">&nbsp;</td>", End), End); }
		else {ret=xcat(ret,xstr("<td class='total ", map_val(map_val(cols,col),"type"), "'>", val_format(map_val(sum,col),map_val(map_val(cols,col),"type"),NULL), "</td>", End), End);}; };
	ret=xcat(ret,"</tr>", End);
//	if(@$sum['debit']!=@$sum['credit']){
//		$ret.="<tr>";
//		if($blanks) $ret.="<td colspan=$blanks class=balance>Balance</td>";
//		foreach(array_keys($cols) as $n=>$col){
//			if($n<$blanks) continue;
//			if($col!='debit' && $col!='credit') $ret.="<td>&nbsp;</td>";
//			else if($sum['debit']>$sum['credit']){
//				if($col=='debit')
//					$ret.="<td class='currency'>".taka($sum['debit']-$sum['credit'])."</td>";
//				else 
//					$ret.="<td>Debit</td>";
//			}
//			else{
//				if($col=='credit')
//					$ret.="<td class='currency'>".taka($sum['credit']-$sum['debit'])."</td>";
//				else 
//					$ret.="<td>Credit</td>";
//			}
//		}
//	}
	return ret;
};
char* table_close(char* in,char* tags){
	if(tags){tags=xstr(" ", tags, End);};
	return xstr("<table", tags, ">", in, "</table>", End);
};
char* html(char* in){
	return str_tr(in,xmap( "<" , "&gt;", ">" , "&lt;", "&" , "&amp;" , End));
};
//char* taka(char* val){
//	$val=trim($val);
//	if(!$val) return '';
//	$neg='';
//	if($val[0]=='-'){
//		$neg='-';
//		$val=substr($val,1);
//	}
//	$deci='';
//	$val=explode('.',$val);
//	if(count($val)==2){
//		$deci='.'.$val[1];
//		$val=$val[0];
//	}
//	else $val=$val[0];
//	if(strlen($val)>7) $val=substr($val,0,-7).",".substr($val,-7,-5).",".substr($val,-5,-3).",".substr($val,-3);
//	else if(strlen($val)>5) $val=substr($val,-7,-5).",".substr($val,-5,-3).",".substr($val,-3);
//	else if(strlen($val)>3) $val=substr($val,-5,-3).",".substr($val,-3);
//	return "Tk. ".$neg.$val.$deci;
//}
char* rows_list(map* rows, void* cols, void* callback){
	map* rows_fmt=NULL;
	if(is_str(cols)){ cols=str_cols(cols); };
	if(callback){
		rows_fmt=new_vec();
		for(int i=next(rows,-1,NULL,NULL); has_id(rows,i); i++){ void* row=map_id(rows,i);
			map* ret=((map*(*)(map*,int))(callback))(row,i);
			if(ret){
				set(rows_fmt,i,ret); }; };
	}else{
		rows_fmt=rows; };
	if(!(cols)){cols=map_val(rows_fmt,"rows_cols");};
	// col definition can have link=<name of another col that contains link>
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* op=map_id(cols,next1); char*  col=map_key(cols, next1);
		if(map_val(op,"link")){
			for(int next1=next(rows_fmt,-1,NULL,NULL); has_id(rows_fmt,next1); next1++){ void* row=map_id(rows_fmt,next1);
				add(row,col,xstr("<a href='", map_val(row,map_val(op,"link")), "'>", map_val(row,col), "</a>", End)); }; };
	};
	char* blank=NULL;
	if(!map_len(rows)){
		blank=xstr("<tr><td style='height:3em;vertical-align:center;text-align:center;' colspan=",int_str( map_len(cols)), ">Blank</td></tr>", End); };
	char* ret=NULL;
	ret=xcat(ret,xstr("", 
	"<tr>", cols_th(cols), "</tr>\n", 
	"<tr>", map_join(rows_td(rows_fmt,cols),"</tr><tr>"), "</tr>\n", 
	blank, "\n", 
	rows_sum(rows_fmt,cols), "\n", 
	"", 
	"", End), End);
	return ret;
};
char* cols_th(map* cols){
	char* ret=NULL;
	int found=0;
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* col=map_id(cols,next1);
		if(str_len(map_val(col,"title"))){ found++; };
		ret=xcat(ret,xstr("<th class='", map_val(col,"type"), "'>", map_val(col,"title"), "</th>", End), End); };
	return found ? ret : NULL;
};
char* row_td(void* row,map* cols){
	if(!is_map(row)){ return row; };
	char* ret=NULL;
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* col=map_id(cols,next1); char* n=map_key(cols, next1);
		void* cls=map_val(col,"class");
		if(cls){cls=xstr(" ",cls, End);};
		char* style=NULL;
		if(map_val(col,"style")){
			style=xstr(" style='", map_val(col,"style"), "'", End); };
		ret=xcat(ret,xstr("<td class='", map_val(col,"type"), cls, "'", style, ">", val_format(map_val(row,n),map_val(col,"type"),col), "</td>", End), End); };
	return ret;
};
map* rows_td(map* rows, map* cols){
	map* ret=new_vec();
	for(int next1=next(rows,-1,NULL,NULL); has_id(rows,next1); next1++){ void* r=map_id(rows,next1);
		vec_add(ret,row_td(r,cols)); };
	return ret;
};
