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

#define SERVER_ID 1         //Server ID for the ADAM Module with temp-sens. is 1
#define NB_SENSORS 6        //Number of temperature sensors

/* read solar panel temperature values */
double* readTemperatureSensors(){

    //Start address to start reading sensors
	uint16_t start_addr = 0x0;

	//Declarations
	uint16_t *temperatures;         //To store the temperatures
	double *cast_temperatures;    //To store casted vlaues
	double *scaled_temperatures;    //Temperatures need to be scaled
	modbus_t *ctx;                  //Modbus connection struct
	int i, rc;

    //ADAM module is connected to USB port 1
	ctx = modbus_new_rtu("/dev/ttyUSB1", 9600,'N',8,1);
	if (ctx == NULL){
		fprintf(stderr,"Unable to create libmodbus contex\n");
	}

    /*Standard Modbus connections routines*/
	//int serial = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);//mode might also be 0

	modbus_set_debug(ctx,TRUE);
	modbus_set_error_recovery(ctx,MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);
	modbus_set_slave(ctx, SERVER_ID);
	//serial = modbus_rtu_get_serial_mode(ctx);

    //Check connection
	if (modbus_connect(ctx) == -1){
		fprintf(stderr,"Connection failed: %s\n",modbus_strerror(errno));
		modbus_free(ctx);
		return 0;
	}

	//Allocate memory to store the temperature values
	temperatures = (uint16_t *) calloc(NB_SENSORS, sizeof(uint16_t));
	scaled_temperatures = (double *) calloc(NB_SENSORS, sizeof(double));

    //Number of succesfully read registers is returned, otherwise rc=-1
	rc = modbus_read_registers(ctx, start_addr, NB_SENSORS, temperatures);
	if(rc!=NB_SENSORS){
		printf("FAILED (rc=%d) \n",rc);
	}

	//Cast from uint16_t to double
    cast_temperatures = CastTempData(temperatures);

    //Perform the necessary scaling
    for(i = 0; i < NB_SENSORS; i++){
        scaled_temperatures[i] = cast_temperatures[i]/1000;
    }

	//Close the connection, free alloc'd memory and return read values
	modbus_close(ctx);
	modbus_free(ctx);
	free(temperatures);
	free(cast_temperatures);
	return scaled_temperatures;

}

double* CastTempData(uint16_t *temperatures){

    //Cast the uint16_t array with temperatures to double
    double *double_temperatures;
    double_temperatures = (double *) calloc(NB_SENSORS, sizeof(double));

    int i;
    for(i = 0; i < NB_SENSORS; i++){
        double_temperatures[i] =(double)((int)((int16_t)temperatures[i]));
    }

    return double_temperatures;
}

/* read values of weather station */
uint16_t* readInput(int registers[]){

	return 0;
}
