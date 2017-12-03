index: main.fox
	cd .. && make install
	fox cgi --opts="-I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lcrypto"
	index.cgi login/
debug:
	fox cgi --keepfiles=1
