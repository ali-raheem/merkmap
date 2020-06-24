# merkmap

Produce a [Merklemap](https://en.wikipedia.org/wiki/Merkle_tree) of a file to add in finding transfer errors.

This means you can identify a error a transferred file down to a chunk by comparing hashs rather than chunks of data.

Can accurately identify good data up the chunk of the error, if it's a substitution (not deletion/insertion) can identify good data beyond it.

The point is if you want to check a transfer over a slow data link, you can run merkmap at both ends. For example on a 708MB CD iso, the full merkmap is 64KB and by transferring that 64KB you could identify the 1MB chunk with an error. You'd rarely even need the entire map.

![Merkle map - David Göthbergvia & Azaghal wikipedia CC1](docs/Hash_Tree.svg)

The first hash would tell you if both files are the same, the second set will tell you if there is an error in the first or second half of the file.

## Usage

`merkmap INFILE OUTFILE`.

If `OUTFILE` is `-` the map will be outputted to stdout.

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
