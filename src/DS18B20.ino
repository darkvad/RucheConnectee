/*

  DS18B20 temerature sensors module

  This program has been written with the help of Rui Santos sample code
  find on randomnerdtutorial.com :
  https://randomnerdtutorials.com/esp32-multiple-ds18b20-temperature-sensors/
  
  Copyright (C) 2021 by Alain Vadon

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
#if NB_DS18B20 > 0
#include <OneWire.h>
#include <DallasTemperature.h>
#include "function.h"

char temp_char[32]; // used to sprintf for Serial output
DeviceAddress mySensors[3] = {TEMP_SENSOR_1, TEMP_SENSOR_2, TEMP_SENSOR_3};
//DeviceAddress sensor2 = TEMP_SENSOR_2;
//DeviceAddress sensor3 = TEMP_SENSOR_3;

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress Thermometer;

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void findDevices(char ident[17],int pin)
{
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  if (!sensors.getAddress(Thermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(Thermometer);
  Serial.println();
  for (uint8_t i = 0; i < 8; i++)
  {
    if (Thermometer[i] < 16) Serial.print("0");
    Serial.print(Thermometer[i], HEX);
    sprintf(&ident[i*2],"%02x",Thermometer[i]);
  }
  
}


void ds18b20_setup(void){
  //Serial.begin(115200);
  sensors.begin();
  for (int i = 0; i < atoi(nb_DS18B20Value); i++) {
    convertchartouint8array(tempSensorsValues[i],8,mySensors[i]);
  }
}

void getTemp(uint8_t txBuffer[21]) {

  float temperature;
  uint16_t payloadTemp;

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  for (int i = 0; i < atoi(nb_DS18B20Value); i++) {

    float temperature = sensors.getTempC(mySensors[i]);

    sprintf(temp_char, "Temperature: %f", temperature);
    Serial.println(temp_char);

      // adjust for the f2sflt16 range (-1 to 1)
    temperature = temperature / 100;

    // float -> int
    uint16_t payloadTemp = LMIC_f2sflt16(temperature);

    // int -> bytes
    byte tempLow = lowByte(payloadTemp);
    byte tempHigh = highByte(payloadTemp);

    txBuffer[11+(2*i)] = tempLow;
    txBuffer[12+(2*i)] = tempHigh;

  }

}
#endif
