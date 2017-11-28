CC=gcc
INSTALL_DIR?=/usr/local
CFLAGS=-Iinclude -std=gnu99 -Wno-logical-op-parentheses -Os -Wno-int-conversion
FOXS=fox.fox core.fox sql.fox cgi.fox cmd.fox main.fox
LIBS=-lm -lfox -lsqlite3
HEADERS=fox.h sql.h
DEPS=$(patsubst %,include/%,$(HEADERS))
_OBJ=core.o dynamic.o fox.o memsize.o sql.o
OBJ = $(patsubst %,obj/%,$(_OBJ))

.PHONY: install clean php tests www

src/%.c: ./%.fox
	fox fox_c $< $@

obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

bin/fox: obj/main.o $(DEPS) $(OBJ) $(FOXS) lib/libfoxstatic.a
	gcc -shared -o lib/libfox.so $(OBJ) obj/cmd.o -lsqlite3
	gcc -shared -o lib/libfoxcgi.so $(OBJ) obj/cgi.o -lsqlite3
	gcc -o $@ obj/main.o $(CFLAGS) -Llib -lm -lsqlite3 -lfoxstatic
	cd tests && ../bin/fox utests

lib/libfoxstatic.a: obj/dynamic.o obj/fox.o obj/memsize.o obj/sql.o obj/cmd.o obj/core.o
	rm -f $@
	ar rcs $@ $^

include/fox.h: $(FOXS)
	fox write_foxh include/fox.h

src/dynamic.c: $(FOXS) include/fox.h
	fox write_dynamic src/dynamic.c

install: bin/fox lib/libfoxstatic.a
	cd tests && ../bin/fox utests
	cp include/fox.h $(INSTALL_DIR)/include/fox.h
	cp bin/fox $(INSTALL_DIR)/bin/fox
	cp lib/libfoxstatic.a $(INSTALL_DIR)/lib/
	cp lib/libfox.so $(INSTALL_DIR)/lib/
	cp lib/libfoxcgi.so $(INSTALL_DIR)/lib/

php: install 
	fox fox_phpc habib.fox php/habibphp.c
	fox fox_c habib.fox php/habib.c
	fox fox_h habib.fox php/habib.h
	cd php && make && make install
	php habib.php

clean:
	rm -f obj/*.o lib/*.a lib/*.so bin/fox

tests: bin/fox
	cd tests && ../bin/fox utests
www:
	cd www && make
