/**
 * IotWebConf03CustomParameters.ino -- IotWebConf is an ESP8266/ESP32
 *   non blocking WiFi/AP web configuration library for Arduino.
 *   https://github.com/prampec/IotWebConf 
 *
 * Copyright (C) 2020 Balazs Kelemen <prampec+arduino@gmail.com>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/**
 * Example: Custom parameters
 * Description:
 *   In this example it is shown how to attach your custom parameters
 *   to the config portal. Your parameters will be maintained by 
 *   IotWebConf. This means, they will be loaded from/saved to EEPROM,
 *   and will appear in the config portal.
 *   Note the configSaved and formValidator callbacks!
 *   (See previous examples for more details!)
 * 
 * Hardware setup for this example:
 *   - An LED is attached to LED_BUILTIN pin with setup On=LOW.
 *   - [Optional] A push button is attached to pin D2, the other leg of the
 *     button should be attached to GND.
 */

#include <IotWebConf.h>
#include <IotWebConfUsing.h> // This loads aliases for easier class names.
#include <stdio.h>
#include "parametres.h"
#include "configuration.h"
#include "credentials.h"

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "testThing";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "alain2505";

#define STRING_LEN 128
#define NUMBER_LEN 32

// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "alainT03"

// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to buld an AP. (E.g. in case of lost password)
#define CONFIG_PIN 2

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN LED_BUILTIN

// -- Method declarations.
void handleRoot();
// -- Callback methods.
void configSaved();
bool formValidator();

DNSServer dnsServer;
WebServer server(80);

/*
char intParamValue[NUMBER_LEN];
char floatParamValue[NUMBER_LEN];
char checkboxParamValue[STRING_LEN];
char chooserParamValue[STRING_LEN];
*/

//convert default values from #define to char
char defaultValueNbDS18B20[NUMBER_LEN];
char defaultValueSingleChannelgateway[NUMBER_LEN];
char defaultValueBMP280[NUMBER_LEN];
uint8_t defaultUintValueTempSensor1[8] = TEMP_SENSOR_1;
uint8_t defaultUintValueTempSensor2[8] = TEMP_SENSOR_2;
uint8_t defaultUintValueTempSensor3[8] = TEMP_SENSOR_3;
char defaultValueTempSensor1[NUMBER_LEN];
char defaultValueTempSensor2[NUMBER_LEN];
char defaultValueTempSensor3[NUMBER_LEN];
char defaultValueSendInterval[NUMBER_LEN];
char defaultValueAltitude[NUMBER_LEN];
u4_t defaultUintValueObjectID = DEVADDR;
char defaultValueObjectID[NUMBER_LEN];
char defaultValueRucher[STRING_LEN] = "Mon Rucher";
char defaultValueRuche[STRING_LEN] = "Ruche 01";

static char selectBMP280Values[][NUMBER_LEN] = { "0", "118", "119" };
static char selectBMP280Names[][STRING_LEN] = { "Aucun", "0x76", "0x77" };

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);
/*
// -- You can also use namespace formats e.g.: iotwebconf::TextParameter
IotWebConfTextParameter monParametre = IotWebConfTextParameter("Mon paramètre", "monParametre", monParametreValue, STRING_LEN);
IotWebConfParameterGroup group1 = IotWebConfParameterGroup("group1", "");
IotWebConfNumberParameter intParam = IotWebConfNumberParameter("Int param", "intParam", intParamValue, NUMBER_LEN, "20", "1..100", "min='1' max='100' step='1'");
// -- We can add a legend to the separator
IotWebConfParameterGroup group2 = IotWebConfParameterGroup("c_factor", "Calibration factor");
IotWebConfNumberParameter floatParam = IotWebConfNumberParameter("Float param", "floatParam", floatParamValue, NUMBER_LEN,  NULL, "e.g. 23.4", "step='0.1'");
IotWebConfCheckboxParameter checkboxParam = IotWebConfCheckboxParameter("Check param", "checkParam", checkboxParamValue, STRING_LEN,  true);
IotWebConfSelectParameter chooserParam = IotWebConfSelectParameter("Choose param", "chooseParam", chooserParamValue, STRING_LEN, (char*)chooserValues, (char*)chooserNames, sizeof(chooserValues) / STRING_LEN, STRING_LEN);
*/
int res = sprintf(defaultValueSendInterval,"%lu",SEND_INTERVAL);
int res1 = sprintf(defaultValueAltitude,"%lu",LOCAL_ALTITUDE);
int res2 = sprintf(defaultValueObjectID,"%lu",defaultUintValueObjectID);

IotWebConfParameterGroup general = IotWebConfParameterGroup("general", "Param&egrave;tres G&eacute;n&eacute;raux");
IotWebConfNumberParameter singleChannelGateway = IotWebConfNumberParameter("Num&eacute;ro de canal si passerelle monocanal", "singleChannelGateway", singleChannelGatewayValue, NUMBER_LEN, itoa(SINGLE_CHANNEL_GATEWAY,defaultValueSingleChannelgateway,10), "0..8", "min='0' max='8' step='1'");
//IotWebConfNumberParameter send_Interval = IotWebConfNumberParameter("Intervalle de messages (ms)", "send_Interval", sendIntervalValue, NUMBER_LEN, itoa(SEND_INTERVAL,defaultValueSendInterval,10), "10000..3600000", "min='10000' max='3600000' step='1000'");
IotWebConfNumberParameter send_Interval = IotWebConfNumberParameter("Intervalle de messages (ms)", "send_Interval", sendIntervalValue, NUMBER_LEN, defaultValueSendInterval, "10000..3600000", "min='10000' max='3600000' step='1000'");
IotWebConfNumberParameter altitude = IotWebConfNumberParameter("Altitude (m)", "altitude", altitudeValue, NUMBER_LEN, defaultValueAltitude, "0..9000", "min='0' max='9000' step='1'");
IotWebConfTextParameter objectID = IotWebConfTextParameter("Id Noeud Lora", "objectID", objectIDValue, STRING_LEN, defaultValueObjectID);
IotWebConfTextParameter rucher = IotWebConfTextParameter("Rucher", "rucher", rucherValue, STRING_LEN, defaultValueRucher);
IotWebConfTextParameter ruche = IotWebConfTextParameter("Ruche", "ruche", rucheValue, STRING_LEN, defaultValueRuche);
IotWebConfCheckboxParameter sleepParam = IotWebConfCheckboxParameter("Mode Sleep", "sleepParam", sleepValue, STRING_LEN,  false);


IotWebConfParameterGroup capteurs = IotWebConfParameterGroup("capteurs", "Capteurs");
IotWebConfSelectParameter BMP280 = IotWebConfSelectParameter("BMP280", "BMP280", BMP280Value, NUMBER_LEN, (char*)selectBMP280Values, (char*)selectBMP280Names, sizeof(selectBMP280Names) / STRING_LEN, STRING_LEN, itoa(BMP280_ADDRESS,defaultValueBMP280,10));
IotWebConfNumberParameter nb_DS18B20 = IotWebConfNumberParameter("Nombre de capteurs DS18B20", "nb_DS18B20", nb_DS18B20Value, NUMBER_LEN, itoa(NB_DS18B20,defaultValueNbDS18B20,10), "0..3", "min='0' max='3' step='1'");
IotWebConfTextParameter tempSensor1 = IotWebConfTextParameter("Id capteur 1", "tempSensor1", tempSensorsValues[0], STRING_LEN, defaultValueTempSensor1);
IotWebConfTextParameter tempSensor2 = IotWebConfTextParameter("Id capteur 2", "tempSensor2", tempSensorsValues[1], STRING_LEN, defaultValueTempSensor2);
IotWebConfTextParameter tempSensor3 = IotWebConfTextParameter("Id capteur 3", "tempSensor3", tempSensorsValues[2], STRING_LEN, defaultValueTempSensor3);

uint8_t* convertchartouint8array(char chaine[],uint8_t len,uint8_t* resultat) {
  char temp[3];
   for (int i = 0; i < len ; i++) {
     temp[0] = chaine[2*i];
     temp[1] = chaine[(2*i)+1];
     temp[2] = NULL;
     resultat[i] = (uint8_t)strtol(temp, NULL,16);
   }

   return resultat;
}

char *convert_uint8tochar(uint8_t tableau[], uint8_t len, char* resultat) {
  char temp[2];
  String tmp;
   for (int i = 0; i < len ; i++) {
     tmp = itoa(tableau[i],temp,16);
     if (tmp.length() == 1) {
       tmp = "0" + tmp;
     }
     resultat[2*i] = tmp[0];
     resultat[(2*i)+1] = tmp[1];
     resultat[(2*i)+2] = 0x00;
   }
   return resultat;
}


void iotwebconf_setup() 
{
  //Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  convert_uint8tochar(defaultUintValueTempSensor1,8,defaultValueTempSensor1);
  convert_uint8tochar(defaultUintValueTempSensor2,8,defaultValueTempSensor2);
  convert_uint8tochar(defaultUintValueTempSensor3,8,defaultValueTempSensor3);
  Serial.println(defaultValueTempSensor1);
  Serial.println(defaultValueTempSensor2);
  Serial.println(defaultValueTempSensor3);
  //sprintf(defaultValueNbDS18B20, "%d", NB_DS18B20);
  //sprintf(defaultValueSingleChannelgateway, "%d", SINGLE_CHANNEL_GATEWAY);
  //sprintf(defaultValueBMP280, "%#X", BMP280_ADDRESS);

  general.addItem(&singleChannelGateway);
  general.addItem(&sleepParam);
  general.addItem(&send_Interval);
  general.addItem(&altitude);
  general.addItem(&objectID);
  general.addItem(&rucher);
  general.addItem(&ruche);

  capteurs.addItem(&BMP280);
  capteurs.addItem(&nb_DS18B20);
  capteurs.addItem(&tempSensor1);
  capteurs.addItem(&tempSensor2);
  capteurs.addItem(&tempSensor3);

  iotWebConf.setStatusPin(STATUS_PIN);
  //iotWebConf.setConfigPin(CONFIG_PIN);
  //iotWebConf.addSystemParameter(&monParametre);
  iotWebConf.addParameterGroup(&general);
  iotWebConf.addParameterGroup(&capteurs);
  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setFormValidator(&formValidator);
  iotWebConf.getApTimeoutParameter()->visible = true;

  // -- Initializing the configuration.
  iotWebConf.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });

  Serial.println("Ready.");
}

void iotwebconf_loop() 
{
  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf Parameters</title></head><body>CONFIGURATION";
  s += "<ul>";
  s += "<li>singleChannelGatewayValue: ";
  s += atoi(singleChannelGatewayValue);
  s += "<li>Mode sleep: ";
  s += sleepParam.isChecked();
  s += "<li>Time between messages: ";
  s += strtoul(sendIntervalValue,NULL,10);
  s += "<li>Altitude: ";
  s += strtoul(altitudeValue,NULL,10);
  s += "</ul>";
  s += "<hr/>";
  s += "<ul>";
  s += "<li>BMP280Value: ";
  s += atoi(BMP280Value);
  s += "<li>nb_DS18B20Value: ";
  s += atoi(nb_DS18B20Value);
  s += "<li>tempSensorsValues: ";
//  s += checkboxParam.isChecked();
  s += "<li>1: ";
  s += tempSensorsValues[0];
  s += "<li>2: ";
  s += tempSensorsValues[1];
  s += "<li>3: ";
  s += tempSensorsValues[2];
  s += "</ul>";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void configSaved()
{
  Serial.println("Configuration was updated.");
  Serial.println("Rebooting after 1 second.");
  iotWebConf.delay(1000);
  ESP.restart();
}

bool formValidator()
{
  Serial.println("Validating form.");
  bool valid = true;

/*
  int l = server.arg(stringParam.getId()).length();
  if (l < 3)
  {
    stringParam.errorMessage = "Please provide at least 3 characters for this test!";
    valid = false;
  }
*/
  return valid;
}