# RasPi: Raspberry-as-mqtt-broker

Info &uuml;ber dieses Repository, Version 2017-11-07   
[English Version](./Raspberry-as-mqtt-broker_README.md "English Version")

Diese Repository zeigt, wie man ein Raspberry Pi als Access Point und MQTT Broker einrichtet und enth&auml;lt dazu hilfreiche Programme.   

## rpi_mqtt_sub2file
Dieses Kommandozeilenprogramm abonniert (subscribe) alle Nachrichten eines Brokers. F&uuml;r jede Nachricht wird eine Datei im Verzeichnis `/var/www/html/mqtt/data/` angelegt. Der Dateiname ist gleich dem Topic, der Dateiinhalt gleich der Payload. Da Leerzeichen und der Schr&auml;gstrich im Dateinamen ung&uuml;nstig sind, werden sie ersetzt:    
`' ' -> '_'` und `'/' -> '&'`   
Das Programm l&auml;uft in einer Endlosschleife, daher Programmabbruch mit &lt;Strg&gt;C.   

