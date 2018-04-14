# RasPi: MQTT Nachrichten als Dateien ablegen und in log-Datei speichern
Programm: rpi_mqtt_sub2file_log.c, Version 2018-03-30   
[English Version](./README.md "English Version")

Dieses Kommandozeilenprogramm meldet sich beim Broker an und abonniert (subscribe) alle Nachrichten. F&uuml;r jede Nachricht wird im Verzeichnis `/var/www/html/mqtt/data/` eine Datei mit dem Topic als Namen und der Payload als Inhalt angelegt. Beim Dateinamen werden Leerzeichen durch Underline und Schr&auml;gstriche durch At-Zeichen ersetzt: `' ' -> '_'` und `'/' -> '@'`   
Zus&auml;tzlich wird jede Nachricht mit Datum, Uhrzeit, Topic und Nachrichteninhalt in der Datei `mqttyymm.log` gespeichert, wobei yy f&uuml;r das Jahr und mm f&uuml;r das Monat steht, zB `mqtt1803.log` f&uuml;r M&auml;rz 2018. Vorgabe-Verzeichnis f&uuml;r die log-Dateien ist `/var/www/html/mqtt/log/`.   
Beispiel f&uuml;r eine Nachrichtenzeile in der log-Datei:   
`31.03.18 23:19:07 | weather2 | 997.5, 24.8, 25.1, 44.4, 203, 4.107`

Da das Programm in einer Endlosschleife l&auml;uft, muss es mit &lt;Strg&gt;C abgebrochen werden.   

### Zusammenfassung Info zu Nachrichten-Dateien
* Dateiname = Topic (Ersetzen von ' ' durch '_' und '/' durch '@')   
* Dateiinhalt = Payload (Nachrichtentext)    
* Verzeichnis = \_PATH\_ (Vorgabe `/var/www/html/mqtt/data/`)   
* MQTT Broker: ip = _HOST_ (default 127.0.0.1)   
 
Die Auswertung der Dateien kann zB durch eine Webseite erfolgen. Um die Namen der Topics zu erhalten, kann man zB die Anzeige des Inhaltsverzeichnisses in eine Datei mit dem Namen _ umleiten und diese auswerten:   
`ls -ghxG --full-time ./data > ./data/_`

Beispiel f&uuml;r den Inhalt der Datei _:
```
insgesamt 16K
-rw-r--r-- 1 0 2017-11-06 18:33:20.612348029 +0100 _
-rw-r--r-- 1 2 2017-11-06 18:18:44.491054227 +0100 button&01
-rw-r--r-- 1 3 2017-11-06 18:18:44.491054227 +0100 button&01&ok
-rw-r--r-- 1 6 2017-11-06 18:18:44.491054227 +0100 button&01&volt
-rw-r--r-- 1 4 2017-11-06 18:18:44.491054227 +0100 Test1
```

## Hardware
* Raspberry Pi mit installiertem libmosquitto-dev
* (PC mit putty.exe und WinSCP)

## Software
Vorbereitung:   
Installation der Bibliothek `libmosquitto-dev` durch   
`sudo apt-get install libmosquitto-dev`   
Kompilieren:   
`gcc -o rpi_mqtt_sub2file_log rpi_mqtt_sub2file_log.c utils_file.c -lmosquitto`   

## RasPi: Automatischer Start eines compilierten C-Programms   
### Datei nach `/usr/local/bin/` kopieren und Besitzer sowie Zugriffsrechte der Datei &auml;ndern
```
sudo cp ~/src_c/mqtt/rpi_mqtt_sub2file_log /usr/local/bin
sudo chown root /usr/local/bin/rpi_mqtt_sub2file_log
sudo chmod u+s /usr/local/bin/rpi_mqtt_sub2file_log
sudo chmod 777 /usr/local/bin/rpi_mqtt_sub2file_log
```   
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
printf "start mqtt log file writer (rpi_mqtt_sub2file_log)\n"
/usr/local/bin/rpi_mqtt_sub2file_log -q &
printf "start date and time publisher (rpi_mqtt_date)\n"
/usr/local/bin/rpi_mqtt_date -q &
printf "_______________________________________________________\n"
#...Farbe der Schrift wieder auf weiﬂ ‰ndern...
echo -e "\\033[00m"
exit 0
```
Speichern und beenden durch &lt;Strg&gt; o &lt;Enter&gt; &lt;Strg&gt; x   

Script ausf&uuml;hrbar machen:   
`$ sudo chmod 777 /usr/local/bin/autostart.sh`   

Weiters muss das Script noch am Ende der Datei `/etc/rc.local` eingebunden werden:   
`$ sudo nano /etc/rc.local`   
Vor exit 0 erg&auml;nzen:
```
#-----Aufruf eines Scripts mit eigenen Befehlen-----
/usr/local/bin/autostart.sh
exit 0
```
Speichern und beenden durch &lt;Strg&gt; o &lt;Enter&gt; &lt;Strg&gt; x   
