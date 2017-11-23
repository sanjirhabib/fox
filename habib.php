<?
$_dbs['news']='/web/habibur/news/news.db;;main;/web/habibur/news/news.txt';
print_r(hello());
function hello(){
	global $_dbs;
	return sql_conn('news');
}
//print_r(sql_rows("select * from news","news"));
