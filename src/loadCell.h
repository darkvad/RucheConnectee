#ifndef LOADCELL_H
#define LOADCELL_H
#include <HX711.h>
//#include <EEPROM.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 13;
const int LOADCELL_SCK_PIN = 12;

//HX711 constructor:
HX711 LoadCell;

void load_setup();
float getWeight();
#endif