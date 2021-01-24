/*

  Main module

  Copyright (C) 2019 by Alain Vadon
  Thanks to Yash Sanghvi for his article : https://medium.com/swlh/how-to-perform-fft-onboard-esp32-and-get-both-frequency-and-amplitude-45ec5712d7da
  and Robin Scheibler for his Library created for ESP-IDF which Yash Sanghvi adapt dor Arduino

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

#ifndef frequency_h
#define frequency_h

#include <Arduino.h>
#include "FFT.h" // include the library

#define ADC1_CHANNEL_0 36

int16_t abuf[FFT_N];
int16_t abufPos = 0;
portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED; 
TaskHandle_t complexHandlerTask;
hw_timer_t * adcTimer = NULL; // our timer
boolean check_for_work = false;
char print_buf[300];
boolean work_finished = false;

void frequency_setup();
void complexHandler(void *param);

#endif