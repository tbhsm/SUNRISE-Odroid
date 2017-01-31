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
double* readWeatherStation(int registersToRead[], int tabs){

#define SERVER_ID 1

	uint16_t startRegister = 0x0000;  //Register to start reading at
    uint16_t numberOfRegisters = 112; //Total number of registers that are being read

	/* registers 2 - 6 (sensor statuses), 34 (temperature), 13 (relative humidity),
	   45 (wind speed), 30 (irradiation), 17 (relative air pressure), 48 (precipitation), 21 (wind direction) */

	/* Declarations */
	uint16_t *outputRegisters;			//To store the 112 to be read reg's from weatherstation
	int *intOutputRegisters;
	double *filteredOutputRegisters;	//To store the 12 'useful' registers to be returned
	modbus_t *ctx;						//Modbus connection struct
	int nb_points, rc, i;

	/* Setup Modbus RTU connection */
	ctx = modbus_new_rtu("/dev/ttyUSB0", 19200,'N',8,1);

	if (ctx == NULL){
		fprintf(stderr,"Unable to create libmodbus contex\n");
		return 0;
	}

	int serial = modbus_rtu_set_serial_mode(ctx,0);

	modbus_set_debug(ctx,TRUE);
	modbus_set_error_recovery(ctx,MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);

	modbus_set_slave(ctx, SERVER_ID);
	serial = modbus_rtu_get_serial_mode(ctx);

	//printf("Serial mode is %d \n",serial);

	if (modbus_connect(ctx) == -1){
		fprintf(stderr,"Connection failed: %s\n",modbus_strerror(errno));
		modbus_close(ctx);
		modbus_free(ctx);
		return 0;
	}

	/* Allocate and initialize memory to store registers */
	nb_points = numberOfRegisters;

	outputRegisters = (uint16_t *) calloc(nb_points, sizeof(uint16_t));
	intOutputRegisters = (int *) calloc(nb_points, sizeof(int));
	filteredOutputRegisters = (double *) calloc(tabs, sizeof(double));

	printf("UNIT TESTING \n");
	printf("TEST READ \n");

	rc = modbus_read_input_registers(ctx, startRegister, numberOfRegisters, outputRegisters);

	intOutputRegisters = castWeatherData(outputRegisters, numberOfRegisters);
	free(outputRegisters);

	/* Filter and scale required data */
	for (i=0; i<tabs; i++){
        //printf("reg no. %d: reg. value %f \n", i, OutputRegisters[i]);
		if (registersToRead[i] > (nb_points-1)){
			fprintf(stderr,"Requested %d weatherstation register unavailable\n", registersToRead[i]);
			return 0;
		}
		filteredOutputRegisters[i] = ((double)intOutputRegisters[registersToRead[i]])/weatherScale[registersToRead[i]];
	}

	free(intOutputRegisters);

	/* Check and print filtered values */
	printf("modbus read input weatherstation registers: \n");

	if(rc==numberOfRegisters){
		for (i=0; i<tabs; i++){
			printf("OK, value %f \n",filteredOutputRegisters[i]);
		}
		modbus_close(ctx);
		modbus_free(ctx);
		return filteredOutputRegisters;
	} else {
		printf("FAILED (nb points %d) \n",rc);
		modbus_close(ctx);
		modbus_free(ctx);

		free(filteredOutputRegisters);
		return 0;
    }

}

/* Casts weather register value to corresponding type (see weatherstation datasheet) */
int* castWeatherData(uint16_t *outputRegisters, int numberOfRegisters){

	int *intOutputRegisters;

	intOutputRegisters = (int *) malloc(numberOfRegisters * sizeof(int));
	memset(intOutputRegisters, 0, numberOfRegisters * sizeof(int));

	int i;
	for (i = 0; i < numberOfRegisters; i++){
		// register is of type uint16_t
		if (weatherType[i] == 0){
			intOutputRegisters[i] = (int) outputRegisters[i];
		}
		// register is of type int16_t
		else if (weatherType[i] == 1){
			int16_t res = (int16_t) outputRegisters[i];
			intOutputRegisters[i] = (int) res;
		}
		// catch any incorrect variables
		else {
			fprintf(stderr, "Something went wrong in casting weather data \n");
		}
	}
	return intOutputRegisters;
}
