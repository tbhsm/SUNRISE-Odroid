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

void readPin();

int main()
{
    readPin();
    printf("done \n");
    return 0;
}


const int ledPorts[] =
{
    24, // GPIOX.BIT0(#97)
    23, // GPIOX.BIT11(#108)
    22, // GPIOX.BIT3(#100)
    21, // GPIOX.BIT4(#101)
    14, // GPIOX.BIT8(#105)
    13, // GPIOX.BIT9(#106)
    12, // GPIOX.BIT10(#107)
    3,  // GPIOX.BIT18(#115)
//    2,  // GPIOX.BIT19(#116)
    0,  // GPIOY.BIT8(#88)
    7,  // GPIOY.BIT3(#83)

    1,  // GPIOY.BIT7(#87)
    4,  // GPIOX.BIT7(#104)
    5,  // GPIOX.BIT5(#102)
    6,  // GPIOX.BIT6(#103)
    10, // GPIOX.BIT20(#117)
    26, // GPIOX.BIT2(#99)
    11, // GPIOX.BIT21(#118)
    27, // GPIOX.BIT1(#98)
};

#define MAX_LED_CNT sizeof(ledPorts) / sizeof(ledPorts[0])

int system_init(void)
{
    int i;
    i=0;
    // GPIO Init(LED Port ALL Output(except pin 2)
    for(i = 0; i < MAX_LED_CNT; i++)    pinMode (ledPorts[i], OUTPUT);

    pinMode(2, INPUT);
    pullUpDnControl(2, PUD_UP);
    return  0;
}

void readPin()
{

    wiringPiSetup();
    if (system_init()<0)
    {
        fprintf(stderr,"Sth. went wrong. \n");
    }

    int val;
    val = digitalRead(2);
//    printf("Pin 2 has value: %d \n", val);

    if(val == 1){
        printf("Charging cable not connected \n");
    }
    else if(val == 0){
        printf("Charging cable connected \n");
    }

}
