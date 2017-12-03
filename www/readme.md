# Fox cgi module
## Enabling

Edit your http.conf and add the following:
```
#osx
LoadModule cgi_module libexec/mod_cgi.so
#linux
LoadModule cgi_module modules/mod_cgi.so
<Directory "/www">
	Options Indexes FollowSymLinks ExecCGI
	AddHandler cgi-script .cgi
<IfModule dir_module>
	DirectoryIndex index.html index.php index.cgi                                                                      
```
Add the following to your .htaccess file in your web document folder
```
RewriteEngine On
RewriteCond %{REQUEST_FILENAME} !-f
RewriteCond %{REQUEST_FILENAME} !-d
RewriteRule ^(.+)$ index [L,QSA]
<FilesMatch "\.(db|txt|sql|fox)$">
Order allow,deny
Deny from all
</FilesMatch>
```
Create a file called index.fox
```
#include "index.h"
main(){
	header=read_http()
	---
	hello world
	---.px()
	header.px()
	http_footer().px()
}
```
Compile it
```
fox cgi index.fox
```
Will create a file called ./cgi in the folder.
Check if you can run it from browser.
```
$ curl http://localhost/fox/www/
hello world{"url":"/fox/www/", "path":"/fox/www/", "param":null, "params":{}, "method":"get", "remote":"::1", "paths":["fox", "www"], "host":"localhost", "protocol":"http", "port":"80"}<div class="small lighten pull-right" style="margin-top:4em;">
Run Time: 2ms=0/2/0</br>
Malloc: 132KB. Total: 940KB.</br>
GC: 0runs.

```
Don't put "cgi" in the URL. This cgi will get dispatched of all calls under this path.
