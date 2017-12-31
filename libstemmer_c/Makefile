include mkinc.mak
CFLAGS=-Iinclude -fPIC -m64 -g
INSTALL_DIR?=/usr/local
all: libstemmer.o stemwords libstemmer.a
libstemmer.o: $(snowball_sources:.c=.o)
	$(AR) -cru $@ $^
libstemmer.a: $(snowball_sources:.c=.o)
	$(AR) -crs $@ $^
stemwords: examples/stemwords.o libstemmer.o
	$(CC) -o $@ $^
clean:
	rm -f stemwords *.o src_c/*.o runtime/*.o libstemmer/*.o examples/stemwords.o
install: all
	cp include/libstemmer.h $(INSTALL_DIR)/include/libstemmer.h
	cp libstemmer.a $(INSTALL_DIR)/lib/libstemmer.a
