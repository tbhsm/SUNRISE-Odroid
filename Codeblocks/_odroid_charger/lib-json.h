#ifndef LIB_JSON_H
#define LIB_JSON_H
#include <stdint.h>
#define BUFFER_LENGTH 200

struct libJson {
  char* jsonEncoded;
  char* msg;
  char bufferAnswer[256];
  uint16_t* jsonArr;
  int jsonArrLength;
  char* msgBack;
  int sockfd;
};


struct libJson* libJson_construct(void);
void libJson_destruct(struct libJson*);
void addHeaders(struct libJson*, const char*, const char*, const char*);
int connectTUDelft(struct libJson*, const char*,int);
int readChunk(struct libJson*); 
char *findMessage(struct libJson*);
int getContentLength(struct libJson*);
void jsonDecodeArr(struct libJson*);
void jsonEncodeArr(struct libJson*, double*, int);
int readAns(struct libJson*);
void addStringToBuffer(struct libJson*);
void emptyBuffer(void);
int bufferHasContent(void);
void cleanupBuffer(void);
#endif // LIB_JSON_H
