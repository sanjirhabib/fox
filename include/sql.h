#pragma once
#include <libstemmer.h>
#include <core.h>
#pragma fox module sql
#include <unicode.h>
#include <sqlite3.h>
#include <playground.h>
#include <dir.h>
#pragma fox cc -lsqlite3
char* str_code(char* in); //sql.fox
void* sql_value(char* sql, char* db, map* param); //sql.fox
map* sql_rows(char* sql, char* db, map* param); //sql.fox
map* sql_row(char* sql, char* db, map* param); //sql.fox
map* sql_vector(char* sql, char* db, map* param); //sql.fox
map* sql_pairs(char* sql, char* db, map* param); //sql.fox
map* sql_vec(char* sql, char* db, map* param); //sql.fox
char* sql_drop(char* tbl); //sql.fox
void* sql_error(char* sql, char* db, sqlite3* conn); //sql.fox
map* sqls_exec(map* sqls, char* db); //sql.fox
map* parse_connection(char* in); //sql.fox
map* conn_db(char* db); //sql.fox
map* sql_exec(char* sql, char* db, map* params); //sql.fox
void* lite_conn(char* db); //sql.fox
map* lite_exec(char* sql, char* db, map* params); //sql.fox
void close_conns(); //sql.fox
