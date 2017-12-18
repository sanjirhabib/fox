#line 2 "/web/fox/generator.fox"
#include <fox.h>

char* gen_htaccess(char* outfile){
	return write_file(""
	"RewriteEngine On\n"
	"#RewriteCond %{HTTPS} off\n"
	"#RewriteRule (.*) https://%{HTTP_HOST}%{REQUEST_URI} [R=301,L]\n"
	"RewriteCond %{REQUEST_FILENAME} !-f\n"
	"RewriteCond %{REQUEST_FILENAME} !-d\n"
	"RewriteRule ^(.+)$ index.cgi [L,QSA]\n"
	"<FilesMatch \"\\.(db|txt|sql)$\">\n"
	"Order allow,deny\n"
	"Deny from all\n"
	"</FilesMatch>\n"
	""
	"",outfile,0,1);
};
char* gen_cgi_makefile(char* name, char* outfile){
	return write_file(xstr("", 
	"all:\n", 
	"\tfox cgi ", name, ".fox\n", 
	"fox:\n", 
	"	cd /web/fox && make install && cd -\n", 
	"c:\n", 
	"	fox cgi salat.fox --keepfiles=1\n", 
	"", 
	"", End),outfile,0,1);
};
char* gen_fox_cgi(char* name,char* outfile){
	return write_file(xstr("", 
	"#line 2 \"", name, ".fox\"\n", 
	"#include \"", name, ".h\"\n", 
	"\n", 
	"int run(map* req){\n", 
	"\t_globals.dbs.", name, "=\"", name, ".db\"\n", 
	"	path=_globals.paths\n", 
	"\n", 
	"	if \"/\".get().end\n", 
	"		{{\n", 
	"\t\t\ttitle ", str_title(name), "'s Application\n", 
	"		}}.show_page()\n", 
	"\n", 
	"	not_found()\n", 
	"	return 0\n", 
	"}\n", 
	"", 
	"", End),outfile,0,1);
};
int cgi_init(char* name){
	if(!is_file((xstr(name,".fox", End)))){ gen_fox_cgi(name,xstr(name,".fox", End)); };
	if(!is_file(".htaccess")){ gen_htaccess(".htaccess"); };
	if(!is_file("Makefile")){ gen_cgi_makefile(name,"Makefile"); };
	return 0;
};
