# RasPi: MQTT Nachrichten als Dateien ablegen
Programm: rpi_mqtt_sub2file.c, Version 2017-11-06   
[English Version](./README.md "English Version")

Dieses Kommandozeilenprogramm meldet sich beim Broker an und abonniert (subscribe) alle Nachrichten. F&uuml;r jede Nachricht wird im Verzeichnis `/var/www/html/mqtt/data/` eine Datei mit dem Topic als Namen und der Payload als Inhalt angelegt. Beim Dateinamen werden Leerzeichen durch Underline und Schr&auml;gstriche durch Et-Zeichen ersetzt: `' ' -> '_'` und `'/' -> '&'`   
Das Programm l&auml;uft in einer Endlosschleife, daher Programmabbruch mit &lt;Strg&gt;C.   
   
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
`gcc -o rpi_mqtt_sub2file rpi_mqtt_sub2file.c utils_file.c -lmosquitto`
