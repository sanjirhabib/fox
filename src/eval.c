#include <fox.h>

char* callfunc_c(map* funcs){
	char* ret="function calls\n";
	map* fdups=new_map();
	for(int i2=next(funcs,-1,NULL,NULL); has_id(funcs,i2); i2++){ void* v=map_id(funcs,i2); char* k=map_key(funcs, i2);
		if(map_val(fdups,map_val(v,"name"))){ continue; };
		add(fdups,map_val(v,"name"),int_var(1));
		if(str_eq(k,"args_map")){ continue; };
		if(is_word(map_val(v,"name"),"main run")){ continue; };
		char* str_params=func_ccall(v);
		if(!str_params){
			continue; };
		//pp1
		ret=xcat(ret,mstr("\t\tcase %p: { %s break; }\n",str_hash(map_val(v,"name")),str_params, End), End); };
	return ret;
};
char* write_c(char* infile,char* outfile){
	source_funcs(NULL);
	return write_file(x_c(file_read(infile,1,1)),outfile,1,1);
};
char* func_ccall(map* fn){
	char* ret=NULL;
	char* preproc=NULL;
	int isvariadic=0;
	map* map_1=map_val(fn,"params"); for(int i=next(map_1,-1,NULL,NULL); has_id(map_1,i); i++){ void* v=map_id(map_1,i); char* k=map_key(map_1, i);
		char* def=map_val(v,"default");
		v=map_val(v,"type");
		v=drop_left(v,"const ");
		v=drop_left(v,"unsigned ");
		char* pre=NULL;
		char* post=NULL;
		char* mid=xstr("v.map_id(",int_str( i), ")", End);
		if(str_eq(k,"...")){
			char* mtype=NULL;
			if(str_eq(map_val(fn,"type"),"void") || str_end(map_val(fn,"type"),"*")){ mtype="ptr"; }
			else if(str_eq(map_val(fn,"type"),"int")){ mtype="int"; }
			else if(str_eq(map_val(fn,"type"),"double")){ mtype="double"; }
			else {return NULL;};
			isvariadic=1;
			ret=xstr("call_variadic_", mtype, "(v,", map_val(fn,"name"), ",\"", map_val(fn,"name"), "\")", End);
			break;
		}else if(str_eq(v,"char*")){ post=".is_str()"; }
		else if(str_eq(v,"char**")){ preproc=xcat(preproc,xstr("char* p", k, "_", map_val(fn,"name"), "=v.map_id(",int_str( i), ").is_str(); ", End), End); mid=xstr("&p", k, "_", map_val(fn,"name"), End); }
		else if(str_eq(v,"map*")){ post=".is_map()"; }
		else if(is_word(v,"int long size_t time_t") || str_eq(v,"long long")){ post=".to_int()"; }
		else if(str_eq(v,"char")){
			post=xstr(".is_str() ? ", mid, ".is_str()[0] : ", mid, ".to_int())", End);
			pre="(";
		}else if(is_word(v,"double float")){ post=".to_double()"; }
		else if(!str_end(v,"*")){
			return NULL; };
		if(def){ ret=xcat(ret,xstr("v->len>=",int_str( i), " ? ", pre, mid, post, " : ", sub_str(x_c(def),0,-1), ",", End), End); }
		else{ ret=xcat(ret,xstr(pre, mid, post, ",", End), End); }; };
	char* postproc=NULL;
	if(is_word(map_val(fn,"type"),"int long long size_t time_t char")){ postproc=".int_var()"; preproc=xcat(preproc,"return ", End); }
	else if(is_word(map_val(fn,"type"),"double float")){ postproc=".double_var()"; preproc=xcat(preproc,"return ", End); }
	else if(str_eq(map_val(fn,"type"),"void")){
		postproc=xcat(postproc,"; return NULL", End);
	}else if(!is_word(map_val(fn,"type"),"void* map* char*")) {return NULL;}
	else {preproc=xcat(preproc,"return ", End);};
	if(isvariadic){ return x_c(xstr(preproc, ret, postproc, End)); };
	return x_c(xstr(preproc, map_val(fn,"name"), "(", null_str(sub_str(ret,0,-1)), ")", postproc, End));
	//pp1
};
char* fox_c(char* infile, char* outfile){ return write_file(x_c(file_read(infile,1,1)),outfile,0,1); };
char* x_c(char* in){ return toks_c(x_toks(in,0)); };
map* type_convert(map* tok,char* outtype,map* env,map* fs,map* fn){
	if(!outtype){ return tok; };
	char* intype=expr_type(tok,0,0,env,fs);	
	if(!intype){ return tok; };
	if(str_eq(intype,outtype)){ return tok; };
	if(is_word(intype,"int double long long") && str_start(outtype,intype) && outtype[str_len(intype)]=='*' && !outtype[str_len(intype)+1]){
		return vec_splice(tok,1,0,xvec("&",NULL, End)); };
	if(is_word(intype,"int long long size_t")){
		if(str_eq(outtype,"void*")){ return wrap_call(tok,"int_var"); }
		else if(str_eq(outtype,"char*")){ return wrap_call(tok,"int_str"); };
	}else if(str_eq(intype,"void*")){
		if(str_eq(outtype,"double")){ return wrap_call(tok,"to_double"); };
	}else if(str_eq(intype,"double")){
		if(str_eq(outtype,"void*")){ return wrap_call(tok,"double_var"); }
		else if(str_eq(outtype,"char*")){ return wrap_call(tok,"double_str"); };
	}else if(str_eq(intype,"map*")){
		if(str_eq(outtype,"char*")){ return wrap_call(tok,"to_str"); };
	}else if(str_eq(intype,"char*")){
		if(str_eq(outtype,"int")){ return wrap_call(tok,"stoi"); }
		else if(str_eq(outtype,"char**")){
			return vec_splice(tok,1,0,xvec("&",NULL, End)); };
	}else if(str_eq(intype,"char**")){
		if(str_eq(outtype,"char*")){ return vec_splice(tok,1,0,xvec("*",NULL, End)); }; };
	return tok;
};
map* param_c(map* params,map* env,map* fs,map* fn){
	assert(fn);
	if(!params){ return NULL; };
	map* xparam=toks_split(params,",",0);
	map* cparam=new_vec();
	void* sig=map_val(fn,"params");
	for(int i=next(sig,-1,NULL,NULL); has_id(sig,i); i++){ void* p=map_id(sig,i); char* name=map_key(sig, i);
		if(str_eq(name,"...")){
			for(int idx=i; idx<xparam->len; idx++){
				if(str_eq(map_id(map_id(xparam,idx),1),"End")){ break; };
				vec_add(cparam,type_convert(map_id(xparam,idx),map_val(p,"type"),env,fs,fn)); };
			if(neq(map_id(map_id(cparam,map_len(cparam)-1),1),"End")){
				if(!map_id(map_id(cparam,map_len(cparam)-1),1)){ set(add_id(cparam,map_len(cparam)-1),1,"End"); }
				else{
					char* endmark=map_len(cparam) ? " End" : "End";
					vec_add(cparam,x_map(endmark)); }; };
			break;
		}else if(map_len(map_id(xparam,i))<2){
			if(!map_val(p,"default")){ break; };
			vec_add(cparam,vec_compact(vec_del(map_id(x_toks(map_val(p,"default"),0),1),-2,0))); //x_map().colon_str().dot_key().str_dollars()
		}else{ 
//			xparam[i].dx(:IN)
//			xparam[i].expr_type(0,0,env,fs).dx(:INTYPE)
//			p.type.dx(:OUT)
//			xparam[i].type_convert(p.type,env,fs,fn).toks_c().dx(:CONVERTED)
//			"^^^^^^^^^^^^".px()
			vec_add(cparam,type_convert(map_id(xparam,i),map_val(p,"type"),env,fs,fn)); }; };
	map* ret=toks_join(cparam,",");
	return ret;
};
char* head_type(map* toks, int idx, int upto, map* env,map* fs){
	int head=expr_head(toks,idx,".");
	return expr_type(toks,head, upto, env, fs);
};
char* help(){ return funcs_cdecl(funcs(),1); };
char* expr_type(map* toks,int idx,int upto,map* env,map* fs){
	void* v=map_id(toks,idx+1);
	if(is_map(v)){ return expr_type(v,0,upto,env,fs); };
	char* w=is_str(v);
	if(!w){ return NULL; };
	assert(is_str(w) && str_len(w));
	if(next_tok(toks,idx,"?",upto)){
		return expr_type(toks,next_tok(toks,idx,"?",0)+1,upto,env,fs);
	}
	else if(str_eq(map_id(toks,idx+3),"(")){
		char* name=is_str(map_id(toks,idx+1));
		if(map_val(fs,name)){ return map_val(map_val(fs,name),"type"); };
		return map_val(map_val(funcs(),name),"type");
	}else if(is_word(w,"++ --")){ return expr_type(toks,idx+2,upto,env,fs); }
	else if(*w=='"'){ return "char*"; }
	else if(*w=='\'' && w[1]=='\''){ return "char*"; }
	else if(*w=='\''){ return "char"; }
	else if(*w>='0' && *w<='9' || *w=='-' || *w=='+'){ return strchr(w,'.') ? "double" : "int"; }
	else if(is_typecast(toks,idx)){ return is_typecast(toks,idx); }
	else if(str_eq(w,"(")){ return expr_type(map_id(toks,idx+3),0,upto,env,fs); }
	else if(str_eq(w,"*")){ return sub_str(expr_type(toks,idx+2,upto,env,fs),0,-1); }
	else if(str_eq(w,"&")){ return xstr(str_dup(expr_type(toks,idx+2,upto,env,fs)),"*", End); }
	else if(str_eq(w,"NULL")){ return "void*"; }
	else if(map_val(env,w)){
		if(next_tok(toks,idx,"-",upto) && is_word(expr_type(toks,next_tok(toks,idx,"-",upto)+1,upto,env,fs),"char* void* map*")){
			return "int"; };
		char* ret=map_val(env,w);	
		if(!upto){ upto=toks->len; };
		map* structtype=NULL;
		for(int i=idx+3; i<upto; i+=2){
			v=map_id(toks,i);
			if(is_word(v,". ->")){
				structtype=map_val(structs(),str_trim(ret,"*"));
			}else if(structtype && is_code(v) && !syn_is_call(toks,i) && map_val(structtype,v)){
				ret=map_val(structtype,v);
			}else if(str_eq(v,"[") && fox_at(ret,-1)=='*'){
				ret=sub_str(ret,0,-1);
				i+=4;
			}else if(str_eq(v,"-")){
				if(str_eq(ret,"int")){ return ret; };
				char* rest=expr_type(toks,i+1,upto,env,fs);
				if(fox_at(rest,-1)=='*'){ return "int"; };
				return ret;
			}else {return ret;}; };
		return ret; };
	return NULL;
};
map* x_toks(char* in,int is_script){
	if(!in){ return NULL; };
	return single_quotes(
		auto_types(
		inline_vectors(
		string_operators(
		heredoc_str(
		toks_syn(
		dot_each(
		dot_func(
		expand_main(
		add_semicolon(
		force_curly(
		add_curly(
		str_dollars(
		dot_key(
		colon_str(
		x_map(in)))),1)),1)))),1)))),"c", is_script,new_map(),new_map(),NULL,0));
};
map* auto_types(map* toks,char* context,int is_script,map* env,map* fns,map* func,int idx){
	int temp=0;
	if(!toks){ return toks; };
	if(str_eq(context,"c")){
		env=xmap("_globals" , "map*", End);
		for(int i=0; i<toks->len; i+=2){
			void* tok=map_id(toks,i+1);
			if(is_func_decl(tok)){
				map* fn=syn_func(tok,1,NULL);
				add(fns,map_val(fn,"name"),fn);
				syn_remove_default(tok); }; };
		for(int i=next(fns,-1,NULL,NULL); has_id(fns,i); i++){ void* fn=map_id(fns,i); char* name=map_key(fns, i);
			map* fenv=map_dup(env);	
			map* map_1=map_val(fn,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* op=map_id(map_1,i2); char* param=map_key(map_1, i2);
				add(fenv,param,map_val(op,"type")); };
			auto_types(add_return(map_val(fn,"body")),"body",is_script,fenv,fns,fn,0);
			if(is_script){
				if(map_val(fn,"body")){ add(add_key(add_key(_globals,"cache",Map),"userfuncs",Map),map_val(fn,"name"),map_val(fn,"body")); };
				add(add_key(add_key(add_key(_globals,"cache",Map),"reflect",Map),"funcs",Map),map_val(fn,"name"),map_del_key(fn,"body")); }; };
		return auto_types(toks,"body",is_script,env,fns,NULL,0);
	}else if(str_eq(context,"body")){
		for(int j=0; j<toks->len; j+=2){
			void* tok=map_id(toks,j+1);
			if(is_func_decl(tok)){
				continue;
			}else if(syn_is_macro(tok,0)){
				map* expanded=toks_syn(syn_expand_macro(vec_dup(tok),0),1);
				map* last=map_id(expanded,map_len(expanded)-1);
				auto_types(vec_compact(vec_del(expanded,-2,1)),"body",is_script,env,fns,func,0);
				auto_types(vec_merge(last,vec_sub(tok,8,0)),"syn",is_script,env,fns,func,0);
				if(is_script){
					vec_splice(toks,j+1,1,expanded); };
			}else {auto_types(tok,"syn",is_script,env,fns,func,0);}; };
		return toks;
	}else if(str_eq(context,"syn")){
		if(is_var_decl(toks)){
			char* type=syn_var_type(toks);
			char* name=syn_var_name(toks);
			add(env,name,type);
			auto_types(toks,"expr",is_script,env,fns,func,syn_assign_val(toks));
		}else if(is_flow(toks)){
			char* type=syn_flow_name(toks);
			if(str_eq(type,"for")){
				env=map_dup(env);
				auto_types(map_id(syn_flow_condition(toks,0),1),"syn",is_script,env,fns,NULL,0); };
			int i=0;
			map* body=NULL;
			while((body=syn_flow_condition(toks,i++))){
				auto_types(body,"expr",is_script,map_dup(env),fns,func,0); };
			i=0;
			while((body=syn_flow_body(toks,i++))){
				auto_types(body,"body",is_script,map_dup(env),fns,func,0); };
		}else if(is_assign(toks)){
			auto_types(toks,"expr",is_script,env,fns,func,0);
			if(is_word(syn_is_call(toks,str_eq(map_id(toks,1),"return") ? 2 : 0),"map_val map_id")){
				auto_types(toks,"expr",is_script,env,fns,func,0);
				int from=syn_assign_val(toks);
				int upto=next_tok(toks,syn_assign_val(toks),"; =",0);
				int len=0;
				if(upto){ len=upto-from-1; };
				map* val=vec_slice(toks,from,len);
				vec_compact(vec_del(vec_compact(toks),next_tok(toks,0,"=",0)-1,2));
				auto_types(map_assign(toks,val),"expr",is_script,env,fns,func,0);
				return toks; };
			char* name=syn_var_name(toks);
			char* type=syn_var_type(toks);
			if(*name!='_' && !map_val(env,name) && !str_len(type)){
				type=expr_type(toks,syn_assign_val(toks),0,env,fns);
				if(type){
					vec_splice(toks,1,0,vec_compact(vec_del(x_map(xstr(type, " ", End)),0,1)));
					add(env,name,type);
				}else{
					px(xstr(map_val(func,"name"), "(): unknown var ", name, End),1); }; };
			auto_types(toks,"expr",is_script,env,fns,func,syn_assign_val(toks));
		}else{
			auto_types(toks,"expr",is_script,env,fns,func,0); };
		return toks;
	}else if(str_eq(context,"expr")){
		int head=idx;
		for(int i=idx; i<toks->len; i+=2){
			if(is_map(map_id(toks,i+1))){
				auto_types(map_id(toks,i+1),"expr",is_script,env,fns,func,0);
			}else if(syn_is_call(toks,i)){
				auto_types(map_id(toks,i+5),"expr",is_script,env,fns,func,0);
				char* name=syn_is_call(toks,i);
				if(str_eq(name,"args_map")){
					char* subs="xmap(";
					map* map_1=map_val(func,"params"); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ char* k2=map_key(map_1, i2);
						subs=xcat(subs,":",k2,",",k2,",", End); };
					map* mp4=vec_compact(vec_del(colon_str(x_map(xstr(subs, " End)", End))),0,1));
					vec_splice(toks,i+1,7,mp4);
				}else{
					map* params=syn_func_param(toks,i);
					map* fn=map_val(fns,name);
					if(!fn){ fn=map_val(funcs(),name); };
					if(fn){
						params=param_c(params,env,fns,fn); };
					syn_set_param(toks,i,params); };
			}else if(str_eq(map_id(toks,i+1),"[") && is_word(head_type(toks,i-2,i,env,fns),"map* void*") && neq(is_typecast(toks,head),"void*") && 1){
				void* name=map_id(toks,i+3);
				if(!map_len(name)){ name=xvec(NULL,"NULL", End); };
				head=expr_head(toks,i-2,".");
				map* mid=vec_merge(xadd(vec_splice(vec_sub(toks,head+1,i-head-1),0,0,xvec(NULL, End)),NULL,",", End),name);
				char* callfunc=str_eq(expr_type(name,0,0,env,fns),"int") ? "map_id" : "map_val";
				vec_splice(toks,head+1,i+5-head,xvec(callfunc,NULL,"(",NULL,mid,NULL,")", End));
				i=head;
			}else if(str_eq(map_id(toks,i+1),".") && is_name(map_id(toks,i+3)) && expr_head(toks,i-2,".")<i){
				head=expr_head(toks,i-2,".");
				if(head==i){ fox_error(xstr("Can't find header for ", json(toks,0), " @ ",int_str( i), End),0); };
				if(is_word(syn_is_call(toks,i+2),"each")){
					map* mid=vec_splice(vec_sub(toks,head+1,i-head-1),0,0,xvec(NULL, End));
					map* params=toks_split(syn_func_param(toks,i+2),",",0);
					if(syn_is_call(mid,0)){
						temp++;
						char* varname=xstr("map_",int_str(temp), End);
						syn_set_param(toks,i+2,toks_join(vec_splice(params,0,0,xvec(xvec(NULL,varname, End), End)),","));
						vec_splice(toks,head+1,i-head+2,vec_merge(vec_merge(vec_splice(x_map(xstr("map* ", varname, "=", End)),0,1,NULL),mid),xvec(NULL,";"," ", End)));
					}else{
						params=toks_join(vec_splice(params,0,0,xvec(mid, End)),",");
						syn_set_param(toks,i+2,params);
						vec_del(toks,head+1,i-head+2);
						vec_compact(toks); };
					i=head;
				}else if(syn_is_call(toks,i+2)){
					map* mid=vec_splice(vec_sub(toks,head+1,i-head-1),0,0,xvec(NULL, End));
					vec_del(toks,head+1,i-head+2);
					map* params=vec_splice(toks_split(syn_func_param(toks,i+2),",",0),0,0,xvec(mid, End));
 					params=toks_join(params,",");
					syn_set_param(toks,i+2,params);
					vec_compact(toks);
					i=head-2;
				}else if(is_word(expr_type(toks,head,i,env,fns),"map* void*") && neq(is_typecast(toks,head),"void*")){
					void* name=map_id(toks,i+3);
					map* mid=vec_splice(xadd(vec_sub(toks,head+1,i-head-1),NULL,",",NULL,str_quote(str_unquote(name)), End),0,0,xvec(NULL, End));
					vec_splice(toks,head+1,i+3-head,xvec("map_val",NULL,"(",NULL,mid,NULL,")", End));
					i=head; }; }; };
		return toks; };					
	return toks;
};
map* file_callmap(char* filename){ return func_depends(file_funcs(filename,1)); };
map* func_depend(map* mp,map* ret){
	for(int i=0; i<map_len(mp); i+=2){
		if(is_map(map_id(mp,i+1))){ func_depend(map_id(mp,i+1),ret); continue; };
		char* name=syn_is_call(mp,i);
		if(name){ add(ret,name,map_val(map_val(funcs(),name),"file")); }; };
	return ret;
};
map* func_depends(map* mp){
	map* ret=new_map();
	for(int next1=next(mp,-1,NULL,NULL); has_id(mp,next1); next1++){ void* fn=map_id(mp,next1); char* name=map_key(mp, next1);
		map* map_1=func_depend(map_val(fn,"body"),new_map()); for(int next1=next(map_1,-1,NULL,NULL); has_id(map_1,next1); next1++){ void* val=map_id(map_1,next1); char* key=map_key(map_1, next1); px(xstr(name, " ", key, " ", val, End),1); }; };
	return ret;
};
int utests(char* test,char* file){
	map* mp=file_vec(file);
	int errs=0;
	int runs=0;
	for(int i=0; i<map_len(mp); i+=3){
		if(test && !str_has(map_id(mp,i),test)){ continue; };
		runs++;
		printf("Running: %20s\r",map_id(mp,i));
		fflush(stdout);
		errs+=utest(eval(map_id(mp,i+1),NULL),map_id(mp,i+2),map_id(mp,i+1),map_id(mp,i)); };
	int passed=runs-errs;
	px(xstr("\n",int_str( passed), "/",int_str( runs), " tests passed.", End),1);
	return errs;
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
map* funcs(){
	if(!map_val(map_val(_globals,"cache"),"reflect")){add(add_key(_globals,"cache",Map),"reflect",reflect()); };
	return map_val(map_val(_globals,"cache"),"funcs") ? map_val(map_val(_globals,"cache"),"funcs") : map_val(map_val(map_val(_globals,"cache"),"reflect"),"funcs");
};
void* fox_run(char* in){
	int halt=0;
	return fox_eval(x_toks(in,1),xmap("args", map_val(_globals,"args"), End),&halt);
};
void* fox_eval(map* mp,map* env,int* halt){
	if(!mp){ return NULL; };
	assert(ptr_type(mp)==Vector);
	enum {HBreak=1, HReturn, HContinue};
	void* v=map_id(mp,1);
	void* ret=NULL;
	if(is_map(v)){
		for(int i3=next(mp,-1,NULL,NULL); has_id(mp,i3); i3++){ void* v3=map_id(mp,i3); char* k3=map_key(mp, i3);
			ret=fox_eval(v3,env,halt);
			if(*halt){ return ret; }; };
		return ret; };
	if(str_eq(v,"if")){
		int idx=5;
		while(1){
			if(is_true(eval_toks(map_id(mp,idx),env))){
				return fox_eval(map_id(mp,idx+6),env,halt);
			}else if(str_eq(map_id(mp,idx+10),"else")){
				if(str_eq(map_id(mp,idx+12),"if")){ idx+=16; }
				else {return fox_eval(map_id(mp,idx+14),env,halt);};
			}else{
				return NULL; }; };
	}else if(str_eq(v,"while")){
		while(is_true(eval_toks(map_id(mp,5),env))){
			ret=fox_eval(map_id(mp,11),env,halt);
			if(*halt==HContinue){ *halt=0; }
			else if(*halt==HBreak){ *halt=0; break; }
			else if(*halt){ break; }; };
		return ret;
	}else if(str_eq(v,"for")){
		map* conds=map_id(mp,5);
		for(eval_toks(map_id(conds,1),env); is_true(eval_toks(map_id(conds,3),env)); eval_toks(map_id(conds,5),env)){
			ret=fox_eval(map_id(mp,11),env,halt);
			if(*halt==HContinue){ *halt=0; }
			else if(*halt==HBreak){ *halt=0; break; }
			else if(*halt){ break; }; };
		return ret;
	}else if(str_eq(v,"return")){
		*halt=HReturn;
		return eval_toks(mp,env);
	}else if(str_eq(v,"break")){
		*halt=HBreak;
	}else if(str_eq(v,"continue")){
		*halt=HContinue;
	}else if(is_func_decl(mp)){
		return ret;
	}else{
		return eval_toks(mp,env); };
	return ret;
};
static void* eval_toks(map* mp,map* env){
	int idx=1;
	return eval_expr(mp,&idx,env,0);
};
static void* eval_expr(map* mp,int* idx,map* env,int level){
	void* last=NULL;
	*idx=eval_expr_cont(mp,*idx,env,&last,level);
	return last;
};
void* binary_op(void* left, char oper, void* right){
	if(is_f(left)||is_f(right)){
		double a=is_double(left);
		double b=is_double(right);
		if(oper=='+'){ return double_var((a+b)); };
		if(oper=='-'){ return double_var((a-b)); };
		if(oper=='/'){ return double_var((a/b)); };
		if(oper=='*'){ return double_var((a*b)); };
		return double_var(0);
		fox_error(xstr("Unknown operator ", oper, End),0); };
	long long a=is_int(left);
	long long b=is_int(right);
	if(oper=='+'){ return int_var((a+b)); };
	if(oper=='-'){ return int_var((a-b)); };
	if(oper=='/'){ return int_var((a/b)); };
	if(oper=='*'){ return int_var((a*b)); };
	fox_error(xstr("Unknown operator ", oper, End),0);
	return NULL;
};
int is_true(void * val){
	if(!val){ return 0; };
	if(is_i(val)){ return is_int(val); };
	if(is_f(val)){ return is_double(val); };
	if(is_map(val)){ return map_len(val); };
	if(is_str(val)){ return str_len(val); };
	return 1;
};
void* eval(char* in,map* env){
	return eval_toks(x_toks(in,0),env);
};
int eval_expr_cont(map* mp,int idx,map* env,void** last,int level){
	if(!mp){ return idx; };
	void* ret=*last;
	if(!level) {level=100;};
	for(;idx<mp->len;idx+=2){
		void* val=map_id(mp,idx);	
		if(!val){ continue; }; //abnormal
		if(is_map(val)){ ret=eval_toks(val,env); continue; }
		else if(is_num(val)){ px("int",1); ret=val; continue; }
		else if(is_numeric(val)){
			if(strchr(val,'.')){
				double v2=0.0;
				sscanf(val,"%lf",&v2);
				ret=double_var(v2);
			}else{
				long long v2=0;
				sscanf(val,"%lld",&v2);
				ret=int_var(v2); };
//				ret=atoi(val).int_var()
			continue;
		}else if(fox_at(val,0)=='"'){
			ret=str_unquote(val);
			while(fox_at(is_str(map_id(mp,idx+2)),0)=='"'){
				ret=xcat(ret,str_unquote(map_id(mp,idx+2)), End);
				idx+=2; };
			continue;
		}else if(str_eq(val,"&")){
			void* name=map_id(mp,idx+2);
			if(!is_code(name)){ fox_error(xstr("operator& on a non variable ", name, End),1); };
			int idx1=map_has_key(env,name);
			if(!idx1){ fox_error(xstr("variable ", name, " doesn't exist", End),1); };
			ret=&(env->pairs[idx1-1].val);
			idx+=2;
		}else if(str_eq(val,"(")){ ret=eval_toks(map_id(mp,idx+2),env); idx+=4; continue; }
		else if(str_eq(val,"End")){
			ret=End;
			idx+=2;
		}else if(is_code(val)){
			if(str_eq(val,"NULL")){
				ret=NULL;
			}else if(str_eq(val,"_globals")){
				ret=_globals;
			}else if(str_eq(map_id(mp,idx+2),"(")){
				map* params=map_split(map_id(mp,idx+4),",",0);
				for(int i=next(params,-1,NULL,NULL); has_id(params,i); i++){ void* v=map_id(params,i); set(params,i,eval_toks(v,env)); };
				ret=call_func(params,val,env);
				idx+=6;
			}else if(map_has_key(env,"_") && map_has_key(map_val(env,"_"),val)){
				ret=map_val(map_val(env,"_"),val);
			}else if(str_eq(map_id(mp,idx+2),"=")){
				idx+=4;
				ret=eval_expr(mp,&idx,env,6);
				add(env,val,ret);
			}else if(str_eq(map_id(mp,idx+2),"++")){
				idx+=2;
				add(env,val,binary_op(map_val(env,val),'+',int_var(1)));
			}else if(str_eq(map_id(mp,idx+2),"--")){
				idx+=2;
				add(env,val,binary_op(map_val(env,val),'-',int_var(1)));
			}else if(str_eq(map_id(mp,idx+2),"+=")){
				idx+=4;
				add(env,val,binary_op(map_val(env,val),'+',eval_expr(mp,&idx,env,6)));
			}else if(str_eq(map_id(mp,idx+2),"-=")){
				idx+=4;
				add(env,val,binary_op(map_val(env,val),'-',eval_expr(mp,&idx,env,6)));
			}else if(str_eq(map_id(mp,idx+2),"*=")){
				idx+=4;
				add(env,val,binary_op(map_val(env,val),'*',eval_expr(mp,&idx,env,6)));
			}else if(str_eq(map_id(mp,idx+2),"/=")){
				idx+=4;
				add(env,val,binary_op(map_val(env,val),'/',eval_expr(mp,&idx,env,6)));
			}else{
				ret=map_val(env,val); };
			continue;
		}else if(str_eq(val,".")){ if(is_map(ret)) {ret=map_val(ret,map_id(mp,idx+=2-1));} continue; }
		else if(str_eq(val,"!")){
			idx+=2;
			ret=eval_expr(mp,&idx,env,0);
			ret = int_var((is_i(ret) ? !is_int(ret) : !ret));
			continue;
		};
		int clevel=1;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"/")){ idx+=2; ret=binary_op(ret,'/',eval_expr(mp,&idx,env,clevel)); continue; }
		else if(str_eq(val,"*")){ idx+=2; ret=binary_op(ret,'*',eval_expr(mp,&idx,env,clevel)); continue; };

		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"-")){ idx+=2; ret=binary_op(ret,'-',eval_expr(mp,&idx,env,clevel)); continue; }
		else if(str_eq(val,"+")){ idx+=2; ret=binary_op(ret,'+',eval_expr(mp,&idx,env,clevel)); continue; };


		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(is_word(val,"== !=")){
			idx+=2;
			void* rest=eval_expr(mp,&idx,env,clevel);
			if(is_i(ret)){ ret=int_var((is_int(ret)==to_int(rest))); }
			else if(is_str(ret)){ ret=int_var((str_eq(ret,rest))); }
			else {ret=NULL;};
			if(str_eq(val,"!=")){ ret=int_var((!is_int(ret))); };
			continue;
		}else if(is_word(val,"> < >= <=")){
			idx+=2;
			int val1=is_int(ret);
			int val2=is_int(eval_expr(mp,&idx,env,clevel));
			if(str_eq(val,">")){ val1=val1>val2; }
			else if(str_eq(val,"<")){ val1=val1<val2; }
			else if(str_eq(val,"<=")){ val1=val1<=val2; }
			else if(str_eq(val,">=")){ val1=val1>=val2; };
			ret=int_var(val1);
			continue;
		};
		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"&&")){
			idx+=2;
			if(is_true(ret)){ ret=eval_expr(mp,&idx,env,clevel); }
			else{
				while(idx<mp->len-2){
					if(is_word(map_id(mp,idx),"&& || ? :")){ idx-=2; break; };
					idx+=2; }; };
			continue;
		};
		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"||")){
			idx+=2;
			if(is_true(ret)){ break; }
			else {ret=eval_expr(mp,&idx,env,clevel);};
			continue;
		};
		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"?")){
			idx+=2;
			if(is_true(ret)){
				ret=eval_expr(mp,&idx,env,clevel);
				break;
			}else{
				while(idx<mp->len && !str_eq(map_id(mp,idx),":")){ idx+=2; };
				continue; }; };
		if(str_eq(val,":")){
			break;
		};
		clevel++;
		if(level<=clevel){ idx-=2; break; };
		if(str_eq(val,"=")){
			idx+=2;
			if(!is_str(ret)){ fox_error("eval() in name=value name should be a valid name",0); };
			void* val=eval_expr(mp,&idx,env,clevel);
			add(env,ret,val);
			ret=val;
			continue; };
	};
	*last=ret;
	return idx;
};
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
		}else if(map_val(v,"default")){
			add(ret,k,eval(map_val(v,"default"),NULL));
		}else{
			fox_error(xstr("Parameter missing in ", map_val(fn,"name"), "(", k, "=?) in ", func_cdecl(fn,1), "\nargs=", json(sent,0), End),0);
			assert(0); }; };
	return ret;
};
void* data_exec(void* data,map* env){
	if(!data){ return NULL; };
	if(is_str(data)){ return data; };	
	if(!is_map(data)){ return to_str(data,"",0); };
	char* func=map_id(data,0);
	return call_func(vec_compact(map_del(data,0,1)),func,env);
};
void* call_func(map* params,char* name,map* env){
	params=eval_params(params,name,env,funcs());
	if(str_start(name,"php_")){ return call_php(params,sub_str(name,5,-2147483648)); };
	map* user=map_val(map_val(map_val(_globals,"cache"),"userfuncs"),name);
	void* ret=NULL;
	if(user){
		int halt=0;
		ret=fox_eval(user,params,&halt);
	}else{
		ret=invoke(params,name); };
	return ret;
};
map* ping_map(map* in){ return in; };
