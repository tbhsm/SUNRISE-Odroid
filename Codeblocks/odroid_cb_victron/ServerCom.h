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
#include </usr/local/include/modbus/modbus.h>
#include <stdint.h>
#include <curl/curl.h>
#include <fcntl.h>

#ifndef SERVERCOM_H_
#define SERVERCOM_H_

int writeToServer();
int writeWeatherToServer(uint16_t*);
int writeSolarToServer(uint16_t*);
int writeVictron1ToServer(uint16_t*);
int writeVictron2ToServer(uint16_t*);
int writeVictron3ToServer(uint16_t*);
char *getChargerState(int b);

#endif /* SERVERCOM_H_ */
