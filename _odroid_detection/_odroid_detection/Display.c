 /*
 ============================================================================
 Name        : Display.c
 Description : Contains all functions regarding the local display
 Author      : Tim Hosman & Job van Staveren
 Version     : 1.0
 Created on  : June 5, 2016
 E-mails     : t.b.hosman@student.tudelft.nl, j.vanstaveren@student.tudelft.nl
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "Weather.h"
#include "ServerCom.h"
#include "BicycleShed.h"

/* READ WEATHER STATION DATA */
/* sendToServer: set to 1 to send to server, set to 0 (or anything else) to not send to server */
void weatherDisplay(int sendToServer){
	/* registers 2 - 6 (sensor statuses), 34 (temperature), 39 (wind-chilltemperature), 13 (relative humidity),
	   42 (wind speed), 30 (radiation), 17 (relative air pressure), 18 (wind direction) */
	int registersToRead[] = {2, 3, 4, 5, 6, 34, 39, 13, 42, 30, 17, 18};
	int length = sizeof(registersToRead)/sizeof(int);

    /* CHECK IF WEATHER WAS CORRECTLY FETCHED */
	if (readWeatherStation(registersToRead, length)){

        /* WRITE TO SERVER IF SET TO 1 */
        if (sendToServer == 1) writeWeatherToServer(weatherFinalValues);

        /* WRITE WEATHER DATA TO LOCAL TEXT FILE */
		FILE *f = fopen("localDisplayData.json", "w");
		if (f == NULL){
			printf("Error opening file!\n");
		} else {
			fprintf(f, "{");
			fprintf(f, "\"temperature\": %f,\n", weatherFinalValues[5]);
			fprintf(f, "\"wind-chill\": %f,\n", weatherFinalValues[6]);
			fprintf(f, "\"humidity\": %f,\n", weatherFinalValues[7]);
			fprintf(f, "\"wind speed\": %f,\n", weatherFinalValues[8]);
			fprintf(f, "\"radiation\": %f,\n", weatherFinalValues[9]);
			fprintf(f, "\"air pressure\": %f,\n", weatherFinalValues[10]);
			fprintf(f, "\"wind direction\": %f\n", weatherFinalValues[11]);
			fprintf(f, "}");
			fclose(f);
		}
	}
}

