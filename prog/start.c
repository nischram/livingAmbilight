/*
gcc -g -o start  start.c
*/

#include <stdlib.h>
#include <unistd.h>

int main()
{
  sleep(2);
  system("sudo /home/pi/livingAmbilight/livingAmbilight &");
  system("sudo /home/pi/livingAmbilight/watchdog &");
}
