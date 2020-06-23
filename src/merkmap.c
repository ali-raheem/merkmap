#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>

#define CHUNK_SIZE (1*1024*1024)
#define VERSION "0.1.0"
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

  size_t numChunks = (float) ceil((double) inFileSize / (double) CHUNK_SIZE);
  size_t numHashs = (2 * numChunks) - 1;

#ifdef __DEBUG
  printf("numChunks: %f\nnumChunksPow2: %lu\nmerkmapSize: %lu\n\n", numChunks, numChunksPow2, merkmapSize);
#endif
  merkmapTree = (char *) calloc(numHashs, SHA256_DIGEST_LENGTH);
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

  size_t hashsDone = numChunks;
  size_t hashsPerTier = numChunks;
  size_t i = 0;
  while (hashsDone < numHashs) {
    for(; i < (hashsDone + hashsPerTier)/2; i++) {
      printf("i: %ul, hashsDone: %ul, hashesPerTier: %ul\n", i, hashsDone, hashsPerTier);
      printf("data: %ul, dest: %ul\n", 2 * i, hashsPerTier - i);
      unsigned char* data = merkmapTree + 2 * i * SHA256_DIGEST_LENGTH;
      unsigned char* dest = data + (hashsPerTier - (i - hashsDone/2)) * SHA256_DIGEST_LENGTH;
      SHA256(data, 2 * SHA256_DIGEST_LENGTH, dest);
    }
    hashsDone += hashsPerTier;
    hashsPerTier /= 2;
  }

#ifdef __DEBUG
  puts("Merkle tree created.");
#endif

  filename = argv[2];
  fp = fopen(filename, "wb");
  fwrite(merkmapTree, SHA256_DIGEST_LENGTH, numHashs, fp);
  fclose(fp);
  free(merkmapTree);
  exit(EXIT_SUCCESS);
}
