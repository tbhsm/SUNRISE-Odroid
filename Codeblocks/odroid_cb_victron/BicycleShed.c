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

		printf("the time taken was %d \n",diff/10000);
		n = 1 -diff/10000;
		sleep(n); /*pause before next iteration*/

		/*NEW THREAD IS CREATED every 5 minutes to write weather values to the server */
		if(j==50 || j==50){
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
		printf("run %d",j);

		/*NEW THREAD IS CREATED every 10 minutes to write temperature values to the server */
		if(j==50){
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
		if(j==1){
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

		printf("j is %d \n",j);
		j++;
	}

	return EXIT_SUCCESS;
}
/*THREAD FUNCTION TO READ CHARGER STATUS FROM THE SERVER. ALSO DISPLAYS THE CURRENT STATUS OF THE CHARGERS ON THE LOCAL DISPLAY*/
void *function15s()
{

	htmlDisplay(); //empty function now
	printf("Charger function running. was.");
	pthread_exit(NULL);
}
/*THREAD FUNCTION TO READ&SEND WEATHER DATA also displays the data on the local display*/
void *function10m()
{
	WeatherDisplay(); //empty function now
	printf("Weather function running. was.");
	pthread_exit(NULL);
}
/*THREAD FUNCTION TO READ&SEND SOLAR PANEL TEMPERATURE*/
void *functionData(){
	/*READ SOLAR TEMP. STATION DATA*/
	uint16_t *solar;
	//solar = (uint16_t*)malloc(6*sizeof(uint16_t));
	solar = readHold(); // empty function
	/*SEND THE SOLAR TEMP. DATA TO SERVER*/
	//int jj = writeSolarToServer(solar); //send data to server
	free(solar);
	pthread_exit(NULL);
}

/*THREAD FUNCTION FOR VICTRON DATA---uploads data to all 4 Victron Databases*/

void *functionVictronData()
{
	/*READ DATA FROM VICTRON AND SEND THEM TO THE SERVER*/
	double *victron;
	int i;
	//victron = (uint16_t*)malloc(35*sizeof(uint16_t));
//	victron = getData(3,38);
//
//	//print results to the screen
//
//	for(i = 3; i<=38; i++){
//        printf("reg. no.: %d, reg. value: %f \n", i, victron[i-3]);
//	}
//
//	//!!!!-------SEND VICTRON VEBUS DATA HERE-------!!!!
//	//int jj = writeVictron1ToServer(victron); //and send data to server
//	free(victron);


	/*repeat for second chunk of data*/
	//victron = (uint16_t*)malloc(35*sizeof(uint16_t));
	victron = getData(41,41);

	for(i = 41; i<=41; i++){
        printf("reg. no.: %d, reg. value: %f \n", i, victron[i-41]);
	}

	//!!!!-------SEND VICTRON BATTERY DATA HERE-------!!!!
	//int jj = writeVictron1ToServer(victron); //and send data to server
	free(victron);

//		/*repeat for third chunk of data*/
//	//victron = (uint16_t*)malloc(8*sizeof(uint16_t));
//	victron = getData(778,785);
//
//	for(i = 778; i<=785; i++){
//        printf("reg. no.: %d, reg. value: %f \n", i, victron[i-778]);
//	}
//	//!!!!-------SEND VICTRON SOLARCHARGER DATA HERE-------!!!!
//	//int jj2 = writeVictron3ToServer(victron);
//	free(victron);
//
//		/*repeat for third chunk of data*/
//	//victron = (uint16_t*)malloc(8*sizeof(uint16_t));
//	victron = getData(2600,2608);
//
//	for(i = 2600; i<2608; i++){
//        printf("reg. no.: %d, reg. value: %f \n", i, victron[i-2600]);
//	}
//	//!!!!-------SEND VICTRON GRID DATA HERE-------!!!!
//	//int jj2 = writeVictron3ToServer(victron);
//	free(victron);

	/*exit thread --- mutex is the reason*/
	sleep(1);
	puts("Last thread. Hope for zero");
	pthread_exit(NULL);
}
