/*
 ============================================================================
 Name        : GPIOs.c
 Author      : odroid
 Version     : 2.0
 Created on  : Oct 22, 2015
 Description : The is the program that can control the needed GPIOs
 Revised     : Tim Hosman & Job van Staveren (18-04-2016 to 01-06-2016)
 E-mails     : t.b.hosman@student.tudelft.nl, j.vanstaveren@student.tudelft.nl
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>
#include <lcd.h>
#include <time.h>

#include "ServerCom.h"
#include "GPIOs.h"

#define DATA_UPDATE_PERIOD  100 // 100ms

#define PORT_ADC1   0   // ADC.AIN0

//static int adcValue = 0;

//static int ledPos = 0;

/* WIRINGPI PIN NUMBERS USED */
int pins[] = {6, 10, 11, 12, 13, 14}; //Pins that turn on the chargers
int detection_pins[] = {21, 22, 23, 24, 26, 27}; // Pins used for cable detection

/* FUNCTION USED TO SET THE CHARGER PINS TO ZERO */
//all pins have a defined voltage
void setPinsZero(void)
{

    //Currently used pins are 6,10,11,12,13,14 (wiringPi)
    digitalWrite(6,LOW);
    digitalWrite(10,LOW);
    digitalWrite(11,LOW);
    digitalWrite(12,LOW);
    digitalWrite(13,LOW);
    digitalWrite(14,LOW);

}

/* SYSTEM INITIALIZATION FUNCTION */
int system_init(void)
{
    int i;
    //i=0;
    // GPIO Init(LED Port ALL Output)
    //for(i = 0; i < MAX_LED_CNT; i++)    pinMode (ledPorts[i], OUTPUT);
    for(i =0; i < 6; i++)
    {
        pinMode(pins[i], OUTPUT);
        pinMode(detection_pins[i], INPUT);
        pullUpDnControl(detection_pins[i], PUD_UP);
    }

    //Set all used output gpio pins to 0
    setPinsZero();

    return  0;
}

/* READ ADC VALUE */
void boardDataUpdate(void) //for future use
{
//	int i;
//
//	// adc value read
//	if((adcValue = analogRead (PORT_ADC1)))    {
//		ledPos = (adcValue * MAX_LED_CNT * 1000) / 1024;
//		ledPos = (MAX_LED_CNT - (ledPos / 1000));
//	}
//	else
//		ledPos = 0;
//
//	//  LED Control
//	for(i = 0; i < MAX_LED_CNT; i++)    digitalWrite (ledPorts[i], 0); // LED All Clear
//	for(i = 0; i < ledPos;      i++)    digitalWrite (ledPorts[i], 1); // LED On
}

/* TEST FUNCTION */
void LEDBlink(void)
{
    //static int timer = 0;
    int i;

    wiringPiSetup();
    if (system_init()<0)
    {
        fprintf(stderr,"Sth. went wrong. \n");
    }

    for (i=0; i<2; i++)
    {
        digitalWrite(1,LOW); //turn off
        sleep(1);
        digitalWrite(1,HIGH);//turn on
        sleep(1);
    }
}

/* SET CHARGERS TO STATE PRESENT ON SERVER */
void setChargers(void)
{
    /* VARIABLES */
    int userTimerMax = 40; // maximum number of cycles between login and plugging in
    int NCMax = 10; // number of cycles as buffer for disconnecting
    int CMax = 4; // number of cycles as buffer for connecting

    /* INITIALISATION */
    int i;
    static int noConnectionTimer; // Used to keep track of the time that internet was lost
    static int cableTimerNC[6] = {0, 0, 0, 0, 0, 0}; // Contains all disconnect timers
    static int cableTimerC[6] =  {0, 0, 0, 0, 0, 0}; // Contains all connect timers
    static int userTimer[6] =  {0, 0, 0, 0, 0, 0};   // Contains all user timers
    typedef enum {OFF, ON, USERTIMER, CONNECTTIMER, DISCONNECTTIMER} chargerStates; // define states
    static chargerStates chargerStateLocal[6] = {OFF,OFF,OFF,OFF,OFF,OFF}; // Contains all currect states
    static chargerStates nextChargerStateLocal[6] = {OFF,OFF,OFF,OFF,OFF,OFF}; // Contains all next states

    /* GET THE STATE OF THE CHARGERS FROM THE SERVER DATABASE */
    uint16_t* chargerStateServer = getChargerState();

    /*  SET THE PHYSICAL CHARGERS */
    if(checkChargerValidity(chargerStateServer))
    {
        /* LOOP THROUGH ALL CHARGERS */
        for (i=0; i<6; i++)
        {

            int changesToServer[6] = {0,0,0,0,0,0};

            /* CHECK CURRENT STATE OF CHARGER */
            switch(chargerStateLocal[i])
            {

            /* CHARGER IS OFF ON BOTH SERVER AND STATION */
            case OFF:
                printf("Charger %d state: OFF \n", i+1);
                if (chargerStateServer[i]) nextChargerStateLocal[i]=USERTIMER; //if state is 1 on server
                else nextChargerStateLocal[i]=OFF; //else remain off
                break;

            /* USER LOGGED IN, STATION WAITING FOR PLUGGED CABLE */
            case USERTIMER:
                printf("Charger %d state: USERTIMER \n", i+1);
                if (userTimer[i] >= userTimerMax-1)       //if not connected after 1 minute
                {
                    userTimer[i] = 0;                     // Reset user timer
                    changesToServer[i] = 1;               // Set charger to changed (off)
                    writeChargeToServer(changesToServer); // Notify server of the change
                    nextChargerStateLocal[i] = OFF;       // Return to OFF state
                }
                else if (digitalRead(detection_pins[i]))  // Cable disconnected
                {
                    userTimer[i]++;                       // Increment timer
                    nextChargerStateLocal[i] = USERTIMER; // Remain in current state
                }
                else     //cable connected
                {
                    userTimer[i] = 0;                     // Reset user timer
                    nextChargerStateLocal[i] = CONNECTTIMER;// Move to next state
                }
                break;

            /* CABLE IS PROBABLY CONNECTED, WAIT A WHILE TO BE SURE */
            case CONNECTTIMER:
                printf("Charger %d state: CONNECTTIMER \n", i+1);
                if (cableTimerC[i] >= CMax-1)             // Connected over 6 seconds
                {
                    cableTimerC[i] = 0;                   // Reset connection timer
                    nextChargerStateLocal[i] = ON;        // Move to next state
                }
                else if (digitalRead(detection_pins[i]))  // Cable disconnected
                {
                    cableTimerC[i] = 0;                   // Reset connection timer
                    nextChargerStateLocal[i] = USERTIMER; // Return to USERTIMER
                }
                else                                      // Cable connected
                {
                    cableTimerC[i]++;                     // Increment connection timer
                    nextChargerStateLocal[i] = CONNECTTIMER;// Remain in current state
                }
                break;

            /* BICYCLE IS CHARGING, SERVER STATE IS ON */
            case ON:
                printf("Charger %d state: ON \n", i+1);
                if (digitalRead(detection_pins[i]))       // Cable disconnected
                {
                    nextChargerStateLocal[i] = DISCONNECTTIMER;// Move to disconnect buffer
                }
                else                                      // Cable connected
                {
                    nextChargerStateLocal[i] = ON;        // Remain in current state
                }
                break;

            /* CHARGING IS PROBABLY FINISHED (DISCONNECTED CHARGER), WAIT A WHILE TO BE SURE */
            case DISCONNECTTIMER:
                printf("Charger %d state: DISCONNECTTIMER \n", i+1);
                if (cableTimerNC[i] >= NCMax-1)           // Disconnected over 15 seconds
                {
                    cableTimerNC[i] = 0;                  // Reset timer
                    changesToServer[i] = 1;               // Set charger to changed (off)
                    writeChargeToServer(changesToServer); // Notify server of the change
                    nextChargerStateLocal[i] = OFF;       // Go to OFF state
                }
                else if (digitalRead(detection_pins[i]))  // Cable disconnected
                {
                    cableTimerNC[i]++;                    // Increment disconnect timer
                    nextChargerStateLocal[i] = DISCONNECTTIMER;// Remain in current state
                }
                else                                      // Cable (re)connected
                {
                    cableTimerNC[i] = 0;                  // Reset disconnect timer
                    nextChargerStateLocal[i] = ON;        // Return to ON state
                }
                break;
            }

            /* ADMIN CONTROLLED CHANGE */
            if (chargerStateServer[i] == 0)
            {
                nextChargerStateLocal[i] = OFF; // Force to OFF state
                userTimer[i] = 0;
                cableTimerC[i] = 0;
                cableTimerNC[i] = 0;
            }

            /* WRITING CHARGER PINS */
            if (chargerStateLocal[i] == ON) digitalWrite(pins[i],HIGH); // Turn on charger
            else digitalWrite(pins[i],LOW); // Turn off charger

            /* MOVE TO NEXT STATE */
            chargerStateLocal [i] = nextChargerStateLocal [i];
        }

        noConnectionTimer = 0; // ODROID has just fetched from the server, so there is a working connection

        free(chargerStateServer); // To avoid memory leaks (getChargerstate uses malloc())

    }

    /* NO CONNECTION COULD BE ESTABLISHED */
    else
    {
        /* ADD ONE TO TIMER */
        if (noConnectionTimer) noConnectionTimer++;
        else noConnectionTimer = 1; //starting without connection

        /* LOOP THROUGH ALL CHARGERS */
        for (i=0; i<6; i++)
        {
            int changesToServer[6] = {0,0,0,0,0,0};
            /* CHECK IF INTERNET HAS BEEN DOWN FOR 2 HOURS OR CABLE IS DISCONNECTED */
            if ((noConnectionTimer > 4800) || (digitalRead(detection_pins[i])))
            {
                /* TURN ALL CHARGERS OFF */
                digitalWrite(pins[i], LOW);
                chargerStateLocal[i] = OFF;
                changesToServer[i] = 1;               // Set charger to changed (off)
                writeChargeToServer(changesToServer); // Notify server of the change
            }

            /* PRINT CHARGER STATES */
            if (chargerStateLocal[i] == OFF) printf("Charger %d state: OFF \n", i+1);
            else if (chargerStateLocal[i] == USERTIMER) printf("Charger %d state: USERTIMER \n", i+1);
            else if (chargerStateLocal[i] == DISCONNECTTIMER) printf("Charger %d state: DISCONNECTTIMER \n", i+1);
            else if (chargerStateLocal[i] == CONNECTTIMER) printf("Charger %d state: CONNECTTIMER \n", i+1);
            else if (chargerStateLocal[i] == ON) printf("Charger %d state: ON \n", i+1);
        }
    }
}

int checkChargerValidity(uint16_t* chargerStateServer){
    int i;

    if (!chargerStateServer) return 0;

    for (i=0;i<6;i++){
        if (chargerStateServer[i] != 0 && chargerStateServer[i] != 1){
            FILE *f = fopen("invalid_state_log.txt", "a+");
            if (f == NULL){
                printf("Error opening file!\n");
            } else {
                time_t currentTime = time(NULL);
                fprintf(f, "\nfailed on ");
                fprintf(f, ctime(&currentTime));
                fprintf(f, "\nvalue 1: %d\n", chargerStateServer[0]);
                fprintf(f, "value 2: %d\n", chargerStateServer[1]);
                fprintf(f, "value 3: %d\n", chargerStateServer[2]);
                fprintf(f, "value 4: %d\n", chargerStateServer[3]);
                fprintf(f, "value 5: %d\n", chargerStateServer[4]);
                fprintf(f, "value 6: %d\n", chargerStateServer[5]);
                fclose(f);
                return 0;
            }
        }
    }
    return 1;
}
