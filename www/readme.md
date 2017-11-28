# Fox cgi module
## Enabling

Edit your http.conf and add the following:
```
LoadModule cgi_module libexec/mod_cgi.so
<Directory "/web">
	SetHandler cgi-script
<IfModule dir_module>
	DirectoryIndex index.php index.html cgi                                                                      
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
fox cgi index.cgi
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