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
//#include <curl/curl.h>
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


void functionDisplayData();
void functionWeatherData();
void functionSolarData();
void functionVictronData();

int main(void) {

/*Uncomment six lines below if setChargers() is used in the loop. Then the system
is init'd only once*/

    wiringPiSetup();
    if (system_init()<0)
	{
	    printf("Sth. went wrong. \n");
		fprintf(stderr,"Sth. went wrong. \n");
	}

	//LEDBlink();
    //setChargers(); //For test purposes outside loop

	int j=1;    // for the infinite loop to make the application run like a donkey in circus
	int n;      // measuring time in seconds

	while (j<5){
        printf("\nj is %d \n",j);

		clock_t start = clock(),diff;

		setChargers();

		/*Read weatherstation and send measurements to server every appr. 5 minutes*/
		if(j==10 || j==30){
		//if(1){
			/*Weather data*/
            functionWeatherData();
		}
		else {
            //If data is not sent to server, only read weather for display
            functionDisplayData();
		}

		/*Read temperature sensors and send measurements to server every appr. 10 minutes*/
		if(j==20){
//			/*Solar data*/
            functionSolarData();
		}

		/*Read Victron and send measurements to server every appr. 10 minutes*/
		if(j==40){
            /*Victron data*/
            functionVictronData();
			j=0; //Reset to 0 if j==40
		}

		diff = clock()-start;

		printf("the time taken was %f \n",((float)diff/CLOCKS_PER_SEC));

        //Make sure the cycles do not take less than 15 seconds
		n = (int)((4 - ((double)diff)/CLOCKS_PER_SEC) * 1000000);
		if (n>0) usleep(n); /*pause before next iteration*/

		j++;
	}
        sleep(3);
		digitalWrite(6,LOW);//turn off
		digitalWrite(10,LOW);
		digitalWrite(11,LOW);
		digitalWrite(12,LOW);
		digitalWrite(13,LOW);
		digitalWrite(14,LOW);

	return EXIT_SUCCESS;
}
/*FUNCTION TO READ CHARGER STATUS FROM THE SERVER. ALSO DISPLAYS THE CURRENT STATUS OF THE CHARGERS ON THE LOCAL DISPLAY*/
void functionDisplayData()
{
	htmlDisplay();
	printf("Charger function running \n");
}
/*FUNCTION TO READ&SEND WEATHER DATA also displays the data on the local display*/
void functionWeatherData()
{
	WeatherDisplay();
	printf("Weather function running \n");
}
/*FUNCTION TO READ&SEND SOLAR PANEL TEMPERATURE*/
void functionSolarData(){
	/*READ SOLAR TEMP. STATION DATA*/
	double *solar;
	solar = readTemperatureSensors(); // reading data from registers
	printf("Temperature function running \n");

	int i = 0;
	for(i = 0; i < 6; i++){
        printf("Temp-Sensor %d: %f degrees\n", i+1, solar[i]);
	}
	/*SEND THE SOLAR TEMP. DATA TO SERVER*/
	writeSolarToServer(solar);
	free(solar);
}

/*FUNCTION FOR VICTRON DATA---uploads data to all 4 Victron Databases*/
void functionVictronData(){

    double *victron;

	/*READ DATA FROM VICTRON AND SEND THEM TO THE SERVER*/
	victron = readVictron(3,38);
	if (victron){
            writeVictron1ToServer(victron); //Victron VEBUS data
	}
	free(victron);

	/*repeat for second chunk of data*/
	victron = readVictron(259,303);
    if (victron){
        writeVictron2ToServer(victron); //Battery data
    }
	free(victron);

    /*repeat for third chunk of data*/
	victron = readVictron(774,785);
	if (victron){
        writeVictron3ToServer(victron); //Solarcharger data
	}
	free(victron);

  /*repeat for fourth chunk of data*/
	//victron = ReadVictron(2600,2608);
	//writeVictron4ToServer(victron); //Grid data
	//free(victron);

	puts("Victron function running");
}
