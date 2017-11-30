#include "main.h"
int main(int argc, char** argv, char** env){ gc_start(); map* args=argv_map(argv, argc);
	add(_globals,"req",http_req());
	void* path=map_val(map_val(map_val(_globals,"req"),"path"),"next");
	if(str_eq(path,"/")){
		return show_page(xmap(
			"title", "Lima's Page",
			"tab", "Lima",
			"body", ""
				"This is a good page"
				"",
			"menu",xmap(
				"source/", "View Source",
				"http://github.com/sanjirhabib/fox", "Github", End), End)); };
//	if path==="login/"
//		form={
//			cols
//				username code
//				password password
//		}
	return not_found(xstr(map_val(map_val(map_val(_globals,"req"),"path"),"full"), " [", path, "] <pre>", map_val(_globals,"req"), "</pre>", End));	
};
int not_found(char* path){ http_out(xstr("The requested content ", path, " was not found on the server.", End),"404 Not Found","text/html; charset=utf-8",NULL); return -404; };
int show_page(map* data){ px(page(page_data(data)),1); return 0; };
map* page_data(map* data){
	if(!(map_val(data,"base"))){add(data,"base","/habibur");};
	if(!(map_val(data,"title"))){add(data,"title","Habib's Site");};
	if(!(map_val(data,"tab"))){add(data,"tab",map_val(data,"title"));};
	if(!(map_val(data,"logo"))){add(data,"logo",xstr(map_val(data,"base"),"/logo.jpg", End));};
	add(data,"home","/");
	char* head=NULL;
	map* map_1=
	xvec("/res/bootstrap.css","/res/bootstrap-responsive.css","/res/jquery.js","/res/bootstrap.js", End); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* v=map_id(map_1,next1);
		if(str_eq(sub_str(v,-4,0),".css")){ head=xcat(head,xstr("<link rel='stylesheet' href='", v, "'></link>\n", End), End); }
		else {head=xcat(head,xstr("<script async src='", v, "'></script>\n", End), End);}; };
	add(data,"header",head);
	add(data,"footer",xstr("", 
	"Run Time: ",int_str( run_time()), "ms=",int_str( gc_time()), "/",int_str( run_time()-total_time()-gc_time()), "/",int_str( total_time()), "\n", 
	"Malloc: ",int_str( total_kb()), "KB. Total: ",int_str( max_mem()/1024), "KB.\n", 
	"GC: ",int_str( gc_runs()), "runs.", 
	"", End));
	char* tabs=NULL;
	map* lddata=xmap("@context", "http://schema.org", "@type", "BreadcrumbList", End);
	map* path=xmap("Home", "/", "Fox", "/fox/", End);
	int i=1;
	for(int next1=next(path,-1,NULL,NULL); has_id(path,next1); next1++){ void* url=map_id(path,next1); char*  name=map_key(path, next1);
		tabs=xcat(tabs,xstr("<li><a href='", url, "'>", name, "</a></li>\n", End), End);
		vec_add(add_key(lddata,"itemListElement",Vector),xmap(
			"@type", "ListItem",
			"position",int_var( i++), "item", xmap( "@id", full_url(url), "name", full_url(url) , End)
		, End)); };
	tabs=xstr(tabs, "<li class='active'><a href='#'>", map_val(data,"tab"), "</a></li>", End);
	vec_add(add_key(lddata,"itemListElement",Vector),xmap(
		"@type", "ListItem",
		"position",int_var( i++), "item", xmap( "@id", full_url(map_val(_globals,"path")), "name", full_url(map_val(_globals,"path")) , End)
	, End));
	add(data,"tabs",xstr("<div><ul class='nav nav-tabs'>", tabs, "</ul></div>", End));
	add(data,"ldmenu",xstr("<script type='application/ld+json'>", lddata, "</script>", End));
	char* body=NULL;
	map* map_2=map_val(data,"menu"); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* name=map_id(map_2,next1); char*  url=map_key(map_2, next1);
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
	if(map_val(map_val(_globals,"req"),"user")){
		login=xstr("<span class='pull-right'>", map_val(map_val(_globals,"req"),"user"), " as ", map_val(map_val(_globals,"req"),"role"), " | <a href='", full_url(NULL), "logout/'>Logout</a> &nbsp; </span>", End);
	}else{
		login=xstr("<span class='pull-right' style='margin-right:3em;'><a href='", full_url(NULL), "login/'>Login</a> | <a href='", full_url(NULL), "register/'>Register</a></span>", End); };
	add(data,"login",login);
	return data;
};
char* page(map* data){
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
