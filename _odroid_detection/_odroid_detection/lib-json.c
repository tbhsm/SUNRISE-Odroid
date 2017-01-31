#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "lib-json.h"

// The libJson constructor
// Fill all the pointers with NULL, since that is free-save
struct libJson* libJson_construct(void) {
  // Malloc the actual struct
  struct libJson* lj = (struct libJson*)malloc(sizeof(struct libJson));
  int i;
  lj->jsonEncoded = NULL;
  lj->msg = NULL;
  // The answer buffer is an array, initialize it with zeros
  for(i = 0; i < sizeof(lj->bufferAnswer) / sizeof(lj->bufferAnswer[0]); i++)
    lj->bufferAnswer[i] = '\0';
  lj->jsonArr = NULL;
  lj->jsonArrLength = 0;
  lj->msgBack = NULL;
  // 0 is an existing file descriptor, so initialize in with -1
  lj->sockfd = -1;
  return lj;
}

// The libJson destructor
// Free all the pointers, close the file descriptor and free the structure
void libJson_destruct(struct libJson* lj) {
  free(lj->jsonEncoded);
  free(lj->msg);
  free(lj->jsonArr);
  free(lj->msgBack);
  if(lj->sockfd >= 0)
    close(lj->sockfd);
  free(lj);
}

/** add the HTTP headers to the message
 * @returns void
 * @param lj the libJson struct
 * @param hostname the site to connect to, e.g. solarpoweredbikes.tudelft.nl
 * @param pagename the page on the site to connect to, e.g. ccon/updatedb.php The whole address is solarpoweredbikes.tudelft.nl/ccon/updatedb.php
 * @param msg the message to prepend the header to
 */
void addHeaders(struct libJson* lj, const char* hostname, const char* pagename, const char* msg) {
  char blueprint[] =
    "POST /%s HTTP/1.1\n"
    "Host: %s\n"
    "Connection: Close\n"
    "Content-type: application/json\n"
    "Content-length: %d\n"
    "\n"
    "%s\n";
    // determine the length of the final message, and malloc the message. Fill the blueprint afterwards. The filled in blueprint will be set in the msg member of the struct
    int length = strlen(blueprint) + strlen(msg) + strlen(hostname) + strlen(pagename) + 2;
    lj->msg = (char*)malloc(length * sizeof(char));
    sprintf(lj->msg, blueprint, pagename, hostname, strlen(msg), msg);
}

/** try to connect to the server, and send the message
 * @returns -1 if error, >= 0 if succes
 * @param lj the libJson struct
 * @param hostname the side to connect to
 * @param portno the port number to connect to
 */
int connectTUDelft(struct libJson* lj, const char* hostname,
                   int portno) {
  int n;
  struct sockaddr_in servAddr;
  struct hostent* server;
  // create the socket for internet
  lj->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  // return -1 if socket creation failed
  if(lj->sockfd < 0) {
    perror("ERROR opening socket");
    return -1;
  }
  // try to find the host
  server = gethostbyname(hostname);
  // if the host is not found, print an error message, close the socket to prevent communication and return -1.
  // The socket file descriptor is also set to -1 to prevent closing it twice.
  if(server == NULL) {
    fprintf(stderr, "ERROR, cannot find host: %s\n", hostname);
    close(lj->sockfd);
    return lj->sockfd = -1;
  }
  // Initialize the servAddr with zeros
  bzero((char*)&servAddr, sizeof(servAddr));
  // Configure the internet address struct to tell it is actually internet
  servAddr.sin_family = AF_INET;
  // copy the address taken from gethostbyname to the address struct
  bcopy((char*)server->h_addr,
    (char*)&servAddr.sin_addr.s_addr,
    server->h_length);
  // the port to connect to (it must be converted to network byte order first with htons
  servAddr.sin_port = htons(portno);
  // Try to connect to the server. If it fails, close the socket and return -1.
  if(connect(lj->sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
    perror("ERROR connecting");
    close(lj->sockfd);
    return lj->sockfd = -1;
  }
  // Write the message to the server. n will be set to the number of characters sent.
  n = write(lj->sockfd, lj->msg, strlen(lj->msg));
  // If not all characters of the message are sent, close the socket and return -1.
  if(n < strlen(lj->msg)) {
    perror("ERROR writing");
    close(lj->sockfd);
    return lj->sockfd = -1;
  }
  return lj->sockfd; // Status code; sockfd is stored in the struct;
}

// Read 255 characters from internet and store it in the bufferAnswer array in the libJson struct
int readChunk(struct libJson* lj) {
  int i, n, length = sizeof(lj->bufferAnswer) / sizeof(*lj->bufferAnswer);
  // initialize the buffer with zeros, to prevent having half the previous message.
  // Note that the buffer is 256 bytes, but 255 will actually be used to enforce the closing '\0'
  for(i = 0; i < length; i++)
    lj->bufferAnswer[i] = '\0';
  // read from the socket specified by the file descriptor
  n = read(lj->sockfd, lj->bufferAnswer, length-1);
  // if nothing read because of an error, return -1 as status
  if(n < 0) {
    perror("ERROR reading from socket");
    return -1;
  }
  // else return the number of characters as status.
  return n;
}

// Find the end of the header, which is always two newline characters or two '\r\n' combos if read from a windows computer.
char* findMessage(struct libJson* lj) {
  int i, length = sizeof(lj->bufferAnswer) / sizeof(*lj->bufferAnswer)-1;
  for(i = 0; i < length; i++) {
    // If a character is a newline character, look if the following character is either a '\r' or a '\n'. If it is a '\n', return a pointer to the character after it.
    // If it is a '\r', return a pointer to the character after the following '\n'
    if(lj->bufferAnswer[i] == '\n' &&
        (lj->bufferAnswer[i+1] == '\n' || lj->bufferAnswer[i+1] == '\r')) {
      if(lj->bufferAnswer[i+1] == '\r') return &lj->bufferAnswer[i+3];
      else return &lj->bufferAnswer[i+2];
    }
  }
  // End of header is not in this chunk, read the next one
  // If there is an error, or when there is nothing more to read, return NULL.
  if(readChunk(lj) <= 0) return NULL;
  return findMessage(lj);
}

// Get the length of the actual message, given the bufferAnswer with in it the header.
int getContentLength(struct libJson* lj) {
  int i, ret, length = sizeof(lj->bufferAnswer) / sizeof(*lj->bufferAnswer)-1;
  for(i = 0; i < length; i++)
    // Check in each line if it starts with "Content-Length". If it does, scan the number behind it and return it.
    if(lj->bufferAnswer[i] == '\n') {
      if(strncmp(lj->bufferAnswer + i+1, "Content-Length: ", strlen("Content-Length: ")) == 0) {
        sscanf(lj->bufferAnswer + i+1, "Content-Length: %d", &ret);
        return ret;
      }
    }
  // No Content-Length found, return -1
  return -1;
}

// Decode the message to find the string containing it.
void jsonDecodeArr(struct libJson* lj) {
  int i, j, k;
  int size = 1;
  char buffer[10];
  // The number of commas in the array determine the length of the array. Since the commas are inbetween the numbers, the size is one bigger.
  for(i = 0; i < strlen(lj->msgBack); i++) {
    if(lj->msgBack[i] == ',') size++;
  }
  // Malloc the array in which everything will be decoded.
  lj->jsonArr = (uint16_t*)malloc(sizeof(uint16_t) * size);
  // If the string only contains the characters '[]', return with a size of 0
  if(strlen(lj->msgBack) <= 2) {
    lj->jsonArrLength = 0;
    return;
  }
  i = j = 0;
  // The main decode loop
  while(j < strlen(lj->msgBack)) {
    // Empty the buffer in which the numbers will be decoded
    for(k = 0; k < 10; k++) buffer[k] = '\0';
    k = 0;
    // If there is a character that is not a number, skip it
    while(j < strlen(lj->msgBack) && (lj->msgBack[j] < '0' || lj->msgBack[j] > '9')) {
      j++;
    }
    // If there is a character that is a number, copy it over in the buffer
    do {
      buffer[k++] = lj->msgBack[j++];
    } while(j < strlen(lj->msgBack) && lj->msgBack[j] >= '0' && lj->msgBack[j] <= '9');
    // If there is something in the buffer, decode it and put it in the array.
    if(buffer[0] != '\0')
      lj->jsonArr[i++] = atoi(buffer);
  }
  // Set the size, then return.
  lj->jsonArrLength = size;
}

// Encode an array of the given length to JSON format. This function has one limitation: The array must contain at least one digit, otherwise a memory error will occur
// To be done in the future.
void jsonEncodeArr(struct libJson* lj, double* arr, int size) {
  int length = 14*size + 1; // Biggest number seen has a size of 12, plus the ', ' makes 14
  int i;
  double cur;
  // Malloc the to be encoded string...
  lj->jsonEncoded = (char*)malloc(length*sizeof(char));
  // ... and initialize it with zeros
  for(i = 1; i < length; i++) lj->jsonEncoded[i] = '\0';
  // The array starts with a [, so put it in.
  lj->jsonEncoded[0] = '[';
  for(i = 0; i < size; i++) {
    // Append ', ' to the string if the first number was put in
    if(i) sprintf(lj->jsonEncoded, "%s, ", lj->jsonEncoded);
    if(arr[i] == 0) cur = 0; else cur = arr[i]; // To fix '-0';
    // append the number to the string
    sprintf(lj->jsonEncoded, "%s%6g", lj->jsonEncoded, cur);
  }
  // Append the final ]
  sprintf(lj->jsonEncoded, "%s]", lj->jsonEncoded);
}

// A combination of readChunk, getContentLength and findMessage to fetch the actual message
int readAns(struct libJson* lj) {
  int i, j, length = sizeof(lj->bufferAnswer) / sizeof(*lj->bufferAnswer) - 1;
  int size;
  char* tmp;
  // retrieve the first chunk
  if(readChunk(lj) <= 0) return -1;
  // If the server doesn't return 200 (OK), discard the message and buffer it if necessary
  if(getStatusCode(lj) != 200) return -1;
  // Try to get the length of the message. On fail, return with an error code
  if(getContentLength(lj) <= 0) return -1;
  size = sizeof(char) * getContentLength(lj) + 1;
  // malloc the message and initialize it
  lj->msgBack = (char*)malloc(size);
  for(i=0; i < size; i++) lj->msgBack[i] = '\0';
  // Find the bottom of the header
  tmp = findMessage(lj);
  if(tmp == NULL) return -1; // Something went wrong, return with nothing
  // copy the rest of the message over in the malloc'ed string
  for(i = 0; i < strlen(tmp); i++) {
    lj->msgBack[i] = tmp[i];
  }
  // copy the rest over if there more to read
  while(readChunk(lj) > 0) {
    for(j = 0; j < strlen(lj->bufferAnswer); j++) {
      lj->msgBack[i++] = lj->bufferAnswer[j];
    }
  }
  // return succes
  return 1;
}

// The buffer of messages when internet in gone
static char* bufferInternetOutage[BUFFER_LENGTH];
// The pointer past the last message in the buffer
static char** curBuf = bufferInternetOutage;
// The pointer past the array, to check if curBuf is in the array.
static char** endBuf = bufferInternetOutage + BUFFER_LENGTH;

// The buffer for status changes, In the beginning, there are no statuses to send, hence it is initialized with zeros
static int chargerStatusOutage[6] = {0,0,0,0,0,0};

// Add a string to the buffer, and remove the first one if the buffer is full
void addStringToBuffer(struct libJson* lj) {
  // If there is still room left, set the message in the buffer and increment curBuf to point past it.
  if(curBuf < endBuf) { 
    *curBuf = lj->msg;
    curBuf++;
  } else {
    // Buffer is full, delete the first ones by freeing it and shifting the rest over.
    char** bufIdx = bufferInternetOutage;
    free(bufferInternetOutage[0]);
    // shifting occurs by incrementing the pointer and moving *(bufIdx + 1) to *bufIdx
    while(bufIdx < endBuf - 1) {
      bufIdx[0] = bufIdx[1];
      bufIdx++;
    }
    // The last message, the one prior to endBuf, since this pointer point past the end of the array, is the place where the message should go
    endBuf[-1] = lj->msg;
  }
  // Prevent freeing (the message has to be sent yet)
  lj->msg = NULL;
  
}

// Empty the buffer and send every message to the server
void emptyBuffer(void) {
  char hostName[100]; int i, stat;
  // While there actually are messages in the buffer, send them.
  while(bufferHasContent()) {
    // Create a new libJson struct to prevent overwriting messages that still have to be freed
    struct libJson* send = libJson_construct();
    // Curbuf pointed past the last message, decrement it to let it point to the message to be sended.
    curBuf--;
    // The headers were already added, so fill send->msg manually.
    send->msg = *curBuf;
    // Fetch the hostname by scanning it out of the message to be send
    for(i = 0; i < 100; i++) hostName[i] = '\0';
    sscanf(send->msg, "POST /%*s HTTP/1.1 Host: %s Connection %*s", hostName);
    // Try to connect to the server
    stat = connectTUDelft(send, hostName, 80);
    // on fail, let curbuf point past the message once again
    if(stat < 0) {
      curBuf++;
// TODO: send->msg = NULL // prevent the message to be freed
      libJson_destruct(send);
      return;
    }
    readAns(send); // Flushing the internal buffer by trying to read from the socket
    // Destruct the structure
    libJson_destruct(send);
  }
}

// curBuf points past the last message, so it can only point to the start of the array if there is no message in it
int bufferHasContent(void) {
  return curBuf > bufferInternetOutage;
}

// A function to free all messages in the buffer.
void cleanupBuffer(void) {
  while(bufferHasContent()) {
    curBuf--;
    free(*curBuf);
  }
}

// returns true if there are status updates to send
int statusHasContent(void) {
  int i;
  for(i = 0; i < 6; i++) {
    // return true if one of the array elements is 1
    if(chargerStatusOutage[i] == 1) return 1;
  }
  return 0;
}

// Copy all 1s in the status buffer. This way, multiple status notifications can be combined into one to send it once.
void addStatusToBuffer(int* status) {
  int i;
  for(i = 0; i < 6; i++) {
    if(status[i] == 1) chargerStatusOutage[i] = 1;
  }
}

// The emptyStatus function set the buffer back to {0,0,0,0,0,0} and sends the status updates in the buffer via the writeChargeToServer
void emptyStatus(void) {
  extern int writeChargeToServer(int*);
  int tmp[6], i;
  // copy the buffer over in a temporary array
  for(i = 0; i < 6; i++)
    tmp[i] = chargerStatusOutage[i];
  // Reset the buffer
  for(i = 0; i < 6; i++)
    chargerStatusOutage[i] = 0;
  // write the array to the server. If something fails, the message will be set back in chargerStatusOutage automatically
  writeChargeToServer(tmp);
}

// Get the status code of the HTTP response
int getStatusCode(struct libJson* lj) {
  int statuscode;
  // The status code always follows "HTTP/1.1 ", so scan it in and return it.
  sscanf(lj->bufferAnswer, "HTTP/1.1 %d", &statuscode);
  return statuscode;
}
