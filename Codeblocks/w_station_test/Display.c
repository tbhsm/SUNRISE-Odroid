/*
 * Display.c
 *
 *  Created on: Oct 7, 2015
 *      Author: odroid
 */

#include <stdio.h>
#include <stdlib.h>

#include "Weather.h"
#include "ServerCom.h"

void htmlDisplay(){
	/*READ WEATHER STATION DATA*/
	/* registers 2 - 6 (sensor statuses), 34 (temperature), 13 (relative humidity),
	   45 (wind speed), 30 (radiation), 17 (relative air pressure), 48 (precipitation), 21 (wind direction) */

	int RegistersToRead[] = {2, 3, 4, 5, 6, 34, 13, 45, 30, 17, 48, 18};
	int length = sizeof(RegistersToRead)/sizeof(int);
	double *weather = readInput(RegistersToRead, length);

	int i;
	for (i=0; i<length; i++){
        printf("reg. no.: %d, reg. value: %f \n", RegistersToRead[i], weather[i]);
	}

    /*Write weather data to the server*/
    writeWeatherToServer(weather);

	/*FREE ALLOCATED MEMORY*/
	free(weather);
}
void WeatherDisplay(){
//	FILE  * fp;
//	/*READ WEATHER STATION DATA*/
//	uint16_t *weather;
//	//weather = (uint16_t*)malloc(6*sizeof(uint16_t));
//	int RegistersToRead[] = {2, 3, 4, 5, 6, 34, 13, 45, 30, 17, 48, 21};
//	weather = readInput(RegistersToRead);
//	/*SEND THE WEATHER DATA TO SERVER*/
//	int jj = writeWeatherToServer(weather);
//	/*READ SERVER TO GET INFORMATION ABOUT CHARGER STATUS*/
//	int i;
//	int charger[7];
//	for (i=0;i<6;i++){
//		char *b=NULL;
//		b  = getChargerState(i+1);
//		charger[i]= atoi(b);}
//
//	/*FREE ALLOCATED MEMORY*/
//	free(weather);
}

