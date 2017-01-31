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

double* getData(int start, int end){

	// START THE MODBUS CONTEX
	modbus_t *ctx;
	uint16_t *tab_rp_registers;
	double *double_tab_rp_registers;
	int *int_tab_rp_registers;

	/*Variables for register manipulation*/
	int regCount = end-start+1;
	int rc;
	int i;

	/*TCP Connection*/
	ctx = modbus_new_tcp("192.168.1.3",502);
	modbus_set_debug(ctx,TRUE);
	modbus_set_slave(ctx,0);

	if (modbus_connect(ctx) == -1){
		fprintf(stderr,"Connection Failed %s \n",modbus_strerror(errno));
		modbus_free(ctx);
		exit(1);
	}

	/*After the we have connection allocate some memory*/
	tab_rp_registers = (uint16_t *) malloc(regCount * sizeof(uint16_t));
	memset(tab_rp_registers, 0, regCount * sizeof(uint16_t));

	double_tab_rp_registers = (double *) malloc(regCount * sizeof(double));
	memset(double_tab_rp_registers, 0, regCount * sizeof(double));

	int_tab_rp_registers = (int *) malloc(regCount * sizeof(int));
	memset(int_tab_rp_registers, 0, regCount * sizeof(int));

	/*Now try to read something*/
	rc = modbus_read_registers(ctx,start,regCount,tab_rp_registers);
	printf("%d", rc);
	if (rc != regCount){
		printf("ERROR : did not read properly") ;
		printf("Messed up on address : %d\n",start);
		//	goto exception;
	}

    int offset;
    if(start >= 3 && start <= 41){offset = 3;}
    else if(start >= 259 && start <= 304){offset = 220;}
    else if(start >= 771 && start <= 790){offset = 686;}
    else if(start == 800){offset = 695;}
    else if(start >= 1026 && start <= 1039){offset = 920;}
    else if(start >= 1282 && start <= 1301){offset = 1162;}
    else if(start >= 2048 && start <= 2053){offset = 1908;}
    else if(start >= 2307 && start <= 2322){offset = 2161;}
    else if(start >= 2600 && start <= 2609){offset = 2438;}
    else if(start >= 2700 && start <= 2702){offset = 2528;}
    else if(start >= 2800 && start <= 2807){offset = 2625;}
    else{fprintf(stderr, "Invalid register start value for Victron request");}

    /* cast to correct int type */
    int_tab_rp_registers = CastVictronData(start, end, tab_rp_registers);

	/* Scale Victron values based on datasheet */
	for (i = 0; i < regCount; i++){
		printf("tab_rp_registers %d, victronscale %f, ", tab_rp_registers[i], VictronScale[start-offset+i]);
		double_tab_rp_registers[i] = ((double)int_tab_rp_registers[i]) / VictronScale[start-offset+i];
		printf("tab_rp_registers %f, victronscale %f \n", double_tab_rp_registers[i], VictronScale[start-offset+i]);
	}

	/*exception: // ERROR TIMEOUT IN CONNECTION
		//sleep(5);
		for (i=0;i<43;i++){
			a[i]=1+30*i;
		}
		tab_rp_registers = &a;
		puts('I have been in exception');
	//getchar();

	/*close the connection*/
	modbus_close(ctx);
	modbus_free(ctx);

	return double_tab_rp_registers;
}

/* casts Victron register value to corresponding type */
int* CastVictronData(int start, int end, uint16_t *tab_rp_registers){
	int regCount = end-start+1;
	int *int_tab_rp_registers;
	int_tab_rp_registers = (int *) malloc(regCount * sizeof(int));
	memset(int_tab_rp_registers, 0, regCount * sizeof(int));
	int i;
	for (i = 0; i <= regCount; i++){
		// register is of type uint16_t
		if (VictronType[start+i] == 0){
			int_tab_rp_registers[i] = (int) tab_rp_registers[i];
		}
		// register is of type int16_t
		else if (VictronType[start+i] == 1){
			int16_t res = (int16_t) tab_rp_registers[i];
			int_tab_rp_registers[i] = (int) res;
		}
		// ignore registers of type int 32, string[6], string[7]
		else {//if (VictronType[start+i] == 2 || VictronType[start+i] == 3 || VictronType[start+i] == 4){
			int_tab_rp_registers[i] = 0;
		}
	}
	return int_tab_rp_registers;
}
