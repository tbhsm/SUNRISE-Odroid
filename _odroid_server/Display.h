/*
 * Display.h
 *
 *  Created on: Oct 7, 2015
 *      Author: odroid
 *      Description: These routines are used to display data on the local monitor in form of html.
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdio.h>

//void htmlDisplay(); //each 15 sec
void weatherDisplay(int sendToServer); //each 10 mins
#endif /* DISPLAY_H_ */
