CC=gcc
INSTALL_DIR?=/usr/local
IMAGIC=$(shell pkg-config --cflags --libs MagickWand)
#CFLAGS=-m64 -Iinclude -std=gnu99 -g -Wno-int-conversion -Llib -L/usr/local/lib -L/usr/lib64/ -fPIC -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lm -g -I/usr/local/include -I/usr/local/include/libxml2 $(IMAGIC) -I/usr/include/libxml2 -Iastro -Wno-pointer-sign -Werror=implicit-function-declaration
#CFLAGS=-m64 -Iinclude -std=gnu99 -O2 -DNDEBUG -Wno-int-conversion -Llib -L/usr/local/lib -L/usr/lib64/ -fPIC -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lm -g -I/usr/local/include -I/usr/local/include/libxml2 $(IMAGIC) -I/usr/include/libxml2 -Iastro -Wno-pointer-sign -Werror=implicit-function-declaration
CFLAGS=-m64 -Iinclude -std=gnu99 -O2 -DNDEBUG -Wno-int-conversion -Llib -L/usr/local/lib -L/usr/lib64/ -fPIC -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lm -g -I/usr/local/include -I/usr/local/include/libxml2 $(IMAGIC) -I/usr/include/libxml2 -Iastro -Wno-pointer-sign -Werror=implicit-function-declaration
FOXFILES=$(wildcard src/*.fox)
FOXS=$(notdir $(FOXFILES))
CFOX = $(FOXS:.fox=.c) meta.c
LIBS=-lsqlite3 -lcrypto -lmarkdown -lcurl -lstemmer -lastro -lxml2 -lfoxdeps
XLIB=libfoxstatic.a libfox.so
XLIBS=$(patsubst %,lib/%,$(XLIB))
FILTER=main.o meta.o
OBJS=$(filter-out $(FILTER),$(notdir $(CFOX:.c=.o)))
OBJ=$(patsubst %,obj/%,$(OBJS))
OBJC=$(patsubst %,src/%,$(OBJS:.o=.c))
HEADERS=$(patsubst %,include/%,$(FOXS:.fox=.h))
FOXLIBS=$(patsubst %,lib/libfox%,$(OBJS:.o=static.a))
#FOXLIBS=lib/libfoxstatic.a

obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

default: bin/fox

habib:
	echo $(FOXLIBS)

fox: $(FOXFILES)
	$(CC) -c $(wildcard src/*.c) $(CFLAGS) $(LIBS)
	mv *.o obj/
	rm -f lib/libfoxstatic.a
	ar rcs lib/libfoxstatic.a $(OBJ)
	$(CC) -o fox obj/main.o obj/meta.o $(CFLAGS) -lfoxstatic -lastro $(LIBS)

temp: obj/main.o $(OBJ) $(FOXFILES) $(XLIBS)
	$(CC) -o $@ obj/main.o $(CFLAGS) -lfoxstatic $(LIBS)

bin/fox: $(HEADERS) obj/main.o obj/meta.o $(OBJ) $(FOXLIBS) $(FOXFILES) $(XLIBS) Makefile
	$(CC) -o $@ obj/main.o obj/meta.o $(CFLAGS) -lfoxstatic -lastro $(LIBS)

lib/libfox%static.a: obj/%.o
	echo ar rcs $@ $<
	ar rcs $@ $<

$(CFOX) src/meta.c $(HEADERS) : $(FOXFILES)
	echo $(CFOX)
	fox write_source $(FOXFILES) "astro/astro.h"
depsx:
	echo deps/fox write_source $(FOXFILES) "astro/astro.h"
	deps/fox write_source $(FOXFILES) "astro/astro.h"
new:
	echo bin/fox write_source $(FOXFILES) "astro/astro.h"
	bin/fox write_source $(FOXFILES) "astro/astro.h"
perf:
	echo perf record -g bin/fox write_source $(FOXFILES) "astro/astro.h"
	perf record -g bin/fox write_source $(FOXFILES) "astro/astro.h"
	perf report --no-children

lib/libastro.a: obj/astro.o
	rm -f $@
	ar rcs $@ $^

lib/libfoxstatic.a: $(OBJ)
	rm -f $@
	ar rcs $@ $^

obj/astro.o: astro/astro.c
	cd astro && make

install: bin/fox $(XLIBS)
	bin/fox utests
	sudo cp include/*.h $(INSTALL_DIR)/include/
	sudo cp bin/fox $(INSTALL_DIR)/bin/fox
	sudo cp lib/*.so $(INSTALL_DIR)/lib/
	sudo cp lib/*.a $(INSTALL_DIR)/lib/

php: install 
	fox fox_phpc src/astrostr.fox php/astrostrphp.c
	cp src/astrostr.c php/
	cp src/astrostr.h php/
	cd php && make && make install
	php habib.php

clean:
	rm -f obj/*.o lib/*.a lib/*.so bin/fox include/*.h src/meta.c

lib/libfox.so: $(OBJ)
	rm -f $@
	$(CC) -shared -o $@ $^ $(CFLAGS) $(LIBS)

tests: bin/fox
	bin/fox utests
www:
	cd www && make
safe: bin/fox
	cp fox /tmp/fox.`now`
	cp bin/fox .
	touch src/*.fox && make
again:
	cp bin/fox .
	make clean
	make
debug:
	gdb -ex run --args bin/fox gc_check
gc:
	make
	bin/fox gc_check
gc1:
	ln -fs gc1.fox.src src/gc.fox
gc2:
	ln -fs gc2.fox.src src/gc.fox
gc3:
	ln -fs gc3.fox.src src/gc.fox
gc4:
	ln -fs gc4.fox.src src/gc.fox
git:
	cp src/* /web/github/habibur/fox/src/
	cp include/* /web/github/habibur/fox/include/
	cp ext/* /web/github/habibur/fox/ext/
	cp Makefile /web/github/habibur/fox/

.PHONY: install clean php tests www habib again
