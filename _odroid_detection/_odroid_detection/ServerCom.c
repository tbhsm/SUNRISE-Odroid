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
// This function sends information from the weatherstation to the server.
int writeWeatherToServer(double *weather) {
// The number of values that will be sended
// retsize is only used to pass a pointer to int
  int size = 12, retsize;
// writeDataToServer returns a malloced pointer. Since it will not be used, free it.
  free(writeDataToServer(weather, size, WEATHER, &retsize));
  return 1;
}
/*----------------------SOLAR TEMPERATURE------------------*/
// This function sends infomation from the temperature sensors from to the server.
int writeSolarToServer(double *solar) {
  int size = 6, retsize, i;
  free(writeDataToServer(solar, size, SOLARTEMP, &retsize));
  return 1;
}
/*----------------------CHARGER STATES-------------------*/
// this function retrieves the current state of chargers.
uint16_t *getChargerState(void) {
  double tmp = 1;
  int size = 1, retsize;
  // the returned array should be freed by the user
  uint16_t* retArray = writeDataToServer(&tmp, size, GETCHARGE, &retsize);
  return retArray;
}
/*----------------------VICTRON DATA chunk no.1---------------------*/
// this function sends part of the information from the victron.
int writeVictron1ToServer(double *data) {
  int size = 36, retsize;
  free(writeDataToServer(data, size, VICTRON1, &retsize));
  return 1;
}

/*----------------------VICTRON DATA chunk no.1.5-------------------*/
// this function sends part of the information from the victron.
int writeVictron15ToServer(double *data) {
  int size = 36, retsize;
  free(writeDataToServer(data, size, VICTRON15, &retsize));
  return 1;
}

/*----------------------VICTRON DATA chunk no.2---------------------*/
// this function sends part of the information from the victron.
int writeVictron2ToServer(double *data) {
  int size = 45, retsize;
  free(writeDataToServer(data, size, VICTRON2, &retsize));
  return 1;
}
/*----------------------VICTRON DATA chunk no.3---------------------*/
// this function sends part of the information from the victron.
int writeVictron3ToServer(double *data) {
  int size = 12, retsize;
  free(writeDataToServer(data, size,  VICTRON3, &retsize));
  return 1;
}
/*----------------------VICTRON DATA chunk no.4---------------------*/
// this function sends part of the information from the victron.
int writeVictron4ToServer(double *data) {
  int size = 9, retsize;
  free(writeDataToServer(data, size, VICTRON4, &retsize));
  return 1;
}

// this function sends the updates for the chargers, when one of them is plugged out.
int writeChargeToServer(int* offarr){
  int size=6, retsize, i;
  double tmp[6];
// copy the values into a double array, since that is requested by the writeDataToServer function.
  for(i = 0; i < 6; i++) tmp[i] = offarr[i];
  free(writeDataToServer(tmp, size, SETCHARGE, &retsize));
  return 1;
}

// Write to the server when the ODROID is starting.
// The tmp=1 is because a bug in the jsonEncode function. That function requires at least a size of one.
int writeStartToServer(void) {
  int size=1, retsize;
  double tmp = 1;
  free(writeDataToServer(&tmp, size, START, &retsize));
  return 1;
}

/** Write general data to the server. This function combines the whole of lib-json.c
 * @returns the read array from internet.
 * @param data the data to be sent
 * @param size the size of the data array
 * @param type the type of the data. The types are defined in "ServerCom.h"
 * @param retSize the size of the returned array. Will be written in the function
 */
uint16_t* writeDataToServer(double *data, int size, int type, int* retSize) {
  // The struct used for the communication
  struct libJson* lj;
  int lengthTime, length, stat, i;
  time_t curTime = time(NULL);
  time_t tmpTime;
  uint16_t* arrBack = NULL;
  // A way to sneak the type number together with the current time and the actual data.
  char* tmpString, *blueprint =
    "{\"type\": %d,"
    "\"time\": %d,"
    "\"array\": %s}", *hostName =
  "solarpoweredbikes.tudelft.nl";
  // Determine the length (in characters) of the current unix epoch time.
  for(lengthTime = 0, tmpTime = curTime;
      tmpTime != 0;
      tmpTime /= 10, lengthTime++)
    ;

  // initialize the libJson structure
  lj = libJson_construct();

  // encode the given array in JSON format
  jsonEncodeArr(lj, data, size);
  // determine the length of the string to be sended, for the malloc below
  length = strlen(blueprint) + strlen(lj->jsonEncoded) + lengthTime;
  // allocate the string for the message.
  tmpString = (char*)malloc(sizeof(char)* length);
  // Fill in the blueprint.
  sprintf(tmpString, blueprint, type, curTime, lj->jsonEncoded);
  // Add the headers with the hostname, the curret page, and the filled in blueprint
  addHeaders(lj, hostName, "ccon/updatedb.php", tmpString);
  // tmpString is not necessary anymore, the total message is now stored in the libJson struct
  free(tmpString);
  // Try to connect to the server, and send the string
  stat = connectTUDelft(lj, hostName, 80);
  if(stat < 0) {
    // connection failed
    if(type != GETCHARGE && type != SETCHARGE) {
      // Add the string to the buffer if it is data from the weather station or from the victron.
      addStringToBuffer(lj);
    } else if(type == SETCHARGE) {
      // If it is an update about the chargers, store it in the special status buffer.
      // The tmp variable in necessary for the cast from double to int.
      int tmp[6];
      for(i=0;i<6;i++) tmp[i] = data[i];
      addStatusToBuffer(tmp);
    }
  } else {
    // Connection succeeded. Read the answer from the function
    stat = readAns(lj);
    if(stat < 0) {
      // If something went wrong with reading the answer, buffer the data with the same code used above.
      if(type != GETCHARGE && type != SETCHARGE) {
        addStringToBuffer(lj);
      } else if(type == SETCHARGE) {
        int tmp[6];
        for(i=0;i<6;i++) tmp[i] = data[i];
        addStatusToBuffer(tmp);
      }
    } else {
      // Answer read succesfully, decode it from JSON to an actual array if it GETCHARGE data.
      if(type == GETCHARGE) {
        jsonDecodeArr(lj);
        // set the retSize, the value passed by pointer.
        *retSize = lj->jsonArrLength;

        if(lj->jsonArrLength != 6) {
          FILE* fp = fopen("nietLangGenoegeArray.txt", "a+");
          if (fp == NULL){
              printf("Error opening file!\n");
          } else {
              time_t currentTime = time(NULL);
              fprintf(fp, "\nfailed on ");
              fprintf(fp, ctime(&currentTime));
              fprintf(fp, "message: %s\n", lj->msgBack);
              fclose(fp);
          }
        }

        // to be able to cleanup the libJson construct, the array has to be copied over into a new one
        // malloc it here
        arrBack = (uint16_t*)malloc(sizeof(uint16_t)*lj->jsonArrLength);
        // copy the array over.
        for(i = 0; i < lj->jsonArrLength; i++) {
          arrBack[i] = lj->jsonArr[i];
        }
      } else *retSize = 0;
      // Everything is done. Since internet works, empty both the buffers if they have content.
      if(bufferHasContent()) {
        emptyBuffer();
      }
      if(statusHasContent()) {
        emptyStatus();
      }
    }
  }
  // cleanup the libJson struct
  libJson_destruct(lj);
  // return the decoded array, or NULL if something went wrong.
  return arrBack;
}
