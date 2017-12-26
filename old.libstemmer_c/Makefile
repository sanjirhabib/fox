include mkinc.mak
CFLAGS=-Iinclude
INSTALL_DIR?=/usr/local
all: libstemmer.o stemwords
libstemmer.o: $(snowball_sources:.c=.o)
	$(AR) -cru $@ $^
stemwords: examples/stemwords.o libstemmer.o
	$(CC) -o $@ $^
clean:
	rm -f stemwords *.o src_c/*.o runtime/*.o libstemmer/*.o
install:
	cp libstemmer.o $(INSTALL_DIR)/lib/libstemmer.a
	cp include/libstemmer.h $(INSTALL_DIR)/include/libstemmer.h
