/*
 ============================================================================
 Name        : BicycleShed.c
 Author      : Palinecko
 Version     :
 Copyright   : Free stuff --- I guess--no warranty for sure
 Description : The is the main program for the eBike Charging station.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <modbus.h>
#include <stdint.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/syscall.h>

#include "readDatabase.h"
#include "Victron.h"
#include "Weather.h"
#include "Display.h"
#include "ServerCom.h"
#include "GPIOs.h"


void *function15s();
void *function10m();
void *functionData();
void *functionVictronData();

int main(void) {

	/*LEDBlink();
	puts("done");*/

	int j=1; // for the infinite loop to make the application run like a donkey in circus
	int n;// measuring time in seconds

	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3; //thread for Solar temperature, similar as thread2
	pthread_t thread4; // thread for Victron data, similar as thread2
	int iret1,iret2,iret3,iret4;

	while (j<=1){
		clock_t start = clock(),diff;

		/*Creating independent threads--each is executing one function(this is a bit tricky, if the data are going to be displayed they have to be read)*/
		iret1 = pthread_create(&thread1,NULL,function15s,NULL);
		if(iret1)
		{
			fprintf(stderr,"Error-pthread_create() return code: %d \n",iret1);
			exit(EXIT_FAILURE);
		}

		printf("pthread_create() was successful, %d \n",iret1);

		pthread_join(thread1,NULL);

		diff = clock()-start;

		printf("the time taken was %d \n",(int)diff/10000);
		n = 1 -diff/10000;
		sleep(n); /*pause before next iteration*/

		/*NEW THREAD IS CREATED every 5 minutes to write weather values to the server */
		if(j==10 || j==30){
			/*write weather data*/
			iret2 = pthread_create(&thread2,NULL,function10m,NULL);
			if(iret2)
			{
				fprintf(stderr,"Error-pthread_create() return code: %d \n",iret2);
				exit(EXIT_FAILURE);
			}
			pthread_join(thread2,NULL);
			//j=1;
		};
		printf("run %d\n",j);

		/*NEW THREAD IS CREATED every 10 minutes to write temperature values to the server */
		if(j==1){
			/*write Solar data*/
			iret3 = pthread_create(&thread3,NULL,functionData,NULL);
			if(iret3)
			{
				fprintf(stderr,"Error-pthread_create() return code: %d \n",iret2);
				exit(EXIT_FAILURE);
			}
			//	j=1;
		};

		/*NEW THREAD IS CREATED every 10 minutes to write Victron values to the server */
		if(j==40){
			/*write Victron data*/
			iret4 = pthread_create(&thread4,NULL,functionVictronData,NULL);
			if(iret4)
			{
				fprintf(stderr,"Error-pthread_create() return code: %d \n",iret2);
				exit(EXIT_FAILURE);
			}
			pthread_join(thread4,NULL);
			j=1;
		};
        sleep(5);
		printf("j is %d \n",j);
		j++;
	}

	return EXIT_SUCCESS;
}
/*THREAD FUNCTION TO READ CHARGER STATUS FROM THE SERVER. ALSO DISPLAYS THE CURRENT STATUS OF THE CHARGERS ON THE LOCAL DISPLAY*/
void *function15s()
{
	htmlDisplay(); // now empty
	printf("Thread1\n");
	pthread_exit(NULL);
}

/*THREAD FUNCTION TO READ&SEND WEATHER DATA also displays the data on the local display*/
void *function10m()
{
	WeatherDisplay(); // now empty
	printf("Thread2\n");
	pthread_exit(NULL);
}

/*THREAD FUNCTION TO READ&SEND SOLAR PANEL TEMPERATURE*/
void *functionData(){
	/*READ SOLAR TEMP. STATION DATA*/
	double *solar;
	//solar = (uint16_t*)malloc(6*sizeof(uint16_t));
	solar = (double *) calloc(6*sizeof(double));
	solar = readTemperatureSensors(); // reading data from registers
	/*SEND THE SOLAR TEMP. DATA TO SERVER*/
	writeSolarToServer(solar);

	//print the temperature sensor data
	int i = 0;
	for(i = 0; i < 6; i++){
        printf("Temp-Sensor %d: %f degrees\n", i+1, solar[i]);
	}

	free(solar);
	pthread_exit(NULL);
}

/*THREAD FUNCTION FOR VICTRON DATA---uploads data to all 4 Victron Databases*/

void *functionVictronData()
{

    //no data is requested from the victron

	/*exit thread --- mutex is the reason*/
	sleep(1);
	puts("Last thread. Hope for zero");
	pthread_exit(NULL);
}
