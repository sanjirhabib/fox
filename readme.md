# Fox Language
Fox language. Transcompiles source into into human readable C. Generated code maintains your original format, comment and indention, lines are exactly the same in number as your original source.

## Features

### Small code base
1K lines for the runtime. The compiler is 4K lines. Creates small 30KB binary for hello world.

### GC Collected
Tracing GC. 50ms max delay in average use cases.

### Inline maps and vectors
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
```
"
Use double quote
"
'
or single quote
'
---
or three dashes
---
".end1
or a signed terminator
.end1"
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

### No semicolons, unless you want to
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
# ./fox sub_str hello 3 -1
```

### Execute code from CL
```
# ./fox ":todo.table('todo.db').cols.px()"
```

### Execute a file from CL
```
# ./fox ./tests.fox
```

### Compile
```
make
make install
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
mp=xmap(:name,:Sanjir)[:age]=int_var(21)
mp=xmap(:name,:Sanjir,:age,int_var(21))
mp="
	name=Sanjir
	age=#21
	".str_map()
mp="name=Sanjir,age=#21".str_map()
```

### Populate Vector
```
mp=xvec(:Habib,:Anas,:Ayman)
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

