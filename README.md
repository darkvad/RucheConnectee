:warning: Attention il s'agit d'un travail en cours :exclamation:

# Ruches connectées

Ce projet a pour intention de créer un système de ruches connectées permettant de remonter des mesures
vers une stack LoraWan.
Le mode de connexion testé est ABP (OTAA à tester avec une vraie passerelle)
Il est pour le moment basé sur un module TTGO Lora32 qui intègre un ESP32, un composant Lora (sx1276)
et une gestion de batterie et de sa charge.
On y adjoint un module hx711 permettant de convertir le signal d'une ou plusieurs cellules de charges
branchées en parallèles.
On y rajoute (voir PCB) un module de charge optionnel pour un panneau solaire (pas de pad pour le connecter
sauf à le connecter sur la prise USB).
On peut y connecter sur un bus one Wire jusqu'à 3 capteurs de température de types DS18B20
Un capteur de type BME280 (SDA/SCL).
Une configuration au travers d'un point d'accès WIFI est possible.
Le mode sleep est géré mais c'est un module qui consomme et le système de charge solaire est recommandé.

## Librairies utilisées

- mcci-catena/MCCI Arduino LoRaWAN Library [ici](https://github.com/mcci-catena/arduino-lorawan)
- mcci-catena/MCCI LoRaWAN LMIC library [ici](https://github.com/mcci-catena/arduino-lmic)
- paulstoffregen/OneWire [ici](https://github.com/PaulStoffregen/OneWire)
- adafruit/Adafruit BME280 Library [ici](https://github.com/adafruit/Adafruit_BME280_Library)
- thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays [ici](https://github.com/ThingPulse/esp8266-oled-ssd1306)
- mcci-catena/MCCI Arduino Development Kit ADK [ici](https://github.com/mcci-catena/Catena-mcciadk)
- milesburton/DallasTemperature [ici](https://github.com/milesburton/Arduino-Temperature-Control-Library)
- prampec/IotWebConf [ici](https://github.com/prampec/IotWebConf)
- bogde/HX711 [ici](https://github.com/bogde/HX711)

## Lora Stack (TTN ou The Things Stack V3)

### Payload Format

Les données collectées par les capteurs sont transmises encodées en Bytes dans la payload.
Il est nécessaire de les décoder au niveau application afin de récupérer les données et pouvoir
les transmettre à votre intégration (MQTT par exemple vers InfluxDb).
Voici le payload formatter nécessaire, il peut êtr emis au niveau Device ou au niveau Application :
```
// TTN Decoder for TTN OTAA Feather US915 DHT22 Sketch
// Link: https://github.com/mcci-catena/arduino-lmic/blob/master/examples/ttn-otaa-feather-us915-dht22/ttn-otaa-feather-us915-dht22.ino
function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};
  
  // Relais
  rawTemp = bytes[0]*1;
  if (rawTemp === 0) {
    decoded.relais = "Off";
  } else {
    decoded.relais = "on";
  }
  
  // humidity 
  
  rawHumid = bytes[1] + bytes[2] * 256;
  decoded.humidity = Math.round(sflt162f(rawHumid) * 100);
  
  // Air pressure 
  
  rawHumid = bytes[3] + bytes[4] * 256;
  decoded.AirPressure = Math.round(sflt162f(rawHumid) * 100000);
  
  // temperature 

  rawTemp = bytes[5] + bytes[6] * 256;
  
  decoded.degreesC = (Math.round(sflt162f(rawTemp) * 100 * 10) / 10);
  
 //compteur
// rawCount = bytes[7] + bytes[8] * 256 + bytes[9] * 65536 + bytes[10] * 16777216;
//  decoded.count = rawCount;
  //decoded.count0 = bytes[7];
  //decoded.count1 = bytes[8];
  //decoded.count2 = bytes[9];
  //decoded.count3 = bytes[10];
 
 //frequence et amplitude
  rawFreq = bytes[7] + bytes[8] * 256;
  decoded.frequence = Math.round(sflt162f(rawFreq) * 100000);

  rawMagn = bytes[9] + bytes[10] * 256;
  decoded.magnitude = Math.round(sflt162f(rawMagn) * 1000);

  
    // temperatures DS18b20 

  rawDS18B20_1 = bytes[11] + bytes[12] * 256;
  decoded.DS_1_degreesC = (Math.round(sflt162f(rawDS18B20_1) * 100 * 10) / 10);

  rawDS18B20_2 = bytes[13] + bytes[14] * 256;
  decoded.DS_2_degreesC = (Math.round(sflt162f(rawDS18B20_2) * 100 * 10) / 10);

  rawDS18B20_3 = bytes[15] + bytes[16] * 256;
  decoded.DS_3_degreesC = (Math.round(sflt162f(rawDS18B20_3) * 100 * 10) / 10);
  
  // Poids
  rawWeight = bytes[17] + bytes[18] * 256;
  decoded.Poids = Math.round(sflt162f(rawWeight) * 1000000);

  // Batterie
  rawVbat = bytes[19] + bytes[20] * 256;
  decoded.Vbat = Math.round(sflt162f(rawVbat) * 10000);

  return decoded;
}

function sflt162f(rawSflt16)
	{
	// rawSflt16 is the 2-byte number decoded from wherever;
	// it's in range 0..0xFFFF
	// bit 15 is the sign bit
	// bits 14..11 are the exponent
	// bits 10..0 are the the mantissa. Unlike IEEE format, 
	// 	the msb is transmitted; this means that numbers
	//	might not be normalized, but makes coding for
	//	underflow easier.
	// As with IEEE format, negative zero is possible, so
	// we special-case that in hopes that JavaScript will
	// also cooperate.
	//
	// The result is a number in the open interval (-1.0, 1.0);
	// 
	
	// throw away high bits for repeatability.
	rawSflt16 &= 0xFFFF;

	// special case minus zero:
	if (rawSflt16 == 0x8000)
		return -0.0;

	// extract the sign.
	var sSign = ((rawSflt16 & 0x8000) !== 0) ? -1 : 1;
	
	// extract the exponent
	var exp1 = (rawSflt16 >> 11) & 0xF;

	// extract the "mantissa" (the fractional part)
	var mant1 = (rawSflt16 & 0x7FF) / 2048.0;

	// convert back to a floating point number. We hope 
	// that Math.pow(2, k) is handled efficiently by
	// the JS interpreter! If this is time critical code,
	// you can replace by a suitable shift and divide.
	var f_unscaled = sSign * mant1 * Math.pow(2, exp1 - 15);

	return f_unscaled;
	}
```

## Configuration par interface web

Le système démarre un Acces Point wifi, soit pendant 60s au reset si le mode veille est actif, soit indéfiniment.
En se connectant à ce point d'accès et en accédant avec un navitateur à l'adresse http://192.168.4.1 on arrive sur la page d'affichage de l'état de la configuration qui va permettre de personnaliser l'objet connecté et aussi de rechercher les identifiants des capteurs DS18B20 ainsi que la tare et la calibration de la balance.

Ci dessous les copies d'écran de l'interface de configuration.

![Status](https://github.com/darkvad/RucheConnectee/blob/master/images/status.PNG)
![DS18B20](https://github.com/darkvad/RucheConnectee/blob/master/images/ds18B20.PNG)
![Balance](https://github.com/darkvad/RucheConnectee/blob/master/images/balance.PNG)
![Tare](https://github.com/darkvad/RucheConnectee/blob/master/images/tare.PNG)
![Calibration](https://github.com/darkvad/RucheConnectee/blob/master/images/calibration.PNG)
![Configuration_1](https://github.com/darkvad/RucheConnectee/blob/master/images/config1.PNG)
![Configuration_2](https://github.com/darkvad/RucheConnectee/blob/master/images/config2.PNG)

## Hardware

### Schéma

<embed>https://github.com/darkvad/RucheConnectee/blob/master/hardware/Schematic_LoraNodeRuche_2021-01-21.pdf</embed>

### PCB

![PCB](https://github.com/darkvad/RucheConnectee/blob/master/hardware/PCB_PCB_2021-01-18_23-31-23_2021-01-21.png)
![PCB](https://github.com/darkvad/RucheConnectee/blob/master/hardware/pcb.jpg)
