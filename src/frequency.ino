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
#include "frequency.h"
#include "parametres.h"

#ifdef USE_ARDUINO_FFT
static void integerToDouble(int32_t * integer, double *vRealt, double *vImagt, uint16_t samples)
{
    for (uint16_t i = 0; i < samples; i++) {
        vRealt[i] = (double)(integer[i] >> 8);
        vImagt[i] = 0.0;
    }
}
#else
static void integerToFloat(int32_t * integer, float *vReal, float *vImag, uint16_t samples)
{
    for (uint16_t i = 0; i < samples; i++) {
        vReal[i] = (float)(integer[i] >> 8) / 1.0;
        vImag[i] = 0.0;
    }
}
#endif

#if ADCMIKE == 1
void complexHandler(void *param) {
  while (true) {
    // Sleep until the ISR gives us something to do, or for 1 second
    /*uint32_t tcount = */ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000));  
    if (check_for_work) {
      // Do something complex and CPU-intensive
      //on arrete l'alarme timer le temps de traiter ?
      work_finished = false;
      timerAlarmDisable(adcTimer);
      //et on traite le buffer
      max_magnitude = 0;
      fundamental_freq = 0;

#ifdef USE_ARDUINO_FFT
      arduinoFFT FFT = arduinoFFT();
      integerToDouble(abuf_i2s, vReal, vImag, FFT_N);
#else      
      
      fft_config_t *real_fft_plan = fft_init(FFT_N, FFT_REAL, FFT_FORWARD, fft_input, fft_output);
      /*
      for (int k = 0 ; k < FFT_N ; k++)
        real_fft_plan->input[k] = (float)abuf[k];
      */
      integerToFloat(abuf_i2s, fft_input, fft_output, FFT_N);

#endif

      long int t1 = micros();

#ifdef USE_ARDUINO_FFT
      FFT.Windowing(vReal, FFT_N, FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
      FFT.Compute(vReal, vImag, FFT_N, FFT_FORWARD); /* Compute FFT */
      FFT.ComplexToMagnitude(vReal, vImag, FFT_N); /* Compute magnitudes */
      FFT.MajorPeak(vReal, FFT_N, SAMPLING_FREQ, &fundamental_freq, &max_magnitude);
#else      
      // Execute transformation
      fft_execute(real_fft_plan);
  
      // Print the output
      for (int k = 2 ; k < real_fft_plan->size / 2 ; k++)
      {
        // The real part of a magnitude at a frequency is followed by the corresponding imaginary part in the output
        float mag = sqrt(pow(real_fft_plan->output[2*k],2) + pow(real_fft_plan->output[2*k+1],2))/1000000;
        float freq = k*1.0/total_time;
        //sprintf(print_buf,"%f Hz : %f", freq, mag);
        //Serial.println(print_buf);
        if(mag > max_magnitude && mag > 1000)
        {
            max_magnitude = mag;
            fundamental_freq = freq;
        }
      }
#endif
      long int t2 = micros();
      
      Serial.println();
      /*Multiply the magnitude of the DC component with (1/FFT_N) to obtain the DC component*/
      /*
      sprintf(print_buf,"DC component : %f g\n", (real_fft_plan->output[0])/1/FFT_N);  // DC is at [0]
      Serial.println(print_buf);
      */
      /*Multiply the magnitude at all other frequencies with (2/FFT_N) to obtain the amplitude at that frequency*/
      /*
      sprintf(print_buf,"Fundamental Freq : %f Hz\t Mag: %f g\n", fundamental_freq, (max_magnitude/1)*2/FFT_N);
      Serial.println(print_buf);
      */
      sprintf(print_buf,"Fundamental Freq : %f Hz\t Mag: %f \n", fundamental_freq, max_magnitude);
      Serial.println(print_buf);
      Serial.print("Time taken: ");Serial.print((t2-t1)*1.0/1000);Serial.println(" milliseconds!");
      
      // Clean up at the end to free the memory allocated
#ifdef USE_ARDUINO_FFT
      FFT.~arduinoFFT();
#else
      fft_destroy(real_fft_plan);
#endif      
      // remise a zero buffer
      for (int k = 0 ; k < FFT_N ; k++)
        abuf_i2s[k] = 0;
      //delay(2000);
      // et on reactive l'alarme timer
      //timerAlarmEnable(adcTimer);
      // on indique qu'on a fini
      check_for_work = false;
      work_finished = true;
    }
  }
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);

  abuf_i2s[abufPos++] = analogRead(ADC1_CHANNEL_0) << 16;

  if (abufPos >= FFT_N) { 
    abufPos = 0;
    check_for_work = true;

    // Notify adcTask that the buffer is full.
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(complexHandlerTask, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif

#if I2SMIKE == 1
void I2S_Audio_Read(void) {
  // Read multiple samples at once and calculate the sound pressure
  i2s_zero_dma_buffer(I2S_PORT);
  i2s_start(I2S_PORT);
  size_t num_bytes_read;
  esp_err_t err = i2s_read(I2S_PORT,
                             (char *) abuf_i2s,
                             FFT_N *4,        // the doc says bytes, but its elements. No It is bytes
                             &num_bytes_read,
                             portMAX_DELAY);    // no timeout
  i2s_stop(I2S_PORT);    
  i2s_zero_dma_buffer(I2S_PORT);
  if (err == ESP_OK) {  
      int samples_read = num_bytes_read / 4;
      // integer to double
#ifdef USE_ARDUINO_FFT
      arduinoFFT FFT = arduinoFFT();
      integerToDouble(abuf_i2s, vReal, vImag, FFT_N);
#else
      fft_config_t *real_fft_plan = fft_init(FFT_N, FFT_REAL, FFT_FORWARD, fft_input, fft_output);
      integerToFloat(abuf_i2s, fft_input, fft_output, FFT_N);
#endif
      sprintf(print_buf,"nombre de samples %d", samples_read);
      Serial.println(print_buf);

      long int t1 = micros();
      // Execute transformation
      max_magnitude = 0;
      fundamental_freq = 0;
#ifdef USE_ARDUINO_FFT
      FFT.Windowing(vReal, FFT_N, FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
      FFT.Compute(vReal, vImag, FFT_N, FFT_FORWARD); /* Compute FFT */
      FFT.ComplexToMagnitude(vReal, vImag, FFT_N); /* Compute magnitudes */

      FFT.MajorPeak(vReal, FFT_N, SAMPLING_FREQ, &fundamental_freq, &max_magnitude);
#else
      // Execute transformation
      fft_execute(real_fft_plan);
      // Print the output
      for (int k = 2 ; k < real_fft_plan->size / 2 ; k++)
      {
        // The real part of a magnitude at a frequency is followed by the corresponding imaginary part in the output
        float mag = sqrt(pow(real_fft_plan->output[2*k],2) + pow(real_fft_plan->output[2*k+1],2))/100000;
        float freq = k*1.0/total_time;
        if(mag > max_magnitude && mag > 5000)
        {
            max_magnitude = mag;
            fundamental_freq = freq;
        }
      }
#endif

      long int t2 = micros();
        
      Serial.println();
      sprintf(print_buf,"Fundamental Freq : %f Hz\t Mag: %f \n", fundamental_freq, max_magnitude);
      Serial.println(print_buf);
      Serial.print("Time taken: ");Serial.print((t2-t1)*1.0/1000);Serial.println(" milliseconds!");
#ifdef USE_ARDUINO_FFT
      FFT.~arduinoFFT();
#else
      fft_destroy(real_fft_plan);
#endif
    } else {
      Serial.printf("Failed reading I2S: %d\n", err);
    }

}
#endif
void frequency_setup() {

  //Serial.begin(115200); // use the serial port
#if I2SMIKE == 1
  /* ---------------- I2S microphone ----------------- */
    Serial.println("Configuring I2S...");
  esp_err_t err;
  // The I2S config as per the example
  const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),      // Receive, not transfer
        .sample_rate = SAMPLING_FREQ,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,   // although the SEL config should be left, it seems to transmit on right
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S/* | I2S_COMM_FORMAT_I2S_MSB*/),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,       // Interrupt level 1
        .dma_buf_count = NB_BUF,     // number of buffers
        .dma_buf_len = BLOCK_SIZE,      // samples per buffer
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
  };

  // The pin config as per the setup
  const i2s_pin_config_t pin_config = {
        .bck_io_num = BCK_PIN,
        .ws_io_num = WS_PIN, 
        .data_out_num = DATA_OUT_PIN, 
        .data_in_num = DATA_IN_PIN};

  // Configuring the I2S driver and pins.
  // This function must be called before any I2S driver read/write operations.
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
        Serial.printf("Failed installing driver: %d\n", err);
        while (true);
  }
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
        Serial.printf("Failed setting pin: %d\n", err);
        while (true);
  }
  Serial.println("I2S driver installed.");
  i2s_stop(I2S_PORT);

  /* ------------------------------------------------------------------------------------*/
#endif

#if ADCMIKE == 1
  analogReadResolution(10);
  analogRead(ADC1_CHANNEL_0);
  //Serial.printf("%u \n",analogRead(ADC1_CHANNEL_0));
  //delay(2000);

  // put your setup code here, to run once:
//  xTaskCreate(complexHandler, "Handler Task", 8192, NULL, 1, &complexHandlerTask);
  adcTimer = timerBegin(3, 80, true); // 80 MHz / 80 = 1 MHz hardware clock for easy figuring
  timerAttachInterrupt(adcTimer, &onTimer, true); // Attaches the handler function to the timer 
  timerAlarmWrite(adcTimer, 45, true); // Interrupts when counter == 45, i.e. 22.222 times a second
  timerAlarmEnable(adcTimer);
#endif
#ifndef USE_ARDUINO_FFT
      fft_n = FFT_N;
      sampling_freq = SAMPLING_FREQ;
      total_time = fft_n / sampling_freq;
#endif
}

void frequency_loop() {
  
}