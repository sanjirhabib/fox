#line 2 "/web/fox/www/main.fox"
#include "main.h"
int main(int argc, char** argv, char** env){ gc_start(); map* args=argv_map(argv, argc);
	map* req=http_req();
	void* path=map_val(map_val(req,"path"),"next");
	add(add_key(_globals,"dbs",Map),"user","/web/habibur/user.db");
	add(add_key(_globals,"dbs",Map),"blog","blog.db");
	sess_init();
	add(_globals,"tabs",xmap("Home", home_url(NULL), End));
	if(str_eq(path,"/")){
		map* rows=sql_rows("select * from blog where status!='draft' order by datetime desc limit 10","blog",NULL);
		char* body=NULL;
		for(int next1=next(rows,-1,NULL,NULL); has_id(rows,next1); next1++){ void* row=map_id(rows,next1);
			body=xcat(body,xstr("", 
			"<h3><a href=\"", map_val(row,"id"), "/\">", map_val(row,"title"), "</a></h3>\n", 
			"<div class='posted'>Posted: ", time_ago(map_val(row,"datetime")), " ago.</div>\n", 
			"<div>", map_val(row,"html"), "</div>\n", 
			"<hr noshade=noshade>", 
			"", End), End); };
		char* css=""
		"hr {width:90%;margin:auto;height:1px;margin-bottom:4em;margin-top:3em;color:gray;}\n"
		"h1, h2, h3, h4, h5, h6 {text-align: left;}\n"
		".posted {text-align: right;}\n"
		"div {text-align:justify;}"
		"";
		map* data=xmap(
			"title", "Habib's Blog",
			"body", body,
			"css", css
		, End);
		if(is_user("habib")){
			add(data,"menu",xmap(
				"add/", "+Write New",
				"draft/", "Drafts"
			, End)); };
		show_page(data); };
	map* parts=NULL;
	if((parts=re_match(path,"^([0-9]+)/"))){
		void* id=map_id(parts,1);
		add(add_key(_globals,"tabs",Map),"Post",home_url((xstr(id,"/", End))));
		map* row=sql_row("select * from blog where id=:id","blog",xmap("id", id, End));
		if(!row){ not_found(xstr("Blog ", id, End)); };
		if(re_match(path,"^[0-9]+/$")){
			map* data=xmap(
				"title", map_val(row,"title"),
				"tab", "Home",
				"body", map_val(row,"html")
			, End);
			if(is_user("habib")){
				add(data,"menu",xmap(
					"edit/", "Edit"
				, End)); };
			show_page(data); };
		if(re_match(path,"^[0-9]+/edit/$")){
			add(add_key(_globals,"tabs",Map),"Edit",home_url((xstr(id,"/edit/", End))));
			map* form=xmap(
				"title", "Post A New Blog",
				"cols",xmap(
					"title", xmap("name", "title", "type", "text", End),
					"body", xmap("name", "body", "type", "source", End), End),
				"buttons",xmap(
					"publish", xmap("name", "publish", End),
					"draft", xmap("name", "draft", End), End),
				"vals", row
			, End);
			map* data=form_data(form);
			if(map_val(data,"draft")){
				add(data,"status","draft");			
				add(data,"html",fox_markdown(map_val(data,"body")));
				id_update(id,"blog","blog",data);
				http_redirect("../");
			}else if(map_val(data,"publish")){
				add(data,"status","live");	
				add(data,"html",fox_markdown(map_val(data,"body")));
				id_update(id,"blog","blog",data);
				http_redirect(home_url(xstr(id, "/", End))); };
			show_form(form); }; };
	if(str_eq(path,"draft/")){
		add(add_key(_globals,"tabs",Map),"Drafts",home_url("draft/"));
		char* body=NULL;
		map* map_1=sql_rows("select * from blog where status='draft' order by datetime desc","blog",NULL); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* row=map_id(map_1,next1);
			body=xcat(body,xstr("", 
			"	<div class=\"row\">\n", 
			"\t\t<div class=\"span8\">", map_val(row,"title"), " [<a href=\"../", map_val(row,"id"), "/edit/\">Edit</a>]</div>\n", 
			"	</div>", 
			"", End), End); };
		show_page(xmap(
			"title", "Saved Drafts",
			"body", ((body ? body : "No drafts found"))
		, End)); };
	if(str_eq(path,"add/")){
		add(add_key(_globals,"tabs",Map),"Add New",home_url("add/"));
		authorized("habib");
		map* form=xmap(
			"title", "Post A New Blog",
			"cols",xmap(
				"title", xmap("name", "title", "type", "text", End),
				"body", xmap("name", "body", "type", "source", End), End),
			"buttons",xmap(
				"publish", xmap("name", "publish", End),
				"draft", xmap("name", "draft", End), End)
		, End);
		map* data=form_data(form);
		if(map_val(data,"save")){
			add(data,"status","draft");			
			add(data,"datetime",datetime());
			add(data,"html",fox_markdown(map_val(data,"body")));
			row_insert(data,"blog","blog");
		}else if(map_val(data,"publish")){
			add(data,"status","live");	
			add(data,"datetime",datetime());
			add(data,"html",fox_markdown(map_val(data,"body")));
			row_insert(data,"blog","blog");
			http_redirect(home_url(NULL)); };
		show_form(form); };
	if(str_eq(path,"login/")){
		add(add_key(_globals,"tabs",Map),"Login",home_url("login/"));
		map* form=xmap(
			"title", "Login",
			"cols",xmap(
				"username", xmap("name", "username", "type", "code", End),
				"password", xmap("name", "password", "type", "password", End), End),
			"buttons",xmap(
				"login", xmap("name", "login", End), End)
		, End);
		save_refere();
		map* data=form_data(form);
		if(data){
			map* row=sql_row("select * from user where username=:name","user",xmap("name", map_val(data,"username"), End));
			if(!row){
				add(add_key(form,"error",Map),"username","User not found");
			}else if(!str_eq(map_val(row,"password"),md5(xstr(map_val(data,"password"),"habibur.com", End)))){
				add(add_key(form,"error",Map),"password","Password mismatched");
			}else{
				add(add_key(_globals,"sess",Map),"user",map_val(row,"username"));
				add(add_key(_globals,"sess",Map),"role",map_val(row,"role"));
				http_redirect(referer());
				add(form,"msg","Login OK");
			};
			if(map_val(form,"error")){
				add(form,"msg","Login failed"); }; };
		show_form(form); };
	if(str_eq(path,"logout/")){
		add(_globals,"sess",new_map());
		http_redirect(referer()); };
	not_found(xstr(map_val(map_val(map_val(_globals,"req"),"path"),"full"), " [", path, "]", End));	
};
void save_refere(){
	char* refer=referer();
	void* req=map_val(_globals,"req");
	if(!str_eq(map_val(req,"method"),"get") || !refer || map_val(map_val(req,"gets"),"_refer") || str_end(refer,map_val(req,"path"))){ return; };
	add(add_key(req,"gets",Map),"_refer",refer);
	http_redirect(xstr("./?", map_amps(map_val(req,"gets")), End));
};
char* home_url(char* path){ return xstr(full_url(map_val(map_val(map_val(_globals,"req"),"path"),"home")),path, End); };
char* referer(){
	return (map_val(map_val(map_val(_globals,"req"),"gets"),"_refer") ? map_val(map_val(map_val(_globals,"req"),"gets"),"_refer") : (map_val(parse_url(map_val(env_vars(),"HTTP_REFERER")),"path") ? map_val(parse_url(map_val(env_vars(),"HTTP_REFERER")),"path") : home_url(NULL)));
};
char* name_id(char* name){
	return name;
};
char* ctrl_html(map* ctrl){
	void* id=(map_val(ctrl,"id") ? map_val(ctrl,"id") : name_id(map_val(ctrl,"name")));
	if(str_eq(map_val(ctrl,"type"),"code")){
		return xstr("", 
		"<input type=text name=", map_val(ctrl,"name"), " value=\"", map_val(ctrl,"value"), "\" id=", id, " size=\"input-", (map_val(ctrl,"size") ? map_val(ctrl,"size") : "medium"), "\"></input>", 
		"", End); };
	if(str_eq(map_val(ctrl,"type"),"text")){
		return xstr("", 
		"<input type=text name=", map_val(ctrl,"name"), " value=\"", map_val(ctrl,"value"), "\" id=", id, " size=\"input-", (map_val(ctrl,"size") ? map_val(ctrl,"size") : "large"), "\"></input>", 
		"", End); };
	if(str_eq(map_val(ctrl,"type"),"para")){
		return xstr("", 
		"<textarea class=\"source\" style=\"width:20em; height:10em;\" id=", id, " name='", map_val(ctrl,"name"), "'>", map_val(ctrl,"value"), "</textarea>", 
		"", End); };
	if(str_eq(map_val(ctrl,"type"),"source")){
		static int codeincluded=0;
		if(!codeincluded){
			vec_add(add_key(_globals,"jsfile",Vector),"jquery.textarea.js");
			vec_add(add_key(_globals,"css",Vector),".source {font-family:courier;overflow:auto;width:100%;height:30em;}");
			vec_add(add_key(_globals,"js",Vector),xstr("$(\"#",id,"\").tabby()", End));
			codeincluded=1; };
		return xstr("", 
		"<textarea id=", id, " class='source' wrap='off' name='", map_val(ctrl,"name"), "'>", map_val(ctrl,"value"), "</textarea>", 
		"", End); };
	if(str_eq(map_val(ctrl,"type"),"password")){
		return xstr("", 
		"<input type=password name=", map_val(ctrl,"name"), " value=\"", map_val(ctrl,"value"), "\" id=", id, "></input>", 
		"", End); };
	return xstr("control type ", map_val(ctrl,"type"), " unspported. please provide support in ctrl_html()", End);
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
int not_found(char* path){ http_out(xstr("The requested content ", (path ? path : map_val(map_val(_globals,"req"),"path")), " was not found on the server.", End),"404 Not Found","text/html; charset=utf-8",NULL); return -404; };
int show_page(map* data){ http_out(page_html(page_data(data)),"200 OK","text/html; charset=utf-8",NULL); return 0; };
map* param_map(char* in){
	map* ret=new_map();
	map* map_1=str_split(in,"&",0); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1);
		map* two=str_split(val,"=",2);
		add(ret,map_id(two,0),map_id(two,1)); };
	return ret;
};
map* form_data(map* form){
	if(!str_eq(map_val(map_val(_globals,"req"),"method"),"post")){ return NULL; };
	map* param=amps_map(map_val(map_val(_globals,"req"),"post"));
	map* map_1=map_val(form,"buttons"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ char* key=map_key(map_1, next1);
		if(map_val(param,key)){
			map* map_1=map_val(form,"cols"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* key=map_key(map_1, next1);
				add(val,"value",map_val(param,key)); };
			return param; }; };
	return NULL;
};
int show_form(map* data){
	add(data,"body",form_html(data));
	return show_page(data);
};
map* page_data(map* data){
	if(!(map_val(data,"base"))){add(data,"base","/habibur");};
	if(!(map_val(data,"title"))){add(data,"title","Habib's Site");};
	if(!(map_val(data,"tab"))){add(data,"tab",map_val(data,"title"));};
	if(!(map_val(data,"logo"))){add(data,"logo",xstr(map_val(data,"base"),"/logo.jpg", End));};
	add(data,"home","/");
	if(map_val(data,"css")){add(data,"css",xstr("<style>\n", map_val(data,"css"), "\n</style>", End));};
	char* head=NULL;
	map* map_1=
	xvec("/res/bootstrap.css","/res/bootstrap-responsive.css","/res/jquery.js","/res/bootstrap.js", End); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* v=map_id(map_1,next1);
		if(str_eq(sub_str(v,-4,0),".css")){ head=xcat(head,xstr("<link rel='stylesheet' href='", v, "'></link>\n", End), End); }
		else {head=xcat(head,xstr("<script async src='", v, "'></script>\n", End), End);}; };
	map* map_2=map_val(_globals,"jsfile"); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* v=map_id(map_2,next1);
		head=xcat(head,xstr("<script async src='", v, "'></script>\n", End), End); };
	add(data,"header",xstr(head,map_val(data,"css")
		,((map_val(_globals,"css") ? xstr("<style>\n",map_join(map_val(_globals,"css"),"\n"),"</style>", End) : NULL)), End));
	add(data,"footer",xstr("", 
	"Run Time: ",int_str( run_time()), "ms=",int_str( gc_time()), "/",int_str( run_time()-total_time()-gc_time()), "/",int_str( total_time()), "\n", 
	"Malloc: ",int_str( total_kb()), "KB. Total: ",int_str( max_mem()/1024), "KB.\n", 
	"GC: ",int_str( gc_runs()), "runs.", 
	"", End));
	char* footer=xcat(footer,((map_val(_globals,"js") ? xstr("<script>\n",map_join(map_val(_globals,"js"),"\n"),"</script>", End) : NULL)), End);
	char* tabs=NULL;
	map* lddata=xmap("@context", "http://schema.org", "@type", "BreadcrumbList", End);
	int i=1;
	map* map_3=map_val(_globals,"tabs"); for(int  idx=next(map_3,-1,NULL,NULL); has_id(map_3, idx);  idx++){ void* url=map_id(map_3, idx); char*  name=map_key(map_3,  idx);
		char* active=NULL;
		if(idx==map_len(map_val(_globals,"tabs"))-1){
			active=" class='active'"; };			
		tabs=xcat(tabs,xstr("<li", active, "><a href='", url, "'>", name, "</a></li>\n", End), End);
			
		vec_add(add_key(lddata,"itemListElement",Vector),xmap(
			"@type", "ListItem",
			"position",int_var( i++), "item", xmap( "@id", full_url(url), "name", full_url(url) , End)
		, End)); };
	vec_add(add_key(lddata,"itemListElement",Vector),xmap(
		"@type", "ListItem",
		"position",int_var( i++), "item", xmap( "@id", full_url(map_val(map_val(_globals,"req"),"path")), "name", full_url(map_val(map_val(_globals,"req"),"path")) , End)
	, End));
	if(map_len(map_val(_globals,"tabs"))>1){
		add(data,"tabs",xstr("<div><ul class='nav nav-tabs'>", tabs, "</ul></div>", End)); };
	add(data,"ldmenu",xstr("<script type='application/ld+json'>", lddata, "</script>", End));
	char* body=NULL;
	map* map_4=map_val(data,"menu"); for(int next1=next(map_4,-1,NULL,NULL); has_id(map_4,next1); next1++){ void* name=map_id(map_4,next1); char*  url=map_key(map_4, next1);
		body=xcat(body,xstr("<li><a style='overflow:hidden;text-overflow:ellipsis;white-space:nowrap;' href='", url, "'>", name, "</a></li>\n", End), End); };
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
		login=xstr("<span class='pull-right'>", map_val(map_val(_globals,"sess"),"user"), " as ", map_val(map_val(_globals,"sess"),"role"), " | <a href='", home_url(NULL), "logout/'>Logout</a> &nbsp; </span>", End);
	}else{
		login=xstr("<span class='pull-right' style='margin-right:3em;'><a href='", home_url(NULL), "login/'>Login</a> | <a href='", home_url(NULL), "register/'>Register</a></span>", End); };
	add(data,"login",login);
	return data;
};
char* page_html(map* data){
	return xstr("", 
	"<!DOCTYPE html>\n", 
	"<html><head><meta charset='utf-8'>\n", 
	"<title>", map_val(data,"title"), "</title>\n", 
	"<script type=\"application/ld+json\">\n", 
	"{\n", 
	"\"@context\": \"http://schema.org\",\n", 
	"\"@type\": \"WebSite\",\n", 
	"\"name\": \"Habib's Site\",\n", 
	"\"alternateName\": \"Sanjir Habib\",\n", 
	"url\": \"", map_val(map_val(_globals,"req"),"path"), "\"", 
	"}\n", 
	"</script>\n", 
	"<link rel=\"apple-touch-icon\" href=\"/apple-touch-icon.png\">\n", 
	"<meta name=\"application-name\" content=\"habib\"/>\n", 
	"<meta name=\"msapplication-square70x70logo\" content=\"/small.jpg\"/>\n", 
	"<meta name=\"msapplication-square150x150logo\" content=\"/medium.jpg\"/>\n", 
	"<meta name=\"msapplication-wide310x150logo\" content=\"/wide.jpg\"/>\n", 
	"<meta name=\"msapplication-square310x310logo\" content=\"/large.jpg\"/>\n", 
	"<meta name=\"msapplication-TileColor\" content=\"#fff\"/>\n", 
	"<meta name=\"description\" content=\"", map_val(data,"desc"), "\"/>\n", 
	"<meta name=\"keywords\" content=\"", map_val(data,"keyword"), "\"/>\n", 
	"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n", 
	"<meta name=\"og:image\" content=\"", map_val(data,"logo"), "\">\n", 
	"<meta name=\"og:description\" content=\"", map_val(data,"desc"), "\">\n", 
	"<meta name=\"og:title\" content=\"", map_val(data,"title"), "\">\n", 
	map_val(data,"header"), "\n", 
	"<link rel='icon' href='", map_val(data,"logo"), "'></link>\n", 
	"</head><body>\n", 
	"<div class='row-fluid alert-success'><span class='span8'>", map_val(data,"msg"), "</span>", map_val(data,"login"), "</div>\n", 
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
	"<hr><div class=footer style='color:#333;font-size:small;text-align:center;'>\n", 
	"&copy; Habib &lt;habib@habibur.com&gt;. ", map_val(data,"hits"), " ", map_val(data,"footer"), "\n", 
	"</div>\n", 
	"</div>\n", 
	"</body></html>\n", 
	"", 
	"", End);
};
char* show_port(){
	if(str_eq(map_val(map_val(_globals,"req"),"protocol"),"http") && str_eq(map_val(map_val(_globals,"req"),"port"),"80")){ return NULL; };
	return xstr(":",map_val(map_val(_globals,"req"),"port"), End);
};
char* full_url(char* url){
	return xstr(map_val(map_val(_globals,"req"),"protocol"),"://",map_val(map_val(_globals,"req"),"server"),show_port(),"/",str_ltrim(url,"/"), End);
};
int is_user(char* who){ return str_eq(map_val(map_val(_globals,"sess"),"user"),who) || str_eq(map_val(map_val(_globals,"sess"),"role"),who); };
void authorized(char* who){ if(!is_user(who)){ not_found(map_val(map_val(_globals,"req"),"path")); }; };
