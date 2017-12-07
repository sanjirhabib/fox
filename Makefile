CC=gcc
INSTALL_DIR?=/usr/local
CFLAGS=-Iinclude -std=gnu99 -Wno-logical-op-parentheses -Os -Wno-int-conversion -L/usr/lib64/ -fPIC -Wno-unused-command-line-argument
FOXS=fox.fox core.fox sql.fox cgi.fox cmd.fox main.fox
LIBS=-lsqlite3 -I/usr/local/opt/openssl/include -L/usr/local/lib -L/usr/local/opt/openssl/lib -lcrypto -lmarkdown -lcurl
HEADERS=fox.h sql.h
_XLIBS=libfoxstatic.a libfox.so libfoxcgi.so libfoxcgistatic.a libfoxcmdstatic.a libfoxastro.a
XLIBS=$(patsubst %,lib/%,$(_XLIBS))
DEPS=$(patsubst %,include/%,$(HEADERS))
_OBJ=core.o meta.o fox.o memsize.o sql.o astro.o
OBJ = $(patsubst %,obj/%,$(_OBJ))
_OCMD=cmd.o
OCMD = $(patsubst %,obj/%,$(_OCMD))

.DEFAULT_GOAL := bin/fox

.PHONY: install clean php tests www

obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

safe:
	$(CC) -c -o obj/fox.o src/fox.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/cmd.o src/cmd.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/sql.o src/sql.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/core.o src/core.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/cgi.o src/cgi.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/main.o src/main.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/meta.o src/meta.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/memsize.o src/memsize.c $(CFLAGS) $(LIBS)
	rm -f lib/libfoxcmdstatic.a
	ar rcs lib/libfoxcmdstatic.a obj/cmd.o
	rm -f lib/libfoxstatic.a
	ar rcs lib/libfoxstatic.a $(OBJ)
	gcc -o bin/fox obj/main.o $(CFLAGS) -Llib -lm $(LIBS) -lfoxstatic -lfoxcmdstatic

temp: obj/main.o $(DEPS) $(OBJ) $(FOXS) $(XLIBS)
	gcc -o $@ obj/main.o $(CFLAGS) -Llib -lm $(LIBS) -lfoxstatic -lfoxcmdstatic

bin/fox: obj/main.o $(DEPS) $(OBJ) $(FOXS) $(XLIBS)
	gcc -o $@ obj/main.o $(CFLAGS) -Llib -lm $(LIBS) -lfoxstatic -lfoxcmdstatic -lfoxastro
	bin/fox utests

src/core.c src/fox.c src/sql.c src/cgi.c src/cmd.c: $(FOXS)
	fox write_source


lib/libfoxcgi.so: $(OBJ) obj/cgi.o
	rm -f $@
	gcc -shared -o $@ $^ $(LIBS)

lib/libfox.so: $(OBJ) obj/cmd.o
	rm -f $@
	gcc -shared -o $@ $^ $(LIBS)

lib/libfoxcgistatic.a: obj/cgi.o
	rm -f $@
	ar rcs $@ $^

lib/libfoxcmdstatic.a: obj/cmd.o
	rm -f $@
	ar rcs $@ $^

lib/libfoxastro.a: obj/astro.o
	rm -f $@
	ar rcs $@ $^
lib/libfoxstatic.a: $(OBJ)
	rm -f $@
	ar rcs $@ $^

src/meta.c: $(FOXS) include/fox.h
	fox write_meta src/meta.c

obj/astro.o: astro/astro.c
	cd astro && make

install: bin/fox lib/libfoxstatic.a lib/libfoxcgistatic.a lib/libfoxastro.a
	bin/fox utests
	cp include/fox.h $(INSTALL_DIR)/include/fox.h
	cp bin/fox $(INSTALL_DIR)/bin/fox
	cp lib/*.a $(INSTALL_DIR)/lib/
	cp lib/*.so $(INSTALL_DIR)/lib/

php: install 
	fox fox_phpc habib.fox php/habibphp.c
	fox fox_c habib.fox php/habib.c
	fox fox_h habib.fox php/habib.h
	cd php && make && make install
	php habib.php

clean:
	rm -f obj/*.o lib/*.a lib/*.so bin/fox

tests: bin/fox
	bin/fox utests
www:
	cd www && make
