/*
 * ServerCom.h
 *
 *  Created on: Oct 13, 2015
 *      Author: odroid
 *      Desription: Routines to communicate with Server.
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
#include <fcntl.h>

#ifndef SERVERCOM_H_
#define SERVERCOM_H_

#define TEST      100
#define WEATHER   0
#define SOLARTEMP 1
#define VICTRON1  2
#define VICTRON2  3
#define VICTRON3  4
#define VICTRON4  5
#define GETCHARGE 10

int writeToServer();
int writeWeatherToServer(double*);
int writeSolarToServer(uint16_t*);
int writeVictron1ToServer(double*);
int writeVictron2ToServer(double*);
int writeVictron3ToServer(double*);
int writeVictron4ToServer(double*);
uint16_t* writeDataToServer(double*, int, int,int*);
uint16_t* getChargerState(int b);

#endif /* SERVERCOM_H_ */
