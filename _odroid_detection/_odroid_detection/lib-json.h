#ifndef LIB_JSON_H
#define LIB_JSON_H
#include <stdint.h>
#define BUFFER_LENGTH 200

struct libJson {
  char* jsonEncoded; // set by the jsonEncodeArr function, read by the writeDataToServer function
  char* msg;         // set by the addHeaders function or by the emptyBuffer function, read by the connectTUDelft function
  char bufferAnswer[256]; // set by the readChunk function, read by the getContentLength, readAns and the findMessage function
  uint16_t* jsonArr;      // set by the jsonDecodeArr function, read by the writeDataToServer function
  int jsonArrLength;      // ^
  char* msgBack;          // set by the readAns function, read by the jsonDecodeArr function
  int sockfd;             // set by the connectTUDelft function, read by the readChunk function
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
int statusHasContent(void);
void addStatusToBuffer(int*);
void emptyStatus(void);
int getStatusCode(struct libJson*);
#endif // LIB_JSON_H
