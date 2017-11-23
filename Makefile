CC=gcc
INSTALL_DIR?=/usr/local
ODIR=./obj
BDIR=./bin
SDIR=./src
LDIR=./lib
IDIR=./include
CFLAGS=-I$(IDIR) -std=gnu99 -Wno-logical-op-parentheses -Os
FOXS=fox.fox core.fox http.fox cmd.fox main.fox
LIBS=-lm -lfoxcore -lfox -lfoxcmd -lsqlite3
_DEPS=fox.h foxcmd.h http.h regexp.h sql.h
_TLIB=libfox.a libfoxcmd.a libfoxcore.a
TLIBS=$(patsubst %,$(LDIR)/%,$(_TLIB))
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ=cmd.o core.o dynamic.o fox.o http.o main.o memsize.o regexp.o sql.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: install clean

.DEFAULT_GOAL:=bin/fox

$(IDIR)/sql.h: sql.fox
	fox fox_h $< $@

$(IDIR)/http.h: http.fox
	fox fox_h $< $@

$(IDIR)/fox.h: core.fox
	fox fox_h $< $@ 1

$(IDIR)/foxcmd.h: fox.fox
	fox fox_h $< $@

$(SDIR)/%.c: ./%.fox
	fox fox_c $< $@

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(LDIR)/libfox.a: $(ODIR)/dynamic.o $(ODIR)/fox.o $(ODIR)/memsize.o $(ODIR)/sql.o
	rm -f $@
	ar rcs $@ $^

$(LDIR)/libfoxcmd.a: $(ODIR)/cmd.o
	rm -f $@
	ar rcs $@ $^

$(LDIR)/libfoxcore.a: $(ODIR)/core.o
	rm -f $@
	ar rcs $@ $^

$(BDIR)/fox: $(TLIBS) $(ODIR)/main.o $(DEPS)
	gcc -o $@ $(ODIR)/main.o $(CFLAGS) $(LIBS) -L$(LDIR)


$(SDIR)/dynamic.c: $(FOXS)
	fox write_callfunc src/dynamic.c

install:
	cp fox.h $(INSTALL_DIR)/include/fox.h
	cp $(ODIR)/fox $(INSTALL_DIR)/bin/fox
	cp $(LDIR)/*.lib $(INSTALL_DIR)/lib/

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f $(LDIR)/*.lib
	rm -f $(BDIR)/fox
