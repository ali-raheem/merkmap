# merkmap

Produce a Merklemap of a file to add in finding transfer errors.

This means you can identify a error a transferred file down to a chunk by comparing hashs rather than chunks of data.

Can accurately identify good data up the chunk of the error, if it's a substitution (not deletion/insertion) can identify good data beyond it.

## Usage

`merkmap INFILE OUTFILE`.

## Building

Need openssl/libcrypto

Use make `make`

Or directly

`gcc -O2 -o bin/merkmap src/merkmap.c -lm -lcrypto`

## Todo

* Reverse structure of merklemap (so head is first).
* Diff tool which tells you last differing chunk_hash.
* merklemap header ?
  - merklemap magic & version
  - filename hash
  - filesize
  - chunk size
  - hashing algorithm