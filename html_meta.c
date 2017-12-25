#include "/web/fox/html.h"
/* This is a generated file. To change it, edit function funcs_meta() in fox.c */
char* /web/fox/html_version(){
	return mstr("Fox: build: 66, date: 2017-12-21 20:10:45 [%s old]",time_ago("2017-12-21 20:10:45"), End);
};
void* /web/fox/html_invoke(map* v,char* name){
	unsigned long long idn=str_hash((unsigned char*)name);
	switch(idn){
//function calls
		case 0x377d48858b938b6: { return page_html(is_map(map_id(v,0))); break; };
		case 0x377d48858b6b73b: { return page_data(is_map(map_id(v,0))); break; };
		case 0x377d2883692a391: { return not_found(v->len>=0 ? is_str(map_id(v,0)) : NULL); break; };
		case 0x377d8a6bdce30a2: { return int_var(show_page(is_map(map_id(v,0)))); break; };
		case 0xd0b3a99a32a6: { return int_var(is_post()); break; };
		case 0x377d51703089604: { return post_data(); break; };
		case 0x1ae7553069d19c: { return name_tab(is_str(map_id(v,0))); break; };
		case 0xb887685: { return get(is_str(map_id(v,0)),v->len>=1 ? is_map(map_id(v,1)) : NULL); break; };
		case 0x377d8a6bdc6be93: { show_body(is_str(map_id(v,0)),is_map(map_id(v,1))); return NULL; break; };
		case 0xc07de40449cc31b8: { return int_var(days_in_feb(to_int(map_id(v,0)))); break; };
		case 0xd786f63dee23e971: { return int_var(days_in_month(to_int(map_id(v,0)),to_int(map_id(v,1)))); break; };
		case 0xd0b11f1c2624: { return add_tab(v->len>=0 ? is_str(map_id(v,0)) : NULL,v->len>=1 ? is_str(map_id(v,1)) : NULL); break; };
		case 0x6531f4aebdd: { return tz_dst(v->len>=0 ? is_str(map_id(v,0)) : NULL,v->len>=1 ? is_str(map_id(v,1)) : NULL); break; };
		case 0x377da9bed180479: { return double_var(tz_offset(v->len>=0 ? is_str(map_id(v,0)) : NULL,v->len>=1 ? is_str(map_id(v,1)) : NULL)); break; };
		case 0x6531f4b343e: { return tz_utc(is_str(map_id(v,0)),v->len>=1 ? is_str(map_id(v,1)) : NULL); break; };
		case 0x727241bd0a7e2a57: { return offset_utc(to_double(map_id(v,0))); break; };
		case 0x1ae6f2062bb40c: { return date_ymd(is_str(map_id(v,0)),map_id(v,1),map_id(v,2),map_id(v,3)); break; };
		case 0x72706b8c3eb9dcbb: { return date_human(is_str(map_id(v,0))); break; };
		case 0x1ae7981dfec10b: { return tz_human(to_double(map_id(v,0))); break; };
		case 0xd0b708a81b4c: { return tz_data(is_str(map_id(v,0)),v->len>=1 ? is_str(map_id(v,1)) : NULL); break; };
		case 0xc08bdddfa0bed25a: { return fts5_create(v->len>=0 ? is_str(map_id(v,0)) : NULL); break; };
		case 0xc0cf7e14c248e166: { save_caller(); return NULL; break; };
		case 0x1ae79fc0144588: { return url_back(); break; };
		case 0x377c84c18e0544d: { return form_html(is_map(map_id(v,0))); break; };
		case 0x377d48b9f1d7ad3: { return param_map(is_str(map_id(v,0))); break; };
		case 0xc08b0bb5e498dfc7: { return form_posted(is_map(map_id(v,0))); break; };
		case 0x377c4a7565f010e: { return ctrl_html(is_map(map_id(v,0))); break; };
		case 0xd0b501779092: { return name_id(is_str(map_id(v,0))); break; };
		case 0x377c84c18df890b: { return form_gets(is_map(map_id(v,0))); break; };
		case 0x377d8a6bdc8f1d9: { return int_var(show_form(is_map(map_id(v,0)))); break; };
		case 0xd0b3a99cffbf: { return int_var(is_user(is_str(map_id(v,0)))); break; };
		case 0x727006805942d604: { authorized(is_str(map_id(v,0))); return NULL; break; };
		case 0x377c473cd0b8eed: { return col_align(is_map(map_id(v,0))); break; };
		case 0xd0c8b9ccd30e473d: { return rows_data_html(is_map(map_id(v,0))); break; };
		case 0x377d7a71d954269: { return rows_data(is_map(map_id(v,0)),is_map(map_id(v,1))); break; };
		case 0x65319e69f89: { return row_id(is_map(map_id(v,0)),is_map(map_id(v,1))); break; };
		case 0xd0b656ba911c: { return row_ids(is_map(map_id(v,0)),is_map(map_id(v,1))); break; };
		case 0x17c954d73: { crud(is_str(map_id(v,0)),is_str(map_id(v,1))); return NULL; break; };
		case 0x1ae6f327051015: { return del_keys(is_map(map_id(v,0)),is_map(map_id(v,1))); break; };
		case 0x1ae78c8f840af7: { return str_bare(is_str(map_id(v,0)),v->len>=1 ? is_str(map_id(v,1)) : NULL); break; };
		case 0x1ae7b2c0011497: { return word_end(is_str(map_id(v,0)),to_int(map_id(v,1))); break; };

	};
	return fox_error(xstr("invoke(): Function ", name, " not defined", End),0);
};
map* /web/fox/html_reflect(){
	return xmap(
		"funcs", xmap("page_html",xmap("name","page_html","type","char*","decltype","char*","params",xmap("data",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"page_data",xmap("name","page_data","type","map*","decltype","map*","params",xmap("data",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"not_found",xmap("name","not_found","type","void*","decltype","void*","params",xmap("path",xmap("type","char*","default","NULL", End), End),"file","/web/fox/html.fox", End),"show_page",xmap("name","show_page","type","int","decltype","int","params",xmap("data",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"is_post",xmap("name","is_post","type","int","decltype","int","params",new_map(),"file","/web/fox/html.fox", End),"post_data",xmap("name","post_data","type","map*","decltype","map*","params",new_map(),"file","/web/fox/html.fox", End),"name_tab",xmap("name","name_tab","type","char*","decltype","char*","params",xmap("name",xmap("type","char*","default",NULL, End), End),"file","/web/fox/html.fox", End),"get",xmap("name","get","type","map*","decltype","map*","params",xmap("path",xmap("type","char*","default",NULL, End),"names",xmap("type","map*","default","NULL", End), End),"file","/web/fox/html.fox", End),"show_body",xmap("name","show_body","type","void","decltype","void","params",xmap("body",xmap("type","char*","default",NULL, End),"page",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"days_in_feb",xmap("name","days_in_feb","type","int","decltype","int","params",xmap("year",xmap("type","int","default",NULL, End), End),"file","/web/fox/html.fox", End),"days_in_month",xmap("name","days_in_month","type","int","decltype","int","params",xmap("month",xmap("type","int","default",NULL, End),"year",xmap("type","int","default",NULL, End), End),"file","/web/fox/html.fox", End),"add_tab",xmap("name","add_tab","type","char*","decltype","char*","params",xmap("path",xmap("type","char*","default","NULL", End),"name",xmap("type","char*","default","NULL", End), End),"file","/web/fox/html.fox", End),"tz_dst",xmap("name","tz_dst","type","char*","decltype","char*","params",xmap("tz",xmap("type","char*","default","NULL", End),"date",xmap("type","char*","default","NULL", End), End),"file","/web/fox/html.fox", End),"tz_offset",xmap("name","tz_offset","type","double","decltype","double","params",xmap("tz",xmap("type","char*","default","NULL", End),"date",xmap("type","char*","default","NULL", End), End),"file","/web/fox/html.fox", End),"tz_utc",xmap("name","tz_utc","type","char*","decltype","char*","params",xmap("tz",xmap("type","char*","default",NULL, End),"date",xmap("type","char*","default","NULL", End), End),"file","/web/fox/html.fox", End),"offset_utc",xmap("name","offset_utc","type","char*","decltype","char*","params",xmap("off",xmap("type","double","default",NULL, End), End),"file","/web/fox/html.fox", End),"date_ymd",xmap("name","date_ymd","type","char*","decltype","char*","params",xmap("in",xmap("type","char*","default",NULL, End),"year",xmap("type","int*","default",NULL, End),"month",xmap("type","int*","default",NULL, End),"day",xmap("type","int*","default",NULL, End), End),"file","/web/fox/html.fox", End),"date_human",xmap("name","date_human","type","char*","decltype","char*","params",xmap("in",xmap("type","char*","default",NULL, End), End),"file","/web/fox/html.fox", End),"tz_human",xmap("name","tz_human","type","char*","decltype","char*","params",xmap("tz",xmap("type","double","default",NULL, End), End),"file","/web/fox/html.fox", End),"tz_data",xmap("name","tz_data","type","map*","decltype","map*","params",xmap("name",xmap("type","char*","default",NULL, End),"date",xmap("type","char*","default","NULL", End), End),"file","/web/fox/html.fox", End),"fts5_create",xmap("name","fts5_create","type","char*","decltype","char*","params",xmap("db",xmap("type","char*","default","NULL", End), End),"file","/web/fox/html.fox", End),"save_caller",xmap("name","save_caller","type","void","decltype","void","params",new_map(),"file","/web/fox/html.fox", End),"url_back",xmap("name","url_back","type","char*","decltype","char*","params",new_map(),"file","/web/fox/html.fox", End),"form_html",xmap("name","form_html","type","char*","decltype","char*","params",xmap("mp",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"param_map",xmap("name","param_map","type","map*","decltype","map*","params",xmap("in",xmap("type","char*","default",NULL, End), End),"file","/web/fox/html.fox", End),"form_posted",xmap("name","form_posted","type","map*","decltype","map*","params",xmap("form",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"ctrl_html",xmap("name","ctrl_html","type","char*","decltype","char*","params",xmap("ctrl",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"name_id",xmap("name","name_id","type","char*","decltype","char*","params",xmap("name",xmap("type","char*","default",NULL, End), End),"file","/web/fox/html.fox", End),"form_gets",xmap("name","form_gets","type","map*","decltype","map*","params",xmap("form",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"show_form",xmap("name","show_form","type","int","decltype","int","params",xmap("data",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"is_user",xmap("name","is_user","type","int","decltype","int","params",xmap("who",xmap("type","char*","default",NULL, End), End),"file","/web/fox/html.fox", End),"authorized",xmap("name","authorized","type","void","decltype","void","params",xmap("who",xmap("type","char*","default",NULL, End), End),"file","/web/fox/html.fox", End),"col_align",xmap("name","col_align","type","char*","decltype","char*","params",xmap("col",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"rows_data_html",xmap("name","rows_data_html","type","char*","decltype","char*","params",xmap("in",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"rows_data",xmap("name","rows_data","type","map*","decltype","map*","params",xmap("rows",xmap("type","map*","default",NULL, End),"cols",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"row_id",xmap("name","row_id","type","char*","decltype","char*","params",xmap("row",xmap("type","map*","default",NULL, End),"cols",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"row_ids",xmap("name","row_ids","type","map*","decltype","map*","params",xmap("row",xmap("type","map*","default",NULL, End),"cols",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"crud",xmap("name","crud","type","void","decltype","void","params",xmap("sql",xmap("type","char*","default",NULL, End),"db",xmap("type","char*","default",NULL, End), End),"file","/web/fox/html.fox", End),"del_keys",xmap("name","del_keys","type","map*","decltype","map*","params",xmap("mp",xmap("type","map*","default",NULL, End),"keys",xmap("type","map*","default",NULL, End), End),"file","/web/fox/html.fox", End),"str_bare",xmap("name","str_bare","type","char*","decltype","char*","params",xmap("in",xmap("type","char*","default",NULL, End),"accept",xmap("type","char*","default","NULL", End), End),"file","/web/fox/html.fox", End),"word_end",xmap("name","word_end","type","char*","decltype","char*","params",xmap("in",xmap("type","char*","default",NULL, End),"len",xmap("type","int","default",NULL, End), End),"file","/web/fox/html.fox", End), End),
		"macros", new_map(),
		"structs", new_map()
	, End);
};

