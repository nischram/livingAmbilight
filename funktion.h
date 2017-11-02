#ifndef __FUNKTION_H_
#define __FUNKTION_H_

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
		loop = 0;
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

// Min and max value of LED
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

// Clear all LED
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

//Dateien schreiben mit übergebenem Pfad (z.B. Sommer/Winterzeit)
void writeData(char filePath[100], char write[100]){
	FILE *fp;
	fp = fopen(filePath, "w");
	if(fp == NULL) {
		printf("%s konnte NICHT geoeffnet werden. (writeData)\n", filePath);
	}
	else {
		fprintf(fp, "%s", write);
		fclose(fp);
	}
}
// Bit aus einer Datei lesen, ändern und schreiben
int BitChange(char *filePath, int Position, int max)
{
  int c = max;
  int out [c];
  char read[128];
  //Lese Byte aus Datei ein
  FILE *fp;
  fp = fopen(filePath, "r+");
  if(fp == NULL) {
    printf("%s konnte NICHT geoeffnet werden. (BitChange read)\n", filePath);
    for( c = 0; c < max; ++c )
      out[c] = 0;
  }
  else {
    for( c = 0; c < max; ++c ){
      fgets(read,128,fp);
      out[c] = atoi(read);
    }
    if (out[Position] == 1)
      out[Position] = 0;
    else
      out[Position] = 1;
    fseek ( fp , 0 , SEEK_SET );
    for( c = 0; c < max; ++c )
      fprintf(fp, "%d\n", out[c]);
    fclose(fp);
  }
  return 0;
}
// Bit in eine Datei schreiben
int BitWrite(char *filePath, int Position, int NewValue, int max)
{
  int c = max;
  int out [c];
  char read[128];
  //Lese Byte aus Datei ein
  FILE *fp;
  fp = fopen(filePath, "r+");
  if(fp == NULL) {
    printf("%s konnte NICHT geoeffnet werden. (BitWrite read+)\n", filePath);
    for( c = 0; c < max; ++c )
      out[c] = 0;
  }
  else {
    for( c = 0; c < max; ++c )
    {
      fgets(read,128,fp);
      out[c] = atoi(read);
    }
    out[Position] = NewValue;
    fseek ( fp , 0 , SEEK_SET );
    for( c = 0; c < max; ++c )
     fprintf(fp, "%d\n", out[c]);
    fclose(fp);
  }
 return 0;
}
// Bit aus einer Datei lesen
int BitRead(char *filePath, int Position, int max)
{
  int c = max;
  int out [c];
  char read[128];
  //Lese Byte aus Datei ein
  FILE *fp;
  fp = fopen(filePath, "r");
  if(fp == NULL) {
    printf("%s konnte NICHT geoeffnet werden. (BitRead)\n", filePath);
    for( c = 0; c < max; ++c )
      out[c] = 0;
  }
  else {
    for( c = 0; c < max; ++c )
    {
      fgets(read,128,fp);
      strtok(read, "\n");
      out[c] = atoi(read);
    }
    fclose(fp);
  }
  return out[Position];
}

// Bit Datei erstellen
int BitMake(char *filePath, int max)
{
  int c = max;
  int out [c];
  char read[128];
  FILE *fp;
  fp = fopen(filePath, "w");
  if(fp == NULL) {
    printf("%s konnte NICHT geoeffnet werden. (BitMake)\n", filePath);
    for( c = 0; c < max; ++c )
      fprintf(fp, "0\n");
  }
  else {
    for( c = 0; c < max; ++c )
      fprintf(fp, "0\n");
    fclose(fp);
  }
  return 0;
}

int readUnixtime(int UnixtimePosition)
{
	int ret = BitRead("/mnt/RAMDisk/Unixtime.txt", UnixtimePosition, UnixtimeMAX);
  return ret;
}
int writeUnixtime(int UnixtimePosition, int NewValue)
{
  BitWrite("/mnt/RAMDisk/Unixtime.txt", UnixtimePosition, NewValue, UnixtimeMAX);
  return 1;
}
int makeUnixtime()
{
	BitMake("/mnt/RAMDisk/Unixtime.txt", UnixtimeMAX);
  return 0;
}

/* Debug Funktion, für weitere Debugpositionen folgendes im Code einbauen.
DEBUG("Text");     // für Char Variablen oder Text
DEBUGint(20)       // für Int Variablen
Der Text wird in Datei RAMDisk/DEBUG.txt gespeichert.
 */
void DEBUG(char *write,...){
  if (debugUse == 1){
    FILE *fp;
  	fp = fopen("/mnt/RAMDisk/DEBUG.txt", "a");
  	if(fp == NULL) {
  		printf("Datei konnte NICHT geoeffnet werden.\n");
  	}
  	else {
  		fprintf(fp, "%s", write);
  		fclose(fp);
  	}
  }
}
void DEBUGint(int write){
  if (debugUse == 1){
    FILE *fp;
  	fp = fopen("/mnt/RAMDisk/DEBUG.txt", "a");
  	if(fp == NULL) {
  		printf("Datei konnte NICHT geoeffnet werden.\n");
  	}
  	else {
  		fprintf(fp, "%i ", write);
  		fclose(fp);
  	}
  }
}
//Speichern der DEBUG.txt
void checkDEBUG()
{
  if (debugUse == 1){
    char CLOCK[20], OUT[100], batch[128];
    time_t currentTime = time(NULL);
    struct tm *now;
    time( &currentTime );
    now = localtime( &currentTime );
    strftime (CLOCK,20,"%H:%M:%S",now);
    if (strcmp ("00:00:00",CLOCK) == 0){
      strftime (OUT,100,"DEBUG_%Y-%m-%d",now);
      snprintf (batch, (size_t)128, "cp /mnt/RAMDisk/DEBUG.txt /home/pi/E3dcGui/DEBUG/%s.txt", OUT);
      system(batch);
      system("rm /mnt/RAMDisk/DEBUG.txt");
    }
  }
}

//Version lesen
int readVersion (char* version, char* datum)
{
  char file_Path [100], read[100], off[100];
  FILE *fp;
  snprintf (file_Path, (size_t)100, "/home/pi/livingAmbilight/README.markdown");
  fp = fopen(file_Path, "r");
  if(fp == NULL) {
    printf("Datei konnte NICHT geoeffnet werden.\n");
    snprintf (version, (size_t)20, "V0.00");
    snprintf (datum, (size_t)20, "01.01.1970");
    return 0;
  }
  else {
    fgets(off,100,fp);
    fgets(read,100,fp);
    sscanf(read, "[Stand: %s %s", version, datum);
    strtok(datum, "]");
    fclose(fp);
  }
  return 1;
}

#endif // __FUNKTION_H_
