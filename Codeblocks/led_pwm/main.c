#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

int main()
{
    wiringPiSetup();
    pinMode(5, OUTPUT);

//    int dutycycle;
//    printf("Hello world!\n");
//    printf("Enter the dutycycle (0-100)\n");
//    scanf("%d", &dutycycle);
//
//    if(dutycycle > 100 || dutycycle < 0){
//        printf("Error, invalid dutycycle");
//    }

    int pulsetime = 10000;
    int timehigh, timelow;

//    timehigh = (int)(((double)dutycycle)/100*pulsetime);
//    timelow = pulsetime - timehigh;
//
//    printf("timehigh %d, timelow %d", timehigh, timelow);


//    while(1){
//        digitalWrite(5, HIGH);
//        usleep(timehigh);
//        digitalWrite(5, LOW);
//        usleep(timelow);
//    }


    //startup pwm
    int i;
    for(i=0; i<=10000; i=i+40){
        timehigh = i;
        timelow = 10000-i;

        digitalWrite(5, HIGH);
        usleep(timehigh);
        digitalWrite(5, LOW);
        usleep(timelow);
    }

    printf("done\n");
    return 0;
}
