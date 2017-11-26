#include "fox.h"

int _is_web=0;

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
void* key_var(char* key){
	if(!key){ return NULL; };
	if((long)key<0){ return int_var(-(long)key); };
	return key;
};
map* map_key_val(map* mp,int idx){//0
	assert(idx>=0 && idx<mp->len);
	return xvec(key_var(mp->pairs[idx].id),map_id(mp,idx), End);
};
char* join_clause(char* pre,map* mp,char* clause,char* by,char* sub1,char* sub2,char* use_cls){
	map* mp1=mp;
	if(clause){ mp1=map_val(mp,clause); };
	char* ret=NULL;
	for(int idx2=next(mp1,-1,NULL,NULL); has_id(mp1,idx2); idx2++){
		char* ret2=NULL;
		if(!sub1){ ret2=to_str(map_id(mp1,idx2),"",0); };
		if(!sub2){ ret2=sql_map_join(map_id(mp1,idx2),sub1); }
		else{
			map* mp2=map_id(mp1,idx2);
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
	map* last_join_type=NULL;
	map* map_1=map_split(map_val(cls,"from"),"join",0); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* v=map_id(map_1,idx);
		map* join_type=NULL;
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
		map_add(wheres,key,val); };
	return wheres;
};
map* de_where(map* cls){
	map* mp=map_val(cls,"where");
	if(!mp){ return cls; };
	return add(cls,"where",parse_where(mp));
};
map* de_having(map* cls){
	map* mp=map_val(cls,"having");
	if(!mp){ return cls; };
	return add(cls,"having",parse_where(mp));
};
map* de_order(map* cls){
	map* mp=map_val(cls,"order");
	if(!mp){ return cls; };
	if(str_eq(map_id(mp,0),"by")){ vec_compact(vec_del(mp,1-1,1)); };
	map* ret=new_map();
	map* map_1=map_split(mp,",",0); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* val=map_id(map_1,idx);
		map* mp1=val;
		char* col=sql_str(map_id(mp1,0));
		char* ord=sql_str(map_id(mp1,1));
		int iord=1;
		if(str_eq(ord,"desc")){ iord=-1; };
		map_add(ret,col,int_var(iord));
	};
	add(cls,"order",ret);
	return cls;
};
map* de_group(map* cls){
	map* mp=map_val(cls,"group");
	if(!mp){ return cls; };
	if(str_eq(map_id(mp,0),"by")){ vec_compact(vec_del(mp,1-1,1)); };
	add(cls,"group",map_split(mp,",",0));
	return cls;
};
map* de_limit(map* cls){
	map* mp=map_val(cls,"limit");
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
map* to_vec(void* val){
	map* mp=new_vec();
	vec_add(mp,val);
	return mp;
};
map* sql_order(char* sql){
	map* mp=sql_map(sql);
	map* ret=new_map();
	map* map_1=map_val(map_id(mp,mp->len-1),"order"); for(int idx=next(map_1,-1,NULL,NULL); has_id(map_1,idx); idx++){ void* val=map_id(map_1,idx);
		map* mp1=val;
		char* col=sql_str(map_id(mp1,0));
		char* ord=sql_str(map_id(mp1,1));
		int iord=1;
		if(str_eq(ord,"desc")){ iord=-1; };
		map_add(ret,col,int_var(iord));
	};
	return ret;
};
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
		map* ret=map_val(sql_select_cols(map_val(sql_tables(sql,db),map_id(exp,0)),db,NULL),map_id(exp,2));
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
		if(str_eq(name,"substr")){
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
			char* talias=substr(k,0,-2);
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
map* cols_collect(map* cols,void* collect){
	assert(is_map(cols));
	if(is_str(collect)){ collect=str_map(collect,Map); };
	map* ret=new_map();
	for(int next1=next(collect,-1,NULL,NULL); has_id(collect,next1); next1++){ void* op=map_id(collect,next1); char* f=map_key(collect, next1);
		if(str_eq(f,"*")){ map_merge(ret,cols); }
		else if(f[0]=='-'){
			map_compact(map_del_key(ret,sub_str(f,1,0)));
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
char* name_type(char* name){
	map* map_1=map_val(_globals,"types"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ char* type=map_key(map_1, next1);
		if(str_has(name,type)){ return type; }; };
	map* map_2=str_map("name=text,desc=para,title=text",Map); for(int next1=next(map_2,-1,NULL,NULL); has_id(map_2,next1); next1++){ void* val=map_id(map_2,next1); char* key=map_key(map_2, next1);
		if(str_has(name,key)){ return val; }; };
	return "text";
};
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
char* table_name(char* tbl,char* db){
	void* ret=map_val(table(tbl,db),"name");
	return ret ? ret : str_title(tbl);
};
char* sql_add_cls(char* sql,char* cls,map* vals){
	map* dsql=sql_map(sql);
	add(dsql,cls,vals);
	return map_sql(dsql);
};
map* sql_cls(char* sql,char* cls){ return map_val(sql_map(sql),cls); };
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
		if(is_str(k)){ ret=xcat(ret,k,"=", End); };
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
	int i=0;
	if(!cls){ return NULL; };
	char* ret=" limit ";
	int ioff=is_int(map_val(cls,"offset"));
	if(ioff){ ret=xcat(ret,int_str(ioff),", ", End); };
	ret=xcat(ret,int_str(is_int(map_val(cls,"limit"))), End);
	return ret;
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
int type_size(char* type){
	return to_int(maptype(""
	"	text=128\n"
	"	code=50\n"
	"	guid=36\n"
	"	int=18\n"
	"	bool=1\n"
	"	para=256\n"
	"	daymonth=5\n"
	"	month=7"
	"",type));
};
char* lite_create_col(map* col){
	int size=to_int(map_val(col,"size"));
	if(!size){ size=type_size(map_val(col,"type")); };
	char* type=maptype(""
	"	text=varchar\n"
	"	para=clob\n"
	"	file=blob\n"
	"	int=number\n"
	"	float=float\n"
	"	date=date\n"
	"	time=time\n"
	"	datetime=datetime\n"
	"	daymonth=varchar\n"
	"	month=varchar"
	"",map_val(col,"type"));
	char* name=map_val(col,"name");
	if(str_eq(type,"varchar")){ return xstr(name, " varchar(",int_str( size), ") collate nocase not null default ''", End); };
	if(str_eq(type,"number")){ return xstr(name, " integer not null default 0", End); };
	if(str_eq(type,"float")){ return xstr(name, " real not null default 0", End); };
	if(str_eq(type,"clob")){ return xstr(name, " clob not null default ''", End); };
	if(str_eq(type,"blob")){ return xstr(name, " blob not null default ''", End); };
	if(str_eq(type,"date")){ return xstr(name, " date not null default ''", End); };
	if(str_eq(type,"time")){ return xstr(name, " time not null default ''", End); };
	if(str_eq(type,"datetime")){ return xstr(name, " datetime not null default ''", End); };
	if(str_eq(type,"yearmonth")){ return xstr(name, " varchar(7) not null default ''", End); };
	assert(0);
	return NULL;
};
int is_indexed(char* type){
	return to_int(maptype(""
	"	text=0\n"
	"	code=1\n"
	"	password=0\n"
	"	email=0"
	"",type));
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
	}else {vec_add(cls,xstr("primary key (", map_join(pkeys,","), ")", End));};
	return xstr("create table ", name, " (", map_join(cls,","), ")", End);
};
char* meta_type(char* type,int size){
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
map* tables(char* db){
	map* ret=new_map();
	map* map_1=table_names(db); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* tbl=map_id(map_1,i);
		add(ret,tbl,table(tbl,db)); };	
	return ret;
};
int db_has_schema(char* db){
	if(!map_has_key(map_val(map_val(_globals,"cache"),"has_schema"),db)){
		add(add_key(add_key(_globals,"cache",Map),"has_schema",Map),db,int_var(has_table(db,"_schema"))); };
	return is_int(map_val(map_val(map_val(_globals,"cache"),"has_schema"),db));
};
map* tbl_cols(char* tbl,char* db){
	return map_val(table(tbl,db),"cols");
};
map* table(char* tbl,char* db){
	if(!db){ return NULL; };
	char* sign=xstr(db,tbl, End);
	void* ret=NULL;
	if((ret=cache(sign,"table",NULL))){ return ret; };
	if(str_eq(tbl,"_schema")){
		ret=cols_table(str_map(""
			"type=:code pkey\n"
			"name=:code pkey\n"
			"data=:source"
			"",Map),"_schema",db); };
	if(str_eq(tbl,"_fts")){
		ret=cols_table(str_map(""
			"id=code\n"
			"id2=code\n"
			"id3=code\n"
			"id4=code\n"
			"tbl=code\n"
			"body=text"
			"",Map),"_fts",db); };
	if(!ret){ ret=db_has_schema(db) ? schema_table(db,tbl) :  db_table(db,tbl); };
	return cache(sign,"table",ret);
};
map* schema_tables(char* db){
	map* ret=new_map();
	map* map_1=schema_list(db,"table"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* tbl=map_id(map_1,next1);
		add(ret,tbl,schema_table(db,tbl)); };
	return ret;
};
map* schema_table(char* db,char* tbl){
	return map_table(str_map(sql_value("select data from _schema where type='table' and name=:name",db,xmap("name", tbl, End)),Map),tbl,db);
};
map* db_table(char* db,char* tbl){
	return cols_table(db_cols(db,tbl),tbl,db);
};
map* db_tables(char* db){
	map* ret=new_map();
	map* map_1=user_tables(db_table_names(db)); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		add(ret,v,db_table(db,v)); };
	return ret;
};
map* db_cols(char* db,char* tbl){
	map* rs=lite_exec(xstr("pragma table_info (", tbl, ")", End),db,NULL);
	map* ret=new_map();
	//name|type|notnull|dflt_value|pk
	for(int idx=next(rs,-1,NULL,NULL); has_id(rs,idx); idx++){ void* v=map_id(rs,idx);
		map* col=new_map();
		map* mp=v;
		char* name=str_lower(map_val(mp,"name"));
		add(col,"name",name);
		map* func=get_func(sql_toks(map_val(mp,"type")),1);
		if(!func){ add(col,"type",meta_type(map_val(mp,"type"),0)); }
		else{
			int size=stoi(map_id(map_id(func,1),0));
			add(col,"size",int_var(size));
			add(col,"type",meta_type(map_id(func,0),size));
		};
		if(stoi(map_val(mp,"pk"))){ add(col,"pkey",int_var(1)); };
		if(!stoi(map_val(mp,"notnull")) && !map_val(col,"pkey")){ add(col,"isnull",int_var(1)); };
		add(col,"db",db);
		add(col,"table",tbl);
		add(ret,name,col);
	};
	add(add_key(_globals,"schema",Map),tbl,ret);
	return ret;
};
map* row_ids(void* row,char* tbl,char* db){
	map* ret=new_map();
	map* map_1=tbl_pkeys(tbl,db); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); add(ret,v,map_val(row,v)); };
	return ret;
};
map* pkeys_where(char* tbl,char* db){
	map* ret=new_map();
	map* map_1=tbl_pkeys(tbl,db); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); map_add(ret,v,xstr(":",v, End)); };
	return ret;
};
map* tbl_row_ids(char* tbl,char* db,map* row){
	map* ret=new_map();
	map* map_1=tbl_pkeys(tbl,db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* f=map_id(map_1,next1);
		add(ret,f,map_val(row,f)); };
	return ret;
};
char* tbl_row_id(char* tbl,char* db,map* row){
	return map_join(tbl_row_ids(tbl,db,row),"\t");
};
char* ids_id(map* ids){ return map_join(ids,"\t"); };
map* url_gets(){ return map_val(map_val(_globals,"url"),"params"); };
map* get_ids(char* tbl,char* db){ return sql_id_ids(tbl,db,map_val(url_gets(),"id")); };
char* sql_table(char* sql){
	if(is_code(sql)){ return sql; };
	return sql_table(map_val(map_id(sql_cls(sql,"from"),0),"tbl"));
};
map* sql_pkeys(char* sql,char* db){ return tbl_pkeys(sql_table(sql),db); };
void* sql_value(char* sql,char* db,map* param){ return map_id(map_id(lite_exec(sql,db,param),0),0); };
map* sql_rows(char* sql,char* db,map* param){
	if(!strchr(sql,' ')){ sql=xstr("select * from ", sql, End); };
	return lite_exec(sql,db,param);
};
map* sql_query(char* sql,char* db,map* where){ return sql_rows(sql_add_where(sql,where),db,where); };
map* tbl_id_ids(char* tbl,char* db,void* id){
	map* pkeys=tbl_pkeys(tbl,db);
	map* ret=new_map();
	if(is_i(id)){ id=to_str(id,"",0); };
	if(is_str(id)){ id=str_split(id,"\t",0); };
	assert(is_map(id));
	if(pkeys->len!=map_len(id)){ fox_error(xstr("PKey=", pkeys, " and id=", id, ", number of field mismatched", End),0); };
	if(is_vec(id)){
		for(int idx=next(pkeys,-1,NULL,NULL); has_id(pkeys,idx); idx++){ void* f=map_id(pkeys,idx);
			add(ret,f,map_id(id,idx)); };
		return ret; };
	for(int next1=next(pkeys,-1,NULL,NULL); has_id(pkeys,next1); next1++){ void* f=map_id(pkeys,next1);
		add(ret,f,map_val(id,f)); };
	return ret;
};
map* sql_id_ids(char* sql,char* db,void* id){ return tbl_id_ids(sql_table(sql),db,id); };
map* sql_id(char* sql,char* db,void* ids){
	return sql_row(map_sql(add(map_del_key(map_del_key(map_del_key(sql_map(sql),"order"),"where"),"limit"),"where",pkeys_where(sql_table(sql),db))),db,sql_id_ids(sql,db,ids));
};
map* sql_row(char* sql,char* db,map* param){ return map_id(lite_exec(sql_lite(sql,db,param),db,param),0); };
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
char* to_sql(char* sql){
	return map_sql(sql_map(sql));
};
map* schema_fkeys(char* db){
	if(cache(NULL,"schema_fkeys",NULL)){ return cache(NULL,"schema_fkeys",NULL); };
	map* ret=new_vec();
	map* map_1=schema_tables(db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* tbl=map_id(map_1,next1); char* name=map_key(map_1, next1);
		map* map_1=map_val(tbl,"cols"); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* col=map_id(map_1,next1); char* f=map_key(map_1, next1);
			if(map_val(col,"sql")){
				char* sql=fkey_sql(map_val(col,"sql"),db);
				vec_add(ret,xmap("table", name, "f", f, "table2", sql_table(sql), "f2", map_val(map_id(sql_cols(sql,db,NULL),0),"name"), "db", map_val(col,"db"), End));
				dx(ret,NULL,0); }; }; };
	return cache(NULL,"schema_fkeys",ret);
};
map* tbl_referred_by(char* tbl,char* db){
	map* ret=new_vec();
	map* map_1=schema_fkeys(db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* rel=map_id(map_1,next1);
		if(str_eq(map_val(rel,"table2"),tbl)){
			vec_add(ret,xmap("table", map_val(rel,"table"), "f", map_val(rel,"f"), "f2", map_val(rel,"f2"), End)); }; };
	return ret;
};
void* id_update(void* ids,char* tbl,char* db,map* row){
	ids=tbl_id_ids(tbl,db,ids);
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
map* row_insert(map* row,char* tbl,char* db){
	map* cols=tbl_cols(tbl,db);
	if(!cols){ return fox_error(xstr("Table ", tbl, " not found", End),0); };
	map* fld=new_vec();
	for(int idx=next(cols,-1,NULL,NULL); has_id(cols,idx); idx++){ char* k=map_key(cols, idx);
		if(!map_val(row,k)){ continue; };
		vec_add(fld,k); };
	return lite_exec(xstr("insert into ", tbl, " (", map_join(fld,", "), ") values (:", map_join(fld,", :"), ")", End),db,row);
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
map* parse_connection2(char* in){
	if(!str_has(in,",")){ return xmap( "type", "lite", "name", in, "desc", in , End); };
	map* ret=str_split(in,",",0);
	return xmap( "type", map_id(ret,0), "host", map_id(ret,1), "user", map_id(ret,2), "pass", map_id(ret,3), "name", map_id(ret,4), "desc", in , End);
};
map* parse_connection(char* in){
	map* ret=str_split(in,";",0);
	ret=xmap("db" , parse_connection2(map_id(ret,0)), "ddb" , map_id(ret,1) ? map_id(ret,1) : map_id(ret,0), "module" , map_id(ret,2), "dd" , str_split(map_id(ret,3),",",0), "desc", in , End);
	add(ret,"type",map_val(map_val(ret,"db"),"type"));
	return ret;
};
map* conn_db(char* db){
	if(!is_code(db)){ return map_val(parse_connection(db),"db"); };
	void* ret=map_val(map_val(map_val(_globals,"dbs"),db),"db");
	if(!ret){ fox_error(xstr("SQLite connection ", db, " not found", End),0); };
	return ret;
};
map* lite_exec(char* sql,char* db,map* params){
	db=map_val(conn_db(db),"name");
	if(!db){ fox_error("query() connection not found",0); };
	sqlite3* conn=NULL;
	sqlite3_stmt* stm=NULL;
	start_time();
	if(sqlite3_open_v2(db,&conn,SQLITE_OPEN_READWRITE|SQLITE_OPEN_URI,NULL)!=SQLITE_OK){
		return sql_error(sql,db,conn); };
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
	if(sqlite3_close_v2(conn)!=SQLITE_OK){ return sql_error(sql,db,conn); };
	end_time();
	return ret;
};
map* db_table_names(char* db){
	if(map_val(map_val(_globals,"schema"),"_tbls")){
		return map_val(map_val(_globals,"schema"),"_tbls"); };
	add(add_key(_globals,"schema",Map),"_tbls",sql_pairs("select name from sqlite_master where type='table' order by 1",db,NULL));
	return map_val(map_val(_globals,"schema"),"_tbls");
};
map* schema_table_names(char* db){
	return sql_vector("select name from _schema where type='table' order by name",db,NULL);
};
map* user_tables(map* tbls){
	map* ret=new_vec();
	for(int i=next(tbls,-1,NULL,NULL); has_id(tbls,i); i++){ char* k=map_key(tbls, i);
		if(fox_at(k,0)=='_' || is_word(k,"sqlite_sequence")){
			continue; };
		vec_add(ret,k); };
	return ret;
};
map* table_names(char* db){
	if(db_has_schema(db)){ return schema_table_names(db); };
	return user_tables(db_table_names(db));
};
char* col_title(map* col){
	return str_title(map_val(col,"name"));
};
map* cols_pkeys(map* cols){
	map* ret=new_map();
	for(int i=next(cols,-1,NULL,NULL); has_id(cols,i); i++){ void* v=map_id(cols,i); char* k=map_key(cols, i);
		if(map_val(v,"pkey")){ add(ret,k,k); }; };
	if(!map_len(ret)){ add(ret,map_key(cols,0),map_key(cols,0)); };
	return ret;
};
map* crud_save(char* sql,char* db,map* cols,char* back,map* data){
	if(map_val(url_gets(),"id")){ id_update(map_val(url_gets(),"id"),sql,db,data); }
	else {row_insert(data,sql_table(sql),db);};
	http_redirect(back);
	return NULL;
};
char* maptype(char* type_map,char* type){
	map* mp=str_map(type_map,Map);
	while(type){
		if(map_val(mp,type)){ return to_str(map_val(mp,type),"",0); };
		type=map_val(map_val(_globals,"types"),type); };
	return type;
};
/*
char* bg_no_en(char* no){
	$len=mb_strlen($no,'UTF-8');
	$ret='';
	for($i=0;$i<$len;$i++){
		$c=mb_substr($no,$i,1,'UTF-8');
		if $c==','||$c==' ' => continue;
		$n=mb_ord($c);
		if $n>=1632 && $n<1632+10 => $ret.=chr($n-1632+ord('0'));
		else if $n>=2534 && $n<2534+10 => $ret.=chr($n-2534+ord('0'));
		else $ret.=$c;
	}
	return $ret;
}
*/
map* tbl_pkeys(char* tbl,char* db){ return cols_pkeys(tbl_cols(tbl,db)); };
map* tbl_skeys(char* tbl,char* db){ return cols_skeys(tbl_cols(tbl,db)); };
map* cols_skeys(map* cols){
	if(!map_len(cols)){ return NULL; };
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ char* name=map_key(cols, next1); if(str_has(name,"name")){ return xmap("name", name, End); }; };	
	return xmap(map_key(cols,0), map_key(cols,0), End);
};
map* toks_sql_params(map* toks, map* ret){
	for(int next1=next(toks,-1,NULL,NULL); has_id(toks,next1); next1++){ void* val=map_id(toks,next1);
		if(is_map(val)){ toks_sql_params(val,ret); continue; };
		if(fox_at(val,0)==':'){ add(ret,sub_str(val,1,0),sub_str(val,1,0)); }; };
	return ret;
};
void* is_null(void* val){
	if(!val){ return NULL; };
	if(is_map(val)){ return map_len(val) ? val : NULL; };
	if(is_str(val)){ return str_len(val) ? val : NULL; };
	if(is_i(val)){ return is_int(val) ? val : NULL; };
	return val;
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
map* sql_params(char* sql){
	return toks_sql_params(sql_toks(sql),new_map());
};
char* fkey_sql(char* sql,char* db){
	map* cols=sql_select_cols(sql,db,NULL);
	void* pkey=map_id(cols_pkeys(cols),0);
	void* skey=map_id(cols_skeys(cols),0);
	map* ret=sql_map(sql);
	add(ret,"select",xmap("id", map_val(map_val(cols,pkey),"expr"), "name", map_val(map_val(cols,skey),"expr"), End));
	return sql_add_order(map_sql(ret),db,NULL);
};
char* callback_php(map* param,char* name){ return call_php(param,name); };
char* fkey_show(char* sql,char* db,char* id){
	char* sign=xstr(sql, "/", id, End);
	char* ret=NULL;
	if((ret=cache(sign,"str_show",NULL))){ return ret; };
	return cache(sign,"str_show",map_val(sql_row(sql_add_where(fkey_sql(sql,db),xmap("id", ":id", End)),db,xmap("id", id, End)),"name"));
};
int fkey_valid(char* sql,char* db,char* id){
	return fkey_show(sql,db,id) ? 1 : 0;
};
char* str_show(char* value,char* type,map* op,int width){
	value=to_str(value,"",0);
	if(!value){ return ""; };
	if(!type){ return value; };
	if(map_val(op,"list")){ return map_val(map_val(op,"list"),value) ? str_title(map_val(map_val(op,"list"),value)) : value; };
	if(map_val(op,"sql") && map_val(op,"db")){ return fkey_show(map_val(op,"sql"),map_val(op,"db"),value); };
	type=maptype(""
	"	text=text\n"
	"	para=para\n"
	"	source=source\n"
	"	password=password\n"
	"	html=html\n"
	"	guid=guid\n"
	"	amount=number\n"
	"	mins=mins\n"
	"	debit=debit\n"
	"	credit=credit\n"
	"	bool=bool\n"
	"	email=email\n"
	"	date=date\n"
	"	quarter=quarter\n"
	"	file=file\n"
	"	jpeg=image\n"
	"	duration=duration"
	"",type);
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
	if(str_eq(type,"mins")){ int n=to_int(value); return mstr("%d:%02d",n/60,n%60, End); };
	if(str_eq(type,"duration")){ return "Duration/Pending"; };
	if(str_eq(type,"date")){ return date_show(value); };
	if(str_eq(type,"quarter")){ return "Pending-Qurter"; };
	if(str_eq(type,"image")){ return value ? xstr("<img src=",map_val(_globals,"base_url"),"/",thumb_name(value),"></img>", End) : "--"; };
	return value;
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
char* cols_show(map* cols,map* row,char* name,int width){
	return str_show(map_val(row,name),map_val(map_val(cols,name),"type"),map_val(cols,name),width);
};
/*
function duration($n){
	if $n<60 => return "less than a minute";
	$n/=60;
	if $n<2 => return ((int)$n)." minute";
	if $n<60 => return ((int)$n)." minutes";
	$n/=60;
	if $n<2 => return ((int)$n)." hour";
	if $n<24 => return ((int)$n)." hours";
	$n/=24;
	if $n<2 => return ((int)$n)." day";
	if $n<30 => return ((int)$n)." days";
	if $n<60 => return ((int)($n/30))." month";
	if $n<365 => return ((int)($n/30))." months";
	$n/=365;
	if $n<2 => return ((int)$n)." year";
	return ((int)$n)." years";
}
char* text_action(char* action,void* value,map* op,char* ns=:form){
	if action===:read
		return value.trim()
	if action==:html
		return <input - input-medium name=ns.."-"..name id=ns_name.str_code().str_quote()>

}
char* val_trim(void* val,char* type){
	if !val.is_map() => return val.to_str().trim()
	type="
		text=text
		html=html
		int=int
		bool=bool
		date=date
		time=time
		datetime=datetime
		daymonth=daymonth
		yearmonth=yearmonth
		month=month
		mins=mins
		source=source
		file=file
		inline=array
		intrange=intrange
		multicheck=multicheck
		duration=duration
		".maptype(type)
	if !type => fox_error("No type found for value $v")
	if type=='multicheck' => return v
	if type=='source' => return v
	if type=='array' => return v
	if type=='file' => return v
	if type=='text' => return trim(v)
	if type=='html' => return v
	if type=='int' => return v ? enno($v) : ''
	if type=='bool' => return v ? 1 : ''
	if $type=='duration' =>{
		if !is_array($v) => return $v;
		$a=array(:sec=>1,:min=>60,:hour=>60*60,:day=>24*3600,:week=>7*86400,:month=>30*86400,:year=>365*86400);
		if !@$v['amount'] => return 0;
		return $v['amount']*@$a[$v['unit']];
	}
	if $type=='intrange' =>{
		if is_array($v) =>{
			if strlen(@$v['from'] && !is_numeric($v['from'])) => $v['from']='';
			if strlen(@$v['upto'] && !is_numeric($v['upto'])) => $v['upto']='';
			if !strlen(@$v['from']) && !strlen(@$v['upto']) => return '';
			return sprintf("%s\t%s",$v['from'],$v['upto']);
		}
		if $v=="\t" => return '';
		return $v;
	}
	if $type=='mins' =>{
		if is_array($v) =>{
			$h=$v['hour'];
			$m=$v['min'];
			return $h*60+$m;
		}
		return $v;
	}
	if $type=='datetime' =>{
		if is_array($v) =>{
			return trim_type('date',$v).' '.trim_type('time',$v);
		}
		return $v;
	}
	if $type=='time' =>{
		if is_array($v) =>{
			$h=$v['hour'];
			$m=$v['min'];
			$am=$v['ampm'];
			if !$h || !$am => return '';
			if $am=='am' && $h==12 => $h=0;
			else if $am=='pm' && $h<12 => $h+=12;
			return sprintf("%02d:%02d:00",$h,$m);
		}
		return $v;
	}
	if $type=='date'||$type=='month'||$type=='yearmonth'||$type=='daymonth' =>{
		if is_string($v) =>{
			if fox_str_has($v,'/') =>{
				$a=explode('/',$v);
				return trim_type('date',array('day'=>@$a[0],'month'=>@$a[1],'year'=>@$a[2]));
			}
			if $v=='0000-00-00 00:00:00' => return '';
			if $v=='0000-00-00' => return '';
			if $v=='00:00:00' => return '';
			if $v=='0000-00' => return '';
			if $v=='00-00' => return '';
			if $v=='0000-00' => return '';
			if strpos(' '.$v,"--") =>{
				$a=explode("--",$v);
				$a[0]=trim_type($type,@$a[0]);
				$a[1]=trim_type($type,@$a[1]);
				$v="$a[0]--$a[1]";
				return $v=="--" ? '' : $v;
			}
			return $v;
		}
		if is_array($v) =>{
			if @$v['from']||@$v['upto'] =>{
				$v=trim_type('date',@$v['from'])."--".trim_type('date',@$v['upto']);
				return $v=="--" ? '' : $v;
			}
			if !(@$v['day']+0) && !(@$v['month']+0) && !(@$v['year']+0) => return '';
			if $type=='month' => return sprintf("%04d-%02d",$v['year'],$v['month']);
			if $type=='yearmonth' => return sprintf("%04d-%02d",$v['year'],$v['month']);
			else if $type=='daymonth' => return sprintf("%02d-%02d",$v['month'],$v['day']);
			else return sprintf("%04d-%02d-%02d",$v['year'],$v['month'],$v['day']);
		}
		return $v;
	}
	fox_error("Type $type trimming failed");
}
*/
char* buttons_html(map* buttons){
	map* ret=new_vec();
	for(int i=next(buttons,-1,NULL,NULL); has_id(buttons,i); i++){ void* v=map_id(buttons,i); char* k=map_key(buttons, i);
		if(str_has(k,"/")){
			vec_add(ret,render_template(xmap(
				"text", v,
				"url", k,
				"css", "btn-default"
				, End),"button-link"));
		}else{
			vec_add(ret,render_template(xmap(
				"name", k,
				"value", v,
				"css", ""
				, End),"button")); }; };
	return render(ret,""
		"--body\n"
		"#{_val}"
		"");
};
//char* cols_html(map* cols,map* buttons,char* title=NULL,map* vals=NULL,map* errs=NULL){
//	cols=cols.cols_ctrls(vals,errs)
//	cols.each v,k,i
//		v.html=v.render_template(v.ctrl)
//		v.html=v.render_template(:label)
//	cols.dx()
//	char* body=xmap(:cols,cols.render("
//			--body
//			#{html}
//			"),
//		:title,title).render_template(:group)
//
//	return xmap(
//		:msg, errs.map_len() ? "Please correct these errors" : "",
//		:state, errs.map_len() ? :fox_error : :help,
//		:body, body,
//		:button,buttons.buttons_html(),
//		:method,:post
//	).render_template(:form)
//}
//char* test_html(){
//	return "
//		name=:text
//		age=:int
//		".str_map().cols_html("
//			save=Save
//			cancel=Cancel
//		".str_map())
//}
map* cols_list(map* cols,char* crud,int max){
	map* ret=new_map();
	char* aligns=""
		"text=left\n"
		"int=right"
		"";
	if(max && crud){ max--; };
	for(int i=next(cols,-1,NULL,NULL); has_id(cols,i); i++){ void* v=map_id(cols,i); char* k=map_key(cols, i);
		if(i>=max){ break; };
		add(ret,k,xmap("title", map_val(v,"title") ? map_val(v,"title") : str_title(k),
			"html", map_val(v,"html") ? map_val(v,"html") : xcat("#{row.",k,"}", End), End));
		if(str_eq(maptype(aligns,map_val(v,"type")),"right")){
			add(add_key(add_key(ret,k,Map),"css",Map),"text-align","right"); };
		if(map_val(v,"link")){
			add(add_key(ret,k,Map),"html",xstr("<a href='",map_val(v,"link"),"'>",map_val(map_val(ret,k),"html"),"</a>", End)); }; };
	if(crud){
		map* actions=new_map();
		if(str_eq(crud,"crud")){ crud="add edit delete"; };
		if(is_word("edit",crud)){
			add(actions,"Edit",xstr("edit/?id=#{row.",map_join(cols_pkeys(cols),"}-#{row."),"}", End)); };
		if(is_word("delete",crud)){
			add(actions,"Delete",xstr("delete/?id=#{row.",map_join(cols_pkeys(cols),"}-#{row."),"}", End)); };
		add(ret,"action",xmap("title","Actions", "html", render_template(actions,"button-dropdown"), End));
	};
	return ret;
};
map* map_table(map* mp,char* tbl,char* db){
	map* ret=cols_table(map_val(mp,"cols"),tbl,db);
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v =map_id(mp,i); char* k=map_key(mp, i); if(!str_eq(k,"cols")){ add(ret,k,v); }; };
	return ret;
};
map* table_map(map* tbl){
	map_compact(map_del_key(map_del_key(tbl,"name"),"db"));
	map* map_1=map_val(tbl,"cols"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* v2=map_id(map_1,i2); char* k2=map_key(map_1, i2);
		map_del_key(map_del_key(v2,"table"),"db");
		if(is_int(map_val(v2,"size"))==type_size(map_val(v2,"type"))){
			map_del_key(v2,"size"); };
		map_compact(v2); };
	return tbl;
};
map* tables_map(map* tbls){
	for(int i=next(tbls,-1,NULL,NULL); has_id(tbls,i); i++){ void* v =map_id(tbls,i); char* k=map_key(tbls, i); table_map(v); };
	return tbls;
};
char* db_file(char* db,char* file){
	return write_file(map_str(xmap("db", tables_map(db_tables(db)), End)),file,0);
};
char* schema_file(char* db,char* file){
	map* ret=new_map();
	map* map_1=sql_rows("_schema",db,NULL); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* row=map_id(map_1,i);
		add(add_key(ret,map_val(row,"type"),Map),map_val(row,"name"),str_map(map_val(row,"data"),Map)); };
	return write_file(map_str(ret),file,0);
};
map* file_compile(char* file,char* db,int go){
	map* ret=file_sync_schema(file,db,go);
	if(go){ file_schema(file,db); };
	return ret;
};
char* file_schema(char* file,char* db){
	init_schema(db);
	lite_exec("delete from _schema",db,NULL);
	map* map_1=file_map(file); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* vals=map_id(map_1,i); char* type=map_key(map_1, i);
		for(int i2=next(vals,-1,NULL,NULL); has_id(vals,i2); i2++){ void* data=map_id(vals,i2); char* name=map_key(vals, i2);
			row_insert(xmap("type", type,"name", name,"data", data, End),"_schema",db); }; };
	return file;	
};
char* init_schema(char* db){
	if(!has_table(db,"_schema")){
		lite_exec(create_sql(table("_schema",db),NULL),db,NULL);
		sqls_exec(create_index_sqls(table("_schema",db)),db);	
	}else {lite_exec("delete from _schema where type='table'",db,NULL);};
	return db;
};
char* db_schema(char* db){
	init_schema(db);
	map* map_1=db_tables(db); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* data=map_id(map_1,i); char* name=map_key(map_1, i);
		row_insert(xmap(
		"type", "db",
		"name", name,
		"data", map_str(table_map(data))
		, End),"_schema",db); };
	return db;
};
char* crud_tables(char* db){
	map* data=new_vec();
	map* map_1=table_names(db); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); char* k=map_key(map_1, i);
		vec_add(data,xmap(
			"table", v,
			"rows", sql_value(xstr("select count(*) from ", v, End),db,NULL),
			"cols",int_var( map_len(tbl_cols(v,db))
		), End)); };
	return render_table(data,cols_list(str_map(""
	"	table=:text link='table-#{table}/'\n"
	"	rows=:int\n"
	"	cols=:int"
	"",Map),NULL,12),"",NULL);
};
char* crud_list(char* sql,char* db,int limit,char* crud){
	return render_table(sql_rows(sql_add_limit(sql,limit,0),db,NULL),cols_list(sql_select_cols(sql,db,NULL),crud,12),sql,crud);
};
char* confirm(char* prompt,char* option1,char* option2,char* title,map* data){
	char* clicked=button_clicked(xmap(option1,NULL,option2,NULL, End));
	if(clicked){ return clicked; };
	call_func(xmap(
		"body",render_template(xmap("prompt",prompt,"option1",option1,"option2",option2,"title",title,"data",data, End),"confirm"),
		"title",title,
		"width",int_var( 2
	), End),"page",NULL);
	return NULL;		
};
char* sql_delete(void* ids,char* tbl,char* db){
	return xstr(xstr("delete from ", tbl, End),lite_exec(re_where(pkeys_where(tbl,db)),db,sql_id_ids(tbl,db,ids)), End);
};
char* crud_delete(char* sql,char* db,char* back){
	if(confirm("Delete this record?","Yes","No","Confirm",NULL)){
		sql_delete(map_val(url_gets(),"id"),sql_table(sql),db); };
	return http_redirect(back);
};
char* edit_schema(char* sql,char* db,map* cols,char* back){
	char* tbl=sql_table(sql);
	map* errs=process_post(xmap("save",xmap(NULL,"save_schema","db",db,"tbl",tbl,"back",back, End), End));
	map* vals=sql_id("_schema",db,xmap("type","db","name",tbl, End));
	return cols_html(tbl_cols("_schema",db),xmap("save","Save",back,"Cancel", End),"Edit Schema",vals,errs);
};
char* crud_add(char* sql,char* db,map* cols,char* back){
	map* errs=process_post(xmap("save",xmap(NULL,"crud_save","db",db,"sql",sql,"back",back,"cols",cols, End), End));
	map* vals=new_map();
	return cols_html(tbl_cols(sql_table(sql),db),xmap("save","Save",back,"Cancel", End),"Add New",vals,errs);
};
char* crud_edit(char* sql,char* db,map* cols,char* back){
	map* errs=process_post(xmap("save",xmap(NULL,"crud_save","db",db,"sql",sql,"back",back,"cols",cols, End), End));
	map* vals=sql_id(sql,db,get_ids(sql,db));
	return cols_html(tbl_cols(sql_table(sql),db),xmap("save","Save",back,"Cancel", End),"Edit Record",vals,errs);
};
char* render_table(map* rows,map* cols,char* sql,char* crud){
	for(int i=next(cols,-1,NULL,NULL); has_id(cols,i); i++){ void* v=map_id(cols,i); char* k=map_key(cols, i);
		if(!map_val(v,"css")){ continue; };
		add(v,"tag",render(map_val(v,"css"),""
			" style='\n"
			"--body\n"
			"#{_key}:#{_val};\n"
			"--foot\n"
			"'"
			"")); };
	char* th=render(cols,""
			"<thead><tr>\n"
			"--body\n"
			"<th#{tag}>#{title}</th>\n"
			"--foot\n"
			"</tr></thead>"
			"");
	char* td=render(cols,""
			"<tr>\n"
			"--body\n"
			"<td#{tag}>#{html}</td>\n"
			"--foot\n"
			"</tr>"
			"");
	if(crud){
		if(str_eq(crud,"crud")){ crud="add edit delete"; };
		map* actions=new_map();
		if(is_word("add",crud)){
			add(actions,"Add New","add/");	
			add(actions,"Edit Schema","schema/"); };
		crud=render_template(actions,"button-dropdown"); };
	return render(rows, str_subst(map_val(map_val(_globals,"html"),"table"),xmap("td",td,"th",th,"sql",sql,"actions",crud, End)) );
};
char* cols_html(map* cols,map* buttons,char* title,map* vals,map* errs){
	cols=cols_ctrls(cols,vals,errs);
	for(int i=next(cols,-1,NULL,NULL); has_id(cols,i); i++){ void* v=map_id(cols,i); char* k=map_key(cols, i);
		add(v,"html",render_template(v,map_val(v,"ctrl")));
		add(v,"html",render_template(v,"label")); };
	char* body=render_template(xmap("cols",render(cols,""
			"--body\n"
			"#{html}"
			""),
		"title",title, End),"group");

	return render_template(xmap(
		"msg", map_len(errs) ? "Please correct these errors" : "",
		"state", map_len(errs) ? "fox_error" : "help",
		"body", body,
		"button",buttons_html(buttons),
		"method","post"
	, End),"form");
};
char* rename_sql(char* from,char* into){
	return xstr("alter table ", from, " rename to ", into, End);
};
map* save_schema(char* db,char* tbl,map* data){
	if(!str_eq(tbl,map_val(data,"name"))){
		if(has_table(db,map_val(data,"name"))){ return xmap("name","Table exists", End); }; };
//		from.rename_sql(into)
//		xmap(:type,:db,:name,tbl).sql_delete(tbl,db)
//		xmap(:type,:db,:name,tbl,:data,data).sql_insert(tbl,db)
	return NULL;
};
map* cols_ctrls(map* cols,map* vals,map* errs){
	char* ctrls=""
	"	text=text\n"
	"	para=para\n"
	"	source=source"
	"";
	char* input_types=""
	"	text=text	\n"
	"	password=password"
	"";
	map* ret=new_map();
	for(int i=next(cols,-1,NULL,NULL); has_id(cols,i); i++){ void* v=map_id(cols,i); char* k=map_key(cols, i);
		map* ctrl=xmap(
		"name", k,
		"id", k,
		"value", map_val(vals,k),
		"placeholder", "",
		"ctrl",maptype(ctrls,map_val(v,"type")),
		"label", map_val(v,"label") ? map_val(v,"label") : str_title(k),
		"msg", map_val(errs,k) ? map_val(errs,k) : map_val(v,"req") ? xstr("<strong>* </strong>",map_val(v,"help"), End) : map_val(v,"help"),
		"state", map_val(errs,k) ? "has-fox_error" : "",
		"icon", map_val(errs,k) ? "remove" : map_val(v,"req") ? "asterisk" : ""
		, End);
		if(str_eq(map_val(ctrl,"ctrl"),"text")){
			add(ctrl,"type",maptype(input_types,map_val(v,"type"))); };
		add(ret,k,ctrl); };
	return ret;
};
map* subtypes(char* type){
	map* ret=new_vec();
	void* parent=map_val(map_val(_globals,"types"),type);
	while(parent){
		vec_add(ret,parent);
		parent=map_val(map_val(_globals,"types"),parent); };
	return vec_reverse(ret);
};
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
map* schema_list(char* db,char* type){ return sql_pairs("select name from _schema where type=:type",db,xmap("type", type, End)); };
map* schema_item(char* db,char* type,char* name){
	char* sign=xstr(db,type,name, End);		
	void* ret=NULL;
	if((ret=cache(sign,"schema_item",NULL))){ return ret; };
	return cache(sign,"schema_item",str_map(sql_value("select data from _schema where type=:type and name=:name",db,xmap("type", type,"name", name, End)),Map));
};
char* month_name(int month){
	char* names[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	return names[abs(month-1)%12];
};
char* date_show(char* in){
	if(!in || !str_len(in)){ return ""; };
	if(str_has(in,"--")){
		map* vals=str_split(in,"--",2);
		return xstr("From ", date_show(map_id(vals,0)), " Upto ", date_show(map_id(vals,0)), End); };
	int len=str_len(in);
	if(len==4){ return in; };
	if(len==7){
		map* vals=str_split(in,"-",0);
		return xstr(month_name(to_int(map_id(vals,1))), "-", map_id(vals,0), End); };
	if(len==10){
		map* vals=str_split(in,"-",0);
		return xstr(to_str(int_var(to_int(map_id(vals,2))),"",0), "/", to_str(int_var(to_int(map_id(vals,1))),"",0), "/", map_id(vals,0), End); };
	if(len==19){
		map* vals=str_split(in," ",0);
		return xstr(date_show(map_id(vals,0)), " ", date_show(map_id(vals,1)), End); };
	if(len==5 || len==8){
		map* vals=str_split(in,":",0);
		int h=to_int(map_id(vals,0));
		if(!h){ h=12; }
		else if(h>12){ h-=12; };
		return xstr(int_str(h), ":", map_id(vals,1), " ", to_int(map_id(vals,0))>=12 ? "pm" : "am", End); };
	return in;
};
void* crud(char* sql,char* db,map* cols,char* crud,int limit,void* link,char* rest,char* table){
	if(!sql){ sql=table; };
	if(!sql){
		return pages_exec(""
		"	index\n"
		"		page\n"
		"		title=Tables\n"
		"		body\n"
		"			crud_tables\n"
		"			db=#{db}\n"
		"		width=2\n"
		"	table-*/ **\n"
		"		crud				"
		"",rest,xmap("db", db, End)); };
	return pages_exec(""
	"index\n"
	"	page\n"
	"	title=Table List\n"
	"	body\n"
	"		crud_list\n"
	"		db=#{db}\n"
	"		sql=#{sql}\n"
	"		crud=crud\n"
	"	width=3\n"
	"edit\n"
	"	page\n"
	"	title=Edit Table\n"
	"	body\n"
	"		crud_edit\n"
	"		db=#{db}\n"
	"		sql=#{sql}\n"
	"	width=2\n"
	"add\n"
	"	page\n"
	"	title=Add New\n"
	"	body\n"
	"		crud_add\n"
	"		db=#{db}\n"
	"		sql=#{sql}\n"
	"	width=2\n"
	"delete\n"
	"	page\n"
	"	title=Delete Record\n"
	"	body\n"
	"		crud_delete\n"
	"		db=#{db}\n"
	"		sql=#{sql}\n"
	"	width=2\n"
	"schema\n"
	"	page\n"
	"	title=Table Structure\n"
	"	body\n"
	"		edit_schema\n"
	"		db=#{db}\n"
	"		sql=#{sql}\n"
	"	width=2"
	"",rest,xmap("db", db, "sql", sql, End));
};
int cmp_ptr_reverse(const void* ptr1, const void* ptr2){ return cmp_ptr(ptr2,ptr1); };
int cmp_ptr(const void* ptr1,const void* ptr2){
	void* p1=(void*)ptr1;
	void* p2=(void*)ptr2;
	if(p1==p2){ return 0; };
	if(is_i(p1) && is_i(p2)){
		return is_int(p1) < is_int(p2) ? -1 : 1; };
	if(is_str(p1) && is_str(p2)){
		if(!p1){ return -1; };
		if(!p2){ return 1; };
		return strcmp(p1,p2); };
	return 0;
};
int cmp_cons_reverse(const void* ptr1, const void* ptr2){ return cmp_cons(ptr2,ptr1); };
int cmp_cons(const void* ptr1, const void* ptr2){
	return cmp_ptr(((cons*)ptr1)->val,((cons*)ptr2)->val);
};
map* map_sort(map* mp,int reverse){
	if(!mp){ return mp; };
	if(reverse){
		if(is_vec(mp)){ qsort(mp,mp->len,sizeof(void*),cmp_ptr_reverse); return mp; };
		qsort(mp->pairs,mp->len,sizeof(cons),cmp_cons_reverse);	
	}else{
		if(is_vec(mp)){ qsort(mp,mp->len,sizeof(void*),cmp_ptr); return mp; };
		qsort(mp->pairs,mp->len,sizeof(cons),cmp_cons); };	
	map_reindex(mp);
	return mp;
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
map* tbls_sync_sqls(map* new_tbls,map* old_tbls){
	if(!new_tbls){ fox_error("sync_sql() final table list is blank!",0); };
	map* newtbls=new_map();
	map* oldtbls=new_map();
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
		vec_merge(sqls,sync_sqls(map_val(old_tbls,oldt),map_val(new_tbls,newt))); };
	for(int next1=next(newtbls,-1,NULL,NULL); has_id(newtbls,next1); next1++){ void* newt=map_id(newtbls,next1);
		vec_add(sqls,create_sql(map_val(new_tbls,newt),NULL)); };
	for(int next1=next(oldtbls,-1,NULL,NULL); has_id(oldtbls,next1); next1++){ void* oldt=map_id(oldtbls,next1);
		vec_add(sqls,drop_sql(oldt)); };
	return sqls;
};
map* schema_sync_db(char* file,char* db,int go){
	map* ret=tbls_sync_sqls(schema_tables(db),db_tables(db));
	if(go && ret->len){ sqls_exec(ret,db); };
	return ret;
};
map* file_tables(char* file){
	void* ret=map_val(file_map(file),"table");
	for(int next1=next(ret,-1,NULL,NULL); has_id(ret,next1); next1++){ void* tbl=map_id(ret,next1); char* name=map_key(ret, next1); add(ret,name,map_table(tbl,name,NULL)); };
	return ret;
};
map* file_sync_schema(char* file,char* db,int go){
	map* ret=tbls_sync_sqls(file_tables(file),schema_tables(db));
	if(go && ret->len){ sqls_exec(ret,db); };
	return ret;
};
map* file_sync_db(char* file,char* db,int go){
	map* ret=tbls_sync_sqls(file_tables(file),db_tables(db));
	if(go && ret->len){ sqls_exec(ret,db); };
	return ret;
};

map* sync_sqls(map* newt,map* oldt){
	if(str_eq(create_sql(oldt,NULL),create_sql(newt,NULL))){ return NULL; };	
	map* match=cols_match(map_val(oldt,"cols"),map_val(newt,"cols"));
	map* ret=new_vec();
	vec_add(ret,drop_sql("_syncing"));
	vec_add(ret,create_sql(newt,"_syncing"));
	vec_add(ret,xstr("insert into _syncing (",map_join(map_keys(match),", "),") select ",map_join(match,", ")," from ",map_val(oldt,"name"), End));
	vec_add(ret,drop_sql(map_val(oldt,"name")));
	vec_add(ret,rename_sql("_syncing",map_val(newt,"name")));
	return ret;	
};
int utf8_strlen(char* in){
	int i=0;
	int j=0;
	while(in[i]){
		if((in[i] & 0xc0) != 0x80){ j++; };
		i++; };
	return j;
};
int ucs2_to_utf8(int ucs2,char* utf8){
	if(ucs2 < 0x80){
		utf8[0] = ucs2;
		utf8[1] = '\0';
		return 1; };
	if(ucs2 >= 0x80  && ucs2 < 0x800){
		utf8[0] = (ucs2 >> 6)   | 0xC0;
		utf8[1] = (ucs2 & 0x3F) | 0x80;
		utf8[2] = '\0';
		return 2; };
	if(ucs2 >= 0x800 && ucs2 < 0xFFFF){
		if(ucs2 >= 0xD800 && ucs2 <= 0xDFFF){ return 0; };
		utf8[0] = ((ucs2 >> 12)	   ) | 0xE0;
		utf8[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
		utf8[2] = ((ucs2	  ) & 0x3F) | 0x80;
		utf8[3] = '\0';
		return 3; };
	if(ucs2 >= 0x10000 && ucs2 < 0x10FFFF){
		utf8[0] = 0xF0 | (ucs2 >> 18);
		utf8[1] = 0x80 | ((ucs2 >> 12) & 0x3F);
		utf8[2] = 0x80 | ((ucs2 >> 6) & 0x3F);
		utf8[3] = 0x80 | (ucs2 & 0x3F);
		utf8[4] = '\0';
		return 4; };
	return 0;
};
int utf8_to_ucs2(char** end_ptr){
	unsigned char* input=(unsigned char*)*end_ptr;
	if(!input[0]){ return 0; };
	if(input[0] < 0x80){
		*end_ptr+=1;
		return input[0]; };
	if((input[0] & 0xE0) == 0xE0){
		if(input[1] < 0x80 || input[1] > 0xBF || input[2] < 0x80 || input[2] > 0xBF){ return 0; };
		*end_ptr+=3;
		return (input[0] & 0x0F)<<12 | (input[1] & 0x3F)<<6  | (input[2] & 0x3F); };
	if((input[0] & 0xC0) == 0xC0){
		if(input[1] < 0x80 || input[1] > 0xBF){ return 0; };
		*end_ptr+=2;
		return (input[0] & 0x1F)<<6  | (input[1] & 0x3F); };
	return 0;
};
char* test_out(char* in){
	px(in,1);
	return in;
};
//int jd_year(int jd){
//	return (jd<=0 ? jd-1 : jd)/365.25
//}
//char* int_date(int jd){
//	y=jd.jd_year()
//	if jd<=0 => jd--
//	d=jd-floor($y*365.25)-1;
//	//if($y && !($y%4) && $d<59) $d++;
//	$days=array(0,31,59,90,120,151,181,212,243,273,304,334,365);
//	foreach($days as $m=>$v) if($d<$v) break;
//	return sprintf("%04d-%02d-%02d",2000+$y,$m,$d-$days[$m-1]+1);
//}
//function jd2($date){
//	list($y,$m,$d)=explode('-',$date);
//	$days=array(0,31,59,90,120,151,181,212,243,273,304,334);
//	$y-=2000;
//	$ret=floor($y*365.25)+$days[$m-1]+$d-1;
//	if($y && !($y%4) && $m<=2) $ret--;
//	return $ret+1;
//}
map* sql_sums(char* sql,char* db,map* cols,map* params){
	map* sum=new_map();
	for(int next1=next(cols,-1,NULL,NULL); has_id(cols,next1); next1++){ void* col=map_id(cols,next1); char* f=map_key(cols, next1);
		if(!to_int(maptype(""
			"text=0\n"
			"amount=1"
			"",map_val(col,"type")))){ continue; };
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
char* read_token(char** line,char* terminators){
	if(!*line || !**line){ return NULL; };
	char* in=*line;
	while(*in && strchr(terminators,*in)) {in++;};
	char* head=in;
	if(!*in){ *line=in; return NULL; };
	int paren=0;
	char term='\0';
	while(*in){
		if(term){
			if(*in=='\\' && in[1]){
				in++;
			}else if(term==*in){
				term='\0'; };
		}else if(strchr("\"'`",*in)){
			term=*in;
		}else if(strchr("([{",*in)){
			paren++;
		}else if(strchr("}])",*in)){
			paren--;
		}else if(!paren && strchr(terminators,*in)){
			break; };
		in++; };
	*line=in;
	return sub_str(head,0,in-head);
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
		}else {ret=xcat(ret,sub_str(line,indent,0),"\n", End);}; };
	*outline=NULL;
	return ret;
};
map* mml_map(map* lines,int* from){
	int level=str_level(map_id(lines,*from));
	map* ret=new_vec();
	for(int idx=next(lines,(from ? *from : 0)-1,NULL,NULL); has_id(lines,idx); idx++){ char* line=map_id(lines,idx);
		int nidx=max(ret->len-1,0);
		if(str_level(line)>level){
			add(add_id(ret,nidx),"text",mml_map(lines,&idx));
			continue; };
		if(str_level(line)<level){
			if(from){ *from=idx-1; };
			return ret; };
		line+=level;
		if(*line=='<'){
			vec_add(ret,line);
			continue; };
		if(str_start(line,"---")){
			add(add_id(ret,nidx),"text",read_textblock(lines,&idx, line, &line));
		};
		map* sub=new_map();
		char* tok=NULL;
		nidx=0;
		while((tok=read_token(&line," \t\n\r="))){
			if(*line=='='){
				line++;
				add(sub,tok,read_token(&line, " \t\n\r"));
				if(str_start(map_val(sub,tok),"---")){
					add(sub,tok,read_textblock(lines,&idx, map_val(sub,tok), &line)); };
			}else{
				if(*tok=='-'){
					tok=read_textblock(lines,&idx, tok, &line); };
				char* tags[4]={"tag", "text", "class", "style"};
				nidx=sub->len;
				if(nidx>=1 && str_level(map_id(lines,idx+1))>level){ nidx++; };
				if(nidx<4){ add(sub,tags[nidx],tok); }
				else {add(sub,tok,tok);}; }; };
		vec_add(ret,sub); };
	return ret;
};
char* val_eval(void* in,map* params,int is_code){
	char* str=in;
	if(!in){ return NULL; }
	else if(is_map(in)){ return map_html(in,params); }
	else if(str_eq(in,"-")){ return NULL; }
	else if(str_start(in,"---")){ return sub_str(in,3,-3); }
	else if(strchr("\"'`",*str)){ return is_code ? str_unquote(in) : in; }
	else if(str[0]=='('){ return eval(in,params); };
	return is_code ? eval(in,params) : in;
};
char* load_mml(char* name){
	map* mmls=NULL;
	mmls=cache(NULL,"templates",NULL);
	if(!mmls){ mmls=cache(NULL,"templates", str_map(call_php(NULL,"templates"),Map)); };
	return map_val(mmls,name);
};
char* map_html(map* in,map* params){
	char* ret=NULL;
	for(int next1=next(in,-1,NULL,NULL); has_id(in,next1); next1++){ void* v=map_id(in,next1); char* k=map_key(in, next1);
		if(is_str(v)){
			if(*(char*)v=='<'){ ret=xcat(ret,str_eval(v,params), End); }
			else {ret=xcat(ret,val_eval(v,params,0), End);};
			continue; };
		if(!map_val(v,"tag")){
			ret=xcat(ret,val_eval(map_val(v,"text"),params,0), End);
			continue; };
		if(((char*)(map_val(v,"tag")))[0]=='+'){
			map* nparams=new_map();
			for(int next1=next(v,-1,NULL,NULL); has_id(v,next1); next1++){ void* val=map_id(v,next1); char* name=map_key(v, next1);
				add(nparams,name,eval(val,params)); };
			ret=xcat(ret,mml_html(load_mml(sub_str(map_val(v,"tag"),1,0)),nparams), End);
			continue; };
		ret=xcat(ret,"<",map_val(v,"tag"), End);
		for(int next1=next(v,-1,NULL,NULL); has_id(v,next1); next1++){ void* val=map_id(v,next1); char* name=map_key(v, next1);
			if(is_word(name,"tag text")){ continue; };
			ret=xcat(ret,xstr(" ", name, "=", val_eval(val,params,0), End), End); };
		if(str_eq(map_val(v,"text"),"+")){
			ret=xcat(ret,">", End);
			continue; };
		ret=xcat(ret,xstr(">", val_eval(map_val(v,"text"),params,1), "</", map_val(v,"tag"), ">\n", End), End); };
	return ret;
};
char* mml_html(char* str,map* params){
	int no=0;
	return map_html(mml_map(str_split(str,"\n",0),&no),params);
};
map* show_map(char* show){
	return str_vec(show);
};
char* data_show(map* data,char* show){
	return NULL;
};
char* http_redirect(char* url){ return http_out(NULL,"301 Moved Permanently","text/html",xvec(xstr("Location: ", url, End), End)); };
void* http_error(char* msg,char* status){ http_out(mstr(map_val(_globals,"fox_error"),status,status,msg, End),status,"text/html; charset=utf-8",NULL); return NULL; };
map* compile_template(char* tem){
	if(!tem) {return NULL;};
	map* blks=str_split(tem,"\n--",0);
	map* ret=new_map();
	for(int i=next(blks,-1,NULL,NULL); has_id(blks,i); i++){ void* v=map_id(blks,i);
		if(i==0 && !str_start(v,"--")){
			add(ret,"head",str_vars(v));
			continue; };
		map* ret1=str_split(v,"\n",2);
		char* name1=str_trim(map_id(ret1,0),"-- ");
		if(!str_len(name1)){ name1="body"; };
		add(ret,name1,str_vars(map_id(ret1,1))); };
	return ret;
};
char* render_template(map* data, char* template_name){
	char* tmp=map_val(map_val(_globals,"html"),template_name);
	if(!tmp){ fox_error(xstr("Template :", template_name, " not found", End),0); };
	return render(data,tmp);
};
char* render(map* data,void* tem){
	map* mp1=tem;
	if(is_str(tem)){ mp1=compile_template(tem); };
	char* ret=NULL;
	for(int i=next(mp1,-1,NULL,NULL); has_id(mp1,i); i++){ void* v=map_id(mp1,i); char* k=map_key(mp1, i);
		if(str_eq(k,"body")){
			for(int i2=next(data,-1,NULL,NULL); has_id(data,i2); i2++){ void* v2=map_id(data,i2); char* k2=map_key(data, i2);
				ret=xcat(ret,render_data(v2,v,k2), End); };
		}else {ret=xcat(ret,render_data(data,v,NULL), End);}; };
	return ret;
};
char* render_data(void* data,map* tp,char* key){
	if(!is_map(data)) {data=xmap("_val", data,"_key", key, End);};
	map* flds=map_id(tp,1);
	map* strs=map_id(tp,0);
	char* ret=NULL;
	for(int i=next(strs,-1,NULL,NULL); has_id(strs,i); i++){ void* v=map_id(strs,i);
		ret=xcat(ret,v, End);
		if(i==strs->len-1) {break;};
		ret=xcat(ret,map_join(eval(map_id(flds,i),data),""), End); };
	return ret;
};
char* file_mime(char* path){
	map* map_1=map_val(_globals,"mime"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); char* k=map_key(map_1, i);
		if(str_end(path,k)) {return v;}; };
	return NULL;
};
char* str_html(char* in){
	if(!str_len(in)){ return ""; };
	char* chars="<>\"'&";
	map* into=xvec("&lt;","&gt;","&quot;","&#039;","&amp;", End);
	char* ret=NULL;
	for(;*in;in++){
		char* match=strchr(chars,*in);
		if(!match){ ret=cat_char(ret,*in); }
		else {ret=xcat(ret,map_id(into,match-chars), End);}; };
	return ret;
};
void header(char* str){ print(str,stdout); print("\r\n",stdout); };
char* http_out(char* str,char* status,char* mime,map* headers){
	if(map_val(_globals,"out")){ return http_out(flush_out(),"200 OK","text/plain",NULL); };
	if(str_eq(status,"200 OK") && str_eq(mime,"text/html; charset=utf-8")){
		str=xcat(str,render(NULL,""
			"\n"
			"<div class=\"small lighten pull-right\" style=\"margin-top:4em;\">\n"
			"Run Time: #{run_time()}ms=#{gc_time()}/#{run_time()-total_time()-gc_time()}/#{total_time()}</br>\n"
			"Malloc: #{total_kb()}KB. Total: #{max_mem()/1024}KB.</br>\n"
			"GC: #{gc_runs()}runs.\n"
			"</div>"
			""), End); };
	header(xstr("HTTP/1.1 ", status, End));
	header(xstr("Content-Type: ", mime, End));
	header(xstr("Content-Length: ",int_str( str_len(str)), End));
	for(int i=next(headers,-1,NULL,NULL); has_id(headers,i); i++){ void* v=map_id(headers,i); header(v); };
	map* map_1=map_val(map_val(_globals,"res"),"cookie"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* v2=map_id(map_1,i2);
		header(xstr("Set-Cookie: ", v2, End)); };
	if(map_val(map_val(_globals,"res"),"sess")){
		write_file(json(map_val(map_val(_globals,"res"),"sess"),0),xstr("/tmp/sess.", sess_id(), End),0); };
	header("");
	print(str,stdout);
	return str;
};
char* static_file(char* path){
	if(has_word(path,"? ../ /.. \\")){ return NULL; };
	if(!str_start(path,"/res/")){ return NULL; };
	char* ret=fox_read_file(cat("/web/",str_trim(path,"/")),0);
	if(ret){ return http_out(ret,"200 OK",file_mime(path),NULL); };
	return NULL;
};

char* str_url(char* in){
	if(!in){ return in; };
	int bad=0;
	char* head=in;
	for(;*in;in++){
		if(!is_alphanum(*in,NULL) && !strchr("-.[]*$%{}()@!~",*in)){ bad=1; break; }; };
	if(!bad){ return head; };
	char* ret=fox_alloc(str_len(head)*3+1,String);		
	int off=0;
	in=head;
	for(;*in;in++){
		if(!is_alphanum(*in,NULL) && !strchr("-.[]*$%{}()@!~",*in)){
			sprintf(ret+off,"%%%02X",*in);
			off+=3;
		}else{
			ret[off]=*in;
			off++; }; };
	return ret;
};
char* url_str(char* in){
	if(!in){ return NULL; };
	int len=0;
	int fix=0;
	char* str=in;
	for(;*in;in++){
		if(*in=='+'){ fix=1; }
		else if(*in=='%'){ fix=1; len-=2; }; };
	if(!fix){ return str; };
	char* ret=fox_alloc(str_len(str)+len+1,String);
	int i=0;
	for(in=str;*in;in++,i++){
		if(*in=='+'){ ret[i]=' '; }
		else if(*in=='%'){ ret[i]=hex_char((++in)); in++; }
		else {ret[i]=*in;}; };
	return ret;
};
map* amps_map(char* in){
	map* ret=new_map();
	map* map_1=str_split(in,"&",0); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		map* pr=str_split(v,"=",2);
		add(ret,url_str(map_id(pr,0)),url_str(map_id(pr,1))); };
	return ret;
};
map* parse_url(char* path){
	map* ret=xmap("full", path, End);
	map* two=str_split(path,"?",2);
	xadd(ret,"path",map_id(two,0),"param",map_id(two,1), End);
	return add(ret,"params",amps_map(map_id(two,1)));
};
void load_theme(char* name){
	if(!str_len(name)) {return;};
	map* mp=file_map((xstr(name,".map", End)));
	if(!mp){ fox_error(xstr("Can't read theme file ", name, ".map", End),0); };
	map_merge(_globals,mp);
};
void sess_load(){
	char* sid=map_val(header_vals("cookie"),"sessid");
	if(!sid) {return;};
	add(_globals,"sessid",sid);
	add(_globals,"sess",xjson_map(fox_read_file(xstr("/tmp/sess.", sid, End),0),Map));
};
void	sess_add(char* name, char* value){ add(add_key(add_key(_globals,"res",Map),"sess",Map),name,value); };
char*	sess_id(){ return map_val(_globals,"sessid"); };
char*	sess_file(){ return sess_id() ? xstr("/tmp/sess.", sess_id(), End) : NULL; };
char*	sess_newid(){ return rand_str(24); };
void cookie_set(char* name,char* value,char* path,char* expire){
	char* xexpire="";
	if(expire){ xexpire=xstr("; expires=", expire, End); };
	char* ss=xstr(name, "=", value, "; path=", path, xexpire, End);
	add(add_key(_globals,"res",Map),"cookie",ss);
};
void sess_delete(){
	if(!sess_file()){ return; };
	remove(sess_file());
	cookie_set("sessid","NULL","/","Thu, 01 Jan 1970 00:00:00 GMT");
};
map* link_relative(map* links,char* url){
	if(!links||!url) {return links;};
	char* prepad=str_times("../",str_char_count(url,'/')+1);
	for(int i=next(links,-1,NULL,NULL); has_id(links,i); i++){ void* v=map_id(links,i);
		if(is_map(v)){
			link_relative(v,url);
			continue; };
		if(fox_at(v,0)=='/') {continue;};
		set(links,i,str_add(prepad,v)); };
	return links;
};
char* links_ul(map* mp,char* class){
	if(!mp){ return NULL; };
	char* ret=NULL;
	if(is_vec(mp)){
		for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i);
			ret=xcat(ret,links_ul(v,class), End); };
		return ret; };
	ret="<ul>";
	if(class){ ret=xstr("<ul class=\"", class, "\">", End); };
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i); char* k=map_key(mp, i);
		if(is_map(v)){ ret=xcat(ret,"<li>",links_ul(v,class),"</li>", End); }
		else {ret=xcat(ret,"<li><a href=",v,">",k,"</a></li>", End);}; };
	ret=xcat(ret,"</ul>", End);
	return ret;
};
map* post_data(){
	map* mime=header_map(map_val(map_val(_globals,"req"),"content-type"));
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
				add(ret,name,substr(map_id(mp3,1),0,-4));
				break; }; }; };
	return ret;
};
char* button_clicked(map* process){
	if(!str_eq(map_val(map_val(_globals,"req"),"method"),"POST")) {return NULL;};
	map* data=post_data();
	for(int i=next(process,-1,NULL,NULL); has_id(process,i); i++){ void* v =map_id(process,i); char* k=map_key(process, i); if(map_val(data,k)){ return k; }; };
	return NULL;
};
map* process_post(map* process){
	char* clicked=button_clicked(process);
	if(!clicked){ return NULL; };
	void* v=map_val(process,clicked);
	if(is_str(v)){
		if(str_has(v,"/")){ http_redirect(v); }
		else {v=xmap(NULL,v, End);}; };
	map* data=post_data();
	return data_exec(add(v,"data",data),data);
};
char* process_logout(char* redirect){
	sess_delete();
	return http_redirect(redirect);
};
map* process_login(char* username,char* password){
	char* sid=sess_newid();
	add(_globals,"sessid",sid);
	cookie_set("sessid",sid,"/",NULL);
	sess_add("user",str_tolower(username));
	sess_add("role","admin");
	http_redirect("../dump/");
	return NULL;
};
char* page_set_cookie(){
	add(add_key(add_key(_globals,"res",Map),"cookie",Map),"sessid","helloworldsession");
	return http_out("../dump/","200 OK","text/html; charset=utf-8",NULL);
};
char* page_dump(){
	return http_out(xcat(
			map_val(map_val(_globals,"req"),"header"),
			to_str(map_val(_globals,"req"),"",0),
			map_val(_globals,"sess")
		, End),"200 OK","text/pain",NULL);
};
map* url_data(char* url,char* match){
	map* pathsig=str_split(str_trim(match,"/"),"/",0);
	map* urls=str_split(str_trim(url,"/"),"/",0);
	if(!urls){ return NULL; };
	map* ret=new_map();
	for(int i=next(pathsig,-1,NULL,NULL); has_id(pathsig,i); i++){ void* v=map_id(pathsig,i);
		if(str_eq(v,"**")){
			if(urls->len>i){
				add(ret,"rest",map_join(vec_sub(urls,i,0),"/")); };
			return ret; };
		if(urls->len<=i){ return NULL; };
		if(str_eq(v,"*")){
			vec_add(ret,map_id(urls,i));
			continue; };
		if(fox_at(v,-1)=='*'){
			if(str_start(map_id(urls,i),sub_str(v,0,-1))){
				add(ret,sub_str(v,0,-2),sub_str(map_id(urls,i),str_len(v)-1,0));
				continue; }; };
		if(str_eq(map_id(urls,i),v)){ continue; };
		return NULL; };
	return urls->len==pathsig->len ? ret : NULL;
};
map* path_params(char* url,char* path){
	map* paths=str_split(str_trim(path,"/"),"/",0);
	map* urlp=str_split(str_trim(url,"/"),"/",0);
	int urli=0;
	map* ids=new_vec();
	map* ret=xmap("ids",ids, End);
	for(int i=next(paths,-1,NULL,NULL); has_id(paths,i); i++){ void* v=map_id(paths,i);
		urli++;
		if(str_eq(v,"**")){
			if(urlp->len>=urli){
				add(ret,"rest",map_join(vec_sub(urlp,urli-1,0),"/")); };
			return ret; };
		if(urlp->len<urli) {return NULL;};
		if(str_eq(v,"*")){
			vec_add(ids,map_id(urlp,urli-1));
			continue; };
		if(fox_at(v,-1)=='*'){
			if(str_start(map_id(urlp,urli-1),sub_str(v,0,-1))){
				vec_add(ids,sub_str(map_id(urlp,urli-1),str_len(v)-1,0));
				continue; }; };
		if(str_eq(map_id(urlp,urli-1),v)) {continue;};
		return NULL; };
	return urlp->len==urli ? ret : NULL;
};
int path_matches(char* match,void* url){
	if(is_str(url)){ url=str_split(str_trim(url,"/"),"/",0); };
	map* paths=str_split(str_trim(match,"/"),"/",0);
	for(int i=next(paths,-1,NULL,NULL); has_id(paths,i); i++){ void* v=map_id(paths,i);
		if(str_eq(map_id(url,i),v)){ continue; };
		if(str_eq(v,"**")){ return 1; };
		if(map_len(url)<=i){ return 0; };
		if(str_eq(v,"*")){ continue; };
		if(fox_at(v,-1)=='*'){ if(str_start(map_id(url,i),sub_str(v,0,-1))){ continue; }; };
		return 0; };
	return map_len(url)==map_len(paths);
};
char* pages_match(map* pages,char* path){
	if(!str_len(path)){ return map_val(pages,"index") ? "index" : NULL; };
	map* paths=str_split(str_trim(path,"/"),"/",0);
	for(int i=next(pages,-1,NULL,NULL); has_id(pages,i); i++){ char* k=map_key(pages, i); if(path_matches(k,paths)){ return k; }; };
	return NULL;
};
void* pages_exec(void* pages,char* path,map* env){
	if(is_str(pages)){ pages=str_map(pages,Map); };
	char* page=pages_match(pages,path);
	if(!page){ return 0; };
	void* data=map_val(pages,page);
	if(env){ data=map_eval(data,env); };
	if(is_str(data)){ http_out(data,"200 OK","text/html; charset=utf-8",NULL); return "ok"; };
	return data_exec(data,map_merge(url_data(path,page),env));
};
char* not_found(char* path){ return http_out(xstr("The requested content ", path, " was not found on the server.", End),"404 Not Found","text/html; charset=utf-8",NULL); };
char* dispatch(map* pages,char* path){
	if(static_file(path)){ return NULL; };
	if(fox_at(path,-1)!='/' && !str_has(path,"/?")){ return NULL; };
	add(_globals,"url",parse_url(path));
	path=str_trim(map_val(map_val(_globals,"url"),"path"),"/");
	sess_load();
	load_theme("dashboard");
	if(pages_exec(pages,path,NULL)){ return NULL; };
	return not_found(path);
};
char* inet(map* pages,char* path){
	if(!path){
		_is_web=1;
		path=map_val(read_http_header(),"path"); };
	return dispatch(pages,path);
};
char* page(void* body,char* title,int width,void* link,char* theme,char* pg,map* process,map* env){
	process_post(process);
	link=str_map(link,Map);
	body=data_exec(body,env);
	return http_out(render(xmap(
		"title",str_eval(title,env),
		"body",body,
		"width",int_var(width),
		"link",links_ul(link,"nav nav nav-sidebar")
	, End),map_val(map_val(_globals,"html"),pg)),"200 OK","text/html; charset=utf-8",NULL);
};
char* map_template(map* mp,char* template){ return render(mp,xstr("--body\n",template, End)); };
map* str_vars(char* str){
	char* cur=str;
	map* ret=xvec(new_vec(),new_vec(), End);
	char* from=cur;
	while(*cur){
		if(*cur=='#' && cur[1]=='#') {cur++;}
		else if(*cur=='#' && cur[1]=='{'){
			vec_add(add_id(ret,0),substr(from,0,cur-from));
			from=cur;
			while(*cur && *cur!='}'){
				if(*cur=='\\') {cur++;}
				else if(strchr("\"'`",*cur)) {cur=skip_quote(cur);};
				if(!*cur||*cur=='}') {break;};
				cur++; };
			vec_add(add_id(ret,1),substr(from,2,cur-from-2));
			from=cur+1; };
		cur++; };
	vec_add(add_id(ret,0),substr(from,0,cur-from));
	return ret;
};
char* str_subst(char* in,map* data){ return render(data,xmap("head", str_vars(in), End)); };
void* map_eval(void* mp,map* env){
	if(!env){ return mp; };
	assert(is_map(env));
	if(is_str(mp)){ return render(env,mp); };
	if(!is_map(mp)){ return mp; };
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i); char* k=map_key(mp, i);
		if(is_map(v)){ map_eval(v,env); }
		else if(is_str(v)){ set(mp,i,render(env,v)); }; };
	return mp;
};
char* str_eval(char* in,map* env){ return render(env,in); };
map* header_val(char* line,map* mp){
	map* pair=str_split(line,":",2);
	add(mp,str_tolower(str_trim(map_id(pair,0)," \t\n\r")),str_trim(map_id(pair,1)," \t\n\r"));
	return mp;
};
map* header_vals(char* name){ return header_map(map_val(map_val(_globals,"req"),name)); };
map* header_map(char* val){
	if(!val){ return NULL; };
	map* ret=new_map();
	map* map_1=str_split(val,";",0); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i);
		if(!strchr(v,'=')){ vec_add(ret,v); continue; };
		map* pair=str_split(v,"=",2);
		add(ret,str_trim(map_id(pair,0)," \t\n\r"),str_unquote(str_trim(map_id(pair,1)," \t\n\r"))); };
	return ret;
};
map* read_http_header(){
	map* ret=new_map();
	char* line=NULL;
	char* header=NULL;
	while((line=read_line(stdin))){
		header=xcat(header,line, End);
		if(strchr("\r\n",line[0])){ break; };
		if(!ret->len){
			map* pair=str_split(str_trim(line," \t\n\r")," ",3);
			add(ret,"method",map_id(pair,0));
			add(ret,"path",map_id(pair,1));
			continue; };
		header_val(line,ret); };
	if(str_eq(map_val(ret,"method"),"POST")){
		int size=to_int(map_val(ret,"content-length"));
		if(!size){ http_error("Content-Lenght was not provided","411 Length Required"); };
		if(size>2000000){ http_error("Request should be with less than 2MB data","413 Request Entity Too Large"); };
		add(ret,"post",read_stdin(size,NULL)); };
	add(ret,"header",header);
	add(_globals,"req",ret);
	return ret;
};
#ifndef __MINGW32__
int ip_connect(char* host,int port,char** err){
	int ret=socket(AF_INET,SOCK_STREAM, 0);
	if(!ret){ return err_msg("Socket creation failed",err); };
	struct hostent *server=gethostbyname(host);
	if(!server){ return err_msg("fox_error: no such host",err); };
	struct sockaddr_in serv_addr={0};
	serv_addr.sin_family = AF_INET;
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr,server->h_length);
	serv_addr.sin_port = htons(port);
	if(connect(ret,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ return err_msg("fox_error connecting",err); };
	return ret;
};
int ip_close(int sock){ close(sock);return 0; };
int ip_send(char* msg, int sock){ return write(sock,msg,mem_size(msg)); };
char* ip_receive(int sock,char* terminator,int size){
	char* ret=NULL;
	return ret;
};
char* remote_ip(int con){
	struct sockaddr_in addr={0};
	socklen_t len=sizeof(struct sockaddr_in);
	getpeername(con,(struct sockaddr*)&addr,&len);
	return inet_ntoa(addr.sin_addr);
};
#endif
