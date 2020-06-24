#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>

#define CHUNK_SIZE (32*1024)
#define VERSION "0.1.3"
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
  if(fp == NULL) {
    perror("Infile: ");
    exit(EXIT_FAILURE);
  }
  
  buffer = (unsigned char *) malloc(CHUNK_SIZE);
  if(buffer == NULL){
    perror("Read buffer");
    exit(EXIT_FAILURE);
  }

  size_t numChunks = (inFileSize / CHUNK_SIZE);
  if (inFileSize % CHUNK_SIZE) numChunks += 1;
  size_t numBase = 1;
  while (numBase < numChunks) numBase <<= 1;
  size_t numHashs = (2 * numBase) - 1;

#ifdef __DEBUG
  printf("numChunks: %f\nnumChunksPow2: %lu\nmerkmapSize: %lu\n\n", numChunks, numChunksPow2, merkmapSize);
#endif
  merkmapTree = (unsigned char *) calloc(numHashs, SHA256_DIGEST_LENGTH);
  if(merkmapTree == NULL){
    perror("Merklemap Tree");
    exit(EXIT_FAILURE);
  }

  size_t i = 0;
  while (fread(buffer, CHUNK_SIZE, 1, fp)) {
    SHA256(buffer, CHUNK_SIZE, merkmapTree + i * SHA256_DIGEST_LENGTH);
    memset(buffer, 0, CHUNK_SIZE);
    i++;
  }

  while (i <= numBase) {
    SHA256(buffer, CHUNK_SIZE, merkmapTree + i * SHA256_DIGEST_LENGTH);
    i++;
   }

  free(buffer);
  buffer = NULL;
  fclose(fp);
  fp = NULL;
  
#ifdef __DEBUG
  puts("Chunks hashed.");
#endif

  for(i = 0; i < numBase - 1; i++) {
    unsigned char* data = merkmapTree + 2 * i * SHA256_DIGEST_LENGTH;
    unsigned char* dest = merkmapTree + (numBase + i) * SHA256_DIGEST_LENGTH;
    SHA256(data, 2 * SHA256_DIGEST_LENGTH, dest);
  }

#ifdef __DEBUG
  puts("Merkle tree created.");
#endif

  filename = argv[2];
  if (strcmp(filename, "-") != 0){
    fp = fopen(filename, "wb");
    if(fp == NULL) {
      perror("Outfile");
      exit(EXIT_FAILURE);
    }
    fwrite(merkmapTree, SHA256_DIGEST_LENGTH, numHashs, fp);
    fclose(fp);
    fp = NULL;
  }else{
     fwrite(merkmapTree, SHA256_DIGEST_LENGTH, numHashs, stdout);
  }
  free(merkmapTree);
  merkmapTree = NULL;
  exit(EXIT_SUCCESS);
}
