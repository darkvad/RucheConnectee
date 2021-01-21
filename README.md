# Ruches connectées

Ce projet a pour intention de créer un système de ruches connectées permettant de remonter des mesures
vers une stack LoraWan
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

- mcci-catena/MCCI Arduino LoRaWAN Library@0.8.0
- mcci-catena/MCCI LoRaWAN LMIC library@3.3.0
- paulstoffregen/OneWire@2.3.5
- adafruit/Adafruit BME280 Library@2.1.2
- thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays@4.1.0
- mcci-catena/MCCI Arduino Development Kit ADK@0.2.2
- milesburton/DallasTemperature@3.9.1
- prampec/IotWebConf@3.0.0
- bogde/HX711@0.7.4