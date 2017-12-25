#include "html.h"

char* page_html(map* data){
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
	"\t\"url\": \"", full_url(map_val(map_val(map_val(_globals,"req"),"path"),"full")), "\"\n", 
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
map* page_data(map* data){
	if(!(map_val(data,"title"))){add(data,"title","Habib's Site");};
	if(!(map_val(data,"tab"))){add(data,"tab",map_val(data,"title"));};
	if(!(map_val(data,"logo"))){add(data,"logo",home_url("logo.jpg"));};
	if(!(map_val(data,"home"))){add(data,"home",home_url(NULL));};
	if(map_val(data,"css")){add(data,"css",xstr("<style>\n", map_val(data,"css"), "\n</style>", End));};
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
		,((map_val(_globals,"css") ? xstr("<style>\n",map_join(map_val(_globals,"css"),"\n"),"</style>", End) : NULL)), End));
	if(!(map_val(data,"desc"))){add(data,"desc",map_val(map_val(data,"meta"),"description"));};
	if(!(map_val(map_val(data,"meta"),"description"))){add(add_key(data,"meta",Map),"description",map_val(data,"title"));};
	add(add_key(data,"meta",Map),"og:description",map_val(map_val(data,"meta"),"description"));
	add(add_key(data,"meta",Map),"og:title",map_val(data,"title"));
	add(add_key(data,"meta",Map),"og:image",full_url(map_val(data,"logo")));
	map* map_3=map_val(data,"meta"); for(int next1=next(map_3,-1,NULL,NULL); has_id(map_3,next1); next1++){ void* val=map_id(map_3,next1); char*  key=map_key(map_3, next1);
		add(data,"header",xcat(map_val(data,"header"),xstr("<meta name='", key, "' content='", val, "'>\n", End), End)); };
	if(map_val(map_val(map_val(_globals,"req"),"get"),"_print")){ return data; };
	add(data,"footer",xstr("", 
	"Run Time: ",int_str( run_time()), "ms=",int_str( gc_time()), "GC+",int_str( run_time()-total_time()-gc_time()), "Code+",int_str( total_time()), "DB\n", 
	"Heap: ", int_kb(_gcdata.max_mem,"B"), ". Stack: ", int_str(_gcdata.max_roots), ", Total: ", int_kb(max_mem(),"B"), ".\n", 
	"GC: ",int_str( gc_runs()), "runs. Queries: ", int_str(_queries), ".", 
	"", End));
	add(data,"footer",xcat(map_val(data,"footer"),((map_val(_globals,"js") ? xstr("<script>\n",map_join(map_val(_globals,"js"),"\n"),"</script>", End) : NULL)), End));
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
		body=xcat(body,xstr("<li><a class='menu' href='", url, "'>", name, "</a></li>\n", End), End); };
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
void* not_found(char* path){ http_out(xstr("The requested content ", (path ? path : map_val(map_val(map_val(_globals,"req"),"path"),"full")), " was not found on the server.", End),"404 Not Found","text/html; charset=utf-8",NULL); return NULL; };
int show_page(map* data){ http_out(page_html(page_data(data)),"200 OK","text/html; charset=utf-8",NULL); return 0; };
int is_post(){ return str_eq(map_val(map_val(_globals,"req"),"method"),"post"); };
map* post_data(){
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
			char* name=map_val(header_map(v2),"name");
			if(name){
				add(ret,name,sub_str(map_id(mp3,1),0,-4));
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
	map* ret=regexp(map_val(map_val(map_val(_globals,"req"),"path"),"next"),xstr("^",path, End));
	if(!ret){ return NULL; };
	void* ret2=map_val(_globals,"paths");
	if(!ret2){ add(_globals,"paths",new_map()); ret2=map_val(_globals,"paths"); };
	if(names){
		for(int i=1; i<map_len(ret) && i<=map_len(names); i++){
			add(ret2,map_id(names,i-1),map_id(ret,i)); }; };
	add(ret2,"matched",map_id(ret,0));
	add(ret2,"end",regexp(map_val(map_val(map_val(_globals,"req"),"path"),"next"),xstr("^",path,"$", End)) ? "yes" : NULL);
	return ret2;
};
void show_body(char* body,map* page){
	add(page,"body",body);
	show_page(page);
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
char* fts5_create(char* db){
	map* ret=xvec(
	"drop table if exists search",
	"create virtual table search using fts5(table,key,title,body,thumb)"
	, End);
	if(db){ sqls_exec(ret,db); };
	return map_join(ret,";\n");
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
map* form_posted(map* form){
	save_caller();
	if(!str_eq(map_val(map_val(_globals,"req"),"method"),"post")){ return NULL; };
	map* param=amps_map(map_val(map_val(_globals,"req"),"post"));
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
		"soruce", "source",
		"password", "password"
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
	if(str_eq(type,"source")){
		static int codeincluded=0;
		if(!codeincluded){
			vec_add(add_key(_globals,"jsfile",Vector),"/res/jquery.textarea.js");
			vec_add(add_key(_globals,"css",Vector),".source {font-family:courier;overflow:auto;width:100%;height:30em;text-wrap:wrap;}");
			vec_add(add_key(_globals,"js",Vector),xstr("$(\"#", id, "\").tabby()", End));
			codeincluded=1; };
		return xstr("", 
		"<textarea id=", id, " class='source' wrap='off' name='", map_val(ctrl,"name"), "'>", map_val(ctrl,"value"), "</textarea>", 
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
int show_form(map* data){
	add(data,"body",form_html(data));
	return show_page(data);
};
int is_user(char* who){ return str_eq(map_val(map_val(_globals,"sess"),"user"),who) || str_eq(map_val(map_val(_globals,"sess"),"role"),who); };
void authorized(char* who){ if(!is_user(who)){ not_found(map_val(map_val(_globals,"req"),"path")); }; };
char* col_align(map* col){
	if(map_val(col,"sql") || map_val(col,"list")){ return NULL; };
	if(str_eq(map_type(xmap(
		"text", "text",
		"int",xmap(End)
	, End),map_val(col,"type")),"text")){ return NULL; };
	return " style='text-align:right;'";
};
char* rows_data_html(map* in){
	return xstr("", 
	"<table class='table table-condensed table-bordered table-striped'>\n", 
	"<thead>\n", 
	"<tr>", map_val(in,"head"), "</tr>\n", 
	"</thead>\n", 
	"<tbody>\n", 
	"<tr>", map_join(map_val(in,"body"),"</tr>\n<tr>"), "</tr>\n", 
	"</tbody>\n", 
	"</table>\n", 
	"", 
	"", End);
};
map* rows_data(map* rows, map* cols){
	char* head=NULL;
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* val=map_id(cols,next1);
		head=xcat(head,xstr("<th", col_align(val), ">", (map_val(val,"label") ? map_val(val,"label") : str_title(map_val(val,"name"))), "</th>", End), End); };
	map* body=new_map();
	for(int next1=next(rows,-1,NULL,NULL); has_id(rows,next1); next1++){ void* row=map_id(rows,next1);
		char* s=NULL;
		for(int next1=next(row,-1,NULL,NULL); has_id(row,next1); next1++){ void* val=map_id(row,next1); char*  key=map_key(row, next1);
			s=xcat(s,xstr("<td", col_align(map_val(cols,key)), ">", val, "</td>", End), End); };
		add(body,row_id(row,cols),s); };
	return xmap("head", head, "body", body, End);
};
char* row_id(map* row, map* cols){ return str_url(map_join(row_ids(row,cols),"\t")); };
map* row_ids(map* row, map* cols){
	map* ret=cols_pkeys(cols);
	for(int next1=next(ret,-1,NULL,NULL); has_id(ret,next1); next1++){ void* name=map_id(ret,next1);
		add(ret,name,map_val(row,name)); };
	return ret;
};
void crud(char* sql,char* db){
	void* path=map_val(_globals,"paths");	
	void* curr=map_val(path,"matched");
	char* table=sql_table(sql);
	if(map_val(path,"end")){
		map* rows=sql_rows(sql,db,NULL);
		map* cols=sql_cols(sql,db,NULL);		
		map* data=rows_data(rows,cols);
		add(data,"head",xstr(""
		"<th style='text-align:center;'>\n"
		"<div class=\"btn-group\">\n"
		"<button class=\"btn btn-mini\">Action</button>\n"
		"<button class=\"btn btn-mini dropdown-toggle\" data-toggle=\"dropdown\">\n"
		"<span class=\"caret\"></span>\n"
		"</button>\n"
		"<ul class=\"dropdown-menu\">\n"
		"	<li><a href=\"add/\">Add New</a></li>\n"
		"</ul>\n"
		"</div>		\n"
		"</th>\n"
		""
		"",map_val(data,"head"), End));
		map* map_1=map_val(data,"body"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* row=map_id(map_1,next1); char* key=map_key(map_1, next1);
			add(add_key(data,"body",Map),key,xstr(xstr("", 
			"<td style='text-align:center;'>\n", 
			"<div class=\"btn-group\">\n", 
			"<button class=\"btn btn-mini dropdown-toggle\" data-toggle=\"dropdown\">\n", 
			"<span class=\"caret\"></span>\n", 
			"</button>\n", 
			"<ul class=\"dropdown-menu\">\n", 
			"\t<li><a href=\"id.", key, "/edit/\">Edit</a></li>\n", 
			"\t<li><a href=\"id.", key, "/delete/\">Delete</a></li>\n", 
			"</ul>\n", 
			"</div>		\n", 
			"</td>\n", 
			"", 
			"", End),row, End)); };
		show_body(rows_data_html(data),xmap(
			"title", xstr(str_title(sql_table(sql))," List", End),
			"width",int_var( 4
		), End));
		return; };
	if(map_val(get(xstr(curr, "add/", End),NULL),"end")){
		name_tab("Add");
		map* cols=del_keys(sql_cols(sql,db,NULL),xvec("lft","rgt", End));
		map* form=xmap(
			"title", xstr("Edit ", str_title(table), End),
			"cols", cols,
			"buttons",xmap(
				"save", xmap("name", "save", "type", NULL, End), End)
		, End);
		map* data=form_posted(form);
		if(data){
			row_insert(data,table, db);
			http_redirect(url_back(),"Record Added"); };
		show_form(form); };
	get(xstr(curr, "id.([^/]+)/", End),xvec("id", End));
	if(map_val(get(xstr(curr, "id.[^/]+/edit/", End),NULL),"end")){
		name_tab("Edit");
		table=sql_table(sql);
		map* row=id_row(map_val(path,"id"),table, db);
		map* cols=del_keys(sql_cols(sql,db,NULL),xvec("lft","rgt", End));
		map* form=xmap(
			"title", xstr("Edit ", str_title(table), End),
			"cols", cols,
			"vals", row,
			"buttons",xmap(
				"save", xmap("name", "save", "type", NULL, End), End)
		, End);
		map* data=form_posted(form);
		if(data){
			id_update(row_ids(row,tbl_cols(table,db)),table, db, data);
			http_redirect(url_back(),"Record Updated"); };
		show_form(form); };
	if(map_val(get(xstr(curr, "id.[^/]+/delete/", End),NULL),"end")){
		name_tab("Delete");
		map* form=xmap(
			"title", "Delete This Record?",
			"buttons",xmap(
				"delete", xmap("name", "delete", "type", NULL, End),
				"cancel", xmap("name", "cancel", "type", NULL, End), End)
		, End);
		map* data=form_posted(form);
		if(map_val(data,"cancel")){
			http_redirect(url_back(),"Cancelled");
		}else if(map_val(data,"delete")){
			id_delete(map_val(path,"id"),table,db);
			http_redirect(url_back(),"Record Deleted"); };
		show_form(form); };
	not_found(NULL);
};
map* del_keys(map* mp,map* keys){
	for(int next1=next(keys,-1,NULL,NULL); has_id(keys,next1); next1++){ void* key=map_id(keys,next1); map_compact(map_del(mp,map_has_key(mp,key)-1,1)); };
	return mp;
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
