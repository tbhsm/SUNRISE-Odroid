/*
 * ScaleTables.c
 *
 *  Created on: May 9, 2016
 *      Author: tim
 */


#include "ScaleTables.h"

double WeatherScale[112] = {
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		1,
		1,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		100,
		100,
		100,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		1000,
		10000,
		10000,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		100,
		100,
		100,
		10,
		10,
		10,
		1000,
		1,
		1,
		1,
		1,
		1,
		10,
		10,
		1,
		1,
		1
};

double VictronScale[181] = {
		10,
		10,
		10,
		10,
		10,
		10,
		100,
		100,
		100,
		0.1,
		0.1,
		0.1,
		10,
		10,
		10,
		10,
		10,
		10,
		100,
		10,
		0.1,
		0.1,
		0.1,
		100,
		10,
		1,
		1,
		10,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		100,
		100,
		10,
		10,
		100,
		100,
		-10,
		10,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		-10,
		-10,
		-10,
		1,
		1,
		-10,
		100,
		100,
		0.01,
		1,
		1,
		1,
		1,
		1,
		100,
		100,
		1,
		1,
		100,
		100,
		10,
		10,
		0.01,
		10,
		100,
		10,
		10,
		1,
		1,
		100,
		10,
		1,
		10,
		1,
		1,
		1,
		1,
		10,
		1,
		10,
		1,
		1,
		10,
		10,
		1,
		1,
		10,
		10,
		1,
		100,
		10,
		10,
		1,
		100,
		10,
		10,
		1,
		100,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		100,
		100,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		100,
		100,
		1,
		10,
		100,
		10,
		10,
		10,
		100,
		10,
		10,
		100,
		10,
		100,
		10,
		10,
		1,
		10,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		100,
		100,
		100,
		100,
		100,
		100,
		1,
		1,
		1,
		10000000,
		10000000,
		100,
		100,
		1,
		1
};

/* Coded values stored in VictronType:
 * 0 = uint16_t
 * 1 = int16_t
 * 2 = int32
 * 3 = string[6]
 * 4 = string[7]
 */
int VictronType[181] = {
		0,
		0,
		0,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		0,
		0,
		0,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		0,
		0,
		1,
		1,
		0,
		0,
		1,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		0,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		3,
		0,
		0,
		1,
		0,
		0,
		0,
		1,
		0,
		0,
		0,
		1,
		0,
		0,
		4,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		1,
		1,
		1,
		0,
		1,
		1,
		0,
		1,
		0,
		1,
		1,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		4,
		1,
		0,
		0,
		2,
		2,
		0,
		0,
		0,
		0
};