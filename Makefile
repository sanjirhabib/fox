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
_DEPS=fox.h foxcmd.h http.h regexp.h sql.h
_TLIB=libfoxstatic.a libfoxcmd.a libfoxcore.a
TLIBS=$(patsubst %,$(LDIR)/%,$(_TLIB))
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ=cmd.o core.o dynamic.o fox.o main.o memsize.o sql.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: install clean

#.DEFAULT_GOAL:=bin/fox


$(SDIR)/%.c: ./%.fox
	fox fox_c $< $@

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/fox: $(TLIBS) $(ODIR)/main.o $(DEPS) $(OBJ) $(FOXS)
	fox fox_h sql.fox $(IDIR)/sql.h
	fox fox_h http.fox $(IDIR)/http.h
	fox write_foxh $(IDIR)/fox.h
	fox fox_h fox.fox $(IDIR)/foxcmd.h
	gcc -o $@ $(ODIR)/main.o $(CFLAGS) $(LIBS) -L$(LDIR)

$(LDIR)/libfoxstatic.a: $(ODIR)/dynamic.o $(ODIR)/fox.o $(ODIR)/memsize.o $(ODIR)/sql.o
	rm -f $@
	ar rcs $@ $^

$(LDIR)/libfoxcmd.a: $(ODIR)/cmd.o
	rm -f $@
	ar rcs $@ $^

$(LDIR)/libfoxcore.a: $(ODIR)/core.o
	rm -f $@
	ar rcs $@ $^


$(SDIR)/dynamic.c: $(FOXS)
	fox write_callfunc src/dynamic.c

install:
	cp fox.h $(INSTALL_DIR)/include/fox.h
	cp $(ODIR)/fox $(INSTALL_DIR)/bin/fox
	cp $(LDIR)/*.lib $(INSTALL_DIR)/lib/
	cp $(LDIR)/*.so $(INSTALL_DIR)/lib/

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f $(LDIR)/*.lib
	rm -f $(LDIR)/*.so
	rm -f $(BDIR)/fox
