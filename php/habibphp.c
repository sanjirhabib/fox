#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <php.h>
#include <fox.h>
#include "habib.h"

void* zval_var(zval* z);
zval var_zval(void* v);
zval map_zval(map* mp);
map* zval_map(zval* z);
void* call_php(map* params,char* func);

PHP_FUNCTION(sql_item_type){
	char* in_type=NULL;
	size_t in_type_len=-1;
	char* in_db=NULL;
	size_t in_db_len=-1;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"ss", &in_type, &in_type_len, &in_db, &in_db_len)==FAILURE){ RETURN_NULL(); }

	void* ret=sql_item_type(in_type,in_db);
	zval zret=var_zval(ret);
	RETURN_ZVAL(&zret,0,0);
}

PHP_FUNCTION(sql_item){
	char* in_name=NULL;
	size_t in_name_len=-1;
	char* in_type=NULL;
	size_t in_type_len=-1;
	char* in_db=NULL;
	size_t in_db_len=-1;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"sss", &in_name, &in_name_len, &in_type, &in_type_len, &in_db, &in_db_len)==FAILURE){ RETURN_NULL(); }

	void* ret=sql_item(in_name,in_type,in_db);
	zval zret=var_zval(ret);
	RETURN_ZVAL(&zret,0,0);
}

PHP_FUNCTION(fetch_global){
	char* in_in=NULL;
	size_t in_in_len=-1;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s", &in_in, &in_in_len)==FAILURE){ RETURN_NULL(); }

	void* ret=fetch_global(in_in);
	zval zret=var_zval(ret);
	RETURN_ZVAL(&zret,0,0);
}

PHP_FUNCTION(sql_conn){
	char* in_name=NULL;
	size_t in_name_len=-1;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"|s", &in_name, &in_name_len)==FAILURE){ RETURN_NULL(); }
	if(in_name_len==-1) in_name=NULL;
	else in_name=str_dup(in_name);

	map* ret=sql_conn(in_name);
	zval zret;
	if(!ret){
		array_init(&zret);
	}
	else zret=var_zval(ret);
	RETURN_ZVAL(&zret,0,0);
}

PHP_FUNCTION(sql_value){
	char* in_sql=NULL;
	size_t in_sql_len=-1;
	char* in_db=NULL;
	size_t in_db_len=-1;
	zval* in_param_zval=NULL;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"ss|z", &in_sql, &in_sql_len, &in_db, &in_db_len, &in_param_zval)==FAILURE){ RETURN_NULL(); }
	void* in_param=NULL;
	if(!in_param_zval) in_param=NULL;
	else in_param=zval_var(in_param_zval);

	void* ret=sql_value(in_sql,in_db,in_param);
	zval zret=var_zval(ret);
	RETURN_ZVAL(&zret,0,0);
}

PHP_FUNCTION(sql_rows){
	char* in_sql=NULL;
	size_t in_sql_len=-1;
	char* in_db=NULL;
	size_t in_db_len=-1;
	zval* in_param_zval=NULL;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"ss|z", &in_sql, &in_sql_len, &in_db, &in_db_len, &in_param_zval)==FAILURE){ RETURN_NULL(); }
	void* in_param=NULL;
	if(!in_param_zval) in_param=NULL;
	else in_param=zval_var(in_param_zval);

	map* ret=sql_rows(in_sql,in_db,in_param);
	zval zret;
	if(!ret){
		array_init(&zret);
	}
	else zret=var_zval(ret);
	RETURN_ZVAL(&zret,0,0);
}

PHP_FUNCTION(sql_pairs){
	char* in_sql=NULL;
	size_t in_sql_len=-1;
	char* in_db=NULL;
	size_t in_db_len=-1;
	zval* in_param_zval=NULL;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"ss|z", &in_sql, &in_sql_len, &in_db, &in_db_len, &in_param_zval)==FAILURE){ RETURN_NULL(); }
	void* in_param=NULL;
	if(!in_param_zval) in_param=NULL;
	else in_param=zval_var(in_param_zval);

	map* ret=sql_pairs(in_sql,in_db,in_param);
	zval zret;
	if(!ret){
		array_init(&zret);
	}
	else zret=var_zval(ret);
	RETURN_ZVAL(&zret,0,0);
}

PHP_FUNCTION(db_table_names){
	char* in_db=NULL;
	size_t in_db_len=-1;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s", &in_db, &in_db_len)==FAILURE){ RETURN_NULL(); }

	map* ret=db_table_names(in_db);
	zval zret;
	if(!ret){
		array_init(&zret);
	}
	else zret=var_zval(ret);
	RETURN_ZVAL(&zret,0,0);
}

static zend_function_entry habib_functions[] = {
	PHP_FE(sql_item_type, NULL)
	PHP_FE(sql_item, NULL)
	PHP_FE(fetch_global, NULL)
	PHP_FE(sql_conn, NULL)
	PHP_FE(sql_value, NULL)
	PHP_FE(sql_rows, NULL)
	PHP_FE(sql_pairs, NULL)
	PHP_FE(db_table_names, NULL)
	{NULL, NULL, NULL}
};
PHP_RINIT_FUNCTION(habib){
	gc_start();
	return SUCCESS;
}
PHP_RSHUTDOWN_FUNCTION(habib){
	gc_end();	
	return SUCCESS;
}

zend_module_entry habib_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"habib",
	habib_functions,
	NULL,
	NULL,
	PHP_RINIT(habib),
	PHP_RSHUTDOWN(habib),
	NULL,
#if ZEND_MODULE_API_NO >= 20010901
	"0.70", //Version Number
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HABIB
ZEND_GET_MODULE(habib)
#endif

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
	printf("utype=%d\n",type);
	return "<UNKNOWN>";
}
zval var_zval(void* v){
	zval ret={0};
	if(!v) { ZVAL_NULL(&ret); }
	else if(is_map(v)) return map_zval(v);
	else if(is_str(v) && v){ ZVAL_STRING(&ret,v); }
	else if(is_i(v)){ ZVAL_LONG(&ret,is_int(v)); }
	else if(is_f(v)){ ZVAL_DOUBLE(&ret,is_double(v)); }
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
