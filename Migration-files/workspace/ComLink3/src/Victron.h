/*
 * Victron.h
 *
 *  Created on: Sep 29, 2015
 *      Author: odroid
 */

#ifndef VICTRON_H_
#define VICTRON_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <modbus.h>
#include <stdint.h>

uint16_t* getData(int start, int end); //in order to use the output it has to be reasigned with &
//void putData(uint16_t *hold_registers);


#endif /* VICTRON_H_ */
