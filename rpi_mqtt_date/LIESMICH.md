# RasPi-MQTT: Datum und Uhrzeit-Publisher
Programm: rpi_mqtt_date, Version 2018-03-30   
[English Version](./README.md "English Version")

Dieses Kommandozeilenprogramm abonniert (subscribe) das MQTT-Topic 'getData' und ver&ouml;ffentlicht (publish) das Topic 'date' mit der Nachricht YYYYmmdd HHMMSS (Jahr-Monat-Tag Stunde-Minute-Sekunde).   
* Auf die Nachricht 'version' sendet das Programm seine Versionsnummer als Antwort.
* Auf die Nachricht 'end' beendet sich das Programm.
* Sonst: Programmende durch Dr&uuml;cken von &lt;Strg&gt; c

Compilieren der Datei durch:   
`gcc rpi_mqtt_date.c -o rpi_mqtt_date -lmosquitto`   
(Ben&ouml;tigt zuvor `sudo apt-get install libmosquitto-dev`)   
LIB-Info: https://mosquitto.org/api/files/mosquitto-h.html   
M&ouml;chte man, dass dieser Publisher bei jedem RasPi-Start automatisch gestartet wird, so kann man die unten stehenden Schritte ausf&uuml;hren. 

## RasPi: Automatischer Start eines compilierten C-Programms   
### Datei nach `/usr/local/bin/` kopieren und Besitzer sowie Zugriffsrechte der Datei &auml;ndern
```
sudo cp ~/src_c/mqtt/rpi_mqtt_date /usr/local/bin
sudo chown root /usr/local/bin/rpi_mqtt_date
sudo chmod u+s /usr/local/bin/rpi_mqtt_date
sudo chmod 777 /usr/local/bin/rpi_mqtt_date
```   
### Autostart-Datei
Soll der C-Client bei jedem Systemstart automatisch aktiviert werden, so kann dies zB durch das Erstellen eines Skripts erfolgen, das bei jedem Systemstart aufgerufen wird:   
`$ sudo nano /usr/local/bin/autostart.sh`   
Inhalt der Datei:
```
#!/bin/bash
#...change font color to yellow...
echo -e "\\033[01;33m"
printf "_____autostart.sh_____14.4.2018_____Karl Hartinger_____\n"
#printf "copy RTC-time to date (rtc2date)\n"
#/usr/local/bin/rtc2date &
#printf "start mqtt log file writer (rpi_mqtt_sub2file_log)\n"
#/usr/local/bin/rpi_mqtt_sub2file_log -q &
printf "start date and time publisher (rpi_mqtt_date)\n"
/usr/local/bin/rpi_mqtt_date -q &
printf "_______________________________________________________\n"
#...Farbe der Schrift wieder auf weisz aendern...
echo -e "\\033[00m"
exit 0
```
Speichern und beenden durch &lt;Strg&gt; o &lt;Enter&gt; &lt;Strg&gt; x   

Script ausf&uuml;hrbar machen:   
`$ sudo chmod 777 /usr/local/bin/autostart.sh`   

### Automatisches Ausf&uuml;hren des Skripts beim Systemstart
Dazu muss das Script noch am Ende der Datei `/etc/rc.local` eingebunden werden:   
`$ sudo nano /etc/rc.local`   
... und vor exit 0 erg&auml;nzen:
```
#-----Aufruf eines Scripts mit eigenen Befehlen-----
/usr/local/bin/autostart.sh
exit 0
```
Speichern und beenden durch &lt;Strg&gt; o &lt;Enter&gt; &lt;Strg&gt; x   



