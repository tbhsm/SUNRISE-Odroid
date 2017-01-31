    /* READ VEBUS DATA FROM VICTRON AND SEND THEM TO THE SERVER */
    if (readVictron(0,3,38))
    {
        writeVictron1ToServer(victronFinalValues); //Victron VEBUS data
    }