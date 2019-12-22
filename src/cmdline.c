#line 2 "src/cmdline.fox"

#include <core.h>
#include <cmdline.h>
#include <generator.h>

map* eval_params(map* sent,char* name,map* env,map* fns){
	assert(name);
	map* ret=new_map();
	void* fn=map_val(fns,name);
	if(!fn){ fox_error(xstr("Function ", name, "() not found", End),0); };
	int named=0;
	map* map_1=map_val(fn,"params"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); char* k=map_key(map_1, i);
		if(str_eq(k,"...")){
			for(int i2=i; i2<sent->len; i2++){
				vec_add(ret,map_id(sent,i2)); };
			break;
		}else if(map_has_key(sent,k)){
			named=1;
			add(ret,k,map_val(sent,k));
		}else if(!named && map_len(sent)>i && is_i(map_key(sent,i))){
			add(ret,k,map_id(sent,i));
		}else if(map_has_key(env,k)){
			named=1;
			add(ret,k,map_val(env,k));
		}else if(str_eq(k,"env")){
			add(ret,k,env);
		}
			//ret[k]=v.default.eval()
			//invoke() will use the default
		else if(!map_val(v,"default")){
			fox_error(xstr("Parameter missing in ", map_val(fn,"name"), "(", k, "=?) in ", func_cdecl(fn,1), "\nargs=", json(sent,0), End),0);
			assert(0); }; };
	return ret;
};
map* cmdline_params(map* args,map* funcs){
	void* func=map_id(args,1);
	args=vec_sub(args,2,0);
	map* params=map_val(map_val(funcs,func),"params");
	if(!params){ fox_error(xstr("Function: ", func, "() not found", End),0); };
	map* ret=new_map();
	int curr=1;
	int variadic=str_eq(map_key(params,map_len(params)-1),"...") ? map_len(params) : 0;
	char* named_param=NULL;
	for(int i=next(args,-1,NULL,NULL); has_id(args,i); i++){ void* v=map_id(args,i); char* k=map_key(args, i);
		if(str_start(v,"-") && str_len(v)>1){
			char* val=NULL;
			if(named_param){
				add(ret,named_param,int_var(1)); };
			if(str_start(v,"--")){
				named_param=sub_str(v,2,-2147483648);
				if(str_has(v,"=")){
					map* mp1=str_split(named_param,"=",2);
					named_param=map_id(mp1,0);
					val=map_id(mp1,1);
				}else {named_param=sub_str(v,2,-2147483648);};
			}else{
				named_param=sub_str(v,1,-2147483648);
				if(!map_val(params,named_param)){
					for(int i2=next(params,-1,NULL,NULL); has_id(params,i2); i2++){ void* v2=map_id(params,i2); char* k2=map_key(params, i2);
						if(map_has_key(ret,k2)){ continue; };
						if(str_start(k2,named_param)){ named_param=k2; break; }; }; }; };
//				if v.str_len()>2
//					val=v.sub_str(2)
			if(!map_val(params,named_param)){ fox_error(xstr("Invalid parameter ", named_param, " in call to...\n", func_cdecl(map_val(funcs,func),1), End),0); };
			if(val){
				add(ret,named_param,val);
				named_param=NULL; };
		}else if(named_param){
			add(ret,named_param,v);
			named_param=NULL;
		}else if(variadic && curr>=variadic){
			vec_add(ret,v);
			curr++;
		}else{
			if(curr>map_len(params)){ fox_error(xstr("Excess number of arguments. Function has only ",int_str( map_len(params)), "\narguments ", func_cdecl(map_val(funcs,func),1), End),0); };
			add(ret,map_key(params,curr-1),v);
			curr++; }; };
	if(named_param){
		add(ret,named_param,int_var(1)); };
	ret=eval_params(ret,func,NULL,funcs);
	for(int i3=next(ret,-1,NULL,NULL); has_id(ret,i3); i3++){ void* v3=map_id(ret,i3); char* k3=map_key(ret, i3);
		if(str_eq(map_val(map_val(params,k3),"type"),"map*")){
			add(ret,k3,str_map(v3,Map)); }; };
	return ret;
};
