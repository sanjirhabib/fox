#line 2 "src/crud.fox"

#include <core.h>
#include <crud.h>
#include <sql.h>
#include <schema.h>
#include <http.h>
#include <html.h>

char* row_id(map* row, map* cols){ return str_url(map_join(row_ids(row,cols),"\t")); };
map* row_ids(map* row, map* cols){
	map* ret=cols_pkeys(cols);
	for(int next1=next(ret,-1,NULL,NULL); has_id(ret,next1); next1++){ void* name=map_id(ret,next1);
		add(ret,name,map_val(row,name)); };
	return ret;
};
char* crud(char* sql,char* db){
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
		return show_body(rows_data_html(data),xmap(
			"title", xstr(str_title(sql_table(sql))," List", End),
			"width",int_var( 4
		), End)); };
	if(map_val(get(xstr(curr, "add/", End),NULL),"end")){
		name_tab("Add");
		map* cols=del_keys(sql_cols(sql,db,NULL),xvec("lft","rgt", End));
		map* form=xmap(
			"title", xstr("Add New ", str_title(table), End),
			"cols", cols,
			"buttons",xmap(
				"save", xmap("name", "save", "type", NULL, End), End)
		, End);
		map* data=form_posted(form);
		if(data){
			row_insert(data,table, db);
			http_redirect(url_back(),"Record Added"); };
		return show_form(form_gets(form)); };
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
		return show_form(form); };
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
			return http_redirect(url_back(),"Cancelled");
		}else if(map_val(data,"delete")){
			id_delete(map_val(path,"id"),table,db);
			return http_redirect(url_back(),"Record Deleted"); };
		return show_form(form); };
	return not_found(NULL);
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
char* fts5_create(char* db){
	map* ret=xvec(
	"drop table if exists search",
	"create virtual table search using fts5(table,key,title,body,thumb)"
	, End);
	if(db){ sqls_exec(ret,db); };
	return map_join(ret,";\n");
};
char* col_align(map* col){
	if(map_val(col,"sql") || map_val(col,"list")){ return NULL; };
	if(str_eq(map_type(xmap(
		"text", "text",
		"int",NULL
	, End),map_val(col,"type")),"text")){ return NULL; };
	return " style='text-align:right;'";
};
