/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor
  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650
  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!
  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "parametres.h"

#ifdef BMP280_ADDRESS
#include <Adafruit_BME280.h>

uint32_t humidity_bme;
uint32_t pressure_bme;
uint32_t temperature_bme;
float loc_altitude;
#endif

union {
  uint32_t x;
  byte dataArray[4];
} myCount;

#ifdef BMP280_ADDRESS

char bme_char[32]; // used to sprintf for Serial output

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

void bme_setup() {
    //Serial.begin(115200);
    bool status;
    status = bme.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
    Serial.println("filter off");
    loc_altitude = strtoul(altitudeValue,NULL,10);

    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF );

}

float humidity() {
    return bme.readHumidity();
}

float pressure() {
    return bme.readPressure();
}

float temperature() {
    return bme.readTemperature();
}

#endif

void buildPacket(uint8_t txBuffer[21])
{
  char buffer[40];

  //
  // Relais
  //

  if (relais_on == true)
  {
    txBuffer[0] = ( 01 ) & 0xFF;
    Serial.println("Relais = On");
  }
  else
  {
    txBuffer[0] = ( 00 ) & 0xFF;
    Serial.println("Relais = Off");
  }

#ifdef BMP280_ADDRESS
if (bmp280_found) {

  //Activer le capteur en mode forced pour le réveiller
  bme.takeForcedMeasurement();

  //
  // Humidity
  //

  //float humidity = random(1,100); //Alain bme.readHumidity();
  float humidity = bme.readHumidity();
  
  sprintf(bme_char, "Humidity: %f", humidity);
  Serial.println(bme_char);
  
  snprintf(buffer, sizeof(buffer), "Humidity: %10.1f\n", humidity);
  screen_print(buffer);

  // adjust for the f2sflt16 range (-1 to 1)
  humidity = humidity / 100;

  // float -> int
  uint16_t payloadHum = LMIC_f2sflt16(humidity);

  // int -> bytes
  byte humLow = lowByte(payloadHum);
  byte humHigh = highByte(payloadHum);

  txBuffer[1] = humLow;
  txBuffer[2] = humHigh;


  //
  // Pressure
  //

  //float pressure =random(800,1200); //Alain  bme.readPressure();
  //sprintf(bme_char, "altitude: %f", loc_altitude);
  //Serial.println(bme_char);
  //float correction = loc_altitude / 8.3f;
  //sprintf(bme_char, "correction: %f", correction);
  //Serial.println(bme_char);
  float pressure = bme.readPressure() + (loc_altitude / 8.3f * 100.0f);
  sprintf(bme_char, "Pressure: %f", pressure / 100);
  Serial.println(bme_char);

  snprintf(buffer, sizeof(buffer), "Pressure: %10.1f\n", pressure / 100);
  screen_print(buffer);

  // adjust for the f2sflt16 range (-1 to 1)
  pressure = pressure / 10000000;

  // float -> int
  uint16_t payloadPress = LMIC_f2sflt16(pressure);

  // int -> bytes
  byte pressLow = lowByte(payloadPress);
  byte pressHigh = highByte(payloadPress);

  txBuffer[3] = pressLow;
  txBuffer[4] = pressHigh;


  //
  // Temperature
  //

  //float temperature =random(-30,60); //Alain  bme.readTemperature();
  float temperature = bme.readTemperature();

  sprintf(bme_char, "Temperature: %f", temperature);
  Serial.println(bme_char);

  snprintf(buffer, sizeof(buffer), "Temperature: %10.1f\n", temperature);
  screen_print(buffer);

  // adjust for the f2sflt16 range (-1 to 1)
  temperature = temperature / 100;

  // float -> int
  uint16_t payloadTemp = LMIC_f2sflt16(temperature);

  // int -> bytes
  byte tempLow = lowByte(payloadTemp);
  byte tempHigh = highByte(payloadTemp);

  txBuffer[5] = tempLow;
  txBuffer[6] = tempHigh;
}
#endif

  // msg counter
  myCount.x = count;
  txBuffer[7] = myCount.dataArray[0];
  txBuffer[8] = myCount.dataArray[1];
  txBuffer[9] = myCount.dataArray[2];
  txBuffer[10] = myCount.dataArray[3];

#if NB_DS18B20 > 0
if (atoi(nb_DS18B20Value) > 0) {
  // lecture temperature
  getTemp(txBuffer);
}
#endif

  //
  // weight
  //

  float weight = getWeight();

  sprintf(bme_char, "poids: %f", weight);
  Serial.println(bme_char);

  snprintf(buffer, sizeof(buffer), "Poids: %10.1f\n", weight);
  screen_print(buffer);

  // adjust for the f2sflt16 range (-1 to 1)
  weight = weight / 1000000;

  // float -> int
  uint16_t payloadWeight = LMIC_f2sflt16(weight);

  // int -> bytes
  byte weightLow = lowByte(payloadWeight);
  byte weightHigh = highByte(payloadWeight);

  txBuffer[17] = weightLow;
  txBuffer[18] = weightHigh;

  float vbat = roundf(1000.0 * 2.0 * (3.6 / 1024) * analogRead(BATTERY_PIN));
  sprintf(bme_char, "batterie: %f", vbat);
  Serial.println(bme_char);

  snprintf(buffer, sizeof(buffer), "Bat: %10.0f\n", vbat);
  screen_print(buffer);

  // adjust for the f2sflt16 range (-1 to 1)
  vbat = vbat / 10000;

  // float -> int
  uint16_t payloadVbat = LMIC_f2sflt16(vbat);

  // int -> bytes
  byte vbatLow = lowByte(payloadVbat);
  byte vbatHigh = highByte(payloadVbat);

  txBuffer[19] = vbatLow;
  txBuffer[20] = vbatHigh;

}
