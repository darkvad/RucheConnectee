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

//#define USE_ARDUINO_FFT

#include <Arduino.h>
#ifdef USE_ARDUINO_FFT
#include <arduinoFFT.h> // include the library
#else
#include "FFT.h"
#endif
#include <driver/i2s.h>

#define I2SMIKE 1
#define ADCMIKE 0

#if ADCMIKE == 1
// analog mike
#define ADC1_CHANNEL_0 36
#define SAMPLING_FREQ 22222  
#endif

#if I2SMIKE == 1
/* --------- I2S mike ------------------ */
#ifdef USE_ARDUINO_FFT
#define SAMPLING_FREQ 22500
#else
#define SAMPLING_FREQ 48000
#endif
#define BCK_PIN GPIO_NUM_14
#define WS_PIN GPIO_NUM_2
#define DATA_OUT_PIN I2S_PIN_NO_CHANGE
#define DATA_IN_PIN GPIO_NUM_39
const int BLOCK_SIZE = 1024;
const int NB_BUF = 3;
const i2s_port_t I2S_PORT = I2S_NUM_0;
//int16_t abufPos_i2s = 0;
/* ------------------------------------- */
#endif

#ifdef USE_ARDUINO_FFT
#define FFT_N 2048 // Must be a power of 2
double vReal[FFT_N];
double vImag[FFT_N];
double max_magnitude = 0;
double fundamental_freq = 0;
#else
#define FFT_N 4096 // Must be a power of 2
float fft_input[FFT_N];
float fft_output[FFT_N];
float max_magnitude = 0;
float fundamental_freq = 0;
float fft_n;
float sampling_freq;
float total_time;
#endif

int32_t abuf_i2s[FFT_N];
//int16_t abuf[FFT_N];
int16_t abufPos = 0;

portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED; 
TaskHandle_t complexHandlerTask;
hw_timer_t * adcTimer = NULL; // our timer
boolean check_for_work = false;
boolean work_finished = false;

void frequency_setup();
#if ADCMIKE == 1
void complexHandler(void *param);
#endif
#if I2SMIKE == 1
void I2S_Audio_Read();
#endif
#endif