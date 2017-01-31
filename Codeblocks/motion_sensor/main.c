#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>

#include "main.h"

int main()
{
    //Initialisation of the pins
    wiringPiSetup();
    pinMode(5, OUTPUT);
    pinMode(4, INPUT);
    pullUpDnControl(4, PUD_OFF);
    typedef enum {OFF, ON, ON2OFF, OFF2ON} LEDStates; // define states
    static LEDStates currentLEDState = OFF; // Contains all currect states
    static LEDStates nextLEDState = OFF; // Contains all next states
    int val;

    while(1){
        val = digitalRead(4);
        printf("\nSensor value: %d\n", val);

        switch(currentLEDState){

        case OFF:
            printf("Current state: OFF\n");
            if (val == 1){
                nextLEDState = OFF2ON;
            } else {
                nextLEDState = OFF;
            }
            break;

        case OFF2ON:
            printf("Current state: OFF2ON\n");
            fadeIn();
            nextLEDState = ON;
            break;

        case ON:
            printf("Current state: ON\n");
            printf("Sleeping for 2 seconds..\n");
            sleep(2);
            if (val == 0){
                nextLEDState = ON2OFF;
            } else {
                nextLEDState = ON;
            }
            break;

        case ON2OFF:
            printf("Current state: ON2OFF\n");
            fadeOut();
            nextLEDState = OFF;
            break;

        }

        if (currentLEDState == ON) digitalWrite(5, HIGH);
        if (currentLEDState == OFF) digitalWrite(5, LOW);

        currentLEDState = nextLEDState;

        sleep(1);
    }
    return 0;
}

void fadeIn(void){
    int pulsetime = 10000;
    int timehigh, timelow;
    int i;

    printf("Fading in...\n");

    // startup pwm
    for(i=0; i<=pulsetime; i=i+40){
        timehigh = i;
        timelow = pulsetime-i;

        digitalWrite(5, LOW);
        usleep(timelow);
        digitalWrite(5, HIGH);
        usleep(timehigh);
    }

    printf("Finished fading in\n");
}

void fadeOut(void){
    int pulsetime = 10000;
    int timehigh, timelow;
    int i;

    printf("Fading out...\n");

    // startup pwm
    for(i=0; i<=pulsetime; i=i+40){
        timelow = i;
        timehigh = pulsetime-i;

        digitalWrite(5, HIGH);
        usleep(timehigh);
        digitalWrite(5, LOW);
        usleep(timelow);
    }

    printf("Finished fading out\n");
}
