/*
 * ServerCom.c
 *
 *  Created on: Oct 13, 2015
 *      Author: odroid
 *      Description: Routines to communicate with the Server.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
//#include </usr/local/include/modbus/modbus.h>
#include <stdint.h>
//#include <curl/curl.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include "ServerCom.h"
#include "lib-json.h"

/*-----------------WEATHER DATA--------------------------*/
int writeWeatherToServer(double *weather) {
  int size = 12, retsize;
  free(writeDataToServer(weather, size, WEATHER, &retsize));
  return 1;
}
/*----------------------SOLAR TEMPERATURE------------------*/
int writeSolarToServer(uint16_t *solar) {
  double tmp[6];
  int size = 6, retsize, i;
  for(i =0 ; i < 6; i++) tmp[i] = solar[i];
  free(writeDataToServer(tmp, size, SOLARTEMP, &retsize));
  return 1;
}
/*----------------------CHARGER STATES-------------------*/
uint16_t *getChargerState(int n) {
  double tmp = n;
  // Eventually, if (or when) all six chargers will be read out at the same time, the size should be set at 0. The retsize is discarded, because 6 is assumed.
  int size = 1, retsize;
  // the returned array should be freed by the user
  uint16_t* ret_array = writeDataToServer(&tmp, size, GETCHARGE, &retsize);
  return ret_array;
}
/*----------------------VICTRON DATA chunk no.1---------------------*/
int writeVictron1ToServer(double *data) {
  int size = 36, retsize;
  free(writeDataToServer(data, size, VICTRON1, &retsize));
  return 1;
}

/*----------------------VICTRON DATA chunk no.2---------------------*/
int writeVictron2ToServer(double *data) {
  int size = 46, retsize;
  free(writeDataToServer(data, size, VICTRON2, &retsize));
  return 1;
}
/*----------------------VICTRON DATA chunk no.3---------------------*/
int writeVictron3ToServer(double *data) {
  int size = 12, retsize;
  free(writeDataToServer(data, size,  VICTRON3, &retsize));
  return 1;
}
/*----------------------VICTRON DATA chunk no.4---------------------*/
int writeVictron4ToServer(double *data) {
  int size = 9, retsize;
  free(writeDataToServer(data, size, VICTRON4, &retsize));
  return 1;
}

uint16_t *writeDataToServer(double *data, int size, int type, int* ret_size) {
  int sockfd, length;
  int i;
  // The actual json object to be sent to the server
  char* json_arr, *msg_back, *msg, *blueprint =
    "{\"type\": %d,"
    "\"array\": %s}";
  // The pointer to array that is given to the json_decode_arr function.
  uint16_t* arr_back;
  // The array to be returned.
  uint16_t* retval;
  // Encode the data into a json array.
  json_arr = json_encode_arr(data, size);
  // Create the message to the server, and allocate enough room for it.
  length = strlen(blueprint) + strlen(json_arr);
  msg = malloc(sizeof(char)* length);
  sprintf(msg, blueprint, type, json_arr);
  // Create a connection and give with it the message.
  sockfd = connect_tudelft("solarpoweredbikes.tudelft.nl", "ccon/updatedb.php", 80, msg);
  // Free the msg variable.
  free(msg);
  // Read the return from the server
  msg_back = read_ans(sockfd);
  // Decode the answer, and give it back via the call by pointer.
  *ret_size = json_decode_arr(msg_back, &arr_back);
  // Allocate room for the returned array....
  retval = malloc(sizeof(uint16_t) * *ret_size);
  // ... and copy the array over. This way, the array returned by json_decode_arr can be cleaned up by the cleanup function.
  for(i = 0; i < *ret_size; i++) {
    retval[i] = arr_back[i];
  }
  // Clean everything up, and return the newly created array.
  cleanup(sockfd);
  return retval;
}
