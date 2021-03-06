#line 2 "src/schema.fox"

#include <core.h>
#include <sql.h>
#include <schema.h>
#include <tsv.h>
#include <foxparser.h>

//int run(map* args){
//	_globals.dbs.nuaim="/web/habibur/kitab/db/nuaim.db"
//	//:nuaim.schema_write("nuaim.schema")
////	"nuaim.schema".schema_load(:nuaim).px()
//	:nuaim.schema_sql("nuaim.schema")
//	:nuaim.schema_sync("nuaim.schema")
//	return 0
//}
void schema_sync(char* db,char* file){ sqls_exec(tbls_sync_sqls(schema_load(file,NULL),lite_schema(db)),db); };
void schema_sql(char* db,char* file){ px(map_join(tbls_sync_sqls(schema_load(file,NULL),lite_schema(db)),";\n"),1); };
char* schema_write(char* db, char* file){ return write_file(schema_str(lite_schema(db)),file,0,1); };
map* schema_load(char* file,char* db){
	map* ret=new_map();
	map* cols=new_map();
	char* name=NULL;
	map* map_1=tsv_load(file,0); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* row=map_id(map_1,next1);
		if(str_len(map_val(row,"table"))){
			if(name){ add(ret,name,cols_table(cols,name,db)); };
			name=map_val(row,"table");
			cols=new_map();
			continue; };
		void* op=map_id(str_cols(map_val(row,"cols")),0);
		add(cols,map_val(op,"name"),op); };
	if(name){ add(ret,name,cols_table(cols,name,db)); };
	return ret;
};
char* schema_str(map* in){
	map* ret=new_vec();
	for(int next1=next(in,-1,NULL,NULL); has_id(in,next1); next1++){ void* prop=map_id(in,next1); char* tbl=map_key(in, next1);
		vec_add(ret,xmap("table", tbl, "cols", "", End));
		map* map_1=map_val(prop,"cols"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* col=map_id(map_1,next1);
			vec_add(ret,xmap("cols", col_str(col) , End)); }; }; 
	return rows_str(ret,str_cols("table,cols"));
};
char* rows_str(map* in,void* cols){
	if(!in || !map_len(in)){ return NULL; };
	cols=map_keys(str_cols(cols));
	if(!cols){ cols=map_keys(map_id(in,0)); };
	if(!cols){ return NULL; };
	char* ret=xstr(map_join(cols,"\t"),"\n", End);
	for(int next1=next(in,-1,NULL,NULL); has_id(in,next1); next1++){ void* row=map_id(in,next1);
		map* rs=new_vec();
		for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* col=map_id(cols,next1);
			vec_add(rs,map_val(row,col)); };
		ret=xcat(ret,map_join(rs,"\t"),"\n", End); };
	return ret;
};
char* col_str(map* in){
	if(!in){ return NULL; };
	char* ret=str_dup(map_val(in,"name"));
	for(int next1=next(in,-1,NULL,NULL); has_id(in,next1); next1++){ void* v=map_id(in,next1); char*  n=map_key(in, next1);
		if(is_word(n,"db table name")){ continue; };
		if(str_eq(n,"size") && type_size(map_val(in,"type"))==to_int(v)){ continue; };
		ret=xcat(ret,xstr(" ", n, "=", v, End), End); };
	return ret;
};
//map* lite_schema(char* db){
//	ret={}
//	db.lite_tables().each tbl
//		ret[tbl]=tbl.lite_cols(db)
//	return ret
//}
//map* lite_tables(char* db){
//	return "select name from sqlite_master where type='table' and name not in ('sqlite_sequence', '_syncing', 'search', 'search_data', 'search_idx', 'search_content', 'search_docsize', 'search_config') order by 1"
//		.sql_pairs(db)
//}
//map* lite_cols(char* tbl,char* db){
//	ret={}
//	"pragma table_info ($tbl)".sql_rows(db).each row
//		name=row.name.str_lower()
//		ret[name].name=name
//		type=row.type.str_split("(")
//		ret[name].type=type[0]
//		if type[1]
//			ret[name].size=type[1].str_trim(")").to_int()
//		if row.pk.to_int()
//			ret[name].pkey=:pkey
//		if !row.notnull.to_int()
//			ret[name].isnull=1
//		if row.dflt_value.str_len() && row.dflt_value !== "''"
//			ret[name].default=row.dflt_value
//	return ret
//}
char* sql_rename(char* from,char* into){ return xstr("alter table ", from, " rename to ", into, End); };
int type_distance(char* type1,char* type2){
	if(str_eq(type1,type2)){ return 0; };
	map* subs1=subtypes(type1);
	map* subs2=subtypes(type2);
	int len1=map_len(subs1);
	int len2=map_len(subs2);
	int i=0;
	while(i<len1 && i<len2){
		i++;
		if(!str_eq(map_id(subs1,i-1),map_id(subs2,i-1))){ i--; break; }; };
	return len1 + len2 - 2 * i;
};
map* cols_match(map* from, map* into){
	map* ret=new_map();
	map* ret2=new_map();
	for(int i=next(into,-1,NULL,NULL); has_id(into,i); i++){ void* op=map_id(into,i); char* f=map_key(into, i);
		if(map_val(from,f)){
			add(ret,f,f);
			add(ret2,f,f); }; };
	for(int i1=next(into,-1,NULL,NULL); has_id(into,i1); i1++){ void* op1=map_id(into,i1); char* f1=map_key(into, i1);
		if(map_val(ret,f1)){ continue; };
		int minid=0;
		int mindist=0;
		for(int i2=next(from,-1,NULL,NULL); has_id(from,i2); i2++){ void* op2=map_id(from,i2); char* f2=map_key(from, i2);
			if(map_val(ret2,f2)){ continue; };
			int dist=type_distance(map_val(op2,"type"),map_val(op1,"type"));
			if(!minid){ minid=i2; mindist=dist; }
			else if(dist<mindist){ minid=i2; }; };
		if(!minid){ break; };
		add(ret,f1,map_key(from,minid));
	};
	return ret;
};
char* nearest_table(map* tbls,map* tbl){
	map* match=new_map();
	for(int next1=next(tbls,-1,NULL,NULL); has_id(tbls,next1); next1++){ void* t=map_id(tbls,next1); char* name=map_key(tbls, next1);
		int score=abs(map_len(map_val(t,"cols"))-map_len(map_val(tbl,"cols")));
		map* map_1=map_val(tbl,"cols"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* col=map_id(map_1,next1); char* f=map_key(map_1, next1);
			if(!str_eq(map_val(map_val(map_val(t,"cols"),f),"type"),map_val(col,"type"))){ score--; }; };
		add(match,name,int_var(score)); };
	if(!match->len){ return NULL; };
	map_sort(match,-1);
	return map_key(match,0);
};
map* lite_trigger_slno(char* name, char* pkey, char* by){
	char* nby=(by ? xstr(" and ", by, "=new.", by, End) : NULL);
	char* oby=(by ? xstr(" and ", by, "=old.", by, End) : NULL);
	if(by){by=xstr(" where ", by, "=new.", by, End);};
	return xvec(
		xstr("drop trigger if exists ", name, "_slno_insert_null", End),
		xstr("create trigger ", name, "_slno_insert_null after insert on ", name, " when new.slno+0=0 or abs(new.slno+0)>(select count(*) from ", name, by, ") begin\n", 
		"\tupdate ", name, " set slno=1+(select count(*) from ", name, " where ", pkey, "!=new.", pkey, nby, ") where ", pkey, "=new.", pkey, ";\n", 
		"end", End),
		xstr("drop trigger if exists ", name, "_slno_insert_negetive", End),
		xstr("create trigger ", name, "_slno_insert_negetive after insert on ", name, " when new.slno<0 and abs(new.slno)<=(select count(*) from ", name, by, ") begin\n", 
		"\tupdate ", name, " set slno=slno+.5 where slno>=(select count(*) from ", name, by, ")+new.slno", nby, ";\n", 
		"\tupdate ", name, " set slno=slno+.5 where slno>=(select count(*) from ", name, by, ")+new.slno", nby, ";\n", 
		"\tupdate ", name, " set slno=(select count(*) from ", name, by, ")+new.slno+1 where ", pkey, "=new.", pkey, ";\n", 
		"end", End),
		xstr("drop trigger if exists ", name, "_slno_insert_positive", End),
		xstr("create trigger ", name, "_slno_insert_positive after insert on ", name, " when new.slno>0 and new.slno<=(select count(*) from ", name, by, ") begin\n", 
		"\tupdate ", name, " set slno=slno+.5 where slno>=new.slno and ", pkey, "!=new.", pkey, nby, ";\n", 
		"\tupdate ", name, " set slno=slno+.5 where slno>=new.slno and ", pkey, "!=new.", pkey, nby, ";\n", 
		"end", End),
		xstr("create trigger ", name, "_slno_delete after delete on ", name, " begin\n", 
		"\tupdate ", name, " set slno=slno-.5 where slno>old.slno", oby, ";\n", 
		"\tupdate ", name, " set slno=slno-.5 where slno>old.slno", oby, ";\n", 
		"end", End),
		xstr("create trigger ", name, "_slno_update_null after update on ", name, " when new.slno is null or new.slno=0 or new.slno='' or abs(new.slno)>(select count(*) from ", name, by, ") begin\n", 
		"\tupdate ", name, " set slno=slno-.5 where slno>old.slno", oby, ";\n", 
		"\tupdate ", name, " set slno=slno-.5 where slno>old.slno", oby, ";\n", 
		"\tupdate ", name, " set slno=(select count(*) from ", name, by, ") where ", pkey, "=new.", pkey, ";\n", 
		"end", End),
		xstr("create trigger ", name, "_slno_update_negetive after update on ", name, " when new.slno<0 and new.slno>=-1*(select count(*) from ", name, by, ") begin\n", 
		"\tupdate ", name, " set slno=slno-.5 where slno>old.slno", oby, ";\n", 
		"\tupdate ", name, " set slno=slno-.5 where slno>old.slno", oby, ";\n", 
		"\tupdate ", name, " set slno=slno+.5 where slno>=(select count(*) from ", name, by, ")+new.slno", nby, ";\n", 
		"\tupdate ", name, " set slno=slno+.5 where slno>=(select count(*) from ", name, by, ")+new.slno", nby, ";\n", 
		"\tupdate ", name, " set slno=(select count(*) from ", name, by, ")+new.slno where ", pkey, "=new.", pkey, ";\n", 
		"end", End),
		xstr("create trigger ", name, "_slno_update_positive after update on ", name, " when new.slno>0 and old.slno>0 and abs(new.slno-old.slno)>=1 and new.slno<=(select count(*) from ", name, by, ") begin\n", 
		"\tupdate ", name, " set slno=slno-.5 where slno>old.slno and ", pkey, "!=new.", pkey, oby, ";\n", 
		"\tupdate ", name, " set slno=slno-.5 where slno>old.slno and ", pkey, "!=new.", pkey, oby, ";\n", 
		"\tupdate ", name, " set slno=slno+.5 where slno>=new.slno and ", pkey, "!=new.", pkey, nby, ";\n", 
		"\tupdate ", name, " set slno=slno+.5 where slno>=new.slno and ", pkey, "!=new.", pkey, nby, ";\n", 
		"end;\n", 
		"", End), End);
};
map* lite_trigger_tree(char* name,char* pkey){
	return xvec(
		xstr("drop trigger if exists ", name, "_tree_insert_null", End),
		
		xstr("create trigger ", name, "_tree_insert_null after insert on ", name, " when new.parent is null or new.parent='' begin\n", 
		"update ", name, " set lft=(select coalesce(max(rgt),0) from ", name, " where ", pkey, "!=new.", pkey, ")+1,rgt=(select coalesce(max(rgt),0) from ", name, " where ", pkey, "!=new.", pkey, ")+2 where ", pkey, "=new.", pkey, ";\n", 
		"end", End),
	
		xstr("drop trigger if exists ", name, "_tree_insert", End),

		xstr("create trigger ", name, "_tree_insert after insert on ", name, " when new.parent is not null and length(new.parent)>0 begin\n", 
		"update ", name, " set lft=(select rgt from ", name, " where ", pkey, "=new.parent),rgt=(select rgt from ", name, " where ", pkey, "=new.parent)+1 where ", pkey, "=new.", pkey, ";\n", 
		"update ", name, " set lft=lft+2 where lft>(select rgt from ", name, " where ", pkey, "=new.parent);\n", 
		"update ", name, " set rgt=rgt+2 where rgt>=(select rgt from ", name, " where ", pkey, "=new.parent) and ", pkey, "!=new.", pkey, ";\n", 
		"end", End),

		xstr("drop trigger if exists ", name, "_tree_delete", End),

		xstr("create trigger ", name, "_tree_delete before delete on ", name, " begin\n", 
		"update ", name, " set lft=lft-(old.rgt-old.lft)-1 where lft>old.rgt;\n", 
		"update ", name, " set rgt=rgt-(old.rgt-old.lft)-1 where rgt>old.rgt;\n", 
		"delete from ", name, " where lft>old.lft and lft<old.rgt;\n", 
		"end", End),

		xstr("drop trigger if exists ", name, "_tree_update_null", End),

		xstr("create trigger ", name, "_tree_update_null after update of parent on ", name, " when new.parent!=old.parent and (new.parent is null or length(new.parent)=0) begin\n", 
		"update ", name, " set lft=lft+(select max(rgt) from ", name, ")-old.lft+1,rgt=rgt+(select max(rgt) from ", name, ")-old.lft+1 where lft>=old.lft and lft<=old.rgt;\n", 
		"update ", name, " set lft=lft-(old.rgt-old.lft)-1 where lft>old.rgt;\n", 
		"update ", name, " set rgt=rgt-(old.rgt-old.lft)-1 where rgt>old.rgt;\n", 
		"end", End),
	
		xstr("drop trigger if exists ", name, "_tree_update_right", End),

		xstr("create trigger ", name, "_tree_update_right after update of parent on ", name, " when new.parent is not null and length(new.parent)>0 and old.lft<(select lft from ", name, " where ", pkey, "=new.parent) begin\n", 
		"update ", name, " set lft=case when lft>old.rgt and lft<(select rgt from ", name, " where ", pkey, "=new.parent) then lft-old.rgt+old.lft-1 when lft>=old.lft and lft<=old.rgt then lft+(select rgt from ", name, " where ", pkey, "=new.parent)-old.rgt-1 else lft end;\n", 
		"update ", name, " set rgt=case when rgt>old.rgt and rgt<(select rgt from ", name, " where ", pkey, "=new.parent) then rgt-old.rgt+old.lft-1 when rgt>=old.lft and rgt<=old.rgt then rgt+(select rgt from ", name, " where ", pkey, "=new.parent)-old.rgt-1 else rgt end;\n", 
		"end", End),
		
		xstr("drop trigger if exists ", name, "_tree_update_left", End),

		xstr("create trigger ", name, "_tree_update_left after update of parent on ", name, " when new.parent is not null and length(new.parent)>0 and old.lft>(select lft from ", name, " where ", pkey, "=new.parent) begin\n", 
		"update ", name, " set lft=case when lft<old.lft and lft>=(select rgt from ", name, " where ", pkey, "=new.parent) then lft+old.rgt-old.lft+1 when lft>=old.lft and lft<=old.rgt then lft-old.lft+(select rgt from ", name, " where ", pkey, "=new.parent) else lft end;\n", 
		"update ", name, " set rgt=case when rgt<old.lft and rgt>=(select lft from ", name, " where ", pkey, "=old.", pkey, ") then rgt+old.rgt-old.lft+1 when rgt>=old.lft and rgt<=old.rgt then rgt-old.lft+(select lft from ", name, " where ", pkey, "=old.", pkey, ") else rgt end;\n", 
		"end", End)
	, End);
};

map* tbls_sync_sqls(map* new_tbls,map* old_tbls){
	if(!new_tbls){ fox_error("sync_sql() final table list is blank!",0); };
	map* newtbls=new_map();
	map* oldtbls=new_map();
	map_compact(map_del_key(old_tbls,"_syncing"));
	map_compact(map_del_key(new_tbls,"search"));
	for(int next1=next(old_tbls,-1,NULL,NULL); has_id(old_tbls,next1); next1++){ void* val=map_id(old_tbls,next1); char* key=map_key(old_tbls, next1); if(!map_val(new_tbls,key)){ add(oldtbls,key,val); }; };
	map* match=new_map();
	for(int next1=next(new_tbls,-1,NULL,NULL); has_id(new_tbls,next1); next1++){ void* val=map_id(new_tbls,next1); char* key=map_key(new_tbls, next1);
		if(!map_val(old_tbls,key)){ add(newtbls,key,val); }
		else {add(match,key,key);}; };
	for(int next1=next(newtbls,-1,NULL,NULL); has_id(newtbls,next1); next1++){ void* tbl=map_id(newtbls,next1); char* name=map_key(newtbls, next1);
		if(!oldtbls->len){ break; };
		add(match,name,nearest_table(oldtbls,tbl));
		map_compact(map_del_key(oldtbls,map_val(match,name))); };
	map* sqls=new_vec();
	for(int next1=next(match,-1,NULL,NULL); has_id(match,next1); next1++){ void* oldt=map_id(match,next1); char* newt=map_key(match, next1);
		map* changes=sync_sqls(map_val(new_tbls,newt),map_val(old_tbls,oldt));
		if(!str_eq(oldt,newt)){
			px(xstr("= ", oldt, " renamed ", newt, End),1);
		}else if(changes){
			px(xstr("= ", newt, " updated", End),1); };
		vec_merge(sqls,changes); };
	for(int next1=next(newtbls,-1,NULL,NULL); has_id(newtbls,next1); next1++){ void* newt=map_id(newtbls,next1); char* name=map_key(newtbls, next1);
		px(xstr("+ ", name, " created", End),1);
		vec_add(sqls,create_sql(newt,NULL));
		vec_merge(sqls,create_index_sqls(newt)); };
//		sqls.vec_merge(newt.tbl_trigger_sqls())
	for(int next1=next(oldtbls,-1,NULL,NULL); has_id(oldtbls,next1); next1++){ void* oldt=map_id(oldtbls,next1); char* name=map_key(oldtbls, next1);
		px(xstr("- ", name, " dropped", End),1);
		vec_add(sqls,drop_sql(map_val(oldt,"name"))); };
	return sqls;
};
map* tbl_cols(char* table,char* db){ return map_val(map_val(lite_schema(db),table),"cols"); };
map* sync_sqls(map* newt,map* oldt){
	if(str_eq(create_sql(oldt,NULL),create_sql(newt,NULL))){ return NULL; };	
	map* match=cols_match(map_val(oldt,"cols"),map_val(newt,"cols"));
	map* ret=new_vec();
	vec_add(ret,drop_sql("_syncing"));
	vec_add(ret,create_sql(newt,"_syncing"));
	vec_add(ret,xstr("insert into _syncing (",map_join(map_keys(match),", "),") select ",map_join(match,", ")," from ",map_val(oldt,"name"), End));
	vec_add(ret,drop_sql(map_val(oldt,"name")));
	vec_add(ret,sql_rename("_syncing",map_val(newt,"name")));
	vec_merge(
		vec_merge(
		ret,create_index_sqls(newt)),tbl_trigger_sqls(newt));
	return ret;	
};
map* trigger_sqls(char* tbl,char* db){ return tbl_trigger_sqls(map_val(lite_schema(db),tbl)); };
void* id_update(void* ids,char* tbl,char* db,map* row){
	ids=id_ids(ids,tbl,db);
	for(int next1=next(ids,-1,NULL,NULL); has_id(ids,next1); next1++){ void* val=map_id(ids,next1); char* f=map_key(ids, next1);
		if(map_val(row,f) && !str_eq(map_val(row,f),val)){
			map* map_1=tbl_referred_by(tbl,db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* ft=map_id(map_1,next1);
				lite_exec(xstr("update ", map_val(ft,"table"), " set ", map_val(ft,"f"), "=:newval where ", map_val(ft,"f"), "=:oldval", End),db,xmap("newval", map_val(row,f), "oldval", val, End)); }; }; };
	map* cols=tbl_cols(tbl,db);
	if(!cols){ return fox_error(xstr("Table ", tbl, " not found", End),0); };
	map* fld=new_vec();
	for(int idx=next(cols,-1,NULL,NULL); has_id(cols,idx); idx++){ char* k=map_key(cols, idx);
		if(!map_val(row,k)){ continue; };
		vec_add(fld,xstr(k, "=:", k, End)); };
	map* v1=new_vec();
	for(int idx2=next(ids,-1,NULL,NULL); has_id(ids,idx2); idx2++){ void* v2=map_id(ids,idx2); char* k2=map_key(ids, idx2);
		vec_add(v1,xstr(k2, "=:_old_", k2, End));
		add(row,xstr("_old_", k2, End),v2); };
	return lite_exec(xstr("update ", tbl, " set ", map_join(fld,", "), " where ", map_join(v1," and "), End),db,row);
};
int row_insert(map* row,char* tbl,char* db){
	map* cols=tbl_cols(tbl,db);
	if(!cols){ return to_int(fox_error(xstr("Table ", tbl, " not found", End),0)); };
	map* fld=new_vec();
	for(int idx=next(cols,-1,NULL,NULL); has_id(cols,idx); idx++){ char* k=map_key(cols, idx);
		if(!map_val(row,k)){ continue; };
		vec_add(fld,k); };
	lite_exec(xstr("insert into ", tbl, " ('", map_join(fld,"', '"), "') values (:", map_join(fld,", :"), ")", End),db,row);
	return to_int(sql_value("select last_insert_rowid()",db,NULL));
};
map* toks_sql_params(map* toks, map* ret){
	for(int next1=next(toks,-1,NULL,NULL); has_id(toks,next1); next1++){ void* val=map_id(toks,next1);
		if(is_map(val)){ toks_sql_params(val,ret); continue; };
		if(fox_at(val,0)==':'){ add(ret,sub_str(val,1,-2147483648),sub_str(val,1,-2147483648)); }; };
	return ret;
};
char* data1_str(void* in){
	char* ret=NULL;
	if(is_vec(in)){
		ret=xcat(ret,"[", End);
		for(int idx=next(in,-1,NULL,NULL); has_id(in,idx); idx++){ void* val=map_id(in,idx);
			ret=xcat(ret,data1_str(val), End);
			if(idx<map_len(in)){ ret=xcat(ret,", ", End); }; };
		ret=xcat(ret,"]", End);
		return ret; };
	if(is_map(in)){
		ret=xcat(ret,"{", End);
		for(int  idx=next(in,-1,NULL,NULL); has_id(in, idx);  idx++){ void* val=map_id(in, idx); char*  key=map_key(in,  idx);
			ret=xcat(ret,data1_str(key), End);
			if(val){ ret=xcat(ret," ", End); };
			ret=xcat(ret,data1_str(val), End);
			if(idx<map_len(in)){ ret=xcat(ret,", ", End); }; };
		ret=xcat(ret,"}", End);
		return ret; };
	if(is_str(in)){
		if(str_chrs(in," \t\n\r")){ return str_quote(in); }; };
	return in;
};
char* data_str(map* in, int level){
	char* ret=NULL;
	for(int next1=next(in,-1,NULL,NULL); has_id(in,next1); next1++){ void* val=map_id(in,next1); char*  key=map_key(in, next1);
		if(is_map(val)){
			if(str_eq(map_val(val,"name"),key) && map_val(val,"type")){
				ret=xcat(ret,str_times("\t",level),key," - ",map_val(val,"type"), End);
				for(int next1=next(val,-1,NULL,NULL); has_id(val,next1); next1++){ void* v=map_id(val,next1); char*  k=map_key(val, next1);
					if(is_word(k,"name type")){ continue; };
					ret=xcat(ret,", ",k," ",data1_str(v), End); };
				ret=xcat(ret,"\n", End);
			}else{
				ret=xcat(ret,str_times("\t",level),key,"\n",data_str(val,level+1),"\n", End); };
			continue; };
		ret=xcat(ret,str_times("\t",level),key," ",val,"\n", End); };
	return ret;
};
//char* callback_php(map* param,char* name) => return param.call_php(name)
char* fkey_show(char* sql,char* db,char* id){
	char* sign=xstr(sql, "/", id, End);
	char* ret=NULL;
	if((ret=cache(sign,"str_show",NULL))){ return ret; };
	return cache(sign,"str_show",map_val(sql_row(sql_add_where(fkey_sql(sql,db),xmap("id", ":id", End)),db,xmap("id", id, End)),"name"));
};
int fkey_valid(char* sql,char* db,char* id){
	return fkey_show(sql,db,id) ? 1 : 0;
};

// habib -- schema
map* lite_schema(char* db){
	if(!db){ return NULL; };
	if(map_val(map_val(map_val(_globals,"dbs"),db),"schema")){ return map_val(map_val(map_val(_globals,"dbs"),db),"schema"); };
	if(is_str(map_val(map_val(_globals,"dbs"),db))){ add(add_key(_globals,"dbs",Map),db,parse_connection(map_val(map_val(_globals,"dbs"),db))); };
	map* ret=db_tables(db);	
	add(add_key(add_key(_globals,"dbs",Map),db,Map),"schema",ret);
	return ret;
};
map* sql_order(char* sql){
	map* mp=sql_map(sql);
	map* ret=new_map();
	map* map_1=map_val(map_id(mp,mp->len-1),"order"); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* val=map_id(map_1,idx);
		void* mp1=val;
		char* col=sql_str(map_id(mp1,0));
		char* ord=sql_str(map_id(mp1,1));
		int iord=1;
		if(str_eq(ord,"desc")){ iord=-1; };
		add(ret,col,int_var(iord));
	};
	return ret;
};
map* crosstab_cols(map* cols,char* sql,char* db,map* params){
	map* mp=sql_map(sql);
	if(!map_val(mp,"crosstab")){ return NULL; };
	void* col1=map_val(map_id(cols,0),"alias");
	void* col2_name=map_val(map_id(cols,cols->len-2),"alias");
	char* col2_expr=map_join(map_val(map_id(cols,cols->len-2),"expr"),"");
	void* sum=map_id(cols,cols->len-1);
	void* table=map_val(map_val(cols,sum),"table");
	map* order=xmap((col2_expr), map_val(map_val(mp,"order"),col2_expr), End);
	sql=xstr("select distinct ",col2_expr," as val",re_from(map_val(mp,"from")),re_where(map_val(mp,"where")),re_order(order), End);
	map* rows=sql_pairs(sql,db,NULL);
	map* ret=new_map();
	void* fsum=map_val(sum,"sum");
	if(str_eq(fsum,"*")){ fsum="1"; };
	for(int i=next(rows,-1,NULL,NULL); has_id(rows,i); i++){ void* val=map_id(rows,i);
		char* f=NULL;
		if(map_val(mp,"prefix")){
			f=xstr(map_val(mp,"prefix"),"_",str_code(val), End);
		}else{
			f=str_code(val); };
		if(params){ add(params,f,val); };
		add(ret,f,xmap("type", "amount", "label", to_str(val,"",0), "alias", f, "expr", sql_toks(xstr("sum(if(", col2_expr, "=:", f, ",", fsum, ",0))", End)), "db", db, "table", table,"aggregate", map_val(sum,"aggregate"), End)); };
	return ret;
};
map* sql_select_cols(char* sql,char* db,map* params){
	map* ret=new_map();
	if(is_code(sql)){
		ret=map_dup(tbl_cols(sql,db));
		for(int i6=next(ret,-1,NULL,NULL); has_id(ret,i6); i6++){ void* v6 =map_id(ret,i6); char* k6=map_key(ret, i6); add(v6,"expr",xvec(k6, End)); };
		return ret; };
	map* cols=sql_cls(sql,"select");
	if(!db){
		char* tbl=sql_table(sql);
		for(int idx=next(cols,-1,NULL,NULL); has_id(cols,idx); idx++){ void* v=map_id(cols,idx); char* k=map_key(cols, idx);
			add(ret,k,xmap("alias", k, "expr", v, "table", tbl, End)); };
		return ret; };
	for(int idx=next(cols,-1,NULL,NULL); has_id(cols,idx); idx++){ void* v=map_id(cols,idx); char* k=map_key(cols, idx);
		if(str_eq(k,"*")){
			map* map_1=sql_tables(sql,db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* ssql=map_id(map_1,next1); char*  talias=map_key(map_1, next1);
				map* map_1=sql_select_cols(ssql,db,NULL); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* col=map_id(map_1,next1); char*  f=map_key(map_1, next1);
					if(map_val(ret,f)){ continue; };
					add(ret,f,map_dup(col));
					add(add_key(ret,f,Map),"expr",is_code(talias) ? xvec(talias,".",f, End) : xvec(f, End));
					add(add_key(ret,f,Map),"alias",f);
					add(add_key(ret,f,Map),"talias",talias); }; };
		}else if(str_end(k,".*")){
			char* talias=sub_str(k,0,-2);
			void* tcols=map_val(sql_select_cols(sql,db,NULL),talias);
			if(!tcols){ fox_error(xstr("Invalid table alias ", k, " in sql ", sql, End),0); };
			for(int idx2=next(tcols,-1,NULL,NULL); has_id(tcols,idx2); idx2++){ void* v2=map_id(tcols,idx2); char* k2=map_key(tcols, idx2);
				if(map_val(cols,k2)||map_val(ret,k2)){ continue; };
				add(ret,k2,map_dup(v2));
				add(add_key(ret,k2,Map),"expr",is_code(talias) ? xvec(talias,".",k2, End) : xvec(k2, End));
				add(add_key(ret,k2,Map),"alias",k2);
				add(add_key(ret,k2,Map),"talias",talias); };
		}else{
			add(ret,k,sql_col(sql,db,v));
			add(add_key(ret,k,Map),"expr",v);
			add(add_key(ret,k,Map),"alias",k); }; };
	map* ccols=crosstab_cols(ret,sql,db,params);	
	if(ccols){
		void* sum=map_id(ret,ret->len-1);
		map_compact(map_del(ret,ret->len-1,1));
		map_merge(ret,ccols);
		add(ret,map_val(sum,"alias"),sum); };
	return ret;
};
char* sql_add_where(char* sql,map* mp){
	return map_sql(add(sql_map(sql),"where",map_merge(sql_cls(sql,"where"),mp)));
};
map* sql_where_cols(char* sql,char* db){
	map* ret=new_map();
	map* map_1=sql_cls(sql,"where"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* k=map_key(map_1, next1);
		if(!is_str(k)){ continue; };
		add(ret,k,sql_col(sql,db,sql_toks(k))); };
	map* map_2=sql_cls(sql,"having"); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* val=map_id(map_2,next1); char* k=map_key(map_2, next1);
		if(!is_str(k)){ continue; };
		add(ret,k,sql_col(sql,db,sql_toks(k))); };
	return ret;
};
map* sql_where_vals(char* sql){
	map* ret=new_map();
	map* map_1=sql_cls(sql,"where"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* k=map_key(map_1, next1);
		if(!is_str(k)){ continue; };
		char* sval=sql_str(val);
		if(strchr("\"'",sval[0])){ sval=str_unquote(sval); };
		add(ret,k,sval); };
	map* map_2=sql_cls(sql,"having"); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* val=map_id(map_2,next1); char* k=map_key(map_2, next1);
		if(!is_str(k)){ continue; };
		char* sval=sql_str(val);
		if(strchr("\"'",sval[0])){ sval=str_unquote(sval); };
		add(ret,k,sql_str(sval)); };
	return ret;
};
//char* sql_col_alias(char* sql,char* db,char* name){
//	toks=name.sql_toks()
//	if toks.map_len()==3 && toks[1]==="." => return name
//	tbls=sql.sql_tables(db)
//	if tbls.map_len()>1 && name.is_code()
//
//}
char* sql_add_limit(char* sql,int rpp,int page){
	return sql_add_cls(sql,"limit",xmap("limit",int_var( rpp), "offset",int_var( rpp*page), End));
};
char* sql_add_order(char* sql,char* db,map* order){
	order=map_merge(sql_cls(sql,"order"),order);
	if(!order){ order=new_map(); };
	map* map_1=sql_pkeys(sql,db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* f=map_id(map_1,next1);
		if(!map_val(order,f)){ add(order,f,int_var(1)); }; };
	return sql_add_cls(sql,"order",order);
};
char* sql_add_filter(char* sql,map* filter){
	map* dsql=sql_map(sql);
	for(int next1=next(filter,-1,NULL,NULL); has_id(filter,next1); next1++){ void* val=map_id(filter,next1); char* k=map_key(filter, next1);
		add(add_key(dsql,"where",Map),k,xstr(":",k, End)); };
	return map_sql(dsql);
};
char* sql_str(void* data){
	if(!data){ return NULL; };
	int type=ptr_type(data);
	if(type==String){ return data; };
	if(type==Map||type==Vector){
		map* mp=(map*)data;
		char* ret=NULL;
		for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){
			if(ret && is_map(map_id(mp,idx)) && is_map(map_id(mp,idx-1))){ ret=xcat(ret,", ", End); }
			else if(ret && !str_is_oper(map_id(mp,idx)) && !str_is_oper(map_id(mp,idx-1))){ ret=xcat(ret," ", End); };
			ret=xcat(ret,sql_str(map_id(mp,idx)), End); };
		return ret; };
	return int_str((long)data);
};
char* re_select(map* mp){
	int i=0;
	if(!map_val(mp,"select")){ return NULL; };
	char* ret="select ";
	if(map_val(mp,"unique")){ ret=xcat(ret,"distinct ", End); };
	if(map_val(mp,"crosstab")){ ret=xcat(ret,"crosstab ", End); };
		if(map_val(mp,"prefix")){ ret=xcat(ret,"prefix ",map_val(mp,"prefix"), End); };
	map* map_1=map_val(mp,"select"); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* v=map_id(map_1,idx); char* k=map_key(map_1, idx);
		if(i++){ ret=xcat(ret,", ", End); };
		char* v2=sql_str(v);
		if(str_end(v2,".*") && map_len(map_val(mp,"select"))==1){ v2="*"; };
		ret=xcat(ret,v2, End);
		if(!str_eq(k,v2) && is_code(k)){ ret=xcat(ret," as ",is_str(k), End); };
	};
	return ret;
};
char* re_from(map* cls){
	int i=0;
	if(!cls){ return NULL; };
	char* ret=" from ";
	for(int idx=next(cls,-1,NULL,NULL); has_id(cls,idx); idx++){ void* v=map_id(cls,idx); char* k=map_key(cls, idx);
		map* mp1=v;
		if(map_val(mp1,"join_type")){ ret=xcat(ret," ",sql_str(map_val(mp1,"join_type")), End); };
		if(i++){ ret=xcat(ret," join ", End); };
		if(is_code(map_val(mp1,"tbl"))){ ret=xcat(ret,map_val(mp1,"tbl"), End); }
		else {ret=xcat(ret,"(",map_val(mp1,"tbl"),")", End);};
		if(!str_eq(map_val(mp1,"tbl"),k)){ ret=xcat(ret," as ",is_str(k), End); };
		if(map_val(mp1,"on")){
			ret=xcat(ret," on ",sql_str(map_val(mp1,"on")), End); }; };
	return ret;
};
char* re_where(map* cls){
	int i=0;
	if(!cls){ return NULL; };
	char* ret=" where ";
	for(int idx=next(cls,-1,NULL,NULL); has_id(cls,idx); idx++){ void* v=map_id(cls,idx); char* k=map_key(cls, idx);
		if(i++){ ret=xcat(ret," and ", End); };
		if(is_str(k)){
			if(is_code(k)){
				ret=xcat(ret,"\"",k,"\"=", End);
			}else{
				ret=xcat(ret,k,"=", End); }; };
		ret=xcat(ret,sql_str(v), End); };
	return ret;
};
char* re_group(map* cls){
	int i=0;
	if(!cls){ return NULL; };
	char* ret=" group by ";
	for(int idx=next(cls,-1,NULL,NULL); has_id(cls,idx); idx++){ void* v=map_id(cls,idx);
		if(i++){ ret=xcat(ret,", ", End); };
		ret=xcat(ret,sql_str(v), End);
	};
	return ret;
};
char* re_having(map* cls){
	int i=0;
	if(!cls){ return NULL; };
	char* ret=" having ";
	for(int idx=next(cls,-1,NULL,NULL); has_id(cls,idx); idx++){ void* v=map_id(cls,idx); char* k=map_key(cls, idx);
		if(i++){ ret=xcat(ret,", ", End); };
		if(is_str(k)){ ret=xcat(ret,k,"=", End); };
		ret=xcat(ret,sql_str(v), End);
	};
	return ret;
};
char* re_order(map* cls){
	int i=0;
	if(!cls){ return NULL; };
	char* ret=" order by ";
	for(int idx=next(cls,-1,NULL,NULL); has_id(cls,idx); idx++){ void* v=map_id(cls,idx); char* k=map_key(cls, idx);
		if(i++){ ret=xcat(ret,", ", End); };
		ret=xcat(ret,k, End);
		if(is_int(v)<0){ ret=xcat(ret," desc", End); }; };
	return ret;
};
char* re_union(map* cls){
	if(!cls){ return NULL; };
	return xstr(" union ",map_join(cls," "), End);
};
char* re_limit(map* cls){
	if(!cls){ return NULL; };
	return xstr(" limit ",str_join(is_int(map_val(cls,"offset")) ? to_str(map_val(cls,"offset"),"",0) : NULL,", ",to_str(map_val(cls,"limit"),"",0)), End);
};
char* map_sql(map* mp){
	if(!mp){ return NULL; };
	return xstr(re_select(mp),re_from(map_val(mp,"from")),re_where(map_val(mp,"where")),re_group(map_val(mp,"group")),re_having(map_val(mp,"having")),re_order(map_val(mp,"order")),re_limit(map_val(mp,"limit")),re_union(map_val(mp,"union")), End);
};
int has_aggregate(char* sql){
	map* map_1=map_val(sql_select_cols(sql,NULL,NULL),"cols"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* col=map_id(map_1,i); if(map_val(col,"aggregate")){ return 1; }; };
	return 0;
};
char* sql_auto_join(char* sql,char* db){
	map* dsql=sql_map(sql);
	map* cols=new_map();
	int ok=1;
	map* map_1=map_val(dsql,"from"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* op2 =map_id(map_1,i2); char* tbl2=map_key(map_1, i2); if(i2 && !map_val(op2,"on")){ ok=0; break; }; };
	if(ok){ return sql; };
	map* map_2=map_val(dsql,"from"); for(int i=next(map_2,-1,NULL,NULL); has_id(map_2,i); i++){ void* op=map_id(map_2,i); char* tbl=map_key(map_2, i);
		if(i && !map_val(op,"on")){
			for(int i2=next(cols,-1,NULL,NULL); has_id(cols,i2); i2++){ void* op2=map_id(cols,i2); char* t2=map_key(cols, i2);
				for(int i3=next(op2,-1,NULL,NULL); has_id(op2,i3); i3++){ void* op3=map_id(op2,i3); char* f3=map_key(op2, i3);
					if(!map_val(op3,"sql")){ continue; };
					char* sql2=fkey_sql(map_val(op3,"sql"),db);
					if(!str_eq(sql_table(sql2),map_val(op,"tbl"))){ continue; };
					map* tblfld=toks_split(map_val(map_id(sql_select_cols(sql2,db,NULL),0),"expr"),".",2);
					void* ff=map_len(tblfld)==2 ? map_id(tblfld,1) : map_id(tblfld,0);	
					add(op,"on",vec_merge(xvec(t2,".",f3, End),xvec("=",tbl,".",ff, End)));
					break; };
				if(map_val(op,"on")){ break; }; };
			if(!map_val(op,"on")){ fox_error("auto join failed",0); }; };
		add(cols,tbl,sql_select_cols(map_val(op,"tbl"),db,NULL)); };
	return map_sql(dsql);
};
char* sql_auto_group(char* sql,char* db){
	map* dsql=sql_map(sql);
	if(map_val(dsql,"group")){ return sql; };
	map* cols=sql_select_cols(sql,db,NULL);
	int agg=0;
	int noagg=0;
	for(int i=next(cols,-1,NULL,NULL); has_id(cols,i); i++){ void* col=map_id(cols,i);
		if(map_val(col,"aggregate")){ agg++; }
		else {noagg++;}; };
	if(!agg || !noagg){ return sql; };
	add(dsql,"group",new_vec());
	for(int i2=next(cols,-1,NULL,NULL); has_id(cols,i2); i2++){ void* col2 =map_id(cols,i2); char* f2=map_key(cols, i2); if(!map_val(col2,"aggregate")){ vec_add(add_key(dsql,"group",Vector),f2); }; };
	return map_sql(dsql);
};
char* sql_add_cls(char* sql,char* cls,map* vals){
	map* dsql=sql_map(sql);
	add(dsql,cls,vals);
	return map_sql(dsql);
};
map* sql_cls(char* sql,char* cls){ return map_val(sql_map(sql),cls); };
map* sql_tables(char* sql,char* db){
	if(is_code(sql)){ return xmap((sql), sql, End); };
	map* ret=new_map();
	map* map_1=sql_cls(sql,"from"); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* v=map_id(map_1,idx); char* k=map_key(map_1, idx);
		add(ret,is_code(k) ? k : NULL,map_val(v,"tbl")); };
	return ret;
};
map* sql_col(char* sql,char* db,map* exp){
	map* ret=new_map();
	if(exp->len==1){
		char* name=map_id(exp,0);
		map* map_1=sql_tables(sql,db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* ssql=map_id(map_1,next1);
			map* cols=sql_select_cols(ssql,db,NULL);
			if(map_val(cols,name)){ return map_val(cols,name); }; };
		ret=map_val(sql_cls(sql,"select"),name);
		if(ret && !str_eq(sql_str(ret),name)){ return sql_col(sql,db,ret); };
		fox_error(xstr("Field ", name, " not found in query.", End),0); };
	if(exp->len==3 && str_eq(is_str(map_id(exp,1)),".")){
		ret=map_val(sql_select_cols(map_val(sql_tables(sql,db),map_id(exp,0)),db,NULL),map_id(exp,2));
		if(!ret){ fox_error(xstr("Field ", sql_str(exp), " not found in ", sql, End),0); };
		return ret; };
	if(exp->len>1){
		if(map_has_word(exp,"|")){
			add(ret,"type","text");
			return ret; }; };
	char* name=is_func(exp,1);
	if(name){
		if(is_word(name,"sum count avg")){
			add(ret,"aggregate",name);
			add(ret,"sum",sql_str(map_id(sql_func_params(exp,1),0)));
			add(ret,"type","amount");
			return ret; };
		if(is_word(name,"month quarter year date time")){
			add(ret,"exp",sql_str(exp));
			add(ret,"type",name);
			return ret; };
		if(str_eq(name,"sub_str")){
			add(ret,"type","text");
			return ret; };
		if(str_eq(name,"floor")){
			add(ret,"type","number");
			return ret; };
		if(str_eq(name,"empty")){
			add(ret,"type","bool");
			return ret; };
		if(str_eq(name,"if")){
			return sql_col(sql,db,map_id(sql_func_params(exp,1),1)); }; };
	name=is_str(map_id(exp,0));
	if(*name=='"'||*name=='\''){
		add(ret,"type","text");
		return ret; };
	fox_error(xstr("can't find ", sql_str(exp), " in ", sql, End),0);
	return NULL;
};
char* sql_table(char* sql){
	if(is_code(sql)){ return sql; };
	return sql_table(map_val(map_id(sql_cls(sql,"from"),0),"tbl"));
};
map* sql_pkeys(char* sql,char* db){ return tbl_pkeys(sql_table(sql),db); };
map* to_cols(void* cols,char* sql,char* db){
	if(is_str(cols)){ cols=xkeys(cols); };
	if(!sql){
		for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* col=map_id(cols,next1); char* f=map_key(cols, next1);
			if(!col){
				add(cols,f,xmap("type", name_type(f), End));
			}else if(is_str(col)){
				add(cols,f,xmap("type", col, End));
			}else if(!map_val(col,"type")){
				add(add_key(cols,f,Map),"type",name_type(f)); }; };
		return cols; };
	map* ret=new_map();
	for(int idx=next(cols,-1,NULL,NULL); has_id(cols,idx); idx++){ void* col=map_id(cols,idx); char* f=map_key(cols, idx);
		add(ret,f,sql_col(sql,db,sql_toks(f)));
		if(is_map(col)){ map_merge(map_val(ret,f),col); };
		if(str_eq(map_val(map_val(ret,f),"type"),"-")){ add(add_key(ret,f,Map),"type",map_val(map_val(cols,f),"type")); }; };
	return ret;
};
map* sql_cols(char* sql,char* db,void* cols){
	if(!cols){ return sql_select_cols(sql,db,NULL); };
	return to_cols(cols,sql,db);
};
map* sql_add_cols(char* sql,char* db,map* cols){
	map* scols=tbl_cols(sql_table(sql),db);
	if(map_len(cols)){ return cols_collect(scols,cols); };
	map* ret=new_map();
	for(int next1=next(scols,-1,NULL,NULL); has_id(scols,next1); next1++){ void* op=map_id(scols,next1); char* f=map_key(scols, next1);
		if(map_val(op,"noshow")){ continue; };
		if(map_val(op,"noadd")){ continue; };
		if(map_val(op,"auto")){ continue; };
		if(map_val(op,"calc")){ continue; };
		if(str_eq(map_val(op,"type"),"guid") && !map_val(op,"sql")){ continue; };
		add(ret,f,op); };
	return ret;
};
map* sql_view_cols(char* sql,char* db,map* cols){
	map* scols=tbl_cols(sql_table(sql),db);
	if(map_len(cols)){ return cols_collect(scols,cols); };
	map* ret=new_map();
	for(int next1=next(scols,-1,NULL,NULL); has_id(scols,next1); next1++){ void* op=map_id(scols,next1); char* f=map_key(scols, next1);
		if(map_val(op,"noshow")){ continue; };
		if(str_eq(map_val(op,"type"),"guid") && !map_val(op,"sql")){ continue; };
		add(ret,f,op); };
	return ret;
};
map* sql_edit_cols(char* sql,char* db,map* cols){
	map* scols=tbl_cols(sql_table(sql),db);
	if(map_len(cols)){ return cols_collect(scols,cols); };
	map* ret=new_map();
	for(int next1=next(scols,-1,NULL,NULL); has_id(scols,next1); next1++){ void* op=map_id(scols,next1); char* f=map_key(scols, next1);
		if(map_val(op,"noshow")){ continue; };
		if(map_val(op,"noedit")){ continue; };
		if(map_val(op,"auto")){ continue; };
		if(map_val(op,"calc")){ continue; };
		if(str_eq(map_val(op,"type"),"guid") && !map_val(op,"sql")){ continue; };
		add(ret,f,op); };
	return ret;
};
map* sql_list_cols(char* sql,char* db,map* cols){
	map* scols=sql_select_cols(sql,db,NULL);
	if(map_len(cols)){ return cols_collect(scols,cols); };
	map* ret=new_map();
	for(int next1=next(scols,-1,NULL,NULL); has_id(scols,next1); next1++){ void* op=map_id(scols,next1); char* f=map_key(scols, next1);
		if(map_val(op,"noshow")){ continue; };
		if(map_val(op,"nolist")){ continue; };
		if(str_eq(map_val(op,"type"),"guid") && !map_val(op,"sql")){ continue; };
		if(str_eq(map_val(op,"type"),"password")){ continue; };
		add(ret,f,op); };
	return ret;
};
char* sql_lite(char* sql,char* db,map* params){
	sql=sql_auto_group(sql_auto_join(sql_add_order(sql,db,NULL),db),db);
	map* ret=sql_map(sql);
	if(db){
		add(ret,"select",new_map());
		map* map_1=sql_select_cols(sql,db,params); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* prop=map_id(map_1,idx); char* name=map_key(map_1, idx);
			add(add_key(ret,"select",Map),name,map_val(prop,"expr")); };
		add(ret,"crosstab",NULL);
		add(ret,"prefix",NULL); };
	return xstr(map_sql(search_sql_func(ret,"lite")),";", End);
};
int sql_utest(char* in,char* out){
	if(!out){ out=in; };
	map* mp=sql_map(in);
	char* ret=map_sql(search_sql_func(mp,"lite"));
	return utest(ret,out,in,"");
};
map* sql_missing_params(char* sql, map* params){
	map* ret=new_map();
	map* map_1=sql_params(sql); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ char* key=map_key(map_1, next1);
		if(!map_val(params,key)){ add(ret,key,key); }; };
	return is_null(ret);
};
map* fkey_list(char* sql,char* db,map* params){
	if(sql_missing_params(sql,params)){ return NULL; };
	return sql_pairs(fkey_sql(sql,db),db,params);
};
char* fkey_sql(char* sql,char* db){
	map* cols=sql_select_cols(sql,db,NULL);
	void* pkey=map_id(cols_pkeys(cols),0);
	void* skey=map_id(cols_skeys(cols),0);
	map* ret=sql_map(sql);
	add(ret,"select",xmap("id", map_val(map_val(cols,pkey),"expr"), "name", map_val(map_val(cols,skey),"expr"), End));
	return sql_add_order(map_sql(ret),db,NULL);
};
map* sql_params(char* sql){
	return toks_sql_params(sql_toks(sql),new_map());
};
void* id_delete(void* ids,char* tbl,char* db){
	ids=id_param(ids,tbl,db);
	return lite_exec((xstr(xstr("delete from ", tbl, End),re_where(where_param(ids)), End)),db,ids);
};
char* read_textblock(map* lines, int* lineno,char* terminator,char** outline){
	terminator=str_trim(terminator," \t\n\r");
	int indent=str_level(map_id(lines,*lineno+1));
	char* ret=NULL;
	for(int no=next(lines,*lineno+1-1,NULL,NULL); has_id(lines,no); no++){ void* line=map_id(lines,no);
		*lineno=no;
		if(str_level(line)<indent || str_start((line+indent),terminator)){
			*outline=line+str_len(terminator)+indent;
			return xstr(terminator,ret,terminator, End);
		}else {ret=xcat(ret,sub_str(line,indent,-2147483648),"\n", End);}; };
	*outline=NULL;
	return ret;
};
map* sql_tokenizer(char** line){
	char* str=*line;
	map* mp=new_vec();
	if(!*str){ return NULL; };
	char term=closing_paren(*str);
	if(term){ str++; };
	while(*str && *str!=term){
		if(strchr("\"'`",*str)){ vec_add(mp,read_quote(&str)); }
		else if(str_start(str,"--")){ read_theline(&str); }
		else if(str_start(str,"/*")){ read_upto(&str,"*/"); }
		else if(*str==':'){ vec_add(mp,fox_read_symbol(&str)); }
		else if(is_oper(*str)){ vec_add(mp,fox_read_oper(&str,term)); }
		else if(*str>='0' && *str<='9'){ vec_add(mp,read_num(&str)); }
		else if(is_alpha(*str,NULL)){ vec_add(mp,read_alpha(&str)); }
		else if(strchr("([{",*str)){ read_paren(mp,&str,sql_tokenizer); }
		else if(strchr(" \t",*str)){ read_space(&str," \t"); }
		else if(strchr("\n\r",*str)){ read_newline(&str); }
		else if(strchr(".,;",*str)){ vec_add(mp,sub_str(str,0,1)); };
		str++;
	};
	*line=str;
	return mp;
};
map* prop_tokenizer(char** line){
	char* str=*line;
	map* mp=new_vec();
	if(!*str){ return NULL; };
	char term=closing_paren(*str);
	if(term){ str++; };
	while(*str && *str!=term){
		if(strchr("\"'`",*str)){ vec_add(mp,read_quote(&str)); }
		else if(str_start(str,"--")){ read_theline(&str); }
		else if(str_start(str,"/*")){ read_upto(&str,"*/"); }
		else if(*str==':'){ vec_add(mp,fox_read_symbol(&str)); }
		else if(is_oper(*str)){ vec_add(mp,fox_read_oper(&str,term)); }
		else if(*str>='0' && *str<='9'){ vec_add(mp,read_num(&str)); }
		else if(is_alpha(*str,NULL)){ vec_add(mp,read_alpha(&str)); }
		else if(strchr("([{",*str)){ read_paren(mp,&str,sql_tokenizer); }
		else if(strchr(" \t",*str)){ read_space(&str," \t"); }
		else if(strchr("\n\r",*str)){ read_newline(&str); }
		else if(strchr(".,;",*str)){ vec_add(mp,sub_str(str,0,1)); };
		str++;
	};
	*line=str;
	return mp;
};
map* tokenizer(char** line,char* comment){
	char* str=*line;
	map* mp=new_vec();
	if(!*str){ return mp; };
	char term=*str;
	if(term=='('){ term=')'; }
	else if(term=='['){ term=']'; }
	else if(term=='{'){ term='}'; }
	else {term=0;};
	if(term){ str++; };
	while(*str && *str!=term){
		if(strchr("\"'`",*str)){ vec_add(mp,read_quote(&str)); }
		else if(str_start(str,"--")){ read_theline(&str); }
		else if(str_start(str,"#")){ vec_add(mp,read_theline(&str)); }
		else if(str_start(str,"/*")){ read_upto_word(&str,"*/"); }
		else if(*str==':'){ vec_add(mp,fox_read_symbol(&str)); }
		else if(is_oper(*str)){ vec_add(mp,fox_read_oper(&str,term)); }
		else if((*str=='.' && (str[1]>='0' && str[1]<='9'))|| (*str>='0' && *str<='9')){ vec_add(mp,read_num(&str)); }
		else if(is_alpha(*str,NULL)){ vec_add(mp,read_alpha(&str)); }
		else if(strchr("([{",*str)){
			char temp[2]={0};
			temp[0]=*str;
			char* signs="([{)]}";
			int hit=signs-strchr(signs,*str);
			vec_add(mp,temp);
			vec_add(mp,tokenizer(&str,comment));
			temp[0]=signs[hit+3];
			vec_add(mp,temp);
		}
		else if(strchr(" \t",*str)){ read_space(&str," \t"); }
		else if(strchr("\n\r",*str)){ read_newline(&str); }
		else if(strchr(".,;",*str)){ vec_add(mp,sub_str(str,0,1)); };
		str++;
	};
	*line=str;
	return mp;
};
char* to_sql(char* sql){
	return map_sql(sql_map(sql));
};
map* schema_fkeys(char* db){
	void* ret=map_val(map_val(map_val(_globals,"schema"),"fkeys"),db);
	if(!ret){
		ret=new_vec();
		map* map_1=map_val(map_val(_globals,"schema"),"db"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* tbl=map_id(map_1,next1); char* name=map_key(map_1, next1);
			map* map_1=map_val(tbl,"cols"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* col=map_id(map_1,next1); char* f=map_key(map_1, next1);
				if(map_val(col,"sql")){
					char* sql=fkey_sql(map_val(col,"sql"),db);
					vec_add(ret,xmap("table", name, "f", f, "table2", sql_table(sql), "f2", map_val(map_id(sql_cols(sql,db,NULL),0),"name"), "db", map_val(col,"db"), End)); }; }; };
		add(add_key(add_key(_globals,"schema",Map),"fkeys",Map),db,ret); };
	return ret;
};
map* tbl_referred_by(char* tbl,char* db){
	map* ret=new_vec();
	map* map_1=schema_fkeys(db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* rel=map_id(map_1,next1);
		if(str_eq(map_val(rel,"table2"),tbl)){
			vec_add(ret,xmap("table", map_val(rel,"table"), "f", map_val(rel,"f"), "f2", map_val(rel,"f2"), End)); }; };
	return ret;
};
map* sql_sums(char* sql,char* db,map* cols,map* params){
	map* sum=new_map();
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* col=map_id(cols,next1); char* f=map_key(cols, next1);
		if(!to_int(map_type(xmap(
			"text", "0",
			"amount", "1"
		, End),map_val(col,"type")))){ continue; };
		char* expr=sql_str(map_val(col,"expr"));
		if(!map_val(col,"aggregate") && !str_start(expr,"sum(") && !str_start(expr,"count(")){ expr=xstr("sum(", expr, ")", End); };
		add(sum,f,sql_toks(expr)); };
	if(!map_len(sum)){ return NULL; };
	map* dsql=sql_map(sql);
	add(dsql,"select",sum);
	add(dsql,"order",NULL);
	add(dsql,"limit",NULL);
	add(dsql,"group",NULL);
	return sql_row(map_sql(dsql),db,params);
};
int sql_count(char* sql,char* db,map* params){
	map* sqls=sql_map(sql);
	char* expr="*";
	if(map_val(sqls,"aggregate")){
		if(map_val(sqls,"having")){ return to_int(sql_value(xstr("select count(*) from (", sql, ")", End),db,params)); };
		map* map_1=sql_cols(sql,db,NULL); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* key=map_key(map_1, next1);
			if(!map_val(val,"aggregate")){ expr=xstr("distinct ", key, End); break; }; }; };
	return to_int(sql_value((xstr(xstr("select count(", expr, ") ", End),re_from(map_val(sqls,"from")),re_where(map_val(sqls,"where")), End)),db,params));
};
map* sql_query(char* sql,char* db,map* where){ return sql_rows(sql_add_where(sql,where),db,where); };
char* join_clause(char* pre,map* mp,char* clause,char* by,char* sub1,char* sub2,char* use_cls){
	map* mp1=mp;
	if(clause){ mp1=map_val(mp,clause); };
	char* ret=NULL;
	for(int idx2=next(mp1,-1,NULL,NULL); has_id(mp1,idx2); idx2++){
		char* ret2=NULL;
		if(!sub1){ ret2=to_str(map_id(mp1,idx2),"",0); };
		if(!sub2){ ret2=sql_map_join(map_id(mp1,idx2),sub1); }
		else{
			void* mp2=map_id(mp1,idx2);
			char* ret3=NULL;
			map* map_1=map_id(mp1,idx2); for(int idx4=next(map_1,-1,NULL,NULL); has_id(map_1,idx4); idx4++){ void* v4=map_id(map_1,idx4); char* k4=map_key(map_1, idx4);
				if(is_str(k4)){ ret3=str_join(ret3,sub1,sql_map_join(map_key_val(mp2,idx4),sub2)); };
			};
			ret2=ret3; };
		ret=str_join(ret,by,ret2);
	};
	if(use_cls && ret){ ret=xstr(use_cls," ",ret, End); };
	return str_join(pre," ",ret);
};
map* de_select(map* cls){
	map* ret=new_map();
	if(str_eq(map_id(map_val(cls,"select"),0),"distinct") || str_eq(map_id(map_val(cls,"select"),0),"unique")){
		add(cls,"unique",int_var(1));
		vec_compact(vec_del(map_val(cls,"select"),0,1)); };
	if(str_eq(map_id(map_val(cls,"select"),0),"crosstab")){
		add(cls,"crosstab",int_var(1));
		vec_compact(vec_del(map_val(cls,"select"),0,1));
		if(str_eq(map_id(map_val(cls,"select"),0),"prefix")){
			add(cls,"prefix",map_id(map_val(cls,"select"),1));
			vec_compact(vec_del(map_val(cls,"select"),0,2)); }; };
	map* map_1=map_split(map_val(cls,"select"),",",0); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* val=map_id(map_1,idx);
		map* mp1=map_split(val,"as",2);
		char* name=mp1->len==2 ? sql_str(map_id(mp1,1)) : sql_str(map_id(mp1,0));
		add(ret,name,map_id(mp1,0)); };
	return add(cls,"select",ret);
};
map* de_from(map* cls){
	map* ret=new_map();
	void* last_join_type=NULL;
	map* map_1=map_split(map_val(cls,"from"),"join",0); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* v=map_id(map_1,idx);
		void* join_type=NULL;
		map* words=v;
		for(int idx1=next(words,-1,NULL,NULL); has_id(words,idx1); idx1++){ void* v1=map_id(words,idx1);
			if(is_word(v1,"full inner left right cross outer")){
				join_type=vec_slice(words,idx1,words->len);
				vec_compact(words);
				break; }; };
		map* parts=split_keywords(words,"as on");
		change_key(parts,0,"tbl");
		if(str_eq(map_id(map_val(parts,"tbl"),0),"(")){ add(parts,"tbl",map_sql(sql_toks_map(map_id(vec_compact(vec_del(vec_del(map_val(parts,"tbl"),0,1),-1,1)),0)))); }
		else {add(parts,"tbl",map_join(map_val(parts,"tbl"),""));};
		if(map_val(parts,"as")){ add(parts,"as",sql_str(map_val(parts,"as"))); }
		else {add(parts,"as",map_val(parts,"tbl"));};
		if(last_join_type){ add(parts,"join_type",last_join_type); };
		add(ret,map_val(parts,"as"),parts);
		last_join_type=join_type; };
	add(cls,"from",ret);
	return cls;
};
map* parse_where(map* cls){
	if(map_has_word(cls,"or")){ return xmap(NULL, cls, End); };
	map* wheres=new_map();
	map* map_1=map_split(cls,"and",0); for(int idx2=next(map_1,-1,NULL,NULL); has_id(map_1,idx2); idx2++){ void* val2=map_id(map_1,idx2);
		map* eqs=map_split(val2,"=",2);	
		void* val=map_id(eqs,0);
		char* key=NULL;
		assert(eqs->len<=2); //to fix support explode(...,limit=2)
		if(eqs->len==2){
			key=sql_str(val);
			val=map_id(eqs,1); };
		add(wheres,key,val); };
	return wheres;
};
map* de_where(map* cls){
	void* mp=map_val(cls,"where");
	if(!mp){ return cls; };
	return add(cls,"where",parse_where(mp));
};
map* de_having(map* cls){
	void* mp=map_val(cls,"having");
	if(!mp){ return cls; };
	return add(cls,"having",parse_where(mp));
};
map* de_order(map* cls){
	void* mp=map_val(cls,"order");
	if(!mp){ return cls; };
	if(str_eq(map_id(mp,0),"by")){ vec_compact(vec_del(mp,1-1,1)); };
	map* ret=new_map();
	map* map_1=map_split(mp,",",0); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* val=map_id(map_1,idx);
		void* mp1=val;
		char* col=sql_str(map_id(mp1,0));
		char* ord=sql_str(map_id(mp1,1));
		int iord=1;
		if(str_eq(ord,"desc")){ iord=-1; };
		add(ret,col,int_var(iord));
	};
	add(cls,"order",ret);
	return cls;
};
map* de_group(map* cls){
	void* mp=map_val(cls,"group");
	if(!mp){ return cls; };
	if(str_eq(map_id(mp,0),"by")){ vec_compact(vec_del(mp,1-1,1)); };
	add(cls,"group",map_split(mp,",",0));
	return cls;
};
map* de_limit(map* cls){
	void* mp=map_val(cls,"limit");
	if(!mp){ return cls; };
	mp=map_split(mp,",",2);
	int offset=stoi(sql_str(map_id(mp,0)));
	int limit=stoi(sql_str(map_id(mp,1)));
	if(offset && !limit){ limit=offset; offset=0; };
	add(cls,"limit",xmap("offset",int_var( offset),"limit",int_var( limit), End));
	return cls;
};
map* sql_toks_map(map* toks){
	toks=map_split(toks,"union",2);
	map* ret=de_limit(
		de_order(
		de_having(
		de_group(
		de_where(
		de_select(
		de_from(
		split_keywords(map_id(toks,0),"select from where group having order limit"))))))));
	if(map_id(toks,1)){ add(ret,"union",map_id(toks,1)); };
	return ret;
};
map* sql_map(char* sql){
	if(!sql){ return NULL; };
	void* ret=cache(sql,"sql_map",NULL);
	if(ret){ return ret; };
	if(!str_has(sql," ")){ sql=xstr("select * from ", sql, End); };
	return sql_toks_map(sql_toks(sql));
};
char* sql_map_join(map* mp,char* joiner){
	char* ret=NULL;
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){ ret=str_join(ret,joiner,sql_str(map_id(mp,idx))); };
	return ret;	
};
char* var_join(void* v,char* joiner){
	if(ptr_type(v)==String){ return v; };
	if(ptr_type(v)==Map||ptr_type(v)==Vector){ return sql_map_join(v,joiner); };
	return int_str((long)v);
};
map* sql_convert_func(map* mp,char* db){
	char* name=map_id(mp,0);
	map* params=new_vec();
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){
		if(idx==0){ continue; };
		vec_add(params,sql_str(map_id(mp,idx)));
	};
	if(str_eq(db,"lite")){
		if(str_eq(name,"time")){ return sql_toks(xstr("substr(", map_id(params,0), ",12,8)", End)); };
		if(str_eq(name,"date")){ return sql_toks(xstr("substr(", map_id(params,0), ",1,10)", End)); };
		if(str_eq(name,"month")){ return sql_toks(xstr("substr(", map_id(params,0), ",1,7)", End)); };
		if(str_eq(name,"year")){ return sql_toks(xstr("substr(", map_id(params,0), ",1,4)", End)); };
		if(str_eq(name,"quarter")){ return sql_toks(xstr("substr(", map_id(params,0), ",1,5)||((substr(", map_id(params,0), ",6,2)-1)/3+1)", End)); };
		if(str_eq(name,"empty")){ return sql_toks(xstr("(", map_id(params,0), " is null or ", map_id(params,0), "='')", End)); };
		if(str_eq(name,"if")){ return sql_toks(xstr("case when ", map_id(params,0), " then ", map_id(params,1), " else ", map_id(params,2), " end", End)); };
	};
	return NULL;
};
char* is_func(map* mp,int idx){
	if(!mp){ return NULL; };
	if(!is_code(map_id(mp,idx-1))){ return NULL; };
	if(!str_eq(map_id(mp,idx),"(")){ return NULL; };
	return map_id(mp,idx-1);
};
map* sql_func_params(map* mp,int idx){
	return is_func(mp,idx) ? map_split(map_id(mp,idx+1),",",0) : NULL;
};
map* get_func(map* mp,int idx){
	return is_func(mp,idx) ? vec_merge(xvec(map_id(mp,idx-1), End),map_split(map_id(mp,idx+1),",",0)) : NULL;
};
map* search_sql_func(map* mp,char* db){
	if(!mp){ return NULL; };
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){ void* v=map_id(mp,idx);
		if(is_map(v)){ set(mp,idx,search_sql_func(v,db)); continue; };
		map* mp1=get_func(mp,idx);
		if(mp1){ mp1=sql_convert_func(mp1,db); };
		if(mp1){ vec_splice(mp,idx-1,4,mp1); }; };
	return mp;
};
void* key_var(char* key){
	if(!key){ return NULL; };
	if((long)key<0){ return int_var(-(long)key); };
	return key;
};
map* map_key_val(map* mp,int idx){//0
	assert(idx>=0 && idx<mp->len);
	return xvec(key_var(mp->pairs[idx].id),map_id(mp,idx), End);
};
map* db_table_names(char* db){
	if(map_val(map_val(_globals,"schema"),"_tbls")){
		return map_val(map_val(_globals,"schema"),"_tbls"); };
	add(add_key(_globals,"schema",Map),"_tbls",sql_pairs("select name from sqlite_master where type='table' and name not in ('sqlite_sequence', '_syncing', 'search', 'search_data', 'search_idx', 'search_content', 'search_docsize', 'search_config') order by 1",db,NULL));
	return map_val(map_val(_globals,"schema"),"_tbls");
};
map* cols_pkeys(map* cols){
	map* ret=new_map();
	for(int i=next(cols,-1,NULL,NULL); has_id(cols,i); i++){ void* v=map_id(cols,i); char* k=map_key(cols, i);
		if(map_val(v,"pkey")){ add(ret,k,k); }; };
	if(!map_len(ret)){ add(ret,map_key(cols,0),map_key(cols,0)); };
	return ret;
};
map* where_param(map* where){
	map* ret=new_map();
	for(int next1=next(where,-1,NULL,NULL); has_id(where,next1); next1++){ char* key=map_key(where, next1); add(ret,key,xstr(":",key, End)); };
	return ret;
};
void* is_null(void* val){
	if(!val){ return NULL; };
	if(is_map(val)){ return map_len(val) ? val : NULL; };
	if(is_str(val)){ return str_len(val) ? val : NULL; };
	if(is_i(val)){ return is_int(val) ? val : NULL; };
	return val;
};
map* db_table(char* db,char* tbl){
	return cols_table(db_cols(db,tbl),tbl,db);
};
map* db_tables(char* db){
	map* ret=new_map();
	map* map_1=db_table_names(db); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		add(ret,v,db_table(db,v)); };
	return ret;
};
map* where_rows(map* where, char* tbl, char* db){ return sql_rows(sql_add_where(tbl,where_param(where)),db,where); };
map* where_row(map* where, char* tbl, char* db){ return sql_row(sql_add_where(tbl,where_param(where)),db,where); };
map* id_row(void* ids,char* tbl,char* db){ return where_row(id_param(ids,tbl,db),tbl, db); };
map* id_param(void* ids, char* tbl, char* db){
	ids=id_ids(ids,tbl,db);
	map* ret=new_map();
	map* map_1=tbl_pkeys(tbl,db); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* val=map_id(map_1,idx); char* key=map_key(map_1, idx);
		add(ret,key,is_vec(ids) ? map_id(ids,idx) : map_val(ids,key)); };
	return ret;
};
map* id_ids(void* id,char* tbl,char* db){
	map* pkeys=tbl_pkeys(tbl,db);
	map* ret=new_map();
	if(is_i(id)){ id=to_str(id,"",0); };
	if(is_str(id)){ id=str_split(id,"\t",0); };
	assert(is_map(id));
	if(pkeys->len!=map_len(id)){ fox_error(xstr("PKey=",to_str( pkeys,"",0), " and id=", id, ", number of field mismatched", End),0); };
	if(is_vec(id)){
		for(int idx=next(pkeys,-1,NULL,NULL); has_id(pkeys,idx); idx++){ void* f=map_id(pkeys,idx);
			add(ret,f,map_id(id,idx)); };
		return ret; };
	for(int next1=next(pkeys,-1,NULL,NULL); has_id(pkeys,next1); next1++){ void* f=map_id(pkeys,next1);
		add(ret,f,map_val(id,f)); };
	return ret;
};
map* db_cols(char* db,char* tbl){
	map* rs=lite_exec(xstr("pragma table_info (", tbl, ")", End),db,NULL);
	map* ret=new_map();
	for(int idx=next(rs,-1,NULL,NULL); has_id(rs,idx); idx++){ void* v=map_id(rs,idx);
		map* col=new_map();
		map* mp=v;
		char* name=str_lower(map_val(mp,"name"));
		add(col,"name",name);
		map* func=get_func(sql_toks(map_val(mp,"type")),1);
		if(!func){
			add(col,"type",schema_type(map_val(mp,"type"),0));
		}else{
			int size=stoi(map_id(map_id(func,1),0));
			add(col,"size",int_var(size));
			add(col,"type",schema_type(map_id(func,0),size));
		};
		if(stoi(map_val(mp,"pk"))){ add(col,"pkey",int_var(1)); };
		if(!stoi(map_val(mp,"notnull")) && !map_val(col,"pkey")){ add(col,"isnull",int_var(1)); };
		add(col,"db",db);
		add(col,"table",tbl);
		add(ret,name,col); };
	return ret;
};
map* tbl_pkeys(char* tbl,char* db){ return cols_pkeys(tbl_cols(tbl,db)); };
map* tbl_skeys(char* tbl,char* db){ return cols_skeys(tbl_cols(tbl,db)); };
map* cols_skeys(map* cols){
	if(!map_len(cols)){ return NULL; };
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ char* name=map_key(cols, next1); if(str_has(name,"name")){ return xmap("name", name, End); }; };	
	return xmap((map_key(cols,0)), map_key(cols,0), End);
};
map* sql_toks(char* line){ return sql_tokenizer(&line); };
map* split_keywords(map* mp,char* words){
	if(!mp){ return mp; };
	map* ret=new_map();
	map* curr=new_vec();
	char* lastword=NULL;
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){
		if(is_word(map_id(mp,i),words)){
			if(curr->len){ add(ret,lastword,curr); };
			lastword=map_id(mp,i);
			curr=new_vec();
			continue; };
		vec_add(curr,map_id(mp,i)); };
	if(curr->len){ add(ret,lastword,curr); };
	return ret;
};
char* lite_create_col(map* col){
	long long size=to_int(map_val(col,"size"));
	if(!size){ size=type_size(map_val(col,"type")); };
	char* type=map_type(xmap(
		"text", "varchar",
		"para", "clob",
		"file", "blob",
		"int", "number",
		"float", "float",
		"date", "date",
		"time", "time",
		"datetime", "datetime",
		"daymonth", "varchar",
		"month", "varchar"
	, End),map_val(col,"type"));
	if(!type){ fox_error(xstr("col.type=", map_val(col,"type"), " not matched", End),0); };
	void* name=map_val(col,"name");
	if(str_eq(type,"varchar")){ return xstr(name, " varchar(",int_str( size), ") collate nocase not null default ''", End); };
	if(str_eq(type,"number")){ return xstr(name, " integer not null default 0", End); };
	if(str_eq(type,"float")){ return xstr(name, " real not null default 0", End); };
	if(str_eq(type,"clob")){ return xstr(name, " clob not null default ''", End); };
	if(str_eq(type,"blob")){ return xstr(name, " blob not null default ''", End); };
	if(str_eq(type,"date")){ return xstr(name, " date not null default ''", End); };
	if(str_eq(type,"time")){ return xstr(name, " time not null default ''", End); };
	if(str_eq(type,"datetime")){ return xstr(name, " datetime not null default ''", End); };
	if(str_eq(type,"yearmonth")){ return xstr(name, " varchar(7) not null default ''", End); };
	fox_error(xstr("litecreate() type ", type, " not supported", End),0);
	return NULL;
};
int is_indexed(char* type){
	return to_int(map_type(xmap(
		"text", "0",
		"code", "1",
		"password", "0",
		"email", "0"
	, End),type));
};
map* tbl_trigger_sqls(map* tbl){
	if(map_val(map_val(tbl,"cols"),"lft")){ return lite_trigger_tree(map_val(tbl,"name"),map_key(cols_pkeys(map_val(tbl,"cols")),0)); };
	if(map_val(map_val(tbl,"cols"),"slno")){ return lite_trigger_slno(map_val(tbl,"name"),map_key(cols_pkeys(map_val(tbl,"cols")),0),map_key(cols_pkeys(map_val(tbl,"cols")),1)); };
	return new_vec();
};
map* create_index_sqls(map* tbl){
	map* ret=new_vec();
	map* map_1=map_val(tbl,"cols"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* f=map_id(map_1,i); char* k=map_key(map_1, i);
		if(!is_indexed(map_val(f,"type"))){ continue; };
		vec_add(ret,xstr("create index idx_", map_val(tbl,"name"), "_", map_val(f,"name"), " on ", map_val(tbl,"name"), "(", map_val(f,"name"), ")", End)); };
	return ret;
};
char* drop_sql(char* name){ return xstr("drop table if exists ", name, End); };
char* create_sql(map* tbl,char* name){
	if(!name){ name=map_val(tbl,"name"); };
	map* cls=new_map();
	map* map_1=map_val(tbl,"cols"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v =map_id(map_1,i); char* k=map_key(map_1, i); add(cls,k,lite_create_col(v)); };
	map* pkeys=cols_pkeys(map_val(tbl,"cols"));
	if(map_len(pkeys)==1 && is_word(map_val(map_val(map_val(tbl,"cols"),map_id(pkeys,0)),"type"),"int integer")){
		add(cls,map_id(pkeys,0),xstr(map_id(pkeys,0), " integer primary key autoincrement", End));
	}else{
		vec_add(cls,xstr("primary key (", map_join(pkeys,","), ")", End)); };
	return xstr("create table ", name, " (", map_join(cls,","), ")", End);
};
char* schema_type(char* type,int size){
	if(size==36){ return "guid"; };
	if(size==1 && is_word(type,"int tinyint")){ return "bool"; };
	if(str_eq(type,"clob")){ return "para"; };
	if(is_word(type,"mediumblob longblob")){ return "blob"; };
	if(is_word(type,"string varchar2 varchar char")){
		if(size>=200){ return "para"; };
		if(size>=128){ return "text"; };
	};
	if(is_word(type,"varchar2 varchar char")){ return "code"; };
	if(is_word(type,"number integer")){ return "int"; };
	if(str_eq(type,"real")){ return "float"; };
	if(str_eq(type,"timestamp")){ return "datetime"; };
	if(str_eq(type,"integer")){ return "int"; };
//"%s=%s".dx(type,type.is_word("varchar2 varchar"))
//type.dxx()
	return type;
};
int has_table(char* db,char* tbl){
	return to_int(sql_value("select count(*) from sqlite_master where type='table' and name=:name",db,xmap("name", tbl, End)));
};
map* map_cols(void* cols,char* tbl,char* db){
	for(int i=next(cols,-1,NULL,NULL); has_id(cols,i); i++){ void* f=map_id(cols,i); char* name=map_key(cols, i);
		add(f,"name",name);
		add(f,"table",tbl);
		add(f,"db",db); };
	return cols;
};
map* cols_table(map* cols,char* tbl,char* db){
	if(!cols){ return NULL; };
	return xmap(
		"name", tbl,
		"cols", map_cols(cols,tbl,db),
		"db", db,
		"item", str_title(tbl)
	, End);
};
map* cols_collect(map* cols,void* collect){
	assert(is_map(cols));
	if(is_str(collect)){ collect=str_map(collect,Map); };
	map* ret=new_map();
	for(int next1=next(collect,-1,NULL,NULL); has_id(collect,next1); next1++){ void* op=map_id(collect,next1); char* f=map_key(collect, next1);
		if(str_eq(f,"*")){ map_merge(ret,cols); }
		else if(f[0]=='-'){
			map_compact(map_del_key(ret,sub_str(f,1,-2147483648)));
		}else if(str_has(f,"-")){
			map* limit=str_split(f,"-",2);
			int inlimit=0;
			for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ char* f1=map_key(cols, next1);
				if(str_eq(map_id(limit,0),f1)){
					inlimit=1; };
				if(inlimit){
					add(ret,f1,map_val(cols,f1)); };
				if(str_eq(map_id(limit,1),f1)){
					inlimit=0; }; };
		}else{
			add(ret,f,map_val(cols,f));
			if(is_map(map_val(ret,f))){
				add(ret,f,map_dup(map_val(ret,f)));
				if(is_map(op)){ map_merge(map_val(ret,f),op); };
				if(str_eq(map_val(map_val(ret,f),"type"),"-")){ add(add_key(ret,f,Map),"type",map_val(map_val(cols,f),"type")); }; }; }; };
	return ret;
};
char* name_type(char* name){
	map* map_1=types(); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ char* type=map_key(map_1, next1);
		if(str_has(name,type)){ return type; }; };
	map* map_2=str_map("name=text,desc=para,title=text",Map); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* val=map_id(map_2,next1); char* key=map_key(map_2, next1);
		if(str_has(name,key)){ return val; }; };
	return "text";
};
void retree(char* db){
	map* rows=sql_rows("section",db,NULL);
	sql_exec("delete from section",db,NULL);
	for(int next1=next(rows,-1,NULL,NULL); has_id(rows,next1); next1++){ void* row=map_id(rows,next1);
		map_del_key(row,"lft");
		map_del_key(row,"rgt");
		row_insert(row,"section",db);
	};};
