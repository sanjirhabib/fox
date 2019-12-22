#pragma once
#include <core.h>
#pragma fox module crud
#include <sql.h>
#include <schema.h>
#include <http.h>
#include <html.h>
char* row_id(map* row, map* cols); //crud.fox
map* row_ids(map* row, map* cols); //crud.fox
char* crud(char* sql, char* db); //crud.fox
char* rows_data_html(map* in); //crud.fox
map* rows_data(map* rows, map* cols); //crud.fox
char* fts5_create(char* db); //crud.fox
char* col_align(map* col); //crud.fox
