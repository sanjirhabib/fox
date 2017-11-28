# Fox cgi module
## Enabling

edit http.conf and add the following:
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
