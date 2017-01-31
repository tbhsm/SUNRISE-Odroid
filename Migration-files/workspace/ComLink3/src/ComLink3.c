/*
 ============================================================================
 Name        : ComLink3.c
 Author      : Palinecko
 Version     :
 Copyright   : Free stuff
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <modbus.h>
#include <stdint.h>

#include "readDatabase.h"
#include "Victron.h"


int main(void) {


	puts("!!!Hello welcome in Matrix. You did a wise choice with the blue pill!!!"); /* prints !!!Hello Future!!! */
	int addr=1;

	uint16_t *tab_rp_registers;
	uint16_t a=600;
	tab_rp_registers = &a;//getData(3,4);

	char buf[50];
	char buf2[50];

	writeData(3,4,tab_rp_registers);
	sprintf(buf,"UPDATE victron SET VALUE= %d", tab_rp_registers[1]);
	sprintf(buf2," WHERE Address= %d", addr);
	strcat(buf,buf2);

	printf("%s \n", buf);



	/*Memory management stuff variables*/
	/*uint16_t *tab_rp_registers;
	tab_rp_registers=getData(ADDRESS_START,ADDRESS_END);
	/* Free the memory */
//	free(tab_rp_registers);


	return EXIT_SUCCESS;
}
