# Fox Language
Fox language. Transcompiles source into into human readable C.
Maintaining your original format, comment and indention and line number.
Builtin Fox & C interpreter. Bootstrapped compiler.

Source in Fox:
```
#include "hello.h"

main(){
	"
	Total $(args.map_len()) command line arguments
	those are $args
	".px()	

	name=:Habib
	msg="hello, $name!"
	msg.px()

	myvector=[:hello, :world]
	myvector.px()

	mymap={name: :Habib, age: 23}
	mymap.name=:Ibrahim
	mymap.px()

	---
	Drawing a box, $name!
	|---------|
	|         |
	|         |
	|---------|
	---.px()

	return 0
}

```
Generated C code:
```
#include "hello.h"

int main(int argc, char** argv, char** env){ gc_start(); map* args=argv_map(argv, argc);
	px(xstr("", 
	"Total ",int_str( map_len(args)), " command line arguments\n", 
	"those are ", args, 
	"", End),1);	

	char* name="Habib";
	char* msg=xstr("hello, ", name, "!", End);
	px(msg,1);

	map* myvector=xvec("hello", "world", End);
	px(myvector,1);

	map* mymap=xmap("name", "Habib", "age",int_var( 23), End);
	add(mymap,"name","Ibrahim");
	px(mymap,1);

	px(xstr("", 
	"Drawing a box, ", name, "!\n", 
	"|---------|\n", 
	"|         |\n", 
	"|         |\n", 
	"|---------|", 
	"", End),1);

	return 0;
};
```
Run and output
```
$ fox cc hello.fox
-> hello.c
-> hello.h
gcc -g -Os hello.c -o hello -std=gnu99 -Wno-logical-op-parentheses -lm -lfox 2>&1

$ ./hello
Total 1 command line arguments were passed
and those are ["./hello"]
hello, Habib!
["hello", "world"]
{"name":"Habib", "age":23}
{"name":"Ibrahim", "age":23}
Drawing a box, Habib!
|---------|
|         |
|         |
|---------|

$
```

## Features

### Small code base
1K lines for the runtime. The compiler with interpreter is 4K lines. Creates small 30KB binary for staticaly linked hello world app.

### GCed
Mark and sweep tracing GC. 50ms max delay in average use cases.

### Minimal boiler code
Primitive types are not boxed. Use C native int, double, float, char* as usual. Variants are void*.
It isn't until you wish to use vectors and maps when you need structures.
Vector and map structure
```
typedef struct map {
	int len;
	char type;
	union {
		struct Mapcell* pairs;
		void** vars;
	};
} map;

typedef struct Mapcell {
	short nextid;
	int hkey;
	char* id;
	void* val;
} Mapcell;

```

### Inline maps and vectors. Inline JSON.
```
data={name: Habib, age: 31}
data.name=:Ibrahim
days=[sun, mon, tue]
days[1].px()
```

### Reflection
```
funcs()
["hello","Abdul"].call_func(:hello_world)
```

### Multiline string
Three dashes with variable substitution.
```
---
Thee dash multiline comment
With $variable. 
Put code like this: $(1+1)
Or call functions $(name.str_upper())
---
```
Or double quotes. Same as triple dashes.
```
"
Escape dollar sign using double dollars, like $$this.
"
```
Or single quote. No variable substitution.
```
'
Single quote, without variable substitution.
No $variable substitution will occure.
'
```
You can add any unique string terminator with single quote version
```
'END123
Using an unique string terminator.
END123'
```
### Function chaining
```
"myfile.txt".file_open().parse().print()
```

### Default parameters
```
int fn(char* p1="hello", p2="world")
fn(,"hi")
```

### Indented blocks
```
if a==1
	:hello.px()
```
Use => in single line blocks
```
if a==1 || b==2 => done=1; return 1
```

### No semicolons, unless needed
```
a=1+1
a.px()
```	
### Inherited types
```
i=0
s=""
vals={a: b, c: d}
```

### Easily call functions from command line
```
./fox sub_str hello 3 -1
```

### Execute code from CL
```
./fox ":todo.table('todo.db').cols.px()"
```

### Execute a file from CL
```
./fox example/script.fox
```

### Compile
```
make
make install
make tests
```

### Unit Tests
```
cd tests
fox utests
```

### Bind with PHP. Every function prefixed with :fox_
```
<?=fox_hello()?>
```

### Embeded variables in string
```
"name=$val"
"name=$(val+1)"
```

### eval() most of any code
```
"(1+2).px()".eval()
```

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
RewriteRule ^(.+)$ index.cgi [L,QSA]
<FilesMatch "\.(db|txt|sql|fox)$">
Order allow,deny
Deny from all
</FilesMatch>
```
Create a file called index.fox
```
#include "index.h"
main(){
	req=read_http()
	---
	hello world
	The requested url was: $(req.path.full)
	---.px()
}
```
Compile it
```
fox cgi index.fox index.cgi
```
Will create a file ./index.cgi in the folder.
Check if you can run it from browser.
```
$ curl http://localhost/fox/www/
hello world{"url":"/fox/www/", "path":"/fox/www/", "param":null, "params":{}, "method":"get", "remote":"::1", "paths":["fox", "www"], "host":"localhost", "protocol":"http", "port":"80"}<div class="small lighten pull-right" style="margin-top:4em;">
Run Time: 2ms=0/2/0</br>
Malloc: 132KB. Total: 940KB.</br>
GC: 0runs.

```
This cgi will get dispatched of all calls under this path.
### String operations
```
cat .. cat .. cat
compare === compare
not !== not
merge .= string
```

## Syntax
```
int i=1;
mp=[];

int i=1
map* mp=[]
```

### Function Chaining
```
px(json(str_split(str_trim("habib,ayman"),",")));
"habib,ayman".str_trim().str_split(",").json().px()
"habib,ayman"
	.str_trim()
	.str_split(",")
	.json()
	.px()
```

### Code Blocks
```
if(n<2){
	n*=2;
	return n;
}
if(n<2)
	n*=2
	return n
if n<2
	n*=2
	return n
if n<2 => n*=2; return n
```

### Strings
```
char* a="hello, world"
char* a=:hello\,\ world
char* a="
	hello, world
	"
char* a=---
	hello, world
	---
```
### Maps
```
map* mp={}
map* mp=[]
```

### Vectors
```
map* mp=[]
```

### Populate Map
```
mp={name: :Sanjir}[:age]=21
mp={name: :Sanjir,age: 21}
mp="
	name=Sanjir
	age=#21
	".str_map()
mp="name=Sanjir,age=#21".str_map()
```

### Initialize Vector
```
mp=[:Habib,:Anas,:Ayman]
mp="
	Habib
	Anas
	Ayman
	".str_vec()
mp="Habib,Anas,Ayman".str_vec()
```

### Access by key	
```
map_val(map_val(:address,mp),:city)
mp[:address][:city]
mp.:address.:city
mp.address.city
```

### Iterate
```
for(int i=1;i<=mp->len;i++)
	mp[i-1].px()

mp.each ,,i
	mp[i-1].px()

mp.each v,,i
	v.px()
```

```
mp.each v,,i => => v.px()
mp.each v,,i => v.px()
mp.each ,,i => mp[i-1].px()
mp.each ,k,i => mp[k].px()
mp.each v ,,i=> v.px()
mp.each v ,k,i=> v.px()
```

```
input.str_split(" ").each v,,i
	v.px()
```

### Functions
```
int add(int a,int b){
	return a+b;
}
int add(int a,int b)
	return a+b
int add(int a,int b) => return a+b
```

### Default Parameters
```
int http_out(
	char* body,
	int status=200,
	char* mime="text/html",
	char* template=_globals.default.template
){
	body.px()
}
```

### Invoke Functions
```
int increase(int num,int inc_by=2) => return num * inc_by

increase(7)
increase(7,2)
[7,2].call_func(:increase)
[7].call_func(:increase)
[NULL,:increase,:num,7,:inc_by,2].data_exec()
"
	increase
	num=7
	inc_by=2
	".str_map().data_exec()
```

### Eval
```
"1+1".eval()==2
"1 && 2".eval()==2
"7 || 4".eval()==7
"info.name".eval("
	info
		name=Habib
	".str_map())
```


### Template
```
"
	name=Habib
	age=32
".render("
	Hi #{name}! Your age is #{age}
")

"
	name=Habib
	age=32
".render("
	We know the following information about you!
	--body
	#{_key}: #{_val}
	--foot
	That was all!
")
```

### String Operation	
```
str="hello world"
str=:hello\ world
str===:hello
str.is_word("hello helo hilo")
```

