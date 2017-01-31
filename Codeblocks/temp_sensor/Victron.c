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

uint16_t* getData(int start, int end){
	/*DEFINE VARIABLES*/
#define LOOP             1
#define SERVER_ID        0 /*used to be 17*/
#define ADDRESS_START    start
#define ADDRESS_END      end
	// START THE MODBUS CONTEX
	modbus_t *ctx;
	/*Memory management variables*/
	//	uint16_t *tab_rq_registers;
	//	uint16_t *tab_rw_rq_registers;
	uint16_t *tab_rp_registers;
	/*Variables for register manipulation*/
	int nb;
	int rc;
	int addr;
	int slave;
	int i;
	int loop = ADDRESS_END-ADDRESS_START;

	/*TCP Connection*/
	ctx = modbus_new_tcp("192.168.1.3",502);
	modbus_set_debug(ctx,TRUE);
	slave = modbus_set_slave(ctx,0);

	if (modbus_connect(ctx) == -1){
		fprintf(stderr,"Connection Failed %s \n",modbus_strerror(errno));
		modbus_free(ctx);
		exit(1);
	}

	/*After the we have connection allocate some memory*/
	nb = loop;
	//static int a[43];
	/*tab_rq_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rq_registers, 0, nb * sizeof(uint16_t));*/

	tab_rp_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rp_registers, 0, nb * sizeof(uint16_t));

	/*	tab_rw_rq_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rw_rq_registers, 0, nb * sizeof(uint16_t));*/



	addr=ADDRESS_START;

	/*Some random numbers to the memory for fun*/
	/*	for (i=0; i<nb; i++) {
		tab_rq_registers[i] = (uint16_t) (65535.0*rand() / (RAND_MAX + 1.0));
		tab_rw_rq_registers[i] = ~tab_rq_registers[i];

	}*/

	/*Now try to read something*/
	rc = modbus_read_registers(ctx,addr,loop,tab_rp_registers);
	if (rc != loop){
		printf("ERROR : did not read properly") ;
		printf("Messed up on address : %d\n",addr);
		//	goto exception;
	}
	else {
		for (i=0; i<loop; i++) {
			printf("%" PRIu16 "\n",tab_rp_registers[i]);}
	}

	/* Scale Victron values based on datasheet */
	for (i = 0; i < nb; i++){
		tab_rp_registers[i] /= VictronScale[start+i];
	}
	/*exception: // ERROR TIMEOUT IN CONNECTION
		//sleep(5);
		for (i=0;i<43;i++){
			a[i]=1+30*i;
		}
		tab_rp_registers = &a;
		puts('I have been in exception');
	//getchar();
	/*free what we can free*/
	// free(tab_rq_registers); deal with this shit
	// free(tab_rw_rq_registers);
	/*close the connection*/
	modbus_close(ctx);
	modbus_free(ctx);

	return tab_rp_registers;
}

