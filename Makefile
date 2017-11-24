CC=gcc
INSTALL_DIR?=/usr/local
ODIR=./obj
BDIR=./bin
SDIR=./src
LDIR=./lib
IDIR=./include
CFLAGS=-I$(IDIR) -std=gnu99 -Wno-logical-op-parentheses -Os
FOXS=fox.fox core.fox http.fox cmd.fox main.fox
LIBS=-lm -lfox -lsqlite3
_DEPS=fox.h http.h regexp.h sql.h
_TLIB=libfoxstatic.a
TLIBS=$(patsubst %,$(LDIR)/%,$(_TLIB))
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ=cmd.o core.o dynamic.o fox.o main.o memsize.o sql.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: install clean php

#.DEFAULT_GOAL:=bin/fox


$(SDIR)/%.c: ./%.fox
	fox fox_c $< $@

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/fox: $(TLIBS) $(ODIR)/main.o $(DEPS) $(OBJ) $(FOXS) $(LDIR)/libfoxstatic.a
	gcc -shared -o $(LDIR)/libfox.so $(OBJ) -lsqlite3
	gcc -o $@ $(ODIR)/main.o $(CFLAGS) -Llib -lm -lsqlite3 -lfoxstatic

$(LDIR)/libfoxstatic.a: $(ODIR)/dynamic.o $(ODIR)/fox.o $(ODIR)/memsize.o $(ODIR)/sql.o $(ODIR)/cmd.o $(ODIR)/core.o
	rm -f $@
	ar rcs $@ $^

$(IDIR)/fox.h: $(FOXS)
	fox write_foxh $(IDIR)/fox.h

$(SDIR)/dynamic.c: $(FOXS) $(IDIR)/fox.h
	fox write_dynamic src/dynamic.c

install: $(BDIR)/fox $(LDIR)/libfoxstatic.a
	cp $(IDIR)/fox.h $(INSTALL_DIR)/include/fox.h
	cp $(BDIR)/fox $(INSTALL_DIR)/bin/fox
	cp $(LDIR)/libfoxstatic.a $(INSTALL_DIR)/lib/
	cp $(LDIR)/libfox.so $(INSTALL_DIR)/lib/

php: habib.fox
	fox fox_phpc habib.fox php/habibphp.c
	fox fox_c habib.fox php/habib.c
	fox fox_h habib.fox php/habib.h
	cd php && make && make install
	php habib.php

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f $(LDIR)/*.lib
	rm -f $(LDIR)/*.so
	rm -f $(BDIR)/fox
