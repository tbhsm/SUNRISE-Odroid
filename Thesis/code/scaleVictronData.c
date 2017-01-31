  	/* SCALE VICTRON VALUES BASED ON DATASHEET */
	for (i = 0; i < regCount; i++){
		victronFinalValues[i] = ((double)victronCastValues[i]) / victronScale[start-offset+i];
		printf("tab_rp_registers %f, victronscale %f \n", victronFinalValues[i], victronScale[start-offset+i]);
	}