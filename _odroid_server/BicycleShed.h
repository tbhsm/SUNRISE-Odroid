#ifndef BICYCLESHED_H_INCLUDED
#define BICYCLESHED_H_INCLUDED


void functionDisplayData();
void functionWeatherData();
void functionSolarData();
void functionVictronData();

/* VICTRON */
extern uint16_t victronReadValues[];     // Holds returned values of all requested Victron registers
extern int victronCastValues[];         // Holds values of all requested Victron registers after casting them to the appropriate type
extern double victronFinalValues[];     // Holds the cast Victron values after they are scaled

/* WEATHER STATION */
extern uint16_t weatherReadValues[];   // Holds returned values of all requested weather station registers
extern int weatherCastValues[];        // Holds values of all requested weather station registers after casting them to the appropriate type
extern double weatherFinalValues[];	   // Holds the cast weather station values after they are scaled

/* TEMPERATURE SENSORS */
extern uint16_t tempReadValues[];        // Holds returned values of all temperature sensor registers
extern int tempCastValues[];             // Holds values of all temperature sensor registers after casting them to the appropriate type
extern double tempFinalValues[];         // Holds the cast temperature sensor values after they are scaled


#endif // BICYCLESHED_H_INCLUDED
