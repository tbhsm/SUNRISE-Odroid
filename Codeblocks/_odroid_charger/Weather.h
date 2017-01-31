/*
 * Weather.h
 *
 *  Created on: Oct 6, 2015
 *      Author: odroid
 */

#ifndef WEATHER_H_
#define WEATHER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include </usr/local/include/modbus/modbus.h>



double* readTemperatureSensors();
double* readWeatherStation();
void ScaleWeather();
double* CastTempData();
int* castWeatherData(uint16_t *outputRegisters, int numberOfRegisters);
#endif /* WEATHER_H_ */
