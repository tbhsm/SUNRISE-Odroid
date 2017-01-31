void setChargers(){
	//Get the state of the chargers from the database
	uint16_t* chargerstate = getChargerState(0);

    //WiringPi pin numbers used
    int pins[] = {6, 10, 11, 12, 13, 14};
    int i;
    //Turn on the physical chargers
    for(i=0; i<6; i++){
        printf("charger %d: %d\n", i+1, chargerstate[i]);
        digitalWrite(pins[i], chargerstate[i]);
    }
    
    free(chargerstate); //To avoid memory leaks (getChargerstate uses malloc())
}