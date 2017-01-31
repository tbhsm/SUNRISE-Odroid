/* CASTS VICTRON REGISTER VALUE TO CORRESPONDING TYPE (SEE VICTRON DATASHEET) */
int castVictronData(int start, int end){

	int regCount = end-start+1;
	int i;

	for (i = 0; i < regCount; i++){
		// register is of type uint16_t
		if (victronType[start+i] == 0){
			victronCastValues[i] = (int) victronReadValues[i];
		}
		// register is of type int16_t
		else if (victronType[start+i] == 1){
			int16_t res = (int16_t) victronReadValues[i];
			victronCastValues[i] = (int) res;
		}
		// ignore registers of type int 32, string[6], string[7]
		else {//if (VictronType[start+i] == 2 || VictronType[start+i] == 3 || VictronType[start+i] == 4){
			victronCastValues[i] = 0;
		}
	}
	return 1;
}