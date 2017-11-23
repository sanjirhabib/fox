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
	gcc -shared -o $(LDIR)/libfox.so $(OBJ) -lsqlite3
	cp $(LDIR)/libfox.so $(INSTALL_DIR)/lib/libfox.so
	gcc -o $@ $(ODIR)/main.o $(CFLAGS) $(LIBS)

$(LDIR)/libfoxstatic.a: $(ODIR)/dynamic.o $(ODIR)/fox.o $(ODIR)/memsize.o $(ODIR)/sql.o $(ODIR)/cmd.o $(ODIR)/core.o
	rm -f $@
	ar rcs $@ $^
#
#$(LDIR)/libfoxcmd.a: $(ODIR)/cmd.o
#	rm -f $@
#	ar rcs $@ $^
#
#$(LDIR)/libfoxcore.a: $(ODIR)/core.o
#	rm -f $@
#	ar rcs $@ $^
#

$(IDIR)/fox.h: $(FOXS)
	fox write_foxh $(IDIR)/fox.h

$(SDIR)/dynamic.c: $(FOXS) $(IDIR)/fox.h
	fox write_dynamic src/dynamic.c

install: $(BDIR)/fox $(LDIR)/libfoxstatic.a
	cp $(IDIR)/fox.h $(INSTALL_DIR)/include/fox.h
	cp $(ODIR)/fox $(INSTALL_DIR)/bin/fox
	cp $(LDIR)/*.lib $(INSTALL_DIR)/lib/
	cp $(LDIR)/*.so $(INSTALL_DIR)/lib/

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f $(LDIR)/*.lib
	rm -f $(LDIR)/*.so
	rm -f $(BDIR)/fox
