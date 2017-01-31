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
#include <time.h>
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
int writeSolarToServer(double *solar) {
  int size = 6, retsize, i;
  free(writeDataToServer(solar, size, SOLARTEMP, &retsize));
  return 1;
}
/*----------------------CHARGER STATES-------------------*/
uint16_t *getChargerState(void) {
  double tmp = 1;
  // Eventually, if (or when) all six chargers will be read out at the same time, the size should be set at 0. The retsize is discarded, because 6 is assumed.
  int size = 1, retsize;
  // the returned array should be freed by the user
  uint16_t* retArray = writeDataToServer(&tmp, size, GETCHARGE, &retsize);
  return retArray;
}
/*----------------------VICTRON DATA chunk no.1---------------------*/
int writeVictron1ToServer(double *data) {
  int size = 36, retsize;
  free(writeDataToServer(data, size, VICTRON1, &retsize));
  return 1;
}

/*----------------------VICTRON DATA chunk no.1.5-------------------*/
int writeVictron15ToServer(double *data) {
  int size = 36, retsize;
  free(writeDataToServer(data, size, VICTRON15, &retsize));
  return 1;
}

/*----------------------VICTRON DATA chunk no.2---------------------*/
int writeVictron2ToServer(double *data) {
  int size = 45, retsize;
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

int writeChargeToServer(int* offarr){
  int size=6, retsize, i;
  double tmp[6];
  for(i = 0; i < 6; i++) tmp[i] = offarr[i];
  free(writeDataToServer(tmp, size, SETCHARGE, &retsize));
  return 1;
}

int writeStartToServer(void) {
  int size=1, retsize;
  double tmp = 1;
  free(writeDataToServer(&tmp, size, START, &retsize));
  return 1;
}

uint16_t* writeDataToServer(double *data, int size, int type, int* retSize) {
  struct libJson* lj;
  int lengthTime, length, stat, i;
  time_t curTime = time(NULL);
  time_t tmpTime;
  uint16_t* arrBack = NULL;
  char* tmpString, *blueprint =
    "{\"type\": %d,"
    "\"time\": %d,"
    "\"array\": %s}", *hostName =
  "solarpoweredbikes.tudelft.nl";

  for(lengthTime = 0, tmpTime = curTime;
      tmpTime != 0;
      tmpTime /= 10, lengthTime++)
    ;

  lj = libJson_construct();

  jsonEncodeArr(lj, data, size);
  length = strlen(blueprint) + strlen(lj->jsonEncoded) + lengthTime;
  tmpString = (char*)malloc(sizeof(char)* length);
  sprintf(tmpString, blueprint, type, curTime, lj->jsonEncoded);
  addHeaders(lj, hostName, "ccon/updatedb.php", tmpString);
  free(tmpString);
  stat = connectTUDelft(lj, hostName, 80);
printf("stat: %d\n", stat);
  if(stat < 0) {
    if(type != GETCHARGE) {
      addStringToBuffer(lj);
    }
  } else {
    stat = readAns(lj);
    if(stat < 0) {
      if(type != GETCHARGE) {
        addStringToBuffer(lj);
      }
    } else {
      jsonDecodeArr(lj);
      *retSize = lj->jsonArrLength;
      arrBack = (uint16_t*)malloc(sizeof(uint16_t)*lj->jsonArrLength);
      for(i = 0; i < lj->jsonArrLength; i++) {
        arrBack[i] = lj->jsonArr[i];
      }
      if(bufferHasContent()) {
        emptyBuffer();
      }
    }
  }
  libJson_destruct(lj);
  return arrBack;
}
