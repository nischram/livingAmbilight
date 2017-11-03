# LivingColors to LED WS2811/WS2812
[Stand: V1.02 03.11.2017](https://github.com/nischram/livingAmbilight#changelog)

Hier beschreibe ich, wie du einen LED-Streifen mit der gleichen Lichtfabe einer LED Lampe ansteuern kannst.

Als Board habe ich einen Raspberry Pi eingesetzt. Für das lesen der Lichtfabe nutzte ich eine "Adafruit TCS34725" Color-Sensor und als LED verwende ich einige LED's mit WS2812 Controler.

Ich nutze diese Applikation auf einem Raspberry Pi 3. Für das Material und die Installation ist weiteres unten im Kapitel __[Material](https://github.com/nischram/livingAmbilight#material)__.

Es kommen C und C++ Programme zum Einsatz.

## Raspberry Pi   
Die Erstinstallation eines Raspberry Pi erkläre ich hier nicht im Detail, nur mit einer Schritt für Schritt Zusammenfassung von diversen Links.    
[Schritt für Schritt](https://github.com/nischram/livingAmbilight/blob/master/STEPBYSTEP.markdown)   
Diese Anleitung setzt einen lokalen Zugriff oder SSH-Zugriff auf den Raspberry voraus.


### Applikation auf den Raspberry kopieren   
Jetzt wird der Lokale oder der SSH-Zugang zum Raspberry benötigt. Ich mache dies per SSH mit dem „Terminal“ für MAC. Möglich ist dies unter Windows mit "Putty".
Jetzt folgt der Download vom Github: (Es ist __kein__ Github Account nötig.)
```shell
git clone git://github.com/nischram/livingAmbilight.git ~/livingAmbilight
```

### Einstellungen vornehmen   
In der Datei „parameter.h“ im Ordner livingAmbilight kann nun alles eingestellt werden, was du zum nutzen der Software benötigst. Entweder öffnet man die Datei mit einem externen Editor. Hier können diverse Editoren zum Einsatz kommen, ich nutze auf meinem Mac „Atom“ und lade die Datei mit „Cyberduck“ runter. Unter Windows ist das runterladen und bearbeiten z.B. mit WinSCP möglich. Alternativ kann man die Datei direkt auf dem Raspberry bearbeiten, dies beschreibe ich jetzt.
Zuerst mit folgendem Befehl in den Ordner livingAmbilight wechseln: (auf Groß und Kleinschreibung achten)
```shell
pi@raspberrypi:~ $  cd livingAmbilight
```
Öffnen der Datei „parameter.h“ zum bearbeiten mit:
```shell
pi@raspberrypi:~/livingAmbilight $  nano parameter.h
```
Es wird jetzt die Datei im Bearbeitungsprogramm „nano“ geöffnet.

### Änderungen speichern   
Die Änderungen in der „parameter.h“ speicherst du mit „STRG“ und „O“ und beendet wird der Editor mit „STRG“ und „X“.

### Applikation Kompilieren   
Das „Makefile“ ist komplett vorbereitet du brauchst nur noch „make“ in der Kommandozeile eingeben, dann läuft das Kompilieren von alleine durch.
```shell
pi@raspberrypi ~/livingAmbilight $  make
```

### Wichtig   
#### 1.
Damit die SD-Karte des Raspberry Pi nicht übermäßig beansprucht wird, nutzt ich ein RAMDisk im Arbeitsspeicher. Die Einrichtung ist unten im Kapitel __[RAMDisk](https://github.com/nischram/livingAmbilight#ramdisk)__ beschrieben.
#### 2.
Für den ColorSensor muss die I2C-Bus Installation vorgenommen werden.
[https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c)

### Applikation starten
Jetzt kann die Applikation für Tests gestartet werden.
```shell
pi@raspberrypi ~/livingAmbilight $  sudo ./livingAmbilight
```
Um die gesamte Applikation mit Watchdog zu starten:
```shell
pi@raspberrypi ~/livingAmbilight $  ./start
```

### Applikation stoppen
Nun kann die Applikation gestoppt werden, hierfür habe ich ein Stop-Programm erstellt. Dies wird mit folgendem Befehl ausgeführt: `./stop` (Eingabe ist schlecht zu erkennen)

### Autostart der Applikation (durch Crontab-Job)
Damit das Programm nun dauerhaft genutzt werden können, muss die Applikation auf dem Raspberry in den Autostart gebracht werden. Dies geschieht bei einem Raspberry Pi in dem ein Crontab-Job erstellt wird.

Die Crontab ruft man auf mit:
```shell
pi@raspberrypi ~/livingAmbilight $  crontab -e
```
- Crontab für die Bearbeitung öffnen

In der geöffneten Crontab wird eine neue Zeile mit diesem Inhalt eingefügt:
```shell
@reboot /home/pi/livingAmbilight/start
```
Die Änderung wird mit „STRG“ + „O“ gespeichert und die Crontab beendet mit
„STRG“ + „X“.
Weitere Informationen zur Crontab entnehmen Sie bitte aus Quellen wie z. B. dieser:
[https://raspberry.tips/raspberrypi-einsteiger/cronjob-auf-dem-raspberry-pi-einrichten](https://raspberry.tips/raspberrypi-einsteiger/cronjob-auf-dem-raspberry-pi-einrichten)

### Raspberry Pi neu starten
Damit die Applikation gestartet wird, kann nun der Raspberry neu gestartet werden mit:
```shell
pi@raspberrypi ~/livingAmbilight $  sudo reboot
```
- Reboot Befehl mit Administrator-Rechten
Der Raspberry Pi startet neu und die Applikation wird im Hintergrund ohne Bildschirmausgaben ausgeführt.

## LED WS2811/WS2812
Als LED kannst du alle verwenden die einen WS2811 / WS2812 Controller integriert haben. Als Beispiel können folgende LED's verwendet werden:   
[https://www.amazon.de/dp/B01MQYW4RR/ref=pe_3044161_189395811_TE_3p_dp_1](https://www.amazon.de/dp/B01MQYW4RR/ref=pe_3044161_189395811_TE_3p_dp_1)      
Im home/pi Verzeichniss muss die Bibliothek von J.Garff kopiert werden danach musst du die LED's in der parameter.h konfigurieren. Folgende Befehle sind nötig:
```shell
pi@raspberrypi:~/livingAmbilight $ cd
```
```shell
pi@raspberrypi:~/livingAmbilight $ git clone https://github.com/jgarff/rpi_ws281x.git
```
```shell
pi@raspberrypi:~/livingAmbilight $ make
```
Informationen zum Anschluss der LED findest du z.B. hier:   
[https://learn.adafruit.com/neopixels-on-raspberry-pi/wiring](https://learn.adafruit.com/neopixels-on-raspberry-pi/wiring)   

## WatchDog
Um Bus-Fehler oder Programmabstürze zu beheben habe ich einen einfachen WatchDog geschrieben. Damit der WatchDog den Betrieb der Applikation überwachen kann, lasse ich UnixTime, in eine Datei im RAMDisk schreiben und diese wiederum liest der WatchDog ein und vergleicht diese mit einer definierten Differenz mit der aktuellen Zeit.  

Ein paar Einstellmöglichkeiten für den WatchDog hast du bestimmt schon in der „parameter.h“ gesehen. Wenn die Applikation auch Daten von der HomeMatic holt können auch diese Daten überwacht werden. Da ich die HomeMatic nicht so häufig abfrage, sollten die Einstellungen zum WatchDog nicht großartig geändert werden.   
Wenn der Watchdog zuschlägt, erstellt er eine Datei "Watchdog.csv" im livingAmbilight/Data Ordner. Somit ist eine Kontrolle der Aktivität möglich. Es wird je Aktivität eine Zeile erstellt, du kannst erkennen was der WatchDog neu gestartet hat.   
Der WatchDog startet die Applikation oder den Raspberry Pi auch neu, wenn die Applikation über längere Zeit keine aktuellen Daten von ColorSensor erhält. Um den WatchDog zu stoppen musst du folgendes in der Kommandozeile eingeben:
```shell
pi@raspberrypi:~ $ sudo killall watchdog
```

## Makefile
Das Makefile ist so aufgebaut, dass jedes Programmteil auch einzeln Kompiliert werden kann. Folgende Befehle sind möglich:   
```shell
make       
make livingAmbilight   
make watchdog   
make start   
make stop   
```
Diese Möglichkeit erspart zum Teil einiges an Zeit, wenn du eigene Änderungen testen möchtest.

## Aktuelle Uhrzeit aus dem Internet holen
Wenn der Watchdog den Pi neu startet, bleibt die Uhrzeit des Pi nicht Aktuell. Hier können schon mal ein paar Minuten Abweichung entstehen.
Ich habe eine Lösung für diese Problem, auf dieser Seite gefunden:
[http://logicals.at/de/forum/raspberry-pi/48-aktuelle-uhrzeit-aus-dem-internet-holen](http://logicals.at/de/forum/raspberry-pi/48-aktuelle-uhrzeit-aus-dem-internet-holen)

## RAMDisk

RAMDisk am Raspberry erstellen:
Zuerst wird ein Mountpoint für die RAM-Disk erzeugt:
```
sudo mkdir /mnt/RAMDisk
```
Für die RAM-Disk muß die Filesystem Table angepasst werden:
```
sudo nano /etc/fstab
```
Einfügen dieser Zeile am Ende der Datei:
```
tmpfs /mnt/RAMDisk tmpfs nodev,nosuid,size=4M 0 0
```
Die Größe wird über den Parameter "4M" auf 4 MB festgelegt. Jetzt montiert man alle Filesysteme über:
```
sudo mount -a
```
Der Erfolg lässt sich mit Diskfree überprüfen:
```
df
```
Es sollte dann ein Eintrag mit der RAM-Disk zu finden sein:
```
Filesystem 1K-blocks Used Available Use% Mounted on
rootfs 15071704 2734624 11674436 19% /
/dev/root 15071704 2734624 11674436 19% /
devtmpfs 218620 0 218620 0% /dev
tmpfs 44580 236 44344 1% /run
tmpfs 5120 0 5120 0% /run/lock
tmpfs 89140 0 89140 0% /run/shm
/dev/mmcblk0p1 57288 19712 37576 35% /boot
tmpfs 4096 0 4096 0% /mnt/RAMDisk
```

Diesen Teil zum RAMDisk habe ich von hier Kopiert:
[http://www.kriwanek.de/raspberry-pi/486-ram-disk-auf-raspberry-pi-einrichten.html](http://www.kriwanek.de/raspberry-pi/486-ram-disk-auf-raspberry-pi-einrichten.html)

## WiringPI einrichten
Auf dem ColorSensor befindet sich eine LED um die Farbe von unbeleuchtetetn Materialien zu lesen. Bei meiner Anwendung wird diese nicht benötigt. Um die LED abzuschalten verwende ich einen GPIO des Raspberry, heirfür benötigst du WiringPi. Die Software WiringPi muss installiert werden.
Es ist folgendes einzugeben.  
Zum home Verzeichnis wechslen:  
```
pi@raspberrypi ~/livingAmbilight $ cd
```   
Nun brauchen wir git:   
```
pi@raspberry:~$ sudo apt-get install git git-core
```   
Jetzt kann WiringPi heruntergeladen   
```
pi@raspberry:~$ git clone git://git.drogon.net/wiringPi
pi@raspberry:~$ cd wiringPi
```
und installiert werden:   
```
pi@raspberrypi ~/wiringPi $ ./build
```

## eMail senden Installieren und aktivieren
Damit der Watchdog oder andere Programmteile eine eMail senden können, muss eine eMail Option installiert werden. Ich habe mich hier für "SendEmail" entschieden. Die Version 1.56 habe ich in meinem Github integriert, somit muss die Software nicht Heruntergeladen werden, was das installieren wesentlich einfacher macht. Folgende Befehle sind der Reihe nach auszuführen:
```
pi@raspberry:~$ cd livingAmbilight
pi@raspberrypi ~/livingAmbilight $ sudo cp -a sendEmail-v1.56/sendEmail /usr/local/bin
pi@raspberrypi ~/livingAmbilight $ sudo chmod +x /usr/local/bin/sendEmail
pi@raspberrypi ~/livingAmbilight $ sudo apt-get update
pi@raspberrypi ~/livingAmbilight $ sudo apt-get install libcrypt-ssleay-perl
pi@raspberrypi ~/livingAmbilight $ sudo apt-get install libnet-ssleay-perl
pi@raspberrypi ~/livingAmbilight $ sudo apt-get install libssl-dev
pi@raspberrypi ~/livingAmbilight $ sudo apt-get install libio-socket-ssl-perl
```
Jetzt müssen die eMail Einstellung in den "parameter.h" definiert werden.
```
// sendEmail Parameter
#define FromEmailAdress             "max.mustermann@web.de"
#define smtpServer                  "smtp.web.de"
#define smtpPort                    "587"
#define smtpTLS                     "yes"
#define smtpBenutzer                "max.mustermann@web.de"
#define smtpPass                    "1234abc"
```
Dies ist für Web.de (von mir getestet) und so muss es für gmail.com aussehen.
```
// sendEmail Parameter
#define FromEmailAdress             "max.mustermann@gmail.com"
#define smtpServer                  "smtp.gmail.com"
#define smtpPort                    "587"
#define smtpTLS                     "yes"
#define smtpBenutzer                "max.mustermann@gmail.com"
#define smtpPass                    "1234abc"
```
Für den Watchdog ist in der "parameter.h" noch die eMail-Adresse einzustellen in der die Nachrichten gesendet werden sollen:
```
#define WDtoEmailAdress             "max.mustermann@web.de"
```
Mit den Parametern
```
#define WDsendEmailReboot           1
#define WDsendEmailKill             0
```
kann noch definiert werden ob für Kill und/oder Reboot die eMail gesendet werden soll. Beide Parameter auf "0", dann wird keine eMail gesendet und die Software muss __nicht__ installiert werden.

__Wichtig:__ Bitte bei neueren Raspberry Versionen zur Fehlerbehebung [Issue#11](https://github.com/nischram/livingAmbilight/issues/11) aus meiner E3dcGui beachten!

## Material
Ich nutze die Software auf einem Komplettpaket von Conrad. Das Set besteht aus dem Raspberry Pi 3, SD-Karte (Noobs vorinstalliert), 7-Zoll Raspberry Touchdisplay, Standgehäuse und Netzteil.  
Hier die Artikelnummer von Conrad: [1437995-62](https://www.conrad.de/de/raspberry-pi-3-model-b-starter-set-1-gb-noobs-inkl-betriebssystem-noobs-inkl-gehaeuse-inkl-netzteil-inkl-software-1437995.html)   
Die Grundinstallation ist einfach, da es im Internet schon viele Anleitungen hierzu gibt, gehe ich im Moment hierauf nicht näher ein.

### Desktop deaktivieren
Es muss die Desktopanwendung „startx“ deaktiviert werden, dies kannst du im Raspberry Konfigurator machen:
```
pi@raspberry:~$ sudo raspi-config
```
Dort unter „Boot Options“ > "B1 Desktop / CLI" > „B2 Console Autologin Text console, automatically logged in as 'pi' user“ auswählen.

### W-Lan einrichten
Zuvor kannst du dein W-Lan scannen, um zu sehen ob der Pi empfang hat:
```
sudo iwlist wlan0 scan | egrep "(ESSID|IEEE)"
```
Dann wird mit folgendem Befehl ein Editor mit einer Datei geöffnet, in der du eine W-Lan Zugangsdaten eintragen kannst:
```
sudo nano /etc/wpa_supplicant/wpa_supplicant.conf
```
Die Einstellung erfolgt in dieser Form:
```
network={
    ssid="W-LAN_SSID"
    psk="ganzgeheimespasswort"
}
```
Bitte nicht vergessen, dass nur der Pi3 W-Lan on Board hat, bei einem älteren Pi musst du entweder per Kabel oder per W-Lan USB-Stick die Netzwerkverbindung herstellen.

## Quellen

#### WS2811/WS2812
[https://github.com/jgarff/rpi_ws281x](https://github.com/jgarff/rpi_ws281x)

#### ColorSensor
[https://github.com/ControlEverythingCommunity/TCS34725](https://github.com/ControlEverythingCommunity/TCS34725)

#### Applikation/WatchDog
[https://github.com/nischram/E3dcGui](https://github.com/nischram/E3dcGui)

## ToDo
- Anleitung erweitern, Fotos, Anschusspläne, etc.
- Programm alternativ für ESP8266
Für Wünsche, Fehler und Anregungen bitte ein Issue erstellen.

## Changelog

#### Wichtige Ergänzungen
V1.00 31.10.2017 Repository neu erstellt   

#### Versionen
V1.02 03.11.2017 Verbesserungen
- DEBUG integriert

V1.01 02.11.2017 Verbesserungen
- I2C-Bus hat oft abgebrochen
- Offset für LED
- Diverse veränderungen

V1.00 31.10.2017 Repository neu erstellt
- Repository neu erstellt

[Changelog Archiv](https://github.com/nischram/livingAmbilight/tree/master/Changelog_Archiv)
