#line 2 "src/sql.fox"

#include <libstemmer.h>
#include <core.h>
#include <sql.h>
#include <unicode.h>
#include <sqlite3.h>
#include <playground.h>
//#include <foxparser.h>
#include <dir.h>

#pragma fox cc -lsqlite3

int _queries=0;

char* str_code(char* in){
	in=to_str(in,"",0);
	int len=str_len(in);
	if(!len){ return "_blank"; };
	char ret[65]={0};
	if(len>64){ len=64; };
	for(int i=0; i<len && i<64; i++){
		if(in[i]>='A' &&  in[i]<='Z'){ ret[i]=in[i]+'a'-'A'; }
		else if(!is_alphanum(in[i],NULL)){ ret[i]='_'; }
		else {ret[i]=in[i];}; };
	ret[len]='\0';
	if(!is_alpha(ret[0],NULL)){ return xstr("_", ret, End); };
	return str_dup(ret);
};
void* sql_value(char* sql,char* db,map* param){ return map_id(map_id(lite_exec(sql,db,param),0),0); };
map* sql_rows(char* sql,char* db,map* param){
	if(!strchr(sql,' ')){ sql=xstr("select * from ", sql, End); };
	return lite_exec(sql,db,param);
};
map* sql_row(char* sql,char* db,map* param){ return map_id(lite_exec(sql,db,param),0); };
map* sql_vector(char* sql,char* db,map* param){
	map* ret=new_vec();
	map* map_1=lite_exec(sql,db,param); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		vec_add(ret,map_id(v,0)); };
	return ret;
};
map* sql_pairs(char* sql,char* db,map* param){
	map* ret=new_map();
	map* map_1=lite_exec(sql,db,param); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		add(ret,map_id(v,0),map_len(v)>1 ? map_id(v,1) : map_id(v,0)); };
	return ret;
};
map* sql_vec(char* sql,char* db,map* param){
	map* ret=new_vec();
	map* map_1=lite_exec(sql,db,param); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		vec_add(ret,map_id(v,0)); };
	return ret;
};
char* sql_drop(char* tbl){
	return xstr("drop table if exists ", tbl, End);
};
void* sql_error(char* sql,char* db,sqlite3* conn){
	char* msg=xstr((char*)sqlite3_errmsg(conn), " ", db, "/", sql, End);
	sqlite3_close_v2(conn);
	return fox_error(msg,0);
};
map* sqls_exec(map* sqls,char* db){
	for(int next1=next(sqls,-1,NULL,NULL); has_id(sqls,next1); next1++){ void* sql=map_id(sqls,next1); lite_exec(sql,db,NULL); };
	return sqls;
};
map* parse_connection(char* in){ return xmap("type", "lite", "file", in, End); };
map* conn_db(char* db){
	if(!is_code(db)){ return parse_connection(db); };
	void* ret=map_val(map_val(_globals,"dbs"),db);
	if(!ret){ fox_error(xstr("SQLite connection ", db, " not found. Only ", map_join(map_keys(map_val(_globals,"dbs")),","), " defined", End),0); };
	if(is_str(ret)){ add(add_key(_globals,"dbs",Map),db,parse_connection(ret)); };
	return map_val(map_val(_globals,"dbs"),db);
};
map* sql_exec(char* sql,char* db,map* params){ return lite_exec(sql,db,params); };
void* lite_conn(char* db){
	char* cname=db;
	db=map_val(conn_db(db),"file");
	if(!db){ fox_error(xstr("query() connection ", str_quote(cname), " not found ", map_val(_globals,"dbs"), End),0); };
	if(!map_val(map_val(_globals,"conn"),db)){
		sqlite3* conn=map_val(map_val(_globals,"conn"),db);
		if(sqlite3_open_v2(db,&conn,SQLITE_OPEN_READWRITE|SQLITE_OPEN_URI,NULL)!=SQLITE_OK){
			return sql_error(NULL,db,conn); };
		add(add_key(_globals,"conn",Map),db,conn); };
	return map_val(map_val(_globals,"conn"),db);
};
map* lite_exec(char* sql,char* db,map* params){
	_queries++;
	sqlite3* conn=lite_conn(db);
	sqlite3_stmt* stm=NULL;
//	start_time()
	if(sqlite3_prepare_v2(conn,sql,-1,&stm,NULL)!=SQLITE_OK){
		return sql_error(sql,db,conn); };
	int i=0;
	for(int idx=next(params,-1,NULL,NULL); has_id(params,idx); idx++){ void* v=map_id(params,idx); char* k=map_key(params, idx);
		assert(is_str(k));
		if((i=sqlite3_bind_parameter_index(stm,xstr(":",k, End)))){
			sqlite3_bind_text(stm,i,to_str(v,"",0),-1,NULL); }; };
	int ncols=sqlite3_column_count(stm);
	map* cols=NULL;
	if(ncols){
		cols=new_vec();
		for(int i=0;i<ncols;i++){ vec_add(cols,str_dup((char*) sqlite3_column_name(stm,i))); }; };
	map* ret=NULL;
	while((i=sqlite3_step(stm))){
		if(i==SQLITE_ROW){
			if(!ret){ ret=new_vec(); };
			map* row=new_map();
			for(int i=0;i<ncols;i++) {add(row,map_id(cols,i),str_dup((char*)sqlite3_column_text(stm,i)));};
			vec_add(ret,row);
		}else if(i==SQLITE_BUSY){ continue; }
		else if(i==SQLITE_DONE){ break; }
		else {return sql_error(sql,db,conn);}; };
	if(sqlite3_finalize(stm)!=SQLITE_OK){ return sql_error(sql,db,conn); };
//	end_time()
	return ret;
};
void close_conns(){
	map* map_1=map_val(_globals,"conn"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* key=map_key(map_1, next1);
		add(add_key(_globals,"conn",Map),key,NULL);
		if(!val){ continue; };
		sqlite3_close_v2(val);
	};};
