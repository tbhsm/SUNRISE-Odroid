/*
 * Weather.c
 *
 *  Created on: Oct 6, 2015
 *      Author: odroid
 */

/*
 ============================================================================
 Name        : Weather.c
 Author      : Palinecko
 Version     : 2.0
 Created on  : Oct. 6, 2015
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
#include <math.h>

#include "Weather.h"
#include "ScaleTables.h"
#include "BicycleShed.h"

#define SERVER_ID 1         //Server ID for the ADAM Module with temp-sens. is 1

/* READ SOLAR PANEL TEMPERATURE VALUES */
int readTemperatureSensors()
{

    /* START ADDRESS TO START READING SENSORS */
    uint16_t start_addr = 0x0;
    int numberOfRegisters = 6; //maximum number of sensors

    /* DECLARATIONS */
    modbus_t *ctx; //Modbus connection struct
    int i, rc;

    /* SETUP CONNECTION WITH ADAM MODULE */
    ctx = modbus_new_rtu("/dev/ttyUSB0", 9600,'N',8,1); // is connected to USB port 1
    if (ctx == NULL)
    {
        fprintf(stderr,"Unable to create libmodbus contex\n");
        return 0; //return failure
    }

    /* STANDARD MODBUS CONNECTIONS ROUTINES */
    modbus_set_debug(ctx,TRUE);
    modbus_set_error_recovery(ctx,MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);
    modbus_set_slave(ctx, SERVER_ID);

    /* CHECK CONNECTION */
    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr,"Connection failed: %s\n",modbus_strerror(errno));
        modbus_free(ctx);
        return 0; //return failure
    }

    /* REQUEST ALL DATA FROM ADAM MODULE */
    rc = modbus_read_registers(ctx, start_addr, numberOfRegisters, tempReadValues);

    /* CHECK IF RECEIVED LENGTH CORRESPONDS WITH REQUESTED LENGTH */
    if(rc!=numberOfRegisters)
    {
        printf("FAILED (rc=%d) \n",rc);
        modbus_close(ctx);
        modbus_free(ctx);
        return 0; //return failure
    }

    /* CAST FROM UINT16_T TO INT */
    CastTempData(numberOfRegisters);

    /* PERFORM THE NECESSARY SCALING */
    for(i = 0; i < numberOfRegisters; i++)
    {
        double tmp = ((double)tempCastValues[i])/65535*200-50; //scale linearly between -50 and 150 degrees
        tempFinalValues[i] = roundf(tmp*100)/100; //round to two decimals
    }

    /* CLOSE THE CONNECTION */
    modbus_close(ctx);
    modbus_free(ctx);
    return 1; //return success

}

/* FUNCTION TO CAST TEMPERATURE DATA */
int CastTempData(int numberOfRegisters)
{

    //Cast the uint16_t array with temperatures to int
    int i;
    for(i = 0; i < numberOfRegisters; i++)
    {
        tempCastValues[i] = (int)tempReadValues[i];
    }

    return 1;
}

/* READ VALUES OF WEATHER STATION */
int readWeatherStation(int registersToRead[], int tabs)
{

#define SERVER_ID 1

    /* READ DECLARATIONS */
    uint16_t startRegister = 0x0000;  //Register to start reading at
    uint16_t numberOfRegisters = 112; //Total number of registers that are being read

    /* registers 2 - 6 (sensor statuses), 34 (temperature), 13 (relative humidity),
       45 (wind speed), 30 (irradiation), 17 (relative air pressure), 48 (precipitation), 21 (wind direction) */

    /* DECLARATIONS */
    modbus_t *ctx; //Modbus connection struct
    int nb_points, rc, i;
    nb_points = numberOfRegisters;

    /* SETUP MODBUS RTU CONNECTION */
    ctx = modbus_new_rtu("/dev/ttyUSB1", 19200,'N',8,1);

    /* CHECK IF MODBUS CAN BE CREATED */
    if (ctx == NULL)
    {
        fprintf(stderr,"Unable to create libmodbus contex\n");
        return 0; //return failure
    }

    /* STANDARD MODBUS CONNECTIONS ROUTINES */
    modbus_set_debug(ctx,TRUE);
    modbus_set_error_recovery(ctx,MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);

    modbus_set_slave(ctx, SERVER_ID);

    /* CHECK IF CONNECTED */
    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr,"Connection failed: %s\n",modbus_strerror(errno));
        modbus_free(ctx);
        return 0; //return failure
    }

    /* REQUEST MODBUS REGISTERS */
    rc = modbus_read_input_registers(ctx, startRegister, numberOfRegisters, weatherReadValues);

    /* CAST RECEIVED DATA */
    castWeatherData(numberOfRegisters);

    /* FILTER AND SCALE REQUIRED DATA */
    for (i=0; i<tabs; i++)
    {
        // requested register exceeds range of registers that is available
        if (registersToRead[i] > (nb_points-1))
        {
            fprintf(stderr,"Requested %d weatherstation register unavailable\n", registersToRead[i]);
            modbus_close(ctx);
            modbus_free(ctx);
            return 0; //return failure
        }
        //filter and scale
        weatherFinalValues[i] = ((double)weatherCastValues[registersToRead[i]])/weatherScale[registersToRead[i]];
    }

    /* CHECK LENGTH OF RECEIVED DATA AND PRINT FILTERED VALUES */
    printf("modbus read input weatherstation registers: \n");

    if(rc==numberOfRegisters)
    {
        for (i=0; i<tabs; i++)
        {
            printf("OK, value %f \n",weatherFinalValues[i]);
        }
        modbus_close(ctx);
        modbus_free(ctx);
        return 1; //return success
    }
    else
    {
        printf("FAILED (nb points %d) \n",rc);
        modbus_close(ctx);
        modbus_free(ctx);

        return 0; //return failure
    }
}

/* CASTS WEATHER REGISTER VALUE TO CORRESPONDING TYPE (SEE WEATHERSTATION DATASHEET) */
int castWeatherData(int numberOfRegisters)
{
    int i;
    for (i = 0; i < numberOfRegisters; i++)
    {
        // register is of type uint16_t
        if (weatherType[i] == 0)
        {
            weatherCastValues[i] = (int) weatherReadValues[i];
        }
        // register is of type int16_t
        else if (weatherType[i] == 1)
        {
            int16_t res = (int16_t) weatherReadValues[i];
            weatherCastValues[i] = (int) res;
        }
        // catch any incorrect variables, some programmer messed up!
        else
        {
            fprintf(stderr, "Something went wrong in casting weather data \n");
            exit(1);
        }
    }
    return 1; //return failure
}
