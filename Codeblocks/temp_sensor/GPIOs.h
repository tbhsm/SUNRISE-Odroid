
/*
 *  Created on: Oct 22, 2015
 *      Author: odroid
 *      Description: Testing and control of GPIO's. These should be used control the chargers.
 */

#ifndef GPIOS_H_
#define GPIOS_H_

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

void LEDBlink();

#endif /* GPIOS_H_ */
