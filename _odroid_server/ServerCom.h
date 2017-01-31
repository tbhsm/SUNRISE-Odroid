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
#define START     200
#define WEATHER   0
#define SOLARTEMP 1
#define VICTRON1  2
#define VICTRON15 25
#define VICTRON2  3
#define VICTRON3  4
#define VICTRON4  5
#define GETCHARGE 10
#define SETCHARGE 20

int writeWeatherToServer(double*);
int writeSolarToServer(double*);
int writeVictron1ToServer(double*);
int writeVictron15ToServer(double*);
int writeVictron2ToServer(double*);
int writeVictron3ToServer(double*);
int writeVictron4ToServer(double*);
int writeChargeToServer(int*);
int writeStartToServer(void);
uint16_t* writeDataToServer(double*, int, int,int*);
uint16_t* getChargerState(void);

#endif /* SERVERCOM_H_ */
