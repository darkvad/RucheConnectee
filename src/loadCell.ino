/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017
   -------------------------------------------------------------------------------------
*/

/*
   Settling time (number of samples) and data filtering can be adjusted in the config.h file
   For calibration and storing the calibration value in eeprom, see example file "Calibration.ino"

   The update() function checks for new data and starts the next conversion. In order to acheive maximum effective
   sample rate, update() should be called at least as often as the HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS.
   If you have other time consuming code running (i.e. a graphical LCD), consider calling update() from an interrupt routine,
   see example file "Read_1x_load_cell_interrupt_driven.ino".

   This is derived from an example sketch on how to use this library

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

#include "loadCell.h"
long t = 0;

void load_setup() {
  //Serial.begin(115200); delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = -52.83; // uncomment this if you want to set the calibration value in the sketch
  float offset; // valeur de la tare
  offset = -8301;
  
  LoadCell.power_up();

  if (LoadCell.wait_ready_timeout(1000)) {
    LoadCell.set_scale(calibrationValue);
    LoadCell.set_offset(offset);
    Serial.println("Startup is complete");
  } else {
    Serial.println("HX711 not found.");
  }

}

float getWeight() {
  float result = 0;
  result = roundf(LoadCell.get_units(15));
  result = std::max(0.0f,result);
  return result;
}

void load_loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity


  if (LoadCell.wait_ready_timeout(1000)) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.get_units(15);
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      t = millis();
    }
  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tare();
  }

  Serial.println("Tare complete");
  Serial.println(LoadCell.get_offset());

}
