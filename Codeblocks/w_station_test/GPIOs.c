
/*
 * GPIOs.c
 *
 *  Created on: Oct 22, 2015
 *      Author: odroid
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


#define DATA_UPDATE_PERIOD  100 // 100ms

#define PORT_ADC1   0   // ADC.AIN0

static int adcValue = 0;

static int ledPos = 0;

const int ledPorts[] = {
		24, // GPIOX.BIT0(#97)
		23, // GPIOX.BIT11(#108)
		22, // GPIOX.BIT3(#100)
		21, // GPIOX.BIT4(#101)
		14, // GPIOX.BIT8(#105)
		13, // GPIOX.BIT9(#106)
		12, // GPIOX.BIT10(#107)
		3,  // GPIOX.BIT18(#115)
		2,  // GPIOX.BIT19(#116)
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
/*System initialize*/
int system_init(void)
{
	int i;
	i=0;
	// GPIO Init(LED Port ALL Output)
	for(i = 0; i < MAX_LED_CNT; i++)    pinMode (ledPorts[i], OUTPUT);

	return  0;
}

/*Read ADC value*/
void boardDataUpdate(void)
{
	int i;

	// adc value read
	if((adcValue = analogRead (PORT_ADC1)))    {
		ledPos = (adcValue * MAX_LED_CNT * 1000) / 1024;
		ledPos = (MAX_LED_CNT - (ledPos / 1000));
	}
	else
		ledPos = 0;

	//  LED Control
	for(i = 0; i < MAX_LED_CNT; i++)    digitalWrite (ledPorts[i], 0); // LED All Clear
	for(i = 0; i < ledPos;      i++)    digitalWrite (ledPorts[i], 1); // LED On
}

void LEDBlink()
{
	static int timer = 0;
	int i;
	int j;

	wiringPiSetup();
	if (system_init()<0)
	{
		fprintf(stderr,"Sth. went wrong. \n");
	}
	j=1;
	for (i=0;i<15;i++)
	{
		digitalWrite(1,LOW); //turn off
		sleep(1);
		digitalWrite(1,HIGH);//turn on
		sleep(1);
	}
}
