/*
 *
 * Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright notice, this list of
 *         conditions and the following disclaimer.
 *     2.  Redistributions in binary form must reproduce the above copyright notice, this list
 *         of conditions and the following disclaimer in the documentation and/or other materials
 *         provided with the distribution.
 *     3.  Neither the name of the owner nor the names of its contributors may be used to endorse
 *         or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <wiringPi.h>


#include "../rpi_ws281x/clk.h"
#include "../rpi_ws281x/gpio.h"
#include "../rpi_ws281x/dma.h"
#include "../rpi_ws281x/pwm.h"
#include "../rpi_ws281x/ws2811.h"

int clear_on_exit = 1;
static uint8_t running = 1;
static uint8_t loop = 1;
int ro, gr, bl;

#include "parameter.h"
#include "funktion.h"
#include "colorSensor.h"

int main(void)
{
  int unixCounter = 60;
  makeUnixtime();

  // LED on TCS34725
  ledColorSensor(LOW);

  // init ws281x
  ws2811_return_t ret;

  setup_handlers();

  if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS){
      fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
      return ret;
  }

  ro = 120;
  gr = 60;
  bl = 30;
  unsigned long rgbHEX;

  // Create I2C bus
  int fp;
  while (running) {
    // init ColorSensor
    fp = initColorSensor(fp);

    if (fp != -1){
      while (loop) {
        // read RGB from ColorSensor
    		readColorSensor(fp, unixCounter);
        // make HEX for LED
        rgbHEX = rgbColor(ro,gr,bl);
        // set LED
        int led_c;
        for (led_c = 0; led_c < LED_COUNT; led_c++) {
          ledstring.channel[0].leds[led_c] = rgbHEX;
        }
        // send to LED
        ws2811_render(&ledstring);
        // Watchdog LED
        if (unixCounter >= 60){
          int AktuallTime = time(NULL);
          writeUnixtime(UnixtimeLED, AktuallTime);
          unixCounter = 0;
        }
        unixCounter ++;
        // looptime
        sleep(1);
      }
    }
  	close(fp);
  }

  if (clear_on_exit) {
    clearAll();
    ws2811_render(&ledstring);
  }
  ws2811_fini(&ledstring);
  return 0;
}
