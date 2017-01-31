/*
 * readDatabase.h
 *
 *  Created on: Oct 1, 2015
 *      Author: odroid
 *      Description: This is only used fro MySQL on ODROID. In current application it is not used.
 */

#ifndef READDATABASE_H_
#define READDATABASE_H_


#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void readData();
void writeData(int START, int END, uint16_t *tab_register);
void writeWeatherData(uint16_t *tab_register);
int* readCharger(int *f);

#endif /* READDATABASE_H_ */
