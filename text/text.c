#include <libstemmer.h>
#include "text.h"

int run(map* args){
	if(exec_cmdline(args)){ return 0; };
	add(add_key(_globals,"dbs",Map),"search","search.db");
	db_init_tokenizer("search");
	px("habib------",1);
	sql_exec("drop table if exists search","search",NULL);
	sql_exec("create virtual table search using fts5(body, tokenize='habib')","search",NULL);
	sql_exec(xstr("insert into search (body) values ('", map_id(args,2), "')", End),"search",NULL);
	sql_exec(xstr("insert into search (body) values ('", map_id(args,1), "')", End),"search",NULL);
	sql_exec(xstr("insert into search (body) values ('", map_id(args,2), "')", End),"search",NULL);
	px(sql_rows(xstr("select * from search where search match '", map_id(args,3), "' order by rank", End),"search",NULL),1);
	px("unicode61------",1);
	sql_exec("drop table if exists search","search",NULL);
	sql_exec("create virtual table search using fts5(body, tokenize='unicode61')","search",NULL);
	sql_exec(xstr("insert into search (body) values ('", map_id(args,2), "')", End),"search",NULL);
	sql_exec(xstr("insert into search (body) values ('", map_id(args,1), "')", End),"search",NULL);
	sql_exec(xstr("insert into search (body) values ('", map_id(args,2), "')", End),"search",NULL);
	px(sql_rows(xstr("select * from search where search match '", map_id(args,3), "' order by rank", End),"search",NULL),1);
	return 0;
};
