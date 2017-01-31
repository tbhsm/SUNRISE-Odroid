/*
 * Weather.c
 *
 *  Created on: Oct 6, 2015
 *      Author: odroid
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include </usr/local/include/modbus/modbus.h>
#include <stdint.h>

#include "Weather.h"
#include "ScaleTables.h"

/* read solar panel temperature values */
uint16_t* readHold(){
return 0;

}

/* read values of weather station */
double* readInput(int registers[], int tabs){

#define SERVER_ID 1

	/* If the following value is used, a bad response is sent.
   It's better to test with a lower value than
   UT_REGISTERS_NB_POINTS to try to raise a segfault. */
	const uint16_t UT_REGISTERS_NB = 112;
	const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x0000;
	const uint16_t UT_INPUT_REGISTERS_NB = 112;
	//const uint16_t UT_INPUT_REGISTERS_TAB[] = { 0x2, 0x3, 0x4, 0x5, 0x6, 0x22, 0xD, 0x2D, 0x1E, 0x11, 0x30, 0x15 };
	/* registers 2 - 6 (sensor statuses), 34 (temperature), 13 (relative humidity),
	   45 (wind speed), 30 (irradiation), 17 (relative air pressure), 48 (precipitation), 21 (wind direction) */

	// ------------DECLARATIONs--------------------//
	uint16_t *tab_rp_registers;
	double *filtered_tab_rp_registers;
	modbus_t *ctx;
	double *a;
	int nb_points;
	int rc;
	int i;

	ctx = modbus_new_rtu("/dev/ttyUSB0", 19200,'N',8,1);
	if (ctx == NULL){
		fprintf(stderr,"Unable to create libmodbus contex\n");
		return 0;
	}
	int serial = modbus_rtu_set_serial_mode(ctx,0);

	printf("Serial mode is %d \n",serial);

	modbus_set_debug(ctx,TRUE);
	modbus_set_error_recovery(ctx,MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);

	modbus_set_slave(ctx, SERVER_ID);
	serial = modbus_rtu_get_serial_mode(ctx);

	printf("Serial mode is %d \n",serial);

	if (modbus_connect(ctx) == -1){
		fprintf(stderr,"Connection failed: %s\n",modbus_strerror(errno));
		modbus_close(ctx);
		modbus_free(ctx);
		return 0;
	}

	/* Allocate and initialize memory to store registers */
	nb_points = UT_REGISTERS_NB; //(UT_REGISTERS_NB > UT_INPUT_REGISTERS_NB) ? UT_REGISTERS_NB : UT_INPUT_REGISTERS_NB;

    //tab_rp_registers = (double*)(nb_points*sizeof(uint16_t));


	tab_rp_registers = (uint16_t *) calloc(nb_points, sizeof(uint16_t));
	filtered_tab_rp_registers = (double *) calloc(tabs, sizeof(double));

	printf("UNIT TESTING \n");
	printf("TEST READ \n");


	rc = modbus_read_input_registers(ctx,UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB, tab_rp_registers);

	for (i=0;i<rc;i++){printf("register %d: %d \n", i, tab_rp_registers[i]);}

	/* Filter and scale required data */
	for (i=0; i<tabs; i++){
        //printf("reg no. %d: reg. value %f \n", i, tab_rp_registers[i]);
		if (registers[i] > (nb_points-1)){
			fprintf(stderr,"Requested %d weatherstation register unavailable\n", registers[i]);
			return 0;
		}
		filtered_tab_rp_registers[i] = ((double)tab_rp_registers[registers[i]])/WeatherScale[registers[i]];
	}

	free(tab_rp_registers);

	printf("modbus read input weatherstation registers: \n");

	if(rc==UT_INPUT_REGISTERS_NB){
//		for (i=0; i< tabs; i++){
//			printf("OK, value %f \n",filtered_tab_rp_registers[i]);
//		}
		modbus_close(ctx);
		modbus_free(ctx);
		return filtered_tab_rp_registers;
	}
	else{
        printf("FAILED (nb points %d) \n",rc);
        modbus_close(ctx);
        modbus_free(ctx);

        a = (double *) calloc(tabs, sizeof(double));

        for (i=0; i<tabs; i++){
            a[i]=1+30*i;
            printf("i: %d \n",i);
        }

        free(filtered_tab_rp_registers);
        return a;
    }

}
