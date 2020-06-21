# merkmap

Produce a Merklemap of a file to add in finding transfer errors.

## Usage

`merkmap INFILE OUTFILE`.

## Building

Needs openssl libcrypto

`gcc -O2 -o bin/merkmap src/merkmap.c -lm -lcrypto`
