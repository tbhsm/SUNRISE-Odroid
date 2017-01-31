#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ScaleTables.h"

int main()
{
    //If negative value is read out of the victron it is casted to uint16;
    //Casting back to int16 result in a negative value

    //int16_t g = -23;
    uint16_t h = 40000;
    int16_t i = (int16_t)h;
    printf("h has value %d and i has value %d \n", h, i);

    int j = (int)i;
    printf("j has value %d \n", j);

    return 0;
}
