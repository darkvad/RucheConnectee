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
#include <string>
#include <SPIFFS.h>
#include "parametres.h"
#include "configuration.h"
#include "secrets/credentials.h"
#include "function.h"
#include "style.h"

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
static const char PROGMEM thingName[] = "BeeConn";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
static const char PROGMEM wifiInitialApPassword[] = "bzzzzz";

// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "conf_01"

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
char defaultValueOffset[NUMBER_LEN];
char defaultValueCalibration[NUMBER_LEN];

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
int res = sprintf(defaultValueSendInterval,"%u",SEND_INTERVAL);
int res1 = sprintf(defaultValueAltitude,"%.0f",LOCAL_ALTITUDE);
int res2 = sprintf(defaultValueObjectID,"%u",defaultUintValueObjectID);

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
IotWebConfNumberParameter offset = IotWebConfNumberParameter("Offset balance", "offset", offsetValue, NUMBER_LEN, "-8301", "...", "min='-50000' max='50000' step='0.01'");
IotWebConfNumberParameter calibration = IotWebConfNumberParameter("Calibration balance", "calibration", calibrationValue, NUMBER_LEN, "-52.83", "...", "min='-50000' max='50000' step='0.01'");

String s;

uint8_t* convertchartouint8array(char chaine[],uint8_t len,uint8_t* resultat) {
  char temp[3];
   for (int i = 0; i < len ; i++) {
     temp[0] = chaine[2*i];
     temp[1] = chaine[(2*i)+1];
//     temp[2] = NULL;
     temp[2] = 0x00;
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

class CustomHtmlFormatProvider : public iotwebconf::HtmlFormatProvider
{
public:
  String getHeadExtension() { 
    return "<style>" + String(FPSTR(style)) + "</style>"; 
  }

protected:
  String getBodyInner() override
  {
    return
      String(FPSTR(CUSTOMHTML_BODY_INNER)) +
      HtmlFormatProvider::getBodyInner();
  }
  
};
// -- An instance must be created from the class defined above.
CustomHtmlFormatProvider customHtmlFormatProvider;

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
  capteurs.addItem(&offset);
  capteurs.addItem(&calibration);

  iotWebConf.setStatusPin(STATUS_PIN);
  //iotWebConf.setConfigPin(CONFIG_PIN);
  //iotWebConf.addSystemParameter(&monParametre);
  iotWebConf.addParameterGroup(&general);
  iotWebConf.addParameterGroup(&capteurs);
  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setFormValidator(&formValidator);
  iotWebConf.getApTimeoutParameter()->visible = true;

  iotWebConf.setHtmlFormatProvider(&customHtmlFormatProvider);
  // -- Initializing the configuration.
  iotWebConf.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/identify", handleOneWire);
  server.on("/balance", handleBalance);
  server.on("/tare", handleTare);
  server.on("/calibrate", handleCalibrate);
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
 * Handle web requests to "/identify" path.
 */
void handleOneWire()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  char device[17];
  findDevices(device,ONE_WIRE_BUS);
  s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>Identification DS18B20</title><style>";
  s += style;
  s += "</style></head><body>";
  s += String(FPSTR(CUSTOMHTML_BODY_INNER));
  s += "<H1>DS18B20</H1>";
  s += "<p>Premier device trouv&eacute : ";
  s += device;
  s += "<br></p>Aller &agrave; la <a href='/'>page d'&eacutetat</a>.<br>";
  s += "Aller &agrave; la <a href='config'>page de configuration</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

/**
 * Handle web requests to "/balance" path.
 */
void handleBalance()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>R&eacute;glages Balance</title><style>";
  s += style;
  s += "</style></head><body>";
  s += String(FPSTR(CUSTOMHTML_BODY_INNER));
  s += "<H1>BALANCE</H1>";
  s += "<p>Commencer par faire la tare : ";
  s += "<a href='tare'>TARE</a>.";
  s += "<p>Puis la calibration apr&egrave;s avoir indiqu&eacute; le poids connu : ";
  s += "<form action=\"calibrate\" method=\"get\">";
  s += "<label for=\"poids\">Poids mis (gr): </label><input type=\"number\" name=\"poids\" id=\"poids\" min=\"0\" max=\"50000\"><br><input type=\"submit\" value=\"Calibrer\"></form><br>";
  s += "<br></p>Aller &agrave; la <a href='/'>page d'&eacutetat</a>.<br>";
  s += "Aller &agrave; la <a href='config'>page de configuration</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

/**
 * Handle web requests to "/identify" path.
 */
void handleTare()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  float valtare;
  valtare = procTare();
  s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>Tare Balance</title><style>";
  s += style;
  s += "</style></head><body>";
  s += String(FPSTR(CUSTOMHTML_BODY_INNER));
  s += "<H1>TARE</H1>";
  s += "<p>Valeur de l'offset &agrave; copier dans la configuration : ";
  s += valtare;
  s += "<br></p>Aller &agrave; la <a href='/'>page d'&eacutetat</a>.<br>";
  s += "Aller &agrave; la <a href='config'>page de configuration</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

/**
 * Handle web requests to "/identify" path.
 */
void handleCalibrate()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  
  //Serial.println("parametres recu ?");
  //Serial.println(server.arg("poids"));

  float valcal;
  valcal = procCalibration(server.arg("poids").toInt());
  s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>Calibration Balance</title><style>";
  s += style;
  s += "</style></head><body>";
  s += String(FPSTR(CUSTOMHTML_BODY_INNER));
  s += "<H1>CALIBRATION</H1>";
  s += "<p>Valeur de calibration &agrave; copier dans la configuration: ";
  s += valcal;
  s += "<br></p>Aller &agrave; la <a href='/'>page d'&eacutetat</a>.<br>";
  s += "Aller &agrave; la <a href='config'>page de configuration</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
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
  s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>Param&egrave;tre BeeConnect</title><style>";
  s += String(FPSTR(style));
  s += "</style></head><body>";
  s += String(FPSTR(CUSTOMHTML_BODY_INNER));
  s += "<H1>CONFIGURATION</H1>";
  s += "<H2>G&eacute;n&eacute;raux</H2><ul>";
  s += "<li>Canal pour gateway monocanal : ";
  s += atoi(singleChannelGatewayValue);
  s += "<li>Mode veille : ";
  s += sleepParam.isChecked();
  s += "<li>D&eacute;lai entre message : ";
  s += strtoul(sendIntervalValue,NULL,10);
  s += "<li>Object ID : ";
  s += atoi(objectIDValue);
  s += "<li>Rucher : ";
  s += rucherValue;
  s += "<li>Ruche : ";
  s += rucheValue;
  s += "<li>Altitude ruche : ";
  s += strtoul(altitudeValue,NULL,10);
  s += "</ul>";
  s += "<hr/>";
  s += "<H2>Configuration Capteurs</H2>";
  s += "<ul>";
  s += "<li>Adresse BME/BMP 280 : ";
  s += atoi(BMP280Value);
  s += "<li>Nombre de DS18B20 : ";
  s += atoi(nb_DS18B20Value);
  s += "<li>Identifiants capteurs de temp&eacute;rature: ";
  s += "<li>1 : ";
  s += tempSensorsValues[0];
  s += "<li>2 : ";
  s += tempSensorsValues[1];
  s += "<li>3 : ";
  s += tempSensorsValues[2];
  s += "</ul><HR><H2>Configuration Balance</H2>";
  s += "<ul>";
  s += "<li>Offset balance (tare) : ";
  s += ((String)offsetValue).toFloat();
  s += "<li>Calibration balance : ";
  s += ((String)calibrationValue).toFloat();;
  s += "</ul><HR>";
  s += "Aller &agrave; la <a href='config'>page de configuration</a> pour changer les valeurs.<br>";
  s += "Aller &agrave; la <a href='identify'>page DS18B20</a> pour trouver les identifiants des capteurs DS18B20<br>";
  s += "Aller &agrave; la <a href='balance'>page balance</a> pour trouver les valeurs de tare et de calibration.<br>";
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