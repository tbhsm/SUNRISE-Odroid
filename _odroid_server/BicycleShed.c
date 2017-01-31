/*
 ============================================================================
 Name        : BicycleShed.c
 Author      : Palinecko
 Version     : 2.0
 Copyright   : Free stuff --- I guess--no warranty for sure
 Description : The is the main program for the eBike Charging station.
 Revised     : Tim Hosman & Job van Staveren (18-04-2016 to 01-06-2016)
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
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/syscall.h>

#include "BicycleShed.h"
#include "readDatabase.h"
#include "Victron.h"
#include "Weather.h"
#include "Display.h"
#include "ServerCom.h"
#include "GPIOs.h"

/* VICTRON */
uint16_t victronReadValues[46];    // Holds returned values of all requested Victron registers
int victronCastValues[46];         // Holds values of all requested Victron registers after casting them to the appropriate type
double victronFinalValues[46];     // Holds the cast Victron values after they are scaled

/* WEATHER STATION */
uint16_t weatherReadValues[112];   // Holds returned values of all requested weather station registers
int weatherCastValues[112];        // Holds values of all requested weather station registers after casting them to the appropriate type
double weatherFinalValues[112];	   // Holds the cast weather station values after they are scaled

/* TEMPERATURE SENSORS */
uint16_t tempReadValues[6];        // Holds returned values of all temperature sensor registers
int tempCastValues[6];             // Holds values of all temperature sensor registers after casting them to the appropriate type
double tempFinalValues[6];         // Holds the cast temperature sensor values after they are scaled

int main(void)
{
    writeStartToServer();

    int j=1;                // for the infinite loop to make the application run like a donkey in circus
    int n;                  // measuring time in seconds
    double loop_time = 1.5; // time of one cycle in seconds

    /* INITIALIZE THE GPIO PINS */
    wiringPiSetup();        //default WiringPi setup function
    if (system_init()<0)    //default GPIO initialization
    {
        printf("Sth. went wrong. \n");
        fprintf(stderr,"Sth. went wrong. \n");
    }

    /* MAIN LOOP, WILL RUN INDEFINATELY */
    while (1)
    {
        printf("\nj is %d \n",j); // Print number of cycle

        clock_t start = clock(),diff;

        /* UPDATE CHARGER STATUS */
        setChargers();

        /* READ WEATHER STATION AND SEND MEASUREMENTS TO SERVER AND LOCAL DISPLAY EVERY APPR. 5 MINUTES */
        if(j==100 || j==300)
        {
            functionWeatherData();
        }

        /* READ WEATHER STATION AND SEND MEASUREMENTS TO LOCAL DISPLAY EVERY APPR. 15 SECONDS */
        else if ((j%10)==0)  //Every 10*1.5s=15s the weatherstation is read
        {
            //If data is not sent to server, only read weather for display
            functionDisplayData();
        }

        /* READ TEMPERATURE SENSORS AND SEND MEASUREMENTS TO SERVER EVERY APPR. 10 MINUTES */
        if(j==200)
        {
            functionSolarData();
        }

        /* READ VICTRON AND SEND MEASUREMENTS TO SERVER EVERY APPR. 10 MINUTES */
        if(j==400)
        {
            functionVictronData();
            j=0; //Reset to 0 if j==400
        }

        /* WAIT FOR LOOP TO TAKE 1.5 SECONDS TO FINISH */
        diff = clock()-start; //calculate remaining time
        printf("the time taken was %f \n",((float)diff/CLOCKS_PER_SEC)); //print time taken
        n = (int)((loop_time - ((double)diff)/CLOCKS_PER_SEC) * 1000000);
        if (n>0) usleep(n); // pause before next iteration, but only if less than 1.5 seconds have passed

        j++; // go to next cycle
    }

    /* FOR TEST PURPOSES, NOT RUN IN PRACTICE */
    setPinsZero(); //For test purposes. Hence no pins are left high when program stops
    return EXIT_SUCCESS;
}

/* READ WEATHER STATION AND SEND TO LOCAL DISPLAY */
void functionDisplayData()
{
    weatherDisplay(0); //set to 0 to only update locally
    printf("Display function running \n");
}

/* READ WEATHER STATION AND SEND MEASUREMENTS TO SERVER AND LOCAL DISPLAY */
void functionWeatherData()
{
    weatherDisplay(1); //set to 1 to send data to server
    printf("Weather and display function running \n");
}

/* FUNCTION TO READ & SEND SOLAR PANEL TEMPERATURES */
void functionSolarData()
{
    printf("Temperature function running \n");

    /* READ SOLAR TEMP. STATION DATA, SKIP IF NOT RECEIVED CORRECTLY */
    if (readTemperatureSensors())
    {
        int i = 0;
        for(i = 0; i < 6; i++)
        {
            printf("Temp-Sensor %d: %f degrees\n", i+1, tempFinalValues[i]);
        }
        /*SEND THE SOLAR TEMP. DATA TO SERVER*/
        writeSolarToServer(tempFinalValues);
    }

//	free(solar);
}

/* FUNCTION FOR VICTRON DATA */
void functionVictronData()
{
    printf("Victron function running \n");

    /* READ VEBUS DATA FROM VICTRON AND SEND THEM TO THE SERVER */
    if (readVictron(0,3,38))
    {
        writeVictron1ToServer(victronFinalValues); //Victron VEBUS data
    }

    /* READ SECOND VEBUS DATA FROM VICTRON AND SEND THEM TO THE SERVER */
    if (readVictron(246,3,38))
    {
        writeVictron15ToServer(victronFinalValues); //Second Victron VEBUS data
    }

    /* READ BATTERY DATA FROM VICTRON AND SEND THEM TO THE SERVER */
    if (readVictron(247,259,303))
    {
        writeVictron2ToServer(victronFinalValues); //Battery data
    }

    /* READ SOLAR DATA FROM VICTRON AND SEND THEM TO THE SERVER */
    if (readVictron(0,774,785))
    {
        writeVictron3ToServer(victronFinalValues); //Solarcharger data
    }
}
