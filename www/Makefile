index: main.fox
	cd .. && make install
	fox cgi
	printf "GET /hello/world/?hi=there&i=am HTTP/1.1\nhost: localhost\n\n" | index.cgi
debug:
	fox cgi --keepfiles=1
