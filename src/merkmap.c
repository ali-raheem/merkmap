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
  unsigned char *buffer;
  char *filename;
  size_t inFileSize;
  unsigned char * merkmapTree;
  struct stat fp_stat;
  
  if(argc != 3) {
    usage(argv[0]);
  }

  filename = argv[1];
  stat(filename, &fp_stat);
  inFileSize = fp_stat.st_size;
  fp = fopen(filename, "rb");
  assert(fp != NULL);
  
  buffer = (unsigned char *) malloc(CHUNK_SIZE);
  assert(buffer != NULL);

  size_t numChunks = (float) ceil((double) inFileSize / (double) CHUNK_SIZE);
  size_t numBase = powf(2, ceil(log(numChunks) / log(2)));
  size_t numHashs = (2 * numBase) - 1;

#ifdef __DEBUG
  printf("numChunks: %f\nnumChunksPow2: %lu\nmerkmapSize: %lu\n\n", numChunks, numChunksPow2, merkmapSize);
#endif
  merkmapTree = (unsigned char *) calloc(numHashs, SHA256_DIGEST_LENGTH);
  assert(merkmapTree != NULL);

  unsigned char* treeptr = merkmapTree;
  size_t i = 0;
  while (fread(buffer, CHUNK_SIZE, 1, fp)) {
    SHA256(buffer, CHUNK_SIZE, treeptr);
    treeptr += SHA256_DIGEST_LENGTH;
    memset(buffer, 0, CHUNK_SIZE);
    i++;
  }

  while (i <= numBase) {
    SHA256(buffer, CHUNK_SIZE, treeptr);
    treeptr += SHA256_DIGEST_LENGTH;
    i++;
   }
  free(buffer);
  buffer = NULL;
  fclose(fp);
  fp = NULL;
  
#ifdef __DEBUG
  puts("Chunks hashed.");
#endif

  size_t hashsDone = numBase;
  size_t hashsPerTier = numBase/2;
  assert(hashsPerTier > 1);
  i = 0;
  while (hashsPerTier) {
    size_t j = 0;
    for(; i < (hashsDone + hashsPerTier)/2; i++) {
      j++;
#ifdef __DEBUG
      printf("i: %ul, hashsDone: %ul, hashesPerTier: %ul\n", i, hashsDone, hashsPerTier + i);
      printf("data: %ul, dest: %ul\n", 2 * i, hashsPerTier - i);
#endif
      unsigned char* data = merkmapTree + 2 * i * SHA256_DIGEST_LENGTH;
      unsigned char* dest = merkmapTree + (hashsDone + j) * SHA256_DIGEST_LENGTH;
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
