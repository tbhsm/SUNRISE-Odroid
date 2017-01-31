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

	while (1){
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
		n = 15 -diff/10000;
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
		printf("run %d",j);

		/*NEW THREAD IS CREATED every 10 minutes to write temperature values to the server */
		if(j==20){
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

		printf("j is %d \n",j);
		j++;
	}

	/*THE CODE IS COMMENTED OUT ---NOT PERMANENT, AS A REFERENCE DURING DEVELOPMENT*/
	/*
	int i;
	puts("!!!Hello welcome in Matrix. You did a wise choice with the red pill!!!"); /* prints !!!Hello Future!!! */
	/*int addr=1;
	 */
	/*	int ADDRESS_START = 1;
	int ADDRESS_END	  = 31;
	uint16_t *tab_rp_registers;
	static uint16_t a[6];
	//int i;
	for(i=0;i<6;i++){
		a[i]=1+i*30;
	}



	char *b=NULL;
	b = getChargerState(3);
	printf("ahoj %d",atoi(b));
//	char buf[50];
//	char buf2[50];
	int try[7];
	//writeWeatherData(tab_rp_registers);
	/*static uint16_t *weather;
	weather = (uint16_t*)malloc(6*sizeof(uint16_t));
	weather= &a;//readInput();
	int jj = writeWeatherToServer(weather);*/


	/*Memory management stuff variables*/

	//tab_rp_registers=getData(ADDRESS_START,ADDRESS_END);
	/* Free the memory */
	//	free(tab_rp_registers);
	/*	uint16_t *b = readInput();
	for(i=0;i<4;i++){
	printf("\n stuff %d \n",b[i]);
	writeWeatherData(b);
	}*/
	//WeatherDisplay();
	/*	int *f;
	f = (int*)malloc(7*sizeof(int));
	int *b;
	b= readCharger(f);
for (i=0;i<7;i++){
	printf("*(b+%d):%d\n",i,*(b+i));
	try[i]=*(b+i);
}*/
	/*uint16_t *lol = readInput();
	for (i=0;i<7;i++){
	printf("Thing is %d",*(lol+i));}*/
	return EXIT_SUCCESS;
}
/*THREAD FUNCTION TO READ CHARGER STATUS FROM THE SERVER. ALSO DISPLAYS THE CURRENT STATUS OF THE CHARGERS ON THE LOCAL DISPLAY*/
void *function15s()
{

	htmlDisplay();
	printf("Charger function running. was.");
	pthread_exit(NULL);
}
/*THREAD FUNCTION TO READ&SEND WEATHER DATA also displays the data on the local display*/
void *function10m()
{
	WeatherDisplay();
	printf("Weather function running. was.");
	pthread_exit(NULL);
}
/*THREAD FUNCTION TO READ&SEND SOLAR PANEL TEMPERATURE*/
void *functionData(){
	/*READ SOLAR TEMP. STATION DATA*/
	uint16_t *solar;
	//solar = (uint16_t*)malloc(6*sizeof(uint16_t));
	solar = readHold(); // reading data from registers
	/*SEND THE SOLAR TEMP. DATA TO SERVER*/
	int jj = writeSolarToServer(solar); //send data to server
	free(solar);
	pthread_exit(NULL);
}

/*THREAD FUNCTION FOR VICTRON DATA---uploads data to all 4 Victron Databases*/

void *functionVictronData()
{
	/*READ DATA FROM VICTRON AND SEND THEM TO THE SERVER*/
	uint16_t *victron;
	//victron = (uint16_t*)malloc(35*sizeof(uint16_t));
	victron = getData(3,38);
	//Rescale based on datasheet
	victron[0] /= 10;
	victron[1] /= 10;
	victron[2] /= 10;
	victron[3] /= 10;
	victron[4] /= 10;
	victron[5] /= 10;
	victron[6] /= 100;
	victron[7] /= 100;
	victron[8] /= 100;
	victron[9] /= 0.1;
	victron[10] /= 0.1;
	victron[11] /= 0.1;
	victron[12] /= 10;
	victron[13] /= 10;
	victron[14] /= 10;
	victron[15] /= 10;
	victron[16] /= 10;
	victron[17] /= 10;
	victron[18] /= 100;
	victron[19] /= 10;
	victron[20] /= 0.1;
	victron[21] /= 0.1;
	victron[22] /= 0.1;
	victron[23] /= 100;
	victron[24] /= 10;
//	victron[25] /= 1;
//	victron[26] /= 1;
	victron[27] /= 10;
//	victron[28] /= 1;
//	victron[29] /= 1;
//	victron[30] /= 1;
//	victron[31] /= 1;
//	victron[32] /= 1;
//	victron[33] /= 1;
//	victron[34] /= 1;
//	victron[35] /= 1;
	//!!!!-------SEND VICTRON VEBUS DATA HERE-------!!!!
	//int jj = writeVictron1ToServer(victron); //and send data to server
	free(victron);


	/*repeat for second chunk of data*/
	//victron = (uint16_t*)malloc(35*sizeof(uint16_t));
	victron = getData(259,304);
	//Rescale based on datasheet
	victron[0] /= 100;
	victron[1] /= 100;
	victron[2] /= 10;
	victron[3] /= 10;
	victron[4] /= 100;
	victron[5] /= 100;
	victron[6] /= -10;
	victron[7] /= 10;
//	victron[8] /= 1;
//	victron[9] /= 1;
//	victron[10] /= 1;
//	victron[11] /= 1;
//	victron[12] /= 1;
//	victron[13] /= 1;
//	victron[14] /= 1;
//	victron[15] /= 1;
//	victron[16] /= 1;
//	victron[17] /= 1;
//	victron[18] /= 1;
//	victron[19] /= 1;
//	victron[20] /= 1;
//	victron[21] /= 1;
	victron[22] /= -10;
	victron[23] /= -10;
	victron[24] /= -10;
//	victron[25] /= 1;
//	victron[26] /= 1;
	victron[27] /= -10;
	victron[28] /= 100;
	victron[29] /= 100;
	victron[30] /= 0.01;
//	victron[31] /= 1;
//	victron[32] /= 1;
//	victron[33] /= 1;
//	victron[34] /= 1;
//	victron[35] /= 1;
	victron[36] /= 100;
	victron[37] /= 100;
//	victron[38] /= 1;
//	victron[39] /= 1;
	victron[40] /= 100;
	victron[41] /= 100;
	victron[42] /= 10;
	victron[43] /= 10;
	victron[44] /= 0.01;
	victron[45] /= 10;
	//!!!!-------SEND VICTRON BATTERY DATA HERE-------!!!!
	//int jj = writeVictron1ToServer(victron); //and send data to server
	free(victron);

		/*repeat for third chunk of data*/
	//victron = (uint16_t*)malloc(8*sizeof(uint16_t));
	victron = getData(778,785);
	//Rescale based on datasheet
//	victron[0] /= 1;
	victron[1] /= 10;
//	victron[2] /= 1;
//	victron[3] /= 1;
//	victron[4] /= 1;
//	victron[5] /= 1;
	victron[6] /= 10;
//	victron[7] /= 1;
	//!!!!-------SEND VICTRON SOLARCHARGER DATA HERE-------!!!!
	//int jj2 = writeVictron3ToServer(victron);
	free(victron);

		/*repeat for third chunk of data*/
	//victron = (uint16_t*)malloc(8*sizeof(uint16_t));
	victron = getData(2600,2608);
	//Rescale based on datasheet
//	victron[0] /= 1;
//	victron[1] /= 1;
//	victron[2] /= 1;
	victron[3] /= 100;
	victron[4] /= 100;
	victron[5] /= 100;
	victron[6] /= 100;
	victron[7] /= 100;
	victron[8] /= 100;
	//!!!!-------SEND VICTRON GRID DATA HERE-------!!!!
	//int jj2 = writeVictron3ToServer(victron);
	free(victron);

	/*exit thread --- mutex is the reason*/
	sleep(1);
	puts("Last thread. Hope for zero");
	pthread_exit(NULL);
}