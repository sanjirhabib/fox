#include "tree.h"
int run(map* args){
	add(add_key(_globals,"dbs",Map),"tree","tree.db");	
	db_sync("tree",2);
	sql_exec("delete from tree","tree",NULL);
	map* map_1=str_split(""
	"books\n"
	"fiction books\n"
	"nonfiction books\n"
	"action fiction\n"
	"romance fiction\n"
	"life nonfiction\n"
	"history nonfiction\n"
	"spy action\n"
	"war action\n"
	"love romance\n"
	"chickflik romance\n"
	"business life\n"
	"politics life\n"
	"ww2 history\n"
	"ww1 history"
	"","\n",0); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* line=map_id(map_1,next1);
		map* vals=str_split(line," ",0);
		row_insert(insert_tree(xmap(
			"id", map_id(vals,0),
			"parent", map_id(vals,1)
		, End),"tree","tree"),"tree","tree"); };
	char* sign=NULL;
	map* map_2=sql_rows("select * from tree","tree",NULL); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* row=map_id(map_2,next1);
		sign=xcat(sign,map_join(row,"\t"),"\n", End); };	
	px(sign,1);
	map* map_3=str_split(""
	"ww1 ww2\n"
	"ww1 history\n"
	"history politics\n"
	"life fiction\n"
	"life nonfiction\n"
	"history nonfiction\n"
	"nonfiction ww1"
	"","\n",0); for(int next1=next(map_3,-1,NULL,NULL); has_id(map_3,next1); next1++){ void* line=map_id(map_3,next1);
		map* vals=str_split(line," ",0);
		id_update(map_id(vals,0),"tree","tree",update_tree(map_id(vals,0),xmap("parent", map_id(vals,1), End),"tree","tree")); };
	char* sign2=NULL;
	map* map_4=sql_rows("select * from tree","tree",NULL); for(int next1=next(map_4,-1,NULL,NULL); has_id(map_4,next1); next1++){ void* row=map_id(map_4,next1);
		sign2=xcat(sign2,map_join(row,"\t"),"\n", End); };	
	if(!str_eq(sign,sign2)){
		px("Error",1);
		px(sign2,1); };
	return 0;
};
map* insert_tree(map* row, char* table, char* db){
	map* cols=tbl_cols(table,db);
	if(!map_val(cols,"mlft")){ return row; };
	void* pkey=map_id(cols_pkeys(cols),0);
	void* parent=map_val(row,"parent");
	if(!str_len(parent)){
		long long mrgt=str_int(sql_value(xstr("select mrgt from ", table, " order by mrgt desc limit 1", End),db,NULL));
		add(row,"mlft",int_str((mrgt+1)));
		add(row,"mrgt",int_str((mrgt+2)));
		return row; };
	map* curr=sql_row(xstr("select mrgt,mlft from ", table, " where ", pkey, "=:id", End),table, xmap("id", parent, End));
	sql_exec(xstr("update ", table, " set mrgt=mrgt+2 where mrgt>=", map_val(curr,"mrgt"), End),table,NULL);
	sql_exec(xstr("update ", table, " set mlft=mlft+2 where mlft>=", map_val(curr,"mrgt"), End),table,NULL);
	add(row,"mlft",int_str((str_int(map_val(curr,"mrgt")))));
	add(row,"mrgt",int_str((str_int(map_val(curr,"mrgt"))+1)));
	return row;
};
map* update_tree(char* id, map* row, char* table, char* db){
	if(!map_has_key(row,"parent")){ return row; };
	map* cols=tbl_cols(table,db);
	if(!map_val(cols,"mlft")){ return row; };
	void* pkey=map_id(cols_pkeys(cols),0);
	map* curr=sql_row(xstr("select mrgt,mlft,parent from ", table, " where ", pkey, "=:id", End),table, xmap("id", id, End));
	if(str_eq(map_val(row,"parent"),map_val(curr,"parent"))){ return row; };
	long long mrgt=str_int(map_val(curr,"mrgt"));
	long long mlft=str_int(map_val(curr,"mlft"));
	if(str_len(map_val(row,"parent"))){
		map* nlimit=sql_row(xstr("select mrgt,mlft from ", table, " where ", pkey, "=:id", End),table, xmap("id", map_val(row,"parent"), End));
		if(str_int(map_val(nlimit,"mrgt"))<=mrgt && str_int(map_val(nlimit,"mlft"))>=mlft){
			//moving to child, abort
			add(row,"parent",map_val(curr,"parent"));
			return row; }; };
	long long diff=mrgt-mlft+1;
	sql_exec(xstr("update ", table, " set mrgt=-mrgt,mlft=-mlft where mrgt<=", map_val(curr,"mrgt"), " and mlft>=", map_val(curr,"mlft"), End),table,NULL);
	sql_exec(xstr("update ", table, " set mrgt=mrgt-",int_str( diff), " where mrgt>", map_val(curr,"mrgt"), End),table,NULL);
	sql_exec(xstr("update ", table, " set mlft=mlft-",int_str( diff), " where mlft>", map_val(curr,"mrgt"), End),table,NULL);
	int add=0;
	if(!str_len(map_val(row,"parent"))){
		add=str_int(sql_value(xstr("select mrgt from ", table, " order by mrgt desc limit 1", End),db,NULL))-mlft+1;
	}else{
		void* right=sql_value(xstr("select mrgt from ", table, " where ", pkey, "=:id", End),table, xmap("id", map_val(row,"parent"), End));
		add=str_int(right)-mlft;
		sql_exec(xstr("update ", table, " set mrgt=mrgt+",int_str( diff), " where mrgt>=", right, End),table,NULL);
		sql_exec(xstr("update ", table, " set mlft=mlft+",int_str( diff), " where mlft>=", right, End),table,NULL); };
	if(add){
		sql_exec(xstr("update ", table, " set mrgt=-mrgt+",int_str( add), ", mlft=-mlft+",int_str( add), " where mrgt<0 and mlft<0", End),table,NULL); };
	map_del_key(map_del_key(row,"mrgt"),"mlft");		
	return row;
};
