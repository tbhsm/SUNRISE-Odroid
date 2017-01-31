/*
 * Victron.h
 *
 *  Created on: Oct 1, 2015
 *      Author: odroid
 */

#ifndef VICTRON_H_
#define VICTRON_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <modbus.h>

int readVictron(int slave_id, int start, int end); //in order to use the output it has to be reasigned with &
//void putData(uint16_t *hold_registers);
int castVictronData(int start, int end);


#endif /* VICTRON_H_ */
