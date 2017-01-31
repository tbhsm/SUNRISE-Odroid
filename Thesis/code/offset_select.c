	/* Select correct offset */
    int offset; 
    if(start >= 3 && start <= 41){offset = 3;}
    else if(start >= 259 && start <= 304){offset = 220;}    //Batteries
    /*Some more lines of else if(start >= x && start <= y){offset = z};*/
    else if(start >= 2800 && start <= 2807){offset = 2625;}
    else{
        fprintf(stderr, "Invalid register start value for Victron request");
        return 0;
    }