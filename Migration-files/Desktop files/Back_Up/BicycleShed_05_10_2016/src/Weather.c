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

uint16_t* readHold(){
	//----should be in header file---//
#define SERVER_ID 1

const uint16_t UT_REGISTERS_ADDRESS = 0x0001;
/* Raise a manual exception when this address is used for the first byte */
const uint16_t UT_REGISTERS_NB = 5;
const uint16_t UT_REGISTERS_TAB[] = { 0x0001, 0x0002, 0x0003, 0x0004, 0x0005 };
/* If the following value is used, a bad response is sent.
   It's better to test with a lower value than
   UT_REGISTERS_NB_POINTS to try to raise a segfault. */


/*const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x08;
const uint16_t UT_INPUT_REGISTERS_NB = 0x6;
const uint16_t UT_INPUT_REGISTERS_TAB[] = {0x0022, 0x000D, 0x002D, 0x001D, 0x0011, 0x0030};
const float UT_REAL = 916.540649;
const uint32_t UT_IREAL = 0x4465229a;*///ERASE THIS THING
const uint16_t UT_INPUT_REGISTERS_NB = 0x6;
	// ------------DECLARATIONs--------------------//
	uint8_t *tab_rp_bits;
	uint16_t *tab_rp_registers;
	modbus_t *ctx;
	int i;
	int nb_points;
	int rc;


	/*THIS WILL NEED CHANGE ONCE THE SOLAR TEMP. IS GOING TO BE CONNECTED*/
	printf("Reading Temperature\n");
	ctx = modbus_new_rtu("/dev/ttyUSB1", 9600,'N',8,1);
	if (ctx == NULL){
		fprintf(stderr,"Unable to create libmodbus contex\n");
		//exit 1;
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
		modbus_free(ctx);
		//exit 1;
	}



	//Allocate and initialize memory to store registers
	nb_points = (UT_REGISTERS_NB > UT_INPUT_REGISTERS_NB) ? UT_REGISTERS_NB : UT_INPUT_REGISTERS_NB;
	tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
	memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));


	printf("UNIT TESTING \n");
	printf("TEST READ \n");

	rc = modbus_read_registers(ctx,UT_REGISTERS_ADDRESS, UT_REGISTERS_NB, tab_rp_registers);
	printf("modbus read registers: ");
	if(rc!=UT_REGISTERS_NB){
		printf("FAILED (nb points %d) \n",rc);
		modbus_free(ctx);
		goto close;
	}
	else{
		for (i=0; i< UT_REGISTERS_NB; i++){
	printf("OK, value %d \n",tab_rp_registers[i]);
		}
		modbus_free(ctx);
		return tab_rp_registers;
	}
	modbus_free(ctx);
	modbus_close(ctx);
	return tab_rp_registers;

	close:
	return tab_rp_registers;
	/*Free memory*/
	free(tab_rp_bits);
	free(tab_rp_registers);
	/*Close connection*/
	modbus_close(ctx);
	modbus_free(ctx);

}

uint16_t* readInput(){

	#define SERVER_ID 1


/* If the following value is used, a bad response is sent.
   It's better to test with a lower value than
   UT_REGISTERS_NB_POINTS to try to raise a segfault. */
	const uint16_t UT_REGISTERS_NB = 6;
	const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x0022;
	const uint16_t UT_INPUT_REGISTERS_NB = 0x6;
	const uint16_t UT_INPUT_REGISTERS_TAB[] = { 0x0022, 0x000D, 0x002D, 0x001D, 0x0011, 0x0030 };

	const float UT_REAL = 916.540649;
	const uint32_t UT_IREAL = 0x4465229a;

	// ------------DECLARATIONs--------------------//
	static uint16_t *tab_rp_registers;
	modbus_t *ctx;
	static int a[6];
	int i;
	int nb_points;
	int rc;


	printf("Reading Wind\n");
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
		modbus_free(ctx);
		return 0;
	}



	//Allocate and initialize memory to store registers
	nb_points = (UT_REGISTERS_NB > UT_INPUT_REGISTERS_NB) ? UT_REGISTERS_NB : UT_INPUT_REGISTERS_NB;
	tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
	memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));

	printf("UNIT TESTING \n");
	printf("TEST READ \n");

	rc = modbus_read_input_registers(ctx,UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB, tab_rp_registers);
	printf("modbus read input registers: ");

	if(rc!=UT_INPUT_REGISTERS_NB){
		printf("FAILED (nb points %d) \n",rc);
		free(ctx);
		goto exception;
		//return 0;
	}
	else{
		for (i=0; i< UT_REGISTERS_NB; i++){
	printf("OK, value %d \n",tab_rp_registers[i]);
		}
		free(ctx);
		return tab_rp_registers;
	}
	exception: // ERROR TIMEOUT IN CONNECTION
	//sleep(5);
	for (i=0;i<6;i++){
		a[i]=1+30*i;
	}
	tab_rp_registers = &a;
	//free(ctx);
	return tab_rp_registers;


}

