CC=gcc

merkmap:
	$(CC) -O2 -Wall -o bin/merkmap src/merkmap.c -lcrypto

clean:
	rm bin/merkmap*
install:
	cp bin/merkmap /usr/local/bin/merkmap

uninstall:
	rm /usr/local/bin/merkmap
