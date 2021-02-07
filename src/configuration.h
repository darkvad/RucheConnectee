/*

TTGO LoRa32 V2.1 for The Things Network

Copyright (C) 2019 by Roel van Wanrooy (www.connectix.nl)

This code requires LMIC library by Matthijs Kooijman
https://github.com/matthijskooijman/arduino-lmic

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

#pragma once
#ifndef configuration_h
#define configuration_h

#include <Arduino.h>
#include <lmic.h>
void ttn_register(void (*callback)(uint8_t message));

// -----------------------------------------------------------------------------
// Version
// -----------------------------------------------------------------------------

#define APP_NAME                "BeeConnect-LoRa32-1"
#define APP_VERSION             "V1.0\n"

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

// If using a single-channel gateway, uncomment this next option and set to your gateway's channel
#define SINGLE_CHANNEL_GATEWAY  2 //Alain

// If you are having difficulty sending messages to TTN after the first successful send,
// uncomment the next option and experiment with values (~ 1 - 5)
#define CLOCK_ERROR             1

#define DEBUG_PORT              Serial      // Serial debug port
#define SERIAL_BAUD             115200      // Serial debug baud rate
#define SLEEP_BETWEEN_MESSAGES  1           // Do sleep between messages
#define SEND_INTERVAL           120000      // Sleep for these many millis
#define MESSAGE_TO_SLEEP_DELAY  5000        // Time after message before going to sleep
#define LOGO_DELAY              5000        // Time to show logo on first boot
#define LORAWAN_PORT            1           // Port the messages will be sent to
#define LORAWAN_CONFIRMED_EVERY 0           // Send confirmed message every these many messages (0 means never)
#define LORAWAN_SF              DR_SF9      // Spreading factor
#define LORAWAN_ADR             1           // Enable ADR

// -----------------------------------------------------------------------------
// DEBUG
// -----------------------------------------------------------------------------

#ifdef DEBUG_PORT
#define DEBUG_MSG(...) DEBUG_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif

// -----------------------------------------------------------------------------
// Custom messages
// -----------------------------------------------------------------------------

#define EV_QUEUED       100
#define EV_PENDING      101
#define EV_ACK          102
#define EV_RESPONSE     103

// -----------------------------------------------------------------------------
// General
// -----------------------------------------------------------------------------

#define I2C_SDA            21   // idem Heltec
#define I2C_SCL            22   // idem Heltec
#define LED_PIN            25   // idem Heltec
//#define RELAIS_PIN         14    // confirmed pin 14 works with board - comment for Heltec

// -----------------------------------------------------------------------------
// OLED
// -----------------------------------------------------------------------------

#define SSD1306_ADDRESS 0x3C

// -----------------------------------------------------------------------------
// BME/BMP 280
// -----------------------------------------------------------------------------

#define BMP280_ADDRESS 0x76
#define LOCAL_ALTITUDE 500.0
// -----------------------------------------------------------------------------
// LoRa SPI TTGO Lora32 V2
// -----------------------------------------------------------------------------

#define SCK_GPIO        5  // idem Heltec
#define MISO_GPIO       19 // idem Heltec
#define MOSI_GPIO       27 // idem Heltec
#define NSS_GPIO        18 // idem Heltec
#define RESET_GPIO      23 // 14 pour Heltec
#define DIO0_GPIO       26 // idem Heltec
#define DIO1_GPIO       33 // 35 pour Heltec
#define DIO2_GPIO       32 // 34 pour Heltec

// -----------------------------------------------------------------------------
// AXP192 (Rev1-specific options)
// -----------------------------------------------------------------------------

#define LORA_POWER_CTRL_CH    2
#define PMU_IRQ               35

// BATTERY
#define BATTERY_PIN 35  // existe pas sur Heltec et utilisé par Lora, donc à commenter

// -----------------------------------------------------------------------------
// ONEWIRE DS18B20 Sensors
// -----------------------------------------------------------------------------
#define NB_DS18B20 2

// Data wire is connected to GPIO15
#define ONE_WIRE_BUS 15  // Heltec utilise ce pin pour OLED ...

// Addresses of the DS18B20 sensors
#define TEMP_SENSOR_1 {0x28, 0xBB, 0x2C, 0xCE, 0x38, 0x20, 0x01, 0x70}
#define TEMP_SENSOR_2 {0x28, 0xCF, 0xB2, 0x9F, 0x39, 0x20, 0x01, 0xDD}
#define TEMP_SENSOR_3 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

// Temps d'attente après un reset pour config par web et AP dans le cas ou le mode sleep est actif
#define AP_CONFIG_WAIT 60000

#endif