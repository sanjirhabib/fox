params=`pkg-config --cflags --libs MagickWand`
default: xml
xml:
	fox cc draw --opts="$(params)"
all:
	cd .. && make install
	../bin/fox cc tree.fox tree debug
	./tree
fox:
	cd .. && make install && cd -
