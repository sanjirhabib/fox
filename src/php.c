#include <php.h>
#include <fox.h>

void xexit(int val){
	zend_error(E_ERROR,"%s","Exiting Abnormally");
}
void* fox_error(char* msg,int dump){
	if(dump) php_printf("<pre>%s</pre>",stack_str());
	zend_error(E_ERROR,"%s",msg);
	return msg;
}
void* px(void* str,int newline){
	php_printf("%s",str);
	return str;
}
void* zval_var(zval* z){
	if(!z) return NULL;
	int type=Z_TYPE_P(z);
	if(type==IS_NULL) return NULL;
	else if(type==IS_ARRAY) return zval_map(z);
	else if(type==IS_STRING) return str_dup(Z_STRVAL_P(z));
	else if(type==IS_TRUE||type==IS_FALSE||type==IS_LONG) return int_var(Z_LVAL_P(z));
	else if(type==IS_DOUBLE) return int_var((int)Z_DVAL_P(z));
	else if(type==IS_RESOURCE) return "<RES>";
	else if(type==IS_OBJECT) return "<OBJ>";
	else if(type==IS_REFERENCE) return "<REF>";
	else if(type==IS_UNDEF) return NULL;
	else if(type==IS_CONSTANT) return "<CONST>";
	else if(type==IS_CONSTANT_AST) return "<AST>";
	else if(type==IS_INDIRECT) return "<INDIRECT>";
	else if(type==IS_PTR) return "<PTR>";
	printf("unknown/utype=%d\n",type);
	return "<UNKNOWN>";
}
zval var_zval(void* v){
	zval ret={0};
	if(!v) { ZVAL_NULL(&ret); }
	else if(is_map(v)) return map_zval(v);
	else if(is_str(v) && v){ ZVAL_STRING(&ret,v); }
	else if(is_int(v)){ ZVAL_LONG(&ret,is_int(v)); }
	return ret;
}
zval map_zval(map* mp){
	zval ret={0};
	array_init(&ret);
	for(int i=next(mp,-1,NULL,NULL);has_id(mp,i);i++){
		char* k=map_key(mp,i);
		zval z=var_zval(map_id(mp,i));
		if(is_i(k)) add_index_zval(&ret,is_int(k)-1,&z);
		else add_assoc_zval(&ret,k,&z);
	}
	return ret;
}
map* zval_map(zval* z){
	if(!z) return NULL;
	map* mp=new_map();
	zval *arr, *data;
	HashTable *arr_hash=Z_ARRVAL_P(z);
	HashPosition pointer;
	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); (data=zend_hash_get_current_data_ex(arr_hash, &pointer)); zend_hash_move_forward_ex(arr_hash, &pointer)){
		zend_string *zkey;
		zend_ulong index;
		char* key;
		if(zend_hash_get_current_key_ex(arr_hash,&zkey,&index,&pointer)==HASH_KEY_IS_LONG) key=int_var(index+1);
		else key=ZSTR_VAL(zkey);
		map_add(mp,key,zval_var(data));
	}
	return mp;
}
void* php_global(char* name){
	zend_string* key=zend_string_init(name,strlen(name),0);
	zval* zret=zend_hash_find(&EG(symbol_table), key);
	zend_string_release(key);
	if(!zret) return NULL;
	if(Z_TYPE_P(zret)==IS_INDIRECT) zret = Z_INDIRECT_P(zret);
	if(Z_TYPE_P(zret)==IS_REFERENCE) ZVAL_DEREF(zret);
	return zval_var(zret);
}
void* call_php(map* params,char* func){
	zval z={0};
	zval php_func={0};
	ZVAL_STRING(&php_func,func);
	int no=params && params->len ? params->len : 0;
	zval* php_args=NULL;
	void* ret={0};
	if(no){
		php_args=emalloc(sizeof(zval)*no);
		for(int i=0;i<no;i++) php_args[i]=var_zval(map_id(params,i));
	}
	if(call_user_function(CG(function_table),NULL,&php_func,&z,no,php_args)!=SUCCESS){
		zend_error(E_ERROR,"Call to %s failed\n",func);
	}
	else ret=zval_var(&z);
	if(no){
		for(int i=0;i<no;i++) zval_dtor(&php_args[i]);
		efree(php_args);
	}
	zval_dtor(&z);
	return ret;
}
