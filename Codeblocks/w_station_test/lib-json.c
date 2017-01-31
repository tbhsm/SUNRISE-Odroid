#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "lib-json.h"

/*
void error(const char *msg) {
  perror(msg);
  exit(0);
}
*/ // Exit is not really what we want...


// the string in which the message will be printed by json_encode_arr. The space will be freed by the cleanup function down below.
char* msg;

// Encodes an integer array with given size into a json object.
char* json_encode_arr(double* arr, int size) {
  // initial length of the string, counting the "[]", de "," and the " ", as well as the 6 characters of the floating point number
  int length = size*8+1;
  // This loop determines the length of each of the components. For instance, 100 takes up 3 characters, and 2 takes up 1 character.
  int i;
  double cur;
  // Now that the size is known, malloc the space for the message.
  msg = malloc(length*sizeof(char));
  // Initialize the message with 0, to prevent random data from ending up in here.
  for(i = 1; i < length; i++) msg[i] = '\0';
  // The begin of the array
  msg[0] = '[';
  // In all but the first loop, append ', ' to the string, as saparator for the digits. Then append the next digit.
  for(i = 0; i < size; i++) {
    if(i) sprintf(msg, "%s, ", msg);
    // To prevent "-0" to be printed
    if(arr[i] == 0) cur = 0; else cur = arr[i];
    sprintf(msg, "%s%6g", msg, cur);
  }
  // Append the closing ']'
  sprintf(msg,"%s]", msg);
  return msg;
}

int connect_tudelft(const char* hostname, const char* pagename, int portno, char* msg) {
  int sockfd, length;
  struct sockaddr_in serv_addr;
  struct hostent* server;
  // The header files, with 'holes' in it for the data from the params.
  char blueprint[256] = 
    "POST /%s HTTP/1.1\n"
    "Host: %s\n"
    "Connection: Close\n"
    "Content-type: application/json\n"
    "Content-length: %d\n"
    "\n"
    "%s\n", *buffer;
  // Create a new internet socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("ERROR opening socket");
    return -1;
  }
  // Try to get the actual ip address (and some more information) by the given name: solarpoweredbikes.tudelft.nl/pagename
  server = gethostbyname(hostname);
  if(server == NULL) {
    fprintf(stderr, "ERROR, cannot find host: %s\n", hostname);
    return -1;
  }
  // Empty the serv_addr, to fill it later. This makes sure that some fields contains 0, not garbage.
  bzero((char*)&serv_addr, sizeof(serv_addr));
  // Fill the serv_addr structure with the address of the server, and with the port number.
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
    (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);
  serv_addr.sin_port = htons(portno);
  // Try to connect!
  if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    return -1;
  }
  // Filling the blueprint, and allocating a new string for it.
  length = strlen(blueprint) + strlen(msg) + strlen(hostname)+ strlen(pagename) + 2;
  buffer = malloc(length * sizeof(char));
  sprintf(buffer, blueprint, pagename, hostname, strlen(msg), msg);
  // Write the data to the server, and free the buffer again. The socket will be closed in the cleanup function.
  write(sockfd, buffer,strlen(buffer));
  free(buffer);
  return sockfd;
}

// The read buffer for the upcoming functions.
char buffer[256];

// Fill the buffer if read data, and return it, or NULL if EOF or error.
char* readchunk(int sockfd) {
  int i,n;
  for(i = 0; i < 256; i++) buffer[i] = '\0';
  n = read(sockfd, buffer, 255);
  if(n < 0) {
    perror("ERROR reading from socket");
    return NULL;
  }
  if(n == 0) return NULL;
  return buffer;
}
// Find the Content-Length header, and return the size of the actual message.
int get_content_length(void) {
  int i, length;
  for(i = 0; i < 255; i++)
    if(buffer[i] == '\n') {
      if(strncmp(&buffer[i+1], "Content-Length: ", strlen("Content-Length: ")) == 0) {
        sscanf(&buffer[i+1], "Content-Length: %d", &length);
        return length;
      }
    }
  return -1;
}
// This function finds the end of the header, aka the place with \r\n\r\n, and returns the pointer to the character therafter
char* find_message(void) {
  int i;
  for(i = 0; i < 255; i++) {
    if(buffer[i] == '\n' && (buffer[i+1] == '\n' || buffer[i+1] == '\r')) {
      if(buffer[i+1] == '\r') return &buffer[i+3];
      else return &buffer[i+2];
    }
  }
  return buffer;
}

// The array in which the decode_arr returns it value. msg_back is a buffer for the read_ans function.
uint16_t* json_arr;
char* msg_back;

// This function closes the socket and frees all global variables that needs to be freed
void cleanup(int sockfd) {
  close(sockfd);
  free(json_arr);
  free(msg);
  free(msg_back);
}

// An json array decode function. json contains a string like this: [a, b, c, d]
// ret will point to the return array. The length of that array is returned.
int json_decode_arr(char* json, uint16_t** ret) {
  int i, size=1,j,k;
  char buffer[10];
fprintf(stderr, "%s\n", json);
  // Determining the length of the array by counting the ','s
  for(i = 0; i < strlen(json); i++) {
    if(json[i] == ',') size++;
  }
  // Creating the return array.
  json_arr = malloc(sizeof(int) * size);
  // If there was no response, or if the array is empty, return length=0;
  if(strlen(json) <= 2) {
    return 0;
  }

  i = j = 0;
  // Continue the code that finds the integers for as long the string is not empty
  while(j < strlen(json)) {
    // Emptying the buffer.
    for(k = 0; k < 10; k++) buffer[k] = '\0';
    k = 0;
    // Finding a digit.
    while(j < strlen(json) && (json[j] < '0' || json[j] > '9')) {
      j++;
    }
    // While there are digits, copy into the buffer.
    do {
      buffer[k++] = json[j++];
    } while(j < strlen(json) && json[j] >= '0' && json[j] <= '9');
    // If there is anything in the buffer, convert it to int and put it in the return array.
    if(buffer[0])
      json_arr[i++] = atoi(buffer);
  }
  // Let the pointer to array point to the array and return its size.
  *ret = json_arr;
  return size;
}
// This function combines the readchunk, get_content_length and the find_message function to get the message (without the headers) into one buffer.
char* read_ans(int sockfd) {
  char* buffer_back;
  // Read the chunk with the headers.
  readchunk(sockfd);
  // With the given content length, create a buffer large enough.
  msg_back = malloc(sizeof(char)*get_content_length() + 1);
  // Put the first part (the message without the headers) into the buffer.
  sprintf(msg_back, "%s", find_message());
  // Read the rest.
  while((buffer_back = readchunk(sockfd))) {
    sprintf(msg_back, "%s%s", msg_back, buffer_back);
  }
  return msg_back;
}


