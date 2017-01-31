/*
 * readDatabase.h
 *
 *  Created on: Sep 28, 2015
 *      Author: odroid
 */

#ifndef READDATABASE_H_
#define READDATABASE_H_

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void readData();
void writeData(int START, int END, uint16_t *tab_register);

#endif /* READDATABASE_H_ */
