    ...
	static int noConnectionTimer;
	...
	
	/*  SET THE PHYSICAL CHARGERS */
    if(chargerStateServer)
    {
        ...
		noConnectionTimer = 0;
		...
    }
    /* NO CONNECTION COULD BE ESTABLISHED */
    else
    {
        /* ADD ONE TO TIMER */
        if (noConnectionTimer) noConnectionTimer++;
        else noConnectionTimer = 1; //starting without connection

        /* CHECK IF CABLE IS UNPLUGGED */
        for (i=0; i<6; i++){
            if (digitalRead(detection_pins[i])) // Cable disconnected
                ...
        }

        /* CHECK IF TIMER HAS BEEN OFF FOR 2 HOURS */
        if (noConnectionTimer > 4800)
        {
            /* TURN ALL CHARGERS OFF */
            for(i=0; i<6; i++)
                ...
        }
    }