 /*
 ============================================================================
 Name        : Display.c
 Author      : Tim Hosman & Job van Staveren
 Version     : 2.0
 Created on  : June 5, 2016
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "Weather.h"
#include "ServerCom.h"
#include "BicycleShed.h"

void weatherDisplay(int sendToServer){
	//FILE  * fp;
	/*READ WEATHER STATION DATA*/
	/* registers 2 - 6 (sensor statuses), 34 (temperature), 39 (wind-chilltemperature), 13 (relative humidity),
	   45 (wind speed), 30 (radiation), 17 (relative air pressure), 18 (wind direction) */

	int registersToRead[] = {2, 3, 4, 5, 6, 34, 39, 13, 45, 30, 17, 18};
	int length = sizeof(registersToRead)/sizeof(int);

	/* Write weather data to local text file */
	if (readWeatherStation(registersToRead, length)){ //Check if weather was correctly fetched
        /* Write to server if set to 1 */
        if (sendToServer == 1) writeWeatherToServer(weatherFinalValues);

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

