void* sql_item_type(char* type, char* db);
void* sql_item(char* name, char* type, char* db);
void* fetch_global(char* in);
map* sql_conn(char* name);
void* sql_value(char* sql, char* db, map* param);
map* sql_rows(char* sql, char* db, map* param);
map* sql_pairs(char* sql, char* db, map* param);
map* db_table_names(char* db);