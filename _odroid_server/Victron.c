/*
 ============================================================================
 Name        : Victron.c
 Author      : odroid
 Version     : 2.0
 Created on  : Oct. 1, 2015
 Description : This part contains functions to read data from the Victron system. The contex is Modbus/TCP
 Revised     : Tim Hosman & Job van Staveren (18-04-2016 to 01-06-2016)
 ============================================================================
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
#include "BicycleShed.h"

/* REQUEST REGISTERS FROM VICTRON THROUGH MODBUS */
// reads all registers between and including values of 'start' and 'end'
int readVictron(int slave_id, int start, int end){

	/* DECLARATIONS */
	modbus_t *ctx;
	int regCount = end-start+1; //number of registers to be read
	int rc;
	int i;
    int offset;

	/* SELECT CORRECT OFFSET */
    if(start >= 3 && start <= 41) offset = 3;
    else if(start >= 259 && start <= 304) offset = 220;
    else if(start >= 771 && start <= 790) offset = 686;
    else if(start == 800) offset = 695;
    else if(start >= 1026 && start <= 1039) offset = 920;
    else if(start >= 1282 && start <= 1301) offset = 1162;
    else if(start >= 2048 && start <= 2053) offset = 1908;
    else if(start >= 2307 && start <= 2322) offset = 2161;
    else if(start >= 2600 && start <= 2609) offset = 2438;
    else if(start >= 2700 && start <= 2702) offset = 2528;
    else if(start >= 2800 && start <= 2807) offset = 2625;
    else{
        fprintf(stderr, "Invalid register start value for Victron request");
        return 0;
    }


	/* SETUP TCP CONNECTION */
	ctx = modbus_new_tcp("192.168.1.3",502);
	modbus_set_debug(ctx,TRUE);
	modbus_set_slave(ctx,slave_id);

    /* CHECK IF CONNECTION SUCCEEDED */
	if (modbus_connect(ctx) == -1){
		fprintf(stderr,"Connection Failed %s \n",modbus_strerror(errno));
        modbus_close(ctx);
		modbus_free(ctx);
		return 0; //return failure
	}

	/* REQUEST DATA FROM VICTRON SYSTEM */
	rc = modbus_read_registers(ctx, start, regCount, victronReadValues);
	printf("%d", rc);

	/* CHECK LENGTH OF RECEIVED DATA ARRAY */
	if (rc != regCount){
        //connection was lost during the sending of data
        modbus_close(ctx);
		modbus_free(ctx);
		printf("ERROR : did not read properly") ;
		printf("Messed up on address : %d\n",start);
		return 0; //return failure
	}

    /* CAST TO CORRECT INT TYPE */
    castVictronData(start-offset, end-offset);

	/* SCALE VICTRON VALUES BASED ON DATASHEET */
	for (i = 0; i < regCount; i++){
		victronFinalValues[i] = ((double)victronCastValues[i]) / victronScale[start-offset+i];
		printf("tab_rp_registers %f, victronscale %f \n", victronFinalValues[i], victronScale[start-offset+i]);
	}

	/* CLOSE THE CONNECTION */
	modbus_close(ctx);
	modbus_free(ctx);

	return 1;
}

/* CASTS VICTRON REGISTER VALUE TO CORRESPONDING TYPE (SEE VICTRON DATASHEET) */
int castVictronData(int start, int end){

	int regCount = end-start+1;
	int i;

	for (i = 0; i < regCount; i++){
		// register is of type uint16_t
		if (victronType[start+i] == 0){
			victronCastValues[i] = (int) victronReadValues[i];
		}
		// register is of type int16_t
		else if (victronType[start+i] == 1){
			int16_t res = (int16_t) victronReadValues[i];
			victronCastValues[i] = (int) res;
		}
		// ignore registers of type int 32, string[6], string[7]
		else {//if (VictronType[start+i] == 2 || VictronType[start+i] == 3 || VictronType[start+i] == 4){
			victronCastValues[i] = 0;
		}
	}
	return 1;
}
