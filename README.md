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

Adafruit BME280 Library
Adafruit Unified Sensor
DallasTemperature
ESP8266 and ESP32 OLED driver for SSD1306 displays
HX711
IotWebConf
MCCI Arduino Development Kit ADK
MCCI Arduino LoRaWAN Library
MCCI LoRaWAN LMIC library
OneWire
