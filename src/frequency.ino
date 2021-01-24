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

void complexHandler(void *param) {
  while (true) {
    // Sleep until the ISR gives us something to do, or for 1 second
    uint32_t tcount = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000));  
    if (check_for_work) {
      // Do something complex and CPU-intensive
      //on arrete l'alarme timer le temps de traiter ?
      work_finished = false;
      timerAlarmDisable(adcTimer);
      //et on traite le buffer
      max_magnitude = 0;
      fundamental_freq = 0;

      fft_config_t *real_fft_plan = fft_init(FFT_N, FFT_REAL, FFT_FORWARD, fft_input, fft_output);
      for (int k = 0 ; k < FFT_N ; k++)
        real_fft_plan->input[k] = (float)abuf[k];

      long int t1 = micros();
      // Execute transformation
      fft_execute(real_fft_plan);
  
      // Print the output
      for (int k = 1 ; k < real_fft_plan->size / 2 ; k++)
      {
        /*The real part of a magnitude at a frequency is followed by the corresponding imaginary part in the output*/
        float mag = sqrt(pow(real_fft_plan->output[2*k],2) + pow(real_fft_plan->output[2*k+1],2))/1;
        float freq = k*1.0/TOTAL_TIME;
        //sprintf(print_buf,"%f Hz : %f", freq, mag);
        //Serial.println(print_buf);
        if(mag > max_magnitude)
        {
            max_magnitude = mag;
            fundamental_freq = freq;
        }
      }
      long int t2 = micros();
      
      Serial.println();
      /*Multiply the magnitude of the DC component with (1/FFT_N) to obtain the DC component*/
      sprintf(print_buf,"DC component : %f g\n", (real_fft_plan->output[0])/1/FFT_N);  // DC is at [0]
      Serial.println(print_buf);

      /*Multiply the magnitude at all other frequencies with (2/FFT_N) to obtain the amplitude at that frequency*/
      sprintf(print_buf,"Fundamental Freq : %f Hz\t Mag: %f g\n", fundamental_freq, (max_magnitude/1)*2/FFT_N);
      Serial.println(print_buf);

      Serial.print("Time taken: ");Serial.print((t2-t1)*1.0/1000);Serial.println(" milliseconds!");
      
      // Clean up at the end to free the memory allocated
      fft_destroy(real_fft_plan);
      
      // remise a zero buffer
      for (int k = 0 ; k < FFT_N ; k++)
        abuf[k] = 0;
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

  abuf[abufPos++] = analogRead(ADC1_CHANNEL_0);

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

void frequency_setup() {

  //Serial.begin(115200); // use the serial port

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
}

void frequency_loop() {
  
}