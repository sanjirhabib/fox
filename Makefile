CC=gcc
INSTALL_DIR?=/usr/local
CFLAGS=-m64 -Iinclude -std=gnu99 -Wno-logical-op-parentheses -Os -Wno-int-conversion -Llib -L/usr/local/lib -L/usr/lib64/ -fPIC -Wno-unused-command-line-argument -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lm -g -I/usr/local/include -I/usr/local/include/libxml2
FOXFILES=$(wildcard src/*.fox)
FOXS=$(notdir $(FOXFILES))
CSRC=$(notdir $(wildcard src/*.c))
CFOX = $(FOXS:.fox=.c)
CEXTRA = $(filter-out $(CFOX), $(CSRC))
CFILES = $(CEXTRA) $(CFOX)
LIBS=-lsqlite3 -lcrypto -lmarkdown -lcurl -lstemmer -lfoxastro -lxml2
XLIB=libfoxstatic.a libfox.so libfoxcgi.so libfoxcgistatic.a libfoxcmdstatic.a libfoxmain.a libfoxmaincgi.a
XLIBS=$(patsubst %,lib/%,$(XLIB))
FILTER=cgi.o cmd.o main.o maincgi.o run.o
OBJS=$(filter-out $(FILTER),$(notdir $(CFILES:.c=.o)))
OBJ=$(patsubst %,obj/%,$(OBJS))
OBJC=$(patsubst %,src/%,$(OBJS:.o=.c))



obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

default: bin/fox

habib:
	echo $(CEXTRA)
fox: $(FOXFILES)
	$(CC) -c $(wildcard src/*.c) $(CFLAGS) $(LIBS)
	mv *.o obj/
	rm -f lib/libfoxcmdstatic.a
	ar rcs lib/libfoxcmdstatic.a obj/cmd.o
	rm -f lib/libfoxmain.a
	ar rcs lib/libfoxmain.a obj/main.o
	rm -f lib/libfoxstatic.a
	ar rcs lib/libfoxstatic.a $(OBJ)
	$(CC) -o fox obj/run.o $(CFLAGS) -lfoxstatic -lfoxcmdstatic -lfoxastro -lfoxmain $(LIBS)

temp: obj/main.o $(OBJ) $(FOXFILES) $(XLIBS)
	$(CC) -o $@ obj/main.o $(CFLAGS) -lfoxstatic -lfoxcmdstatic $(LIBS)

bin/fox: obj/run.o $(OBJ) $(FOXFILES) $(XLIBS) Makefile
	$(CC) -o $@ obj/run.o $(CFLAGS) -lfoxstatic -lfoxcmdstatic -lfoxastro -lfoxmain $(LIBS)
	bin/fox utests

$(CFILES) src/meta.c: $(FOXFILES)
	echo $(CFILES)
	fox write_source $(FOXFILES) "astro/astro.h"

lib/libfoxcgi.so: $(OBJ) obj/cgi.o
	rm -f $@
	$(CC) -shared -o $@ $^ $(CFLAGS) $(LIBS)

lib/libfox.so: $(OBJ) obj/cmd.o
	rm -f $@
	$(CC) -shared -o $@ $^ $(CFLAGS) $(LIBS)

lib/libfoxcgistatic.a: obj/cgi.o
	rm -f $@
	ar rcs $@ $^

lib/libfoxmaincgi.a: obj/maincgi.o
	rm -f $@
	ar rcs $@ $^

lib/libfoxmain.a: obj/main.o
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

obj/astro.o: astro/astro.c
	cd astro && make

install: bin/fox $(XLIBS)
	bin/fox utests
	cp include/fox.h $(INSTALL_DIR)/include/fox.h
	cp bin/fox $(INSTALL_DIR)/bin/fox
	cp lib/*.so $(INSTALL_DIR)/lib/
	sudo cp lib/*.a $(INSTALL_DIR)/lib/

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
safe: bin/fox
	cp fox /tmp/fox.`now`
	cp bin/fox .
	touch src/*.fox && make

.PHONY: install clean php tests www
