/*
g++ -O1 Watchdog.cpp -o watchdog
 */
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <string.h>
#include "parameter.h"

using namespace std;

int WriteWatchdog(int value)
{
  ofstream fout("/home/pi/livingAmbilight/data/WatchdogAktiv.txt");
  if (fout.is_open()) {
    fout << value << endl;
    fout.close();
  }
}
int ReadWatchdog()
{
  int value = 0;
  char read[128];
  fstream datei("/home/pi/livingAmbilight/data/WatchdogAktiv.txt");
  if (datei.is_open()) {
    datei.getline(read ,128, '\n');
    value = atoi(read);
    datei.close();
    return value;
  }
  else cerr << "Konnte Datei WatchdogAktiv nicht erstellen!\n";
  return 0;
}

void WriteDataWDcsv(char DATE[40],char TIME[40], int AktuallTime, int UnixTime, int resetCounter, char OUT[100]){
  ofstream fout("/home/pi/livingAmbilight/data/Watchdog.csv", ios::app);
  if (fout.is_open()) {
    fout << DATE << ";" << TIME << ";" << AktuallTime << ";" << UnixTime << ";" << resetCounter << ";" << OUT << endl;
    fout.close();
  }
}
void WDcsvKontrolle(char DATE[40],char TIME[40], char pingOUT[40], int AktuallTime, int UnixTimeLED, int UnixTimeTCS, char OUT[100]){
  if (WDkontrolle == 1){
    ofstream fout("/home/pi/livingAmbilight/data/WatchdogKontrolle.csv", ios::app);
    if (fout.is_open()) {
      fout << DATE << " ; " << TIME << " ; Ping " << pingOUT << " ; PI " << AktuallTime << " ; LED " << UnixTimeLED << " ; TCS34725 "<< UnixTimeTCS << " ; >>> "<< OUT <<  endl ;
      fout.close();
    }
  }
}

int sendEmail(char EmailAdress[128], char Betreff[128], char Text[512])
{
  char batch[512];
  snprintf(batch, (size_t)768, "sudo sendEmail -o tls=%s -f %s -t %s -s %s:%s -xu %s -xp %s -u \"%s\" -m \"%s\"", smtpTLS, FromEmailAdress, EmailAdress, smtpServer, smtpPort, smtpBenutzer, smtpPass, Betreff, Text);
  printf("sendEmail To: %s\n      Betreff:%s\n      Text:%s\n",EmailAdress, Betreff, Text);
  system(batch);
}

int ReadUnixtime(int Position, int max)
{
  int c = max;
  int out [c];
  char read[128];
  //Lese Byte aus Datei ein
  fstream datei("/mnt/RAMDisk/Unixtime.txt");
  if (datei.is_open()) {
    for( c = 0; c < max; ++c ){
      datei.getline(read ,128, '\n');
      out[c] = atoi(read);
    }
    datei.close();
    return out[Position];
  }
  else cerr << "Konnte Unixtime.txt Datei nicht lesen!\n";
}

int ping (char* respons)
{
  char batch[256], OUT[100];
  FILE *pingOUT;
  snprintf(batch, (size_t)256, "sudo ping -c 1 %s | head -2l | tail -1l | cut -d: -f2 | cut -d\" \" -f4  | cut -b 6-10", RouterIP);
  pingOUT = popen (batch, "r");
  fgets(OUT,sizeof(OUT),pingOUT);
  float pingTime = atof(OUT);
  pclose (pingOUT);
  if (pingTime == 0 || pingTime > 300)
    snprintf(respons, (size_t)128, "NOK");
  else
    snprintf(respons, (size_t)128, "OK");
  return pingTime *100;
}


int main()
{
    int counterReboot = 0,counterRebootLED = 0,counterRebootTCS = 0,resetCounter = 0;
    int resetTime = resetMin *60 / sleepTimeWD;
    int jump = 0;
    char EmailAdress[128], EmailBetreff[128], EmailText[512];
    int Unixtime[4];
    char DATE[40], TIME[40], OUT[100], batch[256], pingOUT[128];

    int readWD = ReadWatchdog();
    if (readWD == WDfail){
      WriteWatchdog(WDwait);
    }
    else if (readWD == WDaktiv){
      sleep (3600);
      WriteWatchdog(WDwait);
    }
    while(1){
      sleep(sleepTimeWD);
      int AktuallTime = time(NULL);
      time_t timestamp;
      tm *sys;
      timestamp = time(NULL);
      sys = localtime(&timestamp);
      strftime (DATE,40,"%d.%m.%Y",sys);
      strftime (TIME,40,"%H:%M:%S",sys);

      float pingTime = ping (pingOUT);
      pingTime = pingTime /100;

      int UnixTimeLED = ReadUnixtime(UnixtimeLED, UnixtimeMAX);
      int DiffTimeLED = AktuallTime - UnixTimeLED;
      printf("%i\n", UnixTimeLED);

      int UnixTimeTCS = ReadUnixtime(UnixtimeTCS, UnixtimeMAX);
      int DiffTimeTCS = AktuallTime - UnixTimeTCS;
      printf("%i\n", UnixTimeTCS);

      if ((strcmp ("NOK",pingOUT) == 0) && (PingWD == 1 || resetWLAN == 1)){
        counterReboot ++;
        if (counterReboot == rebootCounter && PingWD == 1){
          snprintf (OUT, (size_t)100, "PING-reboot");
          WriteDataWDcsv(DATE, TIME, AktuallTime, AktuallTime, resetCounter, OUT);
          if (WDsendEmailReboot == 1){
            snprintf (EmailAdress, (size_t)128, "%s", WDtoEmailAdress);
            snprintf (EmailBetreff, (size_t)128, "livingAmbilight Watchdog");
            snprintf (EmailText, (size_t)512, "livingAmbilight Watchdog >>> WLAN-Reboot\nPing Test > 500ms \nPI: %i", AktuallTime);
            sendEmail(EmailAdress, EmailBetreff, EmailText);
          }
          WriteWatchdog(WDaktiv);
          sleep (5);
          system("sudo reboot");
          return(0);
        }
        if (WDsendEmailKill == 1){
          snprintf (EmailAdress, (size_t)128, "%s", WDtoEmailAdress);
          snprintf (EmailBetreff, (size_t)128, "livingAmbilight Watchdog");
          snprintf (EmailText, (size_t)512, "livingAmbilight Watchdog >>> WLAN-Reset\nPing Test > 500ms \nPI: %i", AktuallTime);
          sendEmail(EmailAdress, EmailBetreff, EmailText);
        }
        if (resetWLAN == 1){
          system("sudo ifconfig wlan0 down");
          sleep(1);
          system("sudo ifconfig wlan0 up");
          snprintf (OUT, (size_t)100, "WLAN-Reset");
          WriteDataWDcsv(DATE, TIME, AktuallTime, AktuallTime, resetCounter, OUT);
        }
        jump ++;
      }
      else if(DiffTimeLED > WDdiff && jump == 0){
        counterReboot ++;
        if (counterReboot == rebootCounter){
          snprintf (OUT, (size_t)100, "LED-Reboot");
          WriteDataWDcsv(DATE, TIME, AktuallTime, UnixTimeLED, resetCounter, OUT);
          if (WDsendEmailReboot == 1){
            snprintf (EmailAdress, (size_t)128, "%s", WDtoEmailAdress);
            snprintf (EmailBetreff, (size_t)128, "livingAmbilight Watchdog");
            snprintf (EmailText, (size_t)512, "livingAmbilight Watchdog >>> LED-Reboot\nLED-Time > %i Sek. \nPI: %i / LED: %i", WDdiff, AktuallTime, UnixTimeLED);
            sendEmail(EmailAdress, EmailBetreff, EmailText);
          }
          WriteWatchdog(WDaktiv);
					sleep (5);
          system("sudo reboot");
          return(0);
        }
        if (WDsendEmailKill == 1){
          snprintf (EmailAdress, (size_t)128, "%s", WDtoEmailAdress);
          snprintf (EmailBetreff, (size_t)128, "livingAmbilight Watchdog");
          snprintf (EmailText, (size_t)512, "livingAmbilight Watchdog >>> LED-Kill\nLED-Time > %i Sek. \nPI: %i / LED: %i", WDdiff, AktuallTime, UnixTimeLED);
          sendEmail(EmailAdress, EmailBetreff, EmailText);
        }
        system("sudo killall -9 livingAmbilight");
        sleep(2);
        system("/home/pi/livingAmbilight/livingAmbilight &");
        snprintf (OUT, (size_t)100, "LED-Kill");
        WriteDataWDcsv(DATE, TIME, AktuallTime, UnixTimeLED, resetCounter, OUT);
        jump ++;
      }
      else if(DiffTimeTCS > WDdiff && jump == 0 ){
        counterRebootLED ++;
        if (counterRebootLED == rebootCounter){
          snprintf (OUT, (size_t)100, "TCS34725-Reboot");
          WriteDataWDcsv(DATE, TIME, AktuallTime, UnixTimeTCS, resetCounter, OUT);
          if (WDsendEmailReboot == 1){
            snprintf (EmailAdress, (size_t)128, "%s", WDtoEmailAdress);
            snprintf (EmailBetreff, (size_t)128, "livingAmbilight Watchdog");
            snprintf (EmailText, (size_t)512, "livingAmbilight Watchdog >>> TCS34725-Reboot\nTCS34725-Time > %i Sek. \nPI: %i / TCS34725: %i", WDdiff, AktuallTime, UnixTimeTCS);
            sendEmail(EmailAdress, EmailBetreff, EmailText);
          }
          WriteWatchdog(WDaktiv);
					sleep (5);
          system("sudo reboot");
          return(0);
        }
        if (WDsendEmailKill == 1){
          snprintf (EmailAdress, (size_t)128, "%s", WDtoEmailAdress);
          snprintf (EmailBetreff, (size_t)128, "livingAmbilight Watchdog");
          snprintf (EmailText, (size_t)512, "livingAmbilight Watchdog >>> TCS34725-Kill\nTCS34725-Time > %i Sek. \nPI: %i / TCS34725: %i", WDdiff, AktuallTime, UnixTimeTCS);
          sendEmail(EmailAdress, EmailBetreff, EmailText);
        }
        system("sudo killall -9 livingAmbilight");
        sleep(2);
        system("/home/pi/livingAmbilight/livingAmbilight &");
        snprintf (OUT, (size_t)100, "TCS34725-Kill");
        WriteDataWDcsv(DATE, TIME, AktuallTime, UnixTimeTCS, resetCounter, OUT);
        jump ++;
      }
      else {
        resetCounter ++;
        snprintf (OUT, (size_t)100, "---");
        if(resetCounter == resetTime){
          counterReboot = 0;
          counterRebootLED = 0;
          counterRebootTCS = 0;
          resetCounter = 0;
        }
      }
      jump = 0;
      cout << "Watchdog: " << DATE << " ; " << TIME << "\n";
      cout << "   PI " << AktuallTime << " ; LED " << UnixTimeLED << " ; TCS34725 "<< UnixTimeTCS << "\n";
      cout << "   Reset Zähler: " << resetCounter << " ; LED Zähler bis Reboot: " << counterRebootLED << " ; TCS34725 Zähler bis Reboot: " << counterRebootTCS << "\n";
      cout << "   Ping: " << pingOUT << " ; Pingzeit: " << pingTime << " ms\n";
      if (WDkontrolle == 1)
        WDcsvKontrolle( DATE, TIME ,pingOUT ,AktuallTime, UnixTimeLED, UnixTimeTCS, OUT);
    }
}
