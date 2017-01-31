/*
 * Victron.c
 *
 *  Created on: Sep 29, 2015
 *      Author: odroid
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <modbus.h>
#include <stdint.h>

uint16_t* getData(int start, int end){
	/*DEFINE SOME USEFUL STUFF*/
	#define LOOP             1
	#define SERVER_ID        0 /*used to be 17*/
	#define ADDRESS_START    start
	#define ADDRESS_END      end

	modbus_t *ctx;
	/*Memory management stuff variables*/
	uint16_t *tab_rq_registers;
	uint16_t *tab_rw_rq_registers;
	uint16_t *tab_rp_registers;
	/*useful variables for register manipulation*/
	int nb;
	int rc;
	int addr;
	int slave;
	int i;
	int loop = ADDRESS_END-ADDRESS_START;

	/*TCP Connection*/
	ctx = modbus_new_tcp("192.168.1.186",502);
	modbus_set_debug(ctx,TRUE);
	slave = modbus_set_slave(ctx,0);

	if (modbus_connect(ctx) == -1){
		fprintf(stderr,"Connection Failed %s \n",modbus_strerror(errno));
		modbus_free(ctx);
		exit(1);
	}

	/*After the we have connection allocate some memory*/
	nb = loop;

	tab_rq_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rq_registers, 0, nb * sizeof(uint16_t));

	tab_rp_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rp_registers, 0, nb * sizeof(uint16_t));

	tab_rw_rq_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rw_rq_registers, 0, nb * sizeof(uint16_t));



	addr=ADDRESS_START;

	/*Some random numbers to the memory for fun*/
	for (i=0; i<nb; i++) {
		tab_rq_registers[i] = (uint16_t) (65535.0*rand() / (RAND_MAX + 1.0));
		tab_rw_rq_registers[i] = ~tab_rq_registers[i];

	}

	/*Now try to read something*/
	rc = modbus_read_registers(ctx,addr,loop,tab_rp_registers);
	if (rc != loop){
		printf("ERROR : did not read properly") ;
		printf("Messed up on address : %d\n",addr);
	}
	else {
		for (i=0; i<loop; i++) {
			printf("%" PRIu16 "\n",tab_rp_registers[i]);}
	}
	//getchar();
	/*free what we can free*/
	 free(tab_rq_registers);
	 free(tab_rw_rq_registers);
	/*close the connection*/
	modbus_close(ctx);
	modbus_free(ctx);

return tab_rp_registers;
}
