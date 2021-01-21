/*
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
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