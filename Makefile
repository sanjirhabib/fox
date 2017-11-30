CC=gcc
INSTALL_DIR?=/usr/local
CFLAGS=-Iinclude -std=gnu99 -Wno-logical-op-parentheses -Os -Wno-int-conversion
FOXS=fox.fox core.fox sql.fox cgi.fox cmd.fox main.fox
LIBS=-lsqlite3
HEADERS=fox.h sql.h
_XLIBS=libfoxstatic.a libfox.so libfoxcgi.so libfoxcgistatic.a libfoxcmdstatic.a
XLIBS=$(patsubst %,lib/%,$(_XLIBS))
DEPS=$(patsubst %,include/%,$(HEADERS))
_OBJ=core.o meta.o fox.o memsize.o sql.o
OBJ = $(patsubst %,obj/%,$(_OBJ))
_OCMD=cmd.o
OCMD = $(patsubst %,obj/%,$(_OCMD))

.PHONY: install clean php tests www

src/%.c: ./%.fox
	fox fox_c $< $@

obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

bin/fox: obj/main.o $(DEPS) $(OBJ) $(FOXS) $(XLIBS)
	gcc -o $@ obj/main.o $(CFLAGS) -Llib -lm $(LIBS) -lfoxstatic -lfoxcmdstatic
	cd tests && ../bin/fox utests

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

lib/libfoxstatic.a: $(OBJ)
	rm -f $@
	ar rcs $@ $^

include/fox.h: $(FOXS)
	fox write_foxh include/fox.h

src/meta.c: $(FOXS) include/fox.h
	fox write_meta src/meta.c

install: bin/fox lib/libfoxstatic.a lib/libfoxcgistatic.a
	cd tests && ../bin/fox utests
	cp include/fox.h $(INSTALL_DIR)/include/fox.h
	cp bin/fox $(INSTALL_DIR)/bin/fox
	cp lib/libfoxstatic.a $(INSTALL_DIR)/lib/
	cp lib/libfoxcmdstatic.a $(INSTALL_DIR)/lib/
	cp lib/libfoxcgistatic.a $(INSTALL_DIR)/lib/
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
