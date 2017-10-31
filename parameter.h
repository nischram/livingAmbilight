#ifndef __PARAMETER_H_
#define __PARAMETER_H_

#include "define.h"

//#######################################
// ws281x
#define GPIO_PIN                    18

//#define STRIP_TYPE                WS2811_STRIP_RGB		    // WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE                WS2811_STRIP_RBG		    // WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE                WS2811_STRIP_GBR		    // WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE                  WS2811_STRIP_GRB		    // WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE                WS2811_STRIP_BRG		    // WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE                WS2811_STRIP_BGR		    // WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE                SK6812_STRIP_RGBW		    // SK6812RGBW (NOT SK6812RGB)

#define LED_COUNT                   36                      // Anzahl der LED's
#define BRIGHTNESS                  220                     // Helligkeit 0-255 100= ideal < 80 niedrige Werte werden nicht angeziegt > 150 zu hell
#define maxValue                    220                     // Maximale Helligkeit der Anzeigen 50-255 100= ideal

//#######################################
// sendEmail Parameter
#define FromEmailAdress             "max.mustermann@web.de"                     //eMail-Adresse von der gesendet wird
#define smtpServer                  "smtp.web.de"                               //eMail-Server zum senden
#define smtpPort                    "587"                                       //Port für den smtpServer
#define smtpTLS                     "yes"                                       //TSL-Verschlüsselung "yes" oder "no"
#define smtpBenutzer                "max.mustermann@web.de"                     //Benutzername für den eMail Account
#define smtpPass                    "1234abc"                                   //Passwort für den eMail Account

//#######################################
//Watchdog Einstellungen
//Zeitdifferenz zur aktuellen Zeit bis zur Watchdog aktivierung (in Sekunden)
#define WDdiff                      300                     // Minimalwert 240
//Zeitinterval für die Abfragen des Watchdog (in Sekunden)
#define sleepTimeWD                 120
//Anzahl Programm Neustarts bis zum Reboot
#define rebootCounter               4
//Nach dieser Zeit wird der rebootCounter zurückgesetzt wenn die Daten aktuell sind (in Minuten)
#define resetMin                    60
//Kontrolldatei aktivieren damit häufige Aktivitäten erkannt werden können
#define WDkontrolle                 0                       //1=aktiv / 0=deaktiv Empfehlung: 0=deaktiv
//Email senden bei aktivität
#define WDsendEmailReboot           1                       // 1= senden erlaubt (sendEmail installiert) 0=deaktiv
#define WDsendEmailKill             0                       // 1= senden erlaubt (sendEmail installiert) 0=deaktiv
#define WDtoEmailAdress             "max.mustermann@web.de"                     // Email Adresse für Watchdog aktivitäten
//Ping zum Router füre einen LAN/WLAN test
#define RouterIP                    "192.168.178.1"         // IP-Adresse vom Router
#define PingOn                      1                       // 1= Ping zu Router aktiv
#define resetWLAN                   1                       // 1= Reset für WLAN bei Pingfehler
#define PingWD                      1                       // 1= Reboot nach dem 3. Pingfehler

//#######################################
// LED Pin für TCS34725
#define ledPin                      7                       // wiringPi Pin Nummer vom GPIO

//#######################################

#endif // __PARAMETER_H_
