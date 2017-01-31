/*
 * Victron.c
 *
 *  Created on: Oct 1, 2015
 *      Author: odroid
 *      Description : This part contains functions to read data from the Victron system. The contex is Modbus/TCP
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include </usr/local/include/modbus/modbus.h>
#include <stdint.h>
#include "ScaleTables.h"
#include "Victron.h"

double* readVictron(int start, int end){

	/* Declarations */
	modbus_t *ctx;
	uint16_t *outputRegisters;
	double *doubleOutputRegisters;
	int *intOutputRegisters;
	int regCount = end-start+1; //number of registers to be read
	int rc;
	int i;

	/* Select correct offset and slave ID */
    int offset, slave_id = 0; //Remove initialization of slave_id if it is know for all start values
    if(start >= 3 && start <= 41){offset = 3, slave_id = 0;}
    else if(start >= 259 && start <= 304){offset = 220, slave_id = 247;}    //Batteries, device_instance = 256
    else if(start >= 771 && start <= 790){offset = 686, slave_id = 0;}
    else if(start == 800){offset = 695;}
    else if(start >= 1026 && start <= 1039){offset = 920;}
    else if(start >= 1282 && start <= 1301){offset = 1162;}
    else if(start >= 2048 && start <= 2053){offset = 1908;}
    else if(start >= 2307 && start <= 2322){offset = 2161;}
    else if(start >= 2600 && start <= 2609){offset = 2438;}
    else if(start >= 2700 && start <= 2702){offset = 2528;}
    else if(start >= 2800 && start <= 2807){offset = 2625;}
    else{
        fprintf(stderr, "Invalid register start value for Victron request");
        return 0;
    }


	/* Setup TCP Connection */
	ctx = modbus_new_tcp("192.168.1.3",502);
	modbus_set_debug(ctx,TRUE);
	modbus_set_slave(ctx,slave_id);

	if (modbus_connect(ctx) == -1){
		fprintf(stderr,"Connection Failed %s \n",modbus_strerror(errno));
        modbus_close(ctx);
		modbus_free(ctx);
		return 0;//exit(1);
	}

	/* Allocate and initialize memory to store registers */
	outputRegisters = (uint16_t *) malloc(regCount * sizeof(uint16_t));
	memset(outputRegisters, 0, regCount * sizeof(uint16_t));

	doubleOutputRegisters = (double *) malloc(regCount * sizeof(double));
	memset(doubleOutputRegisters, 0, regCount * sizeof(double));

	intOutputRegisters = (int *) malloc(regCount * sizeof(int));
	memset(intOutputRegisters, 0, regCount * sizeof(int));

	/* Request data from Victron system */
	rc = modbus_read_registers(ctx,start, regCount, outputRegisters);
	printf("%d", rc);
	if (rc != regCount){ //connection was lost during the sending of data
        free(outputRegisters);
        free(intOutputRegisters);
        free(doubleOutputRegisters);
        modbus_close(ctx);
		modbus_free(ctx);
		printf("ERROR : did not read properly") ;
		printf("Messed up on address : %d\n",start);
		return 0;
	}

    /* cast to correct int type */
    intOutputRegisters = castVictronData(start-offset, end-offset, outputRegisters);

	/* Scale Victron values based on datasheet */
	for (i = 0; i < regCount; i++){
		//printf("tab_rp_registers %d, victronscale %f, ", outputRegisters[i], victronScale[start-offset+i]);
		doubleOutputRegisters[i] = ((double)intOutputRegisters[i]) / victronScale[start-offset+i];
		printf("tab_rp_registers %f, victronscale %f \n", doubleOutputRegisters[i], victronScale[start-offset+i]);
	}

	/* Close the connection */
	modbus_close(ctx);
	modbus_free(ctx);
	free(outputRegisters);
	free(intOutputRegisters);

	return doubleOutputRegisters;
}

/* Casts Victron register value to corresponding type (see Victron datasheet) */
int* castVictronData(int start, int end, uint16_t *outputRegisters){

	int regCount = end-start+1;
	int *intOutputRegisters;

	intOutputRegisters = (int *) malloc(regCount * sizeof(int));
	memset(intOutputRegisters, 0, regCount * sizeof(int));

	int i;
	for (i = 0; i < regCount; i++){
		// register is of type uint16_t
		if (victronType[start+i] == 0){
			intOutputRegisters[i] = (int) outputRegisters[i];
		}
		// register is of type int16_t
		else if (victronType[start+i] == 1){
			int16_t res = (int16_t) outputRegisters[i];
			intOutputRegisters[i] = (int) res;
		}
		// ignore registers of type int 32, string[6], string[7]
		else {//if (VictronType[start+i] == 2 || VictronType[start+i] == 3 || VictronType[start+i] == 4){
			intOutputRegisters[i] = 0;
		}
	}
	return intOutputRegisters;
}
