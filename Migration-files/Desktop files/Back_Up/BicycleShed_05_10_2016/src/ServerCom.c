/*
 * ServerCom.c
 *
 *  Created on: Oct 13, 2015
 *      Author: odroid
 *      Description: Routines to communicate with the Server.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include </usr/local/include/modbus/modbus.h>
#include <stdint.h>
#include <curl/curl.h>
#include <sys/syscall.h>
#include <fcntl.h>
/*--------------------TEST----------------------------*/
int writeToServer(){
	CURL *curl  = curl_easy_init();
	if (curl){
		CURLcode res;
		curl_easy_setopt(curl,CURLOPT_URL,"http://solarpoweredbikes.tudelft.nl/test/php/testing.php?a=3&b=4");
		res 	= curl_easy_perform(curl);
		curl_easy_cleanup(curl);
}
	return 1;
}
/*-----------------WEATHER DATA--------------------------*/
int writeWeatherToServer(uint16_t *weather){
	CURL *curl  = curl_easy_init();
	if (curl){
		CURLcode res;
		char link[100];
		int vys = sprintf(link,"http://solarpoweredbikes.tudelft.nl/test/php/Pavel.php?w1=%d&w2=%d&w3=%d&w4=%d&w5=%d&w6=%d",*(weather)/10,*(weather+1)/10,*(weather+2)/10,*(weather+3)/10,*(weather+4)/10,*(weather+5)/10);///10 is the scalefactor.
		puts(link);
		curl_easy_setopt(curl,CURLOPT_URL,link);
		res 	= curl_easy_perform(curl);
		curl_easy_cleanup(curl);
}
	return 1;
}
/*----------------------SOLAR TEMPERATURE------------------*/
int writeSolarToServer(uint16_t *solar){
	CURL *curl  = curl_easy_init();
	if (curl){
		CURLcode res;
		char link[300];
		int vys = sprintf(link,"http://solarpoweredbikes.tudelft.nl/test/php/SolarTemperature.php?w1=%d&w2=%d&w3=%d&w4=%d&w5=%d&w6=%d",*(solar),*(solar+1),*(solar+2),*(solar+3),*(solar+4),*(solar+5));///10 is maybe the scalefactor,who knows.
		puts(link);
		curl_easy_setopt(curl,CURLOPT_URL,link);
		res 	= curl_easy_perform(curl);
		curl_easy_cleanup(curl);
}
	return 1;
}
/*----------------------SOLAR TEMPERATURE REGISTERS---------*/
/*int writeVictron1ToServer(uint16_t *solar){
	CURL *curl  = curl_easy_init();
	if (curl){
		CURLcode res;
		char link[100];
		int vys = sprintf(link,"http://solarpoweredbikes.tudelft.nl/test/php/SolarTemperature.php?w1=%d&w2=%d&w3=%d&w4=%d&w5=%d&w6=%d",*(solar),*(solar+1),*(solar+2),*(solar+3),*(solar+4),*(solar+5));///10 is maybe the scalefactor,who knows.
		puts(link);
		curl_easy_setopt(curl,CURLOPT_URL,link);
		res 	= curl_easy_perform(curl);
		curl_easy_cleanup(curl);
}
	return 1;
}*/
/*----------------------CHARGER STATES-------------------*/
function_pt(void *ptr, size_t size,size_t nmemb,void *ress){
	char **response_ptr = (char**)ress;
	/*the response should be a string*/
	*response_ptr = strndup(ptr,(size_t)(size *nmemb));
	//return nmemb;

}
int getChargerState(int n){
	CURL *curl  = curl_easy_init();
	//char a;
	char look[100];
	char *ress = NULL;


	if (curl){
			CURLcode res;
			int vys = sprintf(look,"http://solarpoweredbikes.tudelft.nl/test/php/PavelCharger.php?n=%d",n);
			curl_easy_setopt(curl,CURLOPT_URL,look);
			curl_easy_setopt(curl,CURLOPT_HTTPGET,1);
			curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);
			curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,function_pt);
			curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ress);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
	}

	return ress;
}
/*----------------------VICTRON DATA chunk no.1---------------------*/
int writeVictron1ToServer(uint16_t *victron){
	CURL *curl  = curl_easy_init();
	if (curl){
		CURLcode res;
		char link[400];
		int vys = sprintf(link,"http://solarpoweredbikes.tudelft.nl/test/php/Victron1.php"
				"?w1=%d&w2=%d&w3=%d&w4=%d&w5=%d&w6=%d&w7=%d&w8=%d&w9=%d&w10=%d&w11=%d"
				"&w12=%d&w13=%d&w14=%d&w15=%d&w16=%d&w17=%d&w18=%d&w19=%d&w20=%d&w21=%d"
				"&w22=%d&w23=%d&w24=%d&w25=%d&w26=%d&w27=%d&w28=%d&w29=%d&w30=%d&w31=%d"
				"&w32=%d&w33=%d&w34=%d&w35=%d",*(victron),*(victron+1),*(victron+2),*(victron+3)
				,*(victron+4),*(victron+5),*(victron+6),*(victron+7),*(victron+8),*(victron+9)
				,*(victron+10),*(victron+11),*(victron+12),*(victron+13),*(victron+14),*(victron+15)
				,*(victron+16),*(victron+17),*(victron+18),*(victron+19),*(victron+20),*(victron+21)
				,*(victron+22),*(victron+23),*(victron+24),*(victron+25),*(victron+26),*(victron+27)
				,*(victron+28),*(victron+29),*(victron+30),*(victron+31),*(victron+32),*(victron+33)
				,*(victron+34));
		curl_easy_setopt(curl,CURLOPT_URL,link);
		res 	= curl_easy_perform(curl);
		curl_easy_cleanup(curl);

}
	return 1;
}
/*----------------------VICTRON DATA chunk no.2---------------------*/
int writeVictron2ToServer(uint16_t *victron){
	CURL *curl  = curl_easy_init();
	if (curl){
		CURLcode res;
		char link[500];
		int vys = sprintf(link,"http://solarpoweredbikes.tudelft.nl/test/php/Victron2.php"
				"?w1=%d&w2=%d&w3=%d&w4=%d&w5=%d&w6=%d&w7=%d&w8=%d&w9=%d&w10=%d&w11=%d"
				"&w12=%d&w13=%d&w14=%d&w15=%d&w16=%d&w17=%d&w18=%d&w19=%d&w20=%d&w21=%d"
				"&w22=%d&w23=%d&w24=%d&w25=%d&w26=%d&w27=%d&w28=%d&w29=%d&w30=%d&w31=%d"
				"&w32=%d&w33=%d&w34=%d&w35=%d&w36=%d&w37=%d&w38=%d&w39=%d&w40=%d&w41=%d&w42=%d"
				"&w43=%d",*(victron),*(victron+1),*(victron+2),*(victron+3)
				,*(victron+4),*(victron+5),*(victron+6),*(victron+7),*(victron+8),*(victron+9)
				,*(victron+10),*(victron+11),*(victron+12),*(victron+13),*(victron+14),*(victron+15)
				,*(victron+16),*(victron+17),*(victron+18),*(victron+19),*(victron+20),*(victron+21)
				,*(victron+22),*(victron+23),*(victron+24),*(victron+25),*(victron+26),*(victron+27)
				,*(victron+28),*(victron+29),*(victron+30),*(victron+31),*(victron+32),*(victron+33)
				,*(victron+34),*(victron+35),*(victron+36),*(victron+37),*(victron+38),*(victron+39),*(victron+40)
				,*(victron+41),*(victron+42));
		curl_easy_setopt(curl,CURLOPT_URL,link);
		res 	= curl_easy_perform(curl);
		curl_easy_cleanup(curl);
}
	return 1;
}
/*----------------------VICTRON DATA chunk no.3---------------------*/
int writeVictron3ToServer(uint16_t *victron){
	CURL *curl  = curl_easy_init();
	if (curl){
		CURLcode res;
		char link[400];
		int vys = sprintf(link,"http://solarpoweredbikes.tudelft.nl/test/php/Victron3.php"
				"?w1=%d&w2=%d&w3=%d&w4=%d&w5=%d&w6=%d&w7=%d&w8=%d",*(victron),*(victron+1),*(victron+2),*(victron+3)
				,*(victron+4),*(victron+5),*(victron+6),*(victron+7));
		curl_easy_setopt(curl,CURLOPT_URL,link);
		res 	= curl_easy_perform(curl);
		curl_easy_cleanup(curl);
}
	return 1;
}
