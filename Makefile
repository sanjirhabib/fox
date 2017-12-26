CC=gcc
INSTALL_DIR?=/usr/local
CFLAGS=-m64 -Iinclude -std=gnu99 -Wno-logical-op-parentheses -Os -Wno-int-conversion -Llib -L/usr/local/lib -L/usr/lib64/ -fPIC -Wno-unused-command-line-argument -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lm -g
FOXS=fox.fox core.fox cgi.fox cmd.fox main.fox astrostr.fox maincgi.fox generator.fox eval.fox sql.fox text.fox
LIBS=-lsqlite3 -lcrypto -lmarkdown -lcurl -lstemmer
_XLIBS=libfoxstatic.a libfox.so libfoxcgi.so libfoxcgistatic.a libfoxcmdstatic.a libfoxastro.a libfoxmain.a libfoxmaincgi.a
XLIBS=$(patsubst %,lib/%,$(_XLIBS))
_OBJ=core.o meta.o fox.o memsize.o astro.o astrostr.o md5.o generator.o eval.o sql.o text.o
OBJ = $(patsubst %,obj/%,$(_OBJ))
CFILES = $(patsubst %.fox,src/%.c,$(FOXS))

.DEFAULT_GOAL := bin/fox

.PHONY: install clean php tests www

obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)


fox: $(FOXS)
	$(CC) -c -o obj/fox.o src/fox.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/cmd.o src/cmd.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/core.o src/core.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/sql.o src/sql.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/text.o src/text.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/cgi.o src/cgi.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/main.o src/main.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/run.o src/run.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/meta.o src/meta.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/memsize.o memsize.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/md5.o md5.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/astrostr.o src/astrostr.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/main.o src/main.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/maincgi.o src/maincgi.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/run.o src/run.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/generator.o src/generator.c $(CFLAGS) $(LIBS)
	$(CC) -c -o obj/eval.o src/eval.c $(CFLAGS) $(LIBS)
	$(CC) -c astro/astro.c -o obj/astro.o $(CFLAGS)
	rm -f lib/libfoxcmdstatic.a
	ar rcs lib/libfoxcmdstatic.a obj/cmd.o
	rm -f lib/libfoxmain.a
	ar rcs lib/libfoxmain.a obj/main.o
	rm -f lib/libfoxstatic.a
	ar rcs lib/libfoxstatic.a $(OBJ)
	$(CC) -o fox obj/run.o $(CFLAGS) $(LIBS) -lfoxstatic -lfoxcmdstatic -lfoxastro -lfoxmain

temp: obj/main.o $(OBJ) $(FOXS) $(XLIBS)
	$(CC) -o $@ obj/main.o $(CFLAGS) $(LIBS) -lfoxstatic -lfoxcmdstatic

bin/fox: obj/run.o $(OBJ) $(FOXS) $(XLIBS) Makefile
	$(CC) -o $@ obj/run.o $(CFLAGS) $(LIBS) -lfoxstatic -lfoxcmdstatic -lfoxastro -lfoxmain
	bin/fox utests

$(CFILES) src/meta.c: $(FOXS)
	echo $(CFILES)
	fox write_source

lib/libfoxcgi.so: $(OBJ) obj/cgi.o
	rm -f $@
	$(CC) -shared -o $@ $^ $(LIBS) $(CFLAGS)

lib/libfox.so: $(OBJ) obj/cmd.o
	rm -f $@
	$(CC) -shared -o $@ $^ $(LIBS) $(CFLAGS)

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
	touch *.fox && make

