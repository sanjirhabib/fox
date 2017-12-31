#include <fox.h>
char* db_write_meta(char* db){
	return write_file(db_meta_str(db),xstr(file_rename(map_val(conn_db(db),"file"),NULL,".db",NULL,NULL,NULL),".meta", End),0,1);
};
char* db_meta_str(char* db){
	map* ret=new_map();
	map* map_1=db_meta(db); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char*  table=map_key(map_1, next1);
		add(ret,table,new_map());
		for(int next1=next(val,-1,NULL,NULL); has_id(val,next1); next1++){ void* val2=map_id(val,next1); char*  attr=map_key(val, next1);
			if(is_word(attr,"name db item")){ continue; };
			add(add_key(ret,table,Map),attr,new_map());
			if(str_eq(attr,"cols")){
				for(int  idx=next(val2,-1,NULL,NULL); has_id(val2, idx);  idx++){ void* prop=map_id(val2, idx); char*  colname=map_key(val2,  idx);
					for(int next1=next(prop,-1,NULL,NULL); has_id(prop,next1); next1++){ void* v=map_id(prop,next1); char*  n=map_key(prop, next1);
						if(str_eq(n,"size") && type_size(map_val(prop,"type"))==to_int(v)){ continue; };
						if(!idx && str_eq(n,"pkey") && !map_val(map_id(val2,1),"pkey")){ continue; };
						if(is_word(n,"db table")){ continue; };
						add(add_key(add_key(add_key(ret,table,Map),"cols",Map),colname,Map),n,(is_int(v)==1 ? NULL : v)); }; };
			}else{
				add(add_key(ret,table,Map),attr,val2); }; }; };
	return data_str(xmap("db", ret, End),0);						
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
