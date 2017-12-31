#include <fox.h>

map* xadd(map* mp,...){
	int type=ptr_type(mp);
	va_list args;
	va_start(args,mp);
	while(1){
		if(type==Map){
			char* n=va_arg(args,char*);
			if(n==End){ break; };
			void* v=va_arg(args,void*);
			add(mp,n,v);
		}else{
			void* v=va_arg(args,void*);
			if(v==End){ break; };
			vec_add(mp,v); }; };
	va_end(args);
	return mp;
};
map* to_map(void* val){ return is_map(val) ? val : str_map(to_str(val,"",0),Map); };
map* del_index(map* mp,int idx){
	if(!mp){ return mp; };
	assert(idx>=0 && idx<mp->len);
	char* id=map_key(mp,idx);
	if(!id){
		assert(0);
		return mp; };
	int next=mp->pairs[idx].nextid;
	mp->pairs[idx].nextid=-1;
	int i=key_hash(id) & (map_size(mp)-1);
	if(mp->pairs[i].hkey==idx+1){ mp->pairs[i].hkey=next > 0 ? next : 0; } //0=empty for hkey. -1=end for nextid
	else{
		i=mp->pairs[i].hkey;
		int nextid=0;
		while((nextid=mp->pairs[i-1].nextid)!=idx+1 && nextid>0) {i=nextid;};
		if(nextid==idx+1){
			if(i==next){ fox_error(xstr("inf loop with i(",int_str( i), ")=next-1(",int_str( next), "-1)", End),0); };
			mp->pairs[i-1].nextid=next;
		}else {assert(0);}; };
	return mp;
};
map* map_del_key(map* mp,char* key){ return map_del(mp,map_has_key(mp,key)-1,1); };
map* map_del(map* mp,int idx,int len){
	if(!mp||idx<0){ return mp; };
	if(len<=0){ len+=(mp->len-idx); };
	if(len<=0){ return mp; };
	if(ptr_type(mp)==Vector){ return vec_del(mp,idx,len); };
	// don't memset(0) as there is hash indexing info here too.
	while(len--){
		del_index(mp,idx+len);
		mp->pairs[idx+len].val=&skip;
		mp->pairs[idx+len].nextid=0;
		mp->pairs[idx+len].id=(char*)&skip; };
	return mp;
};
map* xjson_map(char* in,int type){ return xjson_map_recurse(&in,type); };
map* xjson_map_recurse(char** line,int type){
	if(!line||!*line||!**line){ return type==Vector ? new_vec() : new_map(); };
	char* str=*line;
	char term='}';
	map* ret=NULL;
	if(*str=='['){
		type=Vector;
		term=']';
		ret=new_vec();
		str++;
	}else if(*str=='{'){
		ret=new_map();
		str++;
	}else{
		ret=type==Vector ? new_vec() : new_map();
		term='\0'; };
	char* last=NULL;
	char* sep=" \t\n\r";
	char* temp=str;
	temp=skip_word(temp,":=,");
	if(strchr(":=",*temp)){
		temp=skip_word(temp,",");
			if(*temp==','){ sep="\t\n\r,"; };
	}else if(*temp==','){ sep="\t\n\r,"; };
	if(term){ sep=xcat(sep,char_str(term), End); };
	while(*str && *str!=term){
		void* name=NULL;
		void* value=NULL;
		if(str==last){ px(str,1); };
		assert(str!=last);
		last=str;
		str=skip_over(str," \t\n\r,]}");
		if(*str=='{'||*str=='['){
			value=xjson_map_recurse(&str,type);
		}else if(*str==':' || *str=='='){
			str=skip_over((str+1)," \t\n\r");
			value=read_value(&str,sep);
		}else if(type==Vector){
			value=read_upto(&str,sep);
		}else{
			name=read_upto(&str," \t\n\r,=:]}");
			str=skip_over(str," \n\r\t");
			if(str_chr(":=",*str)){
				str=skip_over((str+1)," \n\r\t");
				value=read_value(&str,sep);
			}else{
				if(type!=Keys){
					value=name; }; }; };//.str_title()
		map_add_pair(ret,name,value,type); };
	if(*str==term){ str++; };
//	ret.map_add_pair(name,value,type)
//	if type!=Vector && ret.type && ret.name => ret=xmap(ret.name,ret)
	*line=str;
	return ret;
};
int str_level(char* line){
	if(!line||!is_str(line)){ return 0; };
	int ret=0;
	while(*line=='\t'||*line==' '){ ret++; line++; };
	return ret;
};
int block_level(char* str){
	int ret=0;
	if(!str){ return 0; };
	while(*str){
		if(*str=='\n'||*str=='\r'){ ret=0; }
		else if(*str=='\t'||*str==' '){ ret++; }
		else {return ret;};
		str++;
	};
	return ret;
};
map* xkeys(char* str){ return xjson_map(str,Keys); };
map* xindex(char* str){ return xjson_map(str,Index); };
map* xarray(char* str){ return str_map(str,Vector); };
map* str_vec(void* str){
	if(is_map(str)){ return str; };
	return str ? str_map(str,Vector) : NULL;
};
map* str_map(char* str,int type){
	if(is_map(str)){ return is_map(str); };
	if(!str_len(str)){ return xjson_map(str,type); };
	void* ret=cache(str,"str_map",NULL);
	if(ret){ return ret; };
	if((!strchr(str,'\n') && !strchr(str,'\r'))){ ret=xjson_map(str,type); }
	else {ret=block_map(&str,block_level(str),type);};
	return cache(str,"str_map",ret);
};
map* vec_del(map* mp,int from,int len){
	if(!map_len(mp)){ return NULL; };
	assert(is_vec(mp));
	if(from<0){ from+=mp->len; };
	if(len<=0){ len+=mp->len; };
	assert(from>=0 && len>0);
	assert(from<mp->len);
	//assert(from+len<=mp->len);// fix it by, len=mp->len-from
	if(from+len>mp->len){ len=mp->len-from; };
	int rest=mp->len-(from+len);
	if(rest){ for(int i=0;i<len;i++) {set(mp,i+from,&skip);}; }
	else {mp->len-=len;};
	return mp;
};
map* vec_shift(map* mp,int idx,int len,int offset){
	if(!mp){ return mp; };
	if(mp->len<idx+len){ len=mp->len-idx; };
	if(!len){ return mp; };
	int ofrom=offset<0 ? offset+idx : idx+len;
	int olen=abs(offset);
	assert(ofrom>=0 && ofrom<mp->len);
	map* old=vec_sub(mp,ofrom,olen);
	memmove(mp->vars+ofrom,mp->vars+idx,olen*sizeof(void*));
	int ointo=offset<0 ? ofrom+len : idx;
	memcpy(mp->vars+ointo,old->vars,old->len*sizeof(void*));
	return mp;
};
map* vec_sub(map* mp,int from,int len){
	if(from<0){ from=0; };
	if(!mp){ return NULL; };
	if(ptr_type(mp)!=Vector){ return mp; };
	if(from>mp->len){ from=mp->len; len=0; }
	else if(from+len>mp->len){ len=mp->len-from; };
	if(!len){ len=mp->len-from; };
	if(len<=0){ return NULL; };
	map* ret=new_vec();
	vec_realloc(ret,len);	
	memcpy(ret->vars,mp->vars+from,sizeof(void*)*len);
	ret->len=len;
	return ret;
};
map* vec_slice(map* mp,int from,int len){
	map* ret=vec_sub(mp,from,len);
	vec_del(mp,from,len);
	return ret;
};
map* map_splice(map* mp,int from,int del,map* ad){
	map* ret=new_map();
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){ void* v=map_id(mp,idx); char* k=map_key(mp, idx);
		if(idx==from){
			for(int idx2=next(ad,-1,NULL,NULL); has_id(ad,idx2); idx2++){ void* v2=map_id(ad,idx2); char* k2=map_key(ad, idx2);
				if(is_str(k2)){
					if(!map_val(mp,k2)){ add(ret,k2,v2); };
				}else {vec_add(ret,v2);}; }; };
		if(idx>=from && idx<from+del){ continue; };
		if(is_str(k)){
			if(!map_val(mp,k)){ add(ret,k,v); };
		}else {vec_add(ret,v);}; };
	return ret;
};
void* vec_pop(map* mp){
	if(!mp||!mp->len){ return NULL; };
	void* ret=map_id(mp,mp->len-1);
	vec_del(mp,mp->len-1,1);
	return ret;
};
map* vec_splice(map* mp,int from,int del,map* add){
	if(!mp){ return mp; };
	assert(ptr_type(mp)==Vector);
	if(from < 0){ from+=mp->len; };
	assert(del>=0);
	assert(from>=0 && from<=mp->len);

	int inc=add ? add->len : 0;
	int len=mp->len;

	if(from>len){ from=len; del=0; }
	else if(from+del>len){ del=len-from; };

	int offset=inc-del;
	if(offset>0){ vec_realloc(mp,offset); };

	int moved=len-(from+del);
	if(offset && moved){ memmove(mp->vars+from+inc,mp->vars+from+del,sizeof(void*)*moved); };
	if(add){ memcpy(mp->vars+from,add->vars,add->len*sizeof(void*)); };
	mp->len+=offset;
	if(offset<0){ memset(mp->vars+mp->len,0,sizeof(void*)*offset*-1); };
	return mp;
};
map* vec_dup(map* mp){
	if(!mp||!mp->len){ return new_vec(); };
	map* ret=new_vec();
	vec_realloc(ret,mp->len);
	memcpy(ret->vars,mp->vars,mp->len*sizeof(void*));
	ret->len=mp->len;
	return ret;
};
map* map_dup(map* mp){
	if(!mp){ return NULL; };
	map* ret=new_map();
	if(!map_len(mp)){ return ret; };
	assert(ptr_type(mp)==Map);
	ret->pairs=fox_alloc(map_size(mp)*sizeof(Mapcell),Cell2);
	memcpy(ret->pairs,mp->pairs,map_size(mp)*sizeof(Mapcell));
	ret->len=mp->len;
	return ret;
};
map* map_merge(map* mp1,map* mp2){
	if(!mp1){ mp1=new_map(); };
	for(int idx=next(mp2,-1,NULL,NULL); has_id(mp2,idx); idx++){ void* v =map_id(mp2,idx); char* k=map_key(mp2, idx); map_add(mp1,is_int(k) ? NULL : k,v); };
	return mp1;
};
map* vec_merge(map* mp1,map* mp2){
	if(!mp2){ return mp1; };
	if(!mp1){ return vec_dup(mp2); };
	vec_realloc(mp1,mp2->len);
	memcpy(((char*)mp1->vars)+mp1->len*sizeof(void*),mp2->vars,mp2->len*sizeof(void*));
	mp1->len+=mp2->len;
	return mp1;
};
map* map_compact(map* mp){
	if(!mp || ptr_type(mp)!=Map){ return mp; };
	int off=0;
	int i=0;
	for(i=0;i+off<mp->len;i++){
		while(i+off<mp->len && mp->pairs[i+off].id==(char*)&skip) {off++;};
		if(i+off>=mp->len){ break; };
		if(off){ mp->pairs[i]=mp->pairs[i+off]; }; };
	mp->len-=off;
	if(off){ map_reindex(mp); };
	return mp;
};
map* vec_reverse(map* mp){
	if(!mp){ return mp; };
	int len=mp->len;
	for(int i=0; i<len/2; i++){
		void* temp=mp->vars[len-i-1];
		mp->vars[len-i-1]=mp->vars[i];
		mp->vars[i]=temp; };
	return mp;
};
map* vec_compact(map* mp){
	if(!mp || ptr_type(mp)!=Vector){ return mp; };
	int off=0;
	int i=0;
	for(i=0;i+off<mp->len;i++){
		while(i+off<mp->len && mp->vars[i+off]==&skip) {off++;};
		if(i+off>=mp->len){ break; };
		if(off){ mp->vars[i]=mp->vars[i+off]; };
	};
	while(i<mp->len) {mp->vars[i++]=NULL;};
	mp->len-=off;
	return mp;
};
map* change_key(map* mp,int idx,char* id){
	if(!mp){ return mp; };
	assert(idx>=0 && idx<=mp->len);
	assert(ptr_type(mp)==Map);
	if(key_eq(mp,idx,id)){ return mp; };
	del_index(mp,idx);
	map_del(mp,map_has_key(mp,id)-1,1);
	mp->pairs[idx].id=id;
	map_index(mp,idx);
	return mp;
};
map* add_id(map* mp,int idx){
	if(!is_map(map_id(mp,idx))){ set(mp,idx,new_map()); };
	return map_id(mp,idx);
};
map* add_key(map* mp,char* k,int type){
	if(!is_map(map_val(mp,k))){ add(mp,k,type==Vector ? new_vec() : new_map()); };
	return map_val(mp,k);
};
char* map_join(map* mp,char* joiner){
	if(!is_map(mp)){ return to_str(mp,"",0); };
	char* ret=NULL;
	for(int idx=next(mp,-1,NULL,NULL); has_id(mp,idx); idx++){ void* v=map_id(mp,idx);
		if(idx){ ret=xcat(ret,joiner, End); };
		if(is_map(v)){ ret=xcat(ret,map_join(v,joiner), End); }
		else {ret=xcat(ret,to_str(v,"",0), End);};
	};
	return ret;
};
char* map_str(map* mp){ return map_str_indent(mp,0); };
char* map_str_indent(map* mp,int indent){
	char* ret=NULL;
	if(!mp){ return ret; };
	int type=ptr_type(mp);
	char* istr=str_times("\t",indent);
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i); char* k=map_key(mp, i);
		ret=xcat(ret,istr, End);
		if(is_map(v)){
			if(ptr_type(v)!=Vector && map_val(v,"type")){
				ret=xcat(ret,k,"=:",map_val(v,"type"), End);
				for(int i2=next(v,-1,NULL,NULL); has_id(v,i2); i2++){ void* v2=map_id(v,i2); char* k2=map_key(v, i2);
					if(str_eq(k2,"type") || str_eq(k2,"name") && map_val(v,"type")){
						continue;
					}else if(is_map(v2)){
						ret=xcat(ret," ",k2,"=",json(v2,0), End);
					}else if(!v2 || str_eq(k2,v2) || is_int(v2)==1){
						ret=xcat(ret," ",k2, End);
					}else if(is_code(v2)){
						ret=xcat(ret," ",k2,"=",v2, End);
					}else if(is_num(v2)){
						if(is_int(v2)){
							ret=xcat(ret," ",k2,"=#",int_str(is_int(v2)), End);
						}else {is_double(v2);};
							ret=xcat(ret," ",k2,"=#",double_str(is_double(v2)), End);
					}else{
						ret=xcat(ret," ",k2,"=",str_quote(v2), End); }; };
				ret=xcat(ret,"\n", End);
			}else{
				int eq=0;
				if(type==Vector||is_int(k)){ ret=xcat(ret,"=", End); eq=1; }
				else {ret=xcat(ret,k, End);};
				if(ptr_type(v)==Vector){
					if(!eq){ ret=xcat(ret,"=", End); };
					ret=xcat(ret,"[]", End);
				};
				char* bl=map_str_indent(v,indent+1);
				if(bl){ ret=xcat(ret,"\n",bl, End); }; };
		}else if(type==Vector||is_int(k)){
			ret=xcat(ret,to_str(v,"",0),"\n", End);
		}else if(is_i(v)){ ret=xcat(ret,k,"=#",int_str(is_int(v)),"\n", End); }
		else if(is_double(v)){ ret=xcat(ret,k,"=#",double_str(is_double(v)),"\n", End); }
		else if(is_str(v)){
			if(strchr(v,'\n')){
				ret=xcat(ret,k,"=|\n", End);
				map* map_1=str_split(v,"\n",0); for(int i2=next(map_1,-1,NULL,NULL); has_id(map_1,i2); i2++){ void* v2=map_id(map_1,i2); char* k2=map_key(map_1, i2);
					ret=xcat(ret,str_times("\t",indent+1),v2,"\n", End); };
			}else{
				char* marker="\\";
				if(is_alpha(fox_at(v,0),NULL)){ marker=NULL; };
				ret=xcat(ret,k,"=",marker,v,"\n", End); }; };
	};
	return ret;
};
map* map_vec(map* mp){
	if(ptr_type(mp)==Vector){ return mp; };
	map* ret=new_vec();
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i); vec_add(ret,v); };
	return ret;
};
map* map_split(map* mp,char* str,int limit){
	if(!mp){ return mp; };
	if(limit==1){ return map_vec(mp); };
	map* ret=new_vec();
	map* curr=new_vec();
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){
		if(str_eq(map_id(mp,i),str)){
			if(curr->len){
				vec_add(ret,curr);
				curr=new_vec(); };
			if(limit==1){ break; };
			limit--;
			continue; };
		vec_add(curr,map_id(mp,i)); };
	if(curr->len){ vec_add(ret,curr); };
	return ret;
};
map* map_implode(map* mp,char* joiner){
	map* ret=new_vec();
	for(int i=next(mp,-1,NULL,NULL); has_id(mp,i); i++){ void* v=map_id(mp,i);
		if(i>0){ vec_add(ret,joiner); };
		vec_merge(ret,v); };
	return ret;
};
