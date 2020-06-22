#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>

#define CHUNK_SIZE (1*1024*1024)
#define VERSION "0.0.3"
#define AUTHOR "Ali Raheem"
#define URL "https://github.com/ali-raheem/merkmap"

void usage(char *name) {
  printf("merkmap\t\t %sv\n%s\t %s\n", VERSION, AUTHOR, URL);
  printf("Usage:\t%s INPUT_FILE OUT_FILE\n\n", name);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
  FILE* fp;
  char *buffer;
  char *filename;
  size_t inFileSize;
  size_t merkmapSize;
  char * merkmapTree;
  struct stat fp_stat;
  
  if(argc != 3) {
    usage(argv[0]);
  }

  filename = argv[1];
  stat(filename, &fp_stat);
  inFileSize = fp_stat.st_size;
  fp = fopen(filename, "rb");
  assert(fp != NULL);
  
  buffer = (char *) malloc(CHUNK_SIZE);
  assert(buffer != NULL);

  float numChunks = (float) ceil((double) inFileSize / (double) CHUNK_SIZE);
  size_t numChunksPow2 =  (size_t) powf(2, ceil(log(numChunks) / log(2.0)));
  merkmapSize = (size_t) powf(2, ceil(log(numChunks) / log(2.0)) + 1) - 1;
#ifdef __DEBUG
  printf("numChunks: %f\nnumChunksPow2: %lu\nmerkmapSize: %lu\n\n", numChunks, numChunksPow2, merkmapSize);
#endif
  merkmapTree = (char *) calloc(merkmapSize, SHA256_DIGEST_LENGTH);
  assert(merkmapTree != NULL);

  char* treeptr = merkmapTree;
  while (fread(buffer, CHUNK_SIZE, 1, fp)) {
    SHA256(buffer, CHUNK_SIZE, treeptr);
    treeptr += SHA256_DIGEST_LENGTH;
    memset(buffer, 0, CHUNK_SIZE);
  }

  free(buffer);
  buffer = NULL;
  fclose(fp);
  
#ifdef __DEBUG
  puts("Chunks hashed.");
#endif
  
  char* hashptr = merkmapTree;
  treeptr = merkmapTree + (numChunksPow2 - 1) * SHA256_DIGEST_LENGTH;
  char* endLastRow = treeptr;
  char* endTree = merkmapTree + (merkmapSize - 1) * SHA256_DIGEST_LENGTH;
  while (hashptr < endTree) {
    while (hashptr < endLastRow) {
      SHA256(hashptr, SHA256_DIGEST_LENGTH * 2, treeptr);
      hashptr += SHA256_DIGEST_LENGTH * 2;
      treeptr += SHA256_DIGEST_LENGTH;
    }
    hashptr = endLastRow;
    endLastRow = treeptr;
  }

#ifdef __DEBUG
  puts("Merkle tree created.");
#endif

  filename = argv[2];
  fp = fopen(filename, "wb");
  fwrite(merkmapTree, SHA256_DIGEST_LENGTH, merkmapSize, fp);
  fclose(fp);
  free(merkmapTree);
  exit(EXIT_SUCCESS);
}
