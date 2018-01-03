default: xml
xml:
	fox cc xpath --opts="-I/usr/local/include/libxml2 -lxml2"
all:
	cd .. && make install
	../bin/fox cc tree.fox tree debug
	./tree
fox:
	cd .. && make install && cd -
