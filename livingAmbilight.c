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

#include "parameter.h"
#include "funktion.h"
#include "readColor.h"


int clear_on_exit = 1;
static uint8_t running = 1;

ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = BRIGHTNESS,
            .strip_type = STRIP_TYPE,
        },
        [1] =
        {
            .gpionum = 0,
            .count = 0,
            .invert = 0,
            .brightness = 0,
        },
    },
};

ws2811_led_t dotcolors[] =
{
    0x00200000,  // [0] = red
    0x00201000,  // [1] = orange
    0x00202000,  // [2] = yellow
    0x00002000,  // [3] = green
    0x00002010,  // [4] = cyan
    0x00002020,  // [5] = lightblue
    0x00000020,  // [6] = blue
    0x00100010,  // [7] = purple
    0x00200010,  // [8] = pink
};

static void ctrl_c_handler(int signum)
{
	(void)(signum);
    running = 0;
}

static void setup_handlers(void)
{
    struct sigaction sa =
    {
        .sa_handler = ctrl_c_handler,
    };

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int minMax(int color)
{
  if (color > 255)
    color = 255;
  if (color > maxValue)
    color = maxValue;
  if (color < 0)
    color = 0;

    return color;
}

int clearAll()
{
  int led_c;
  for (led_c = 0; led_c < LED_COUNT; led_c++) {
    ledstring.channel[0].leds[led_c] = 0x00000000;
  }
  return 1;
}

int rgbColor(int ro, int gr, int bl)
{
  unsigned long rgbHEX;
  ro = minMax(ro);
  gr = minMax(gr);
  bl = minMax(bl);
  rgbHEX = (ro << 16) + (gr << 8) + bl;
  printf("Red: %i  Green: %i  Blue: %i  LED: %#08x \n", ro, gr, bl, rgbHEX);
  return rgbHEX;
}

int main(void)
{
  int unixCounter = 60;
  makeUnixtime();

  // LED auf TCS34725
	if ( wiringPiSetup() == -1 )
		printf("wiringPi error\n");
	pinMode( ledPin, OUTPUT );
	delay(20);
	// LED off
	digitalWrite( ledPin, LOW );

  // ws281x
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

  while (running) {

    //ledstring.channel[0].leds[0] = dotcolors[5];
    readColor(unixCounter);
    rgbHEX = rgbColor(ro,gr,bl);
    //ledstring.channel[0].leds[0] = rgbHEX ;
    int led_c;
    for (led_c = 0; led_c < LED_COUNT; led_c++) {
      ledstring.channel[0].leds[led_c] = rgbHEX;
    }

    ws2811_render(&ledstring);
    //Watchdog LED
    if (unixCounter >= 60){
      int AktuallTime = time(NULL);
      writeUnixtime(UnixtimeLED, AktuallTime);
      unixCounter = 0;
    }
    unixCounter ++;

    sleep(1);
  }

  if (clear_on_exit) {
    clearAll();
    ws2811_render(&ledstring);
  }
  ws2811_fini(&ledstring);
  return 0;
}
