#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "lib-json.h"


struct libJson* libJson_construct(void) {
  struct libJson* lj = (struct libJson*)malloc(sizeof(struct libJson));
  int i;
  lj->jsonEncoded = NULL;
  lj->msg = NULL;
  for(i = 0; i < sizeof(lj->bufferAnswer) / sizeof(lj->bufferAnswer[0]); i++)
    lj->bufferAnswer[i] = '\0';
  lj->jsonArr = NULL;
  lj->jsonArrLength = 0;
  lj->msgBack = NULL;
  lj->sockfd = -1;
  return lj;
}

void libJson_destruct(struct libJson* lj) {
  free(lj->jsonEncoded);
  free(lj->msg);
  free(lj->jsonArr);
  free(lj->msgBack);
  if(lj->sockfd >= 0)
    close(lj->sockfd);
  free(lj);
}

void addHeaders(struct libJson* lj, const char* hostname, const char* pagename, const char* msg) {
  char blueprint[] =
    "POST /%s HTTP/1.1\n"
    "Host: %s\n"
    "Connection: Close\n"
    "Content-type: application/json\n"
    "Content-length: %d\n"
    "\n"
    "%s\n";
    int length = strlen(blueprint) + strlen(msg) + strlen(hostname) + strlen(pagename) + 2;
    lj->msg = (char*)malloc(length * sizeof(char));
    sprintf(lj->msg, blueprint, pagename, hostname, strlen(msg), msg);
}

int connectTUDelft(struct libJson* lj, const char* hostname,
                   int portno) {
  int n;
  struct sockaddr_in servAddr;
  struct hostent* server;
  lj->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(lj->sockfd < 0) {
    perror("ERROR opening socket");
    return -1;
  }
  server = gethostbyname(hostname);
  if(server == NULL) {
    fprintf(stderr, "ERROR, cannot find host: %s\n", hostname);
    close(lj->sockfd);
    return lj->sockfd = -1;
  }
  bzero((char*)&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  bcopy((char*)server->h_addr,
    (char*)&servAddr.sin_addr.s_addr,
    server->h_length);
  servAddr.sin_port = htons(portno);
  if(connect(lj->sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
    perror("ERROR connecting");
    close(lj->sockfd);
    return lj->sockfd = -1;
  }
//  lj->msg = add_headers(hostname, pagename, msg);
  n = write(lj->sockfd, lj->msg, strlen(lj->msg));
  if(n < strlen(lj->msg)) {
    perror("ERROR writing");
    close(lj->sockfd);

    return lj->sockfd = -1;
  }
  return lj->sockfd; // Status code; sockfd is stored in the struct;
}

int readChunk(struct libJson* lj) {
  int i, n, length = sizeof(lj->bufferAnswer) / sizeof(*lj->bufferAnswer);
  for(i = 0; i < length; i++)
    lj->bufferAnswer[i] = '\0';
  n = read(lj->sockfd, lj->bufferAnswer, length-1);
  if(n < 0) {
    perror("ERROR reading from socket");
    return -1;
  }
  return n;
}

char* findMessage(struct libJson* lj) {
  int i, length = sizeof(lj->bufferAnswer) / sizeof(*lj->bufferAnswer)-1;
  for(i = 0; i < length; i++) {
    if(lj->bufferAnswer[i] == '\n' &&
        (lj->bufferAnswer[i+1] == '\n' || lj->bufferAnswer[i+1] == '\r')) {
      if(lj->bufferAnswer[i+1] == '\r') return &lj->bufferAnswer[i+3];
      else return &lj->bufferAnswer[i+2];
    }
  }
  // End of header is not in this chunk, read the next one
  // If there is an error, or when there is no more to read, return NULL.
  if(readChunk(lj) <= 0) return NULL;
  return findMessage(lj);
}

int getContentLength(struct libJson* lj) {
  int i, ret, length = sizeof(lj->bufferAnswer) / sizeof(*lj->bufferAnswer)-1;
  for(i = 0; i < length; i++)
    if(lj->bufferAnswer[i] == '\n') {
      if(strncmp(lj->bufferAnswer + i+1, "Content-Length: ", strlen("Content-Length: ")) == 0) {
        sscanf(lj->bufferAnswer + i+1, "Content-Length: %d", &ret);
        return ret;
      }
    }
  return -1;
}

void jsonDecodeArr(struct libJson* lj) {
  int i, j, k;
  int size = 1;
  char buffer[10];
  for(i = 0; i < strlen(lj->msgBack); i++) {
    if(lj->msgBack[i] == ',') size++;
  }
  lj->jsonArr = (uint16_t*)malloc(sizeof(uint16_t) * size);
  if(strlen(lj->msgBack) <= 2) {
    lj->jsonArrLength = 0;
    return;
  }
  i = j = 0;
  while(j < strlen(lj->msgBack)) {
    for(k = 0; k < 10; k++) buffer[k] = '\0';
    k = 0;
    while(j < strlen(lj->msgBack) && (lj->msgBack[j] < '0' || lj->msgBack[j] > '9')) {
      j++;
    }
    do {
      buffer[k++] = lj->msgBack[j++];
    } while(j < strlen(lj->msgBack) && lj->msgBack[j] >= '0' && lj->msgBack[j] <= '9');
    if(buffer[0] != '\0')
      lj->jsonArr[i++] = atoi(buffer);
  }
  lj->jsonArrLength = size;
}

void jsonEncodeArr(struct libJson* lj, double* arr, int size) {
  int length = 14*size + 1; // Biggest number seen has a size of 12, plus the ', ' makes 14
  int i;
  double cur;
  lj->jsonEncoded = (char*)malloc(length*sizeof(char));
  for(i = 1; i < length; i++) lj->jsonEncoded[i] = '\0';
  lj->jsonEncoded[0] = '[';
  for(i = 0; i < size; i++) {
    if(i) sprintf(lj->jsonEncoded, "%s, ", lj->jsonEncoded);
    if(arr[i] == 0) cur = 0; else cur = arr[i]; // To fix '-0';
    sprintf(lj->jsonEncoded, "%s%6g", lj->jsonEncoded, cur);
  }
  sprintf(lj->jsonEncoded, "%s]", lj->jsonEncoded);
}

int readAns(struct libJson* lj) {
  int i, j, length = sizeof(lj->bufferAnswer) / sizeof(*lj->bufferAnswer) - 1;
  int size;
  char* tmp;
  if(readChunk(lj) <= 0) return -1;
  if(getContentLength(lj) <= 0) return -1;
  size = sizeof(char) * getContentLength(lj) + 1;
  lj->msgBack = (char*)malloc(size);
  for(i=0; i < size; i++) lj->msgBack[i] = '\0';
  tmp = findMessage(lj);
  if(tmp == NULL) return -1; // Something went wrong, return with nothing
  for(i = 0; i < strlen(tmp); i++) {
    lj->msgBack[i] = tmp[i];
  }
  while(readChunk(lj) > 0) {
    for(j = 0; j < strlen(lj->bufferAnswer); j++) {
      lj->msgBack[i++] = lj->bufferAnswer[j];
    }
  }
  return 1;
}

static char* bufferInternetOutage[BUFFER_LENGTH];
static char** curBuf = bufferInternetOutage;
static char** endBuf = bufferInternetOutage + BUFFER_LENGTH;

void addStringToBuffer(struct libJson* lj) {
  if(curBuf < endBuf) { 
    *curBuf = lj->msg;
    curBuf++;
  } else {
    // Buffer is full, delete the first ones.
    char** bufIdx = bufferInternetOutage;
    free(bufferInternetOutage[0]);
    while(bufIdx < endBuf - 1) {
      bufIdx[0] = bufIdx[1];
      bufIdx++;
    }
    endBuf[-1] = lj->msg;
  }
  // Prevent freeing (the message has to be sent yet)
  lj->msg = NULL;
  
}

void emptyBuffer(void) {
  char hostName[100]; int i, stat;
  while(bufferHasContent()) {
    struct libJson* send = libJson_construct();
    curBuf--;
    send->msg = *curBuf;
    for(i = 0; i < 100; i++) hostName[i] = '\0';
    sscanf(send->msg, "POST /%*s HTTP/1.1 Host: %s Connection %*s", hostName);
    stat = connectTUDelft(send, hostName, 80);
    if(stat < 0) {
      curBuf++;
      libJson_destruct(send);
      return;
    }
    readAns(send); // Flushing the internal buffer
    libJson_destruct(send);
  }
}

int bufferHasContent(void) {
  return curBuf > bufferInternetOutage;
}

void cleanupBuffer(void) {
  while(bufferHasContent()) {
    curBuf--;
    free(*curBuf);
  }
}
