# RasPi: SMS zu MQTT Wandler
Sketch: sms2mqtt.cpp, Version 2021-05-23   
[--> English Version](./README.md "English Version")   

Das Programm __`sms2mqtt`__ f&uuml;r das Raspberry Pi ("RasPi") dient zum Senden und Empfangen von SMS &uuml;ber MQTT-Meldungen. Die Eigenschaften des Programms werden in der  Konfigurationsdatei `sms2mqtt.conf` festgelegt.   
Als unbedingt notwendige Parameter m&uuml;ssen die Telefonnummern der Teilnehmer, die zum Senden und Empfangen von Nachrichten berechtigt sind, angegeben werden:   
```
# authorisized phone numbers
from: 6700,+43680xxxxx
to: 6700,+43680xxxxx
```
*Anmerkung*: Die Telefonnummern sind durch Beistriche zu trennen.   

H&auml;ngt das GSM-Modem nicht an der seriellen Schnittstelle `ttyS0` (Vorgabe im Programm), so muss diese ebenfalls angegeben werden, zB:   
`device: /dev/ttyUSB0`

## Erforderliche Hardware
1. Raspberry Pi 3B+ oder 4 (mit Zubeh&ouml;r: Netzteil, micro-SD-Karte, Display, Tastatur, ...)
2. GSM-Modul SIM800C oder SIM808 etc.
3. Verbindungskabel f&uuml;r das GSM-Modul (zur seriellen Schnittstelle des Raspi oder &uuml;ber einen USB zu TTL Seriell Adapter)
4. SIM-Karte mit entsprechendem Guthaben (in &Ouml;sterreich zB von HOT = Hofer)   

## Inbetriebnahme
1. RasPi mit dem GSM-Modul verbinden, starten
2. Den MQTT-Broker "Mosquitto" auf dem RasPi installieren und starten:   
`sudo apt-get install mosquitto`   
`sudo apt-get install mosquitto-clients`   
`sudo apt-get install libmosquitto-dev`   
`sudo /etc/init.d/mosquitto start`   
3. Ein erstes Terminal-/putty-/cmd-Fenster zur __Anzeige__ aller MQTT-Nachrichten &ouml;ffnen und den Mosquitto-Subscribe-Client starten:   
`mosquitto_sub -h 127.0.0.1 -t "#" -v`   
*Anmerkung*: hinter -h muss die IP-Adresse des Hosts angegeben werden, auf dem der MQTT-Broker l&auml;uft. Zeigt man die Nachrichten auf dem selben Rechner an, kann die Angabe von -h entfallen.   
4. Ein zweites Terminal-/putty-/cmd-Fenster zum __Senden__ von MQTT-Nachrichten &ouml;ffnen.   
Das Senden erfolgt mit dem Mosquitto-Publish-Client:   
`mosquitto_pub -h 127.0.0.1 -t topic -m payload -r`   
Die Angabe von -r (retain) kann auch entfallen, wenn die Nachricht nicht dauerhaft gespeichert werden soll.   

5. Die Konfigurationsdatei `sms2mqtt.conf` erstellen bzw. bearbeiten (siehe oben oder f&uuml;r Details weiter unten)   
Die Konfigurationsdatei muss sich im Programmverzeichnis von sms2mqtt befinden und kann zB mit dem Texteditor nano erstellt werden:   
`nano sms2mqtt.conf`   
6. Das Programm `sms2mqtt` starten:   
`./sms2mqtt`   
Im Anzeigefenster (siehe Punkt 3. oben) erscheint folgende MQTT-Start-Nachricht (Vorgabe):   
`info/start sms2mqtt (23.05.2021 21:50:27)`   


## Beispiele
### Beispiel 1: Mit MQTT die sms2mqtt-Programm-Version abfragen   
Die Abfrage der Programmversion dient auch dazu, um festzustellen, ob das Programm &uuml;berhaupt l&auml;uft ;)   
```mosquitto_pub -h 127.0.0.1 -t sms2mqtt/send -m cmd_version```   
L&auml;uft das Programm `sms2mqtt`, so erscheinen folgende MQTT-Nachrichten im Anzeigefenster:   
```
sms2mqtt/send cmd_version
sms2mqtt/send/ret 2021-05-23
```   
Erscheint nur die erste Meldung, so l&auml;uft `sms2mqtt` nicht.   
* *Anmerkung 1*: Bei -h die richtige Host-IP-Adresse eintragen (oder weglassen)   
* *Anmerkung 2*: Die Abfrage-Nachricht f&uuml;r die Programmversion (cmd_version) kann in der Konfigurationsdatei ge&auml;ndert werden, zB. auf "-version":   
`cmdversion=-version`   

&nbsp;
### Beispiel 2: Mit MQTT eine SMS-Nachricht senden
Zum Senden einer SMS muss in der Payload lediglich die Telefonnummer und der Inhalt der SMS (durch ein Leerzeichen getrennt) angegeben werden (Eingabe im Sendefenster):   
`mosquitto_pub -h 127.0.0.1 -t sms2mqtt/send -m "+43680xxx Hallo von sms2mqtt auf dem RasPi"`
* *Anmerkung 1*: Die angegebene Telefonnummer (+43680xxx) muss in der Konfigurationsdatei unter "to:" angef&uuml;hrt sein!   
* *Anmerkung 2*: Die Payload muss unter Hochkomma ("") gesetzt werden, da sie Leerzeichen enth&auml;lt.   

Im Anzeigefenster erscheinen folgende MQTT-Nachrichten:   
```   
sms2mqtt/send +43680xxx Hallo von sms2mqtt auf dem RasPi
sms2mqtt/send/ret SMS sent (+43680xxx: Hallo von sms2mqtt auf dem RasPi)
```   
Ist das GSM-Modem nicht bereit oder die Schnittstellenangabe passt nicht (Vorgabe /dev/ttyS0), so erh&auml;lt man zB folgende MQTT-Nachricht:   
`sms2mqtt/send/ret SMS NOT sent #3: ERROR  while open serial (+43680xxx: Hallo von sms2mqtt auf dem RasPi)
`   

&nbsp;
### Beispiel 3: Mit MQTT das Provider-Guthaben abfragen
Verwendet man als SIM-Karte zB eine Prepaid-Karte von HOT, so kann man das Guthaben durch Senden einer SMS an `6700` mit dem Inhalt `GUTHABEN` abfragen. Dazu muss die Telefonnummer `6700` in der Konfigurationsdatei unter `from:` und `to:` angef&uuml;hrt sein!   
```mosquitto_pub -h 127.0.0.1 -t "sms2mqtt/send" -m "6700 GUTHABEN"```   
Im Anzeigefenster erscheinen folgende MQTT-Nachrichten:   
```
sms2mqtt/send 6700 GUTHABEN
sms2mqtt/send/ret SMS sent (6700: GUTHABEN)
sms2mqtt/send/ret Lieber Kunde, Ihr aktuelles Guthaben betr&auml;gt 18,09 Euro. Ihr HoT Service Team
```
Erh&auml;lt man diese Nachrichten, so wei&szlig; man, dass das Senden und Empfangen von SMS funktioniert.   


&nbsp;
### Beispiel 4: Mit SMS die sms2mqtt-Programm-Version abfragen
Die Abfrage der sms2mqtt-Programm-Version kann auch durch eine SMS erfolgen. Dazu muss die Telefonnummer des Smartphones in der Konfigurationsdatei registriert sein (bei `from:` UND bei `to:`, da man sonst keine Antwort-SMS erh&auml;lt ;) )    
Inhalt der SMS:   
`cmd_version`   
Als Antwort erh&auml;lt man eine SMS mit der Versionsnummer (zB `2021-05-23`).   
Im Anzeigefenster erkennt man, dass vom RasPi eine (Antwort-)SMS gesendet wurde:   
```
sms2mqtt/send/ret SMS sent (+43680xxx: 2021-05-23)
```
Erh&auml;lt man keine SMS-Antwort, so wei&szlig; man, dass das Programm `sms2mqtt` nicht l&auml;uft.   

&nbsp;
### Beispiel 5: Mit SMS eine MQTT-Nachricht senden
Zum Senden einer MQTT-Nachricht wird die SMS gleich aufgebaut wie eine Mosquitto-Publish-Nachricht:   
`-t test -m Hallo vom Smartphone an das RasPi`   
Weiters kann bei Bedarf auch der Schalter -r verwendet werden.   
Im Anzeigefenster erscheint die MQTT-Nachricht:   
`test Hallo vom Smartphone an das RasPi`   
* *Anmerkung*: Die Telefonnummer des (Sende-)Smartphones muss in der Konfigurationsdatei unter "from:" angef&uuml;hrt sein!   

&nbsp;
### Beispiel 6: Mit MQTT die Konfigurationsdatei neu laden
F&uuml;hrt man &Auml;nderungen in der Konfigurationsdatei durch (zB Erg&auml;nzen einer Telefonnummer), so muss das laufende Programm `sms2mqtt` nicht beendet und neu gestartet werden, um die &Auml;nderungen wirksam werden zu lassen. Es reicht, eine MQTT-Nachricht mit dem Inhalt `cmd_reload` zu senden:   
`mosquitto_pub -h 127.0.0.1 -t sms2mqtt/send -m cmd_reload`   
Im Anzeigefenster erscheinen die MQTT-Nachrichten:   
```
sms2mqtt/send cmd_reload
sms2mqtt/send/ret Config file read: OK
```
* *Anmerkung*: Der Reload-Befehl (cmd_reload) kann in der Konfigurationsdatei ge&auml;ndert werden, zB. auf "-reload":   
`cmdreload=-reload`   

&nbsp;
### Beispiel 7: Mit MQTT oder SMS das Programm beenden
Das Beenden des Programms `sms2mqtt` erfolgt mit einer MQTT-Nachricht oder SMS mit dem Inhalt `cmd_end`:   
`mosquitto_pub -h 127.0.0.1 -t sms2mqtt/send -m cmd_end`   
Im Anzeigefenster erscheinen die MQTT-Nachrichten:   
```
sms2mqtt/send cmd_end
info/end sms2mqtt (24.05.2021 11:15:21) - Program terminated by MQTT
```
* *Anmerkung*: Der Ende-Befehl (cmd_end) und die Ende-Nachricht (Topic und Payload) k&ouml;nnen in der Konfigurationsdatei ge&auml;ndert werden, zB.:   
```
endmqtt: info/end sms2mqtt @ raspi
cmdend: -end
```

&nbsp;
## Details zur Konfigurationsdatei  
Eine vollst&auml;ndige Konfigurationsdatei enth&auml;lt zB folgende Zeilen:   
```
#________sms2mqtt.conf_____________________________20210523_____
# authorisized phone numbers
from: 6700,+43680xxx,+43316yyy
to: 6700,+4368xxx,+43316yyy
# (serial) interface for sim module
device: /dev/ttyS0
# mqtt base topics for sending and receiving
sub: mysms/send
pub: mysms/send/ret
# mqtt messages on program start and end
addtime:    yes
startmqtt:  info/start sms2mqtt @ raspi
endmqtt:    info/end sms2mqtt @ raspi: 
# sms on program start and end
startsms:   +43680xxx sms2mqtt @ raspi gestartet
endsms:     +43680xxx sms2mqtt @ raspi beendet: 
# sms or mqtt commands to which the program responds
cmdend:     --end
cmdversion: --version
cmdreload:  --reload
```
Die Bedeutung der Eintr&auml;ge im Einzelnen:   
| Key         | Vorgabe       | Bedeutung                                       |   
|-------------|---------------|-------------------------------------------------|   
| from:       | &nbsp;        | Aufz&auml;hlung aller Telefonnummern (durch Beistriche getrennt), von denen SMS empfangen werden d&uuml;rfen. |   
| to:         | &nbsp;        | Aufz&auml;hlung aller Telefonnummern (durch Beistriche getrennt), an die SMS gesendet werden d&uuml;rfen. |   
| device:     | ttyS0         | Schnittstelle, an der das GSM-Modul angeschlossen ist. <br>Weitere m&ouml;gliche Werte sind `ttyACM0`, `ttyAMA0`, `ttyUSB0`, `ttyUSB1`, ...  |   
| sub:        | sms2mqtt/send | Topic, auf das das Programm `sms2mqtt` "h&ouml;rt". An dieses Topic m&uuml;ssen alle MQTT-Nachrichten geschickt werden. |   
| pub:        | sms2mqtt/send/ret | Topic, unter dem das Programm `sms2mqtt` Nachrichten verschickt. |   
| addtime:    | yes (=true)   | Der Schalter steuert, ob Datum und Uhrzeit sowie der Grund f&uuml;r das Programmende an die Start-/bzw. Ende-Meldungen/SMS angeh&auml;ngt werden sollen. |   
| startmqtt:  | <nobr>info/start sms2mqtt</nobr> | Topic und Payload (durch ein Leerzeichen getrennt) der MQTT-Nachricht, die beim Starten des Programms `sms2mqtt` gesendet wird. <br>Soll keine MQTT-Start-Nachricht gesendet werden, darf nach dem Doppelpunkt kein Wert stehen: <br>`startmqtt: ` |   
| endmqtt:    | <nobr>info/end sms2mqtt</nobr> | Topic und Payload (durch ein Leerzeichen getrennt) der MQTT-Nachricht, die beim Ende des Programms `sms2mqtt` gesendet wird. <br>Soll keine MQTT-Ende-Nachricht gesendet werden, darf nach dem Doppelpunkt kein Wert stehen: <br>`endmqtt: ` |   
| startsms:   | &nbsp;        | Telefonnummer und Nachricht (durch ein Leerzeichen getrennt), die beim Starten des Programms `sms2mqtt` als SMS gesendet wird. Vorgabe ist, keine SMS zu senden. |   
| endsms:     | &nbsp;        | Telefonnummer und Nachricht (durch ein Leerzeichen getrennt), die beim Ende des Programms `sms2mqtt` als SMS gesendet wird. Vorgabe ist, keine SMS zu senden. |   
| cmdend:     | cmd_end       | Kommando, mit dem das Programm `sms2mqtt` beendet wird (sowohl als MQTT-Nachricht als auch SMS) |   
| cmdversion: | cmd_version   | Kommando, mit dem die Versionsnummer des Programms `sms2mqtt` abgefragt werden kann (sowohl als MQTT-Nachricht als auch SMS) |   
| cmdreload:  | cmd_reload    | Kommando, mit dem die Konfigurationsdatei neu eingelesen wird. |    
 

## Software-Komponenten  
#### C_Conf_s2m.cpp
Die Datei enth&auml;lt die Realisierung der Klasse `Conf`, die die Konfigurationsdatei mit `readFile(filename)` einliest und alle Parameter des Programms verwaltet.   
Mit Hilfe der Methoden `isAuthSmsFrom(phoneNumber)` und `isAuthSmsTo(phoneNumber)` kann &uuml;berpr&uuml;ft werden, ob die angegebene Telefonnummer in der entsprechenden Liste enthalten ist.   
Weiters sind Hilfsmethoden zum Entfernen von Leerzeichen, zum Ersetzen von Teilstrings, zum Aufspalten von Strings usw. enthalten.   
&nbsp;
#### C_Conf_s2m.h
Die Datei enth&auml;lt die Deklaration der Klasse `Conf` sowie die Definition der Vorgabewerte aller Parameter.   
&nbsp;
#### C_Gsm.cpp und C_Gsm.h
Die Dateien enthalten die Klasse `Gsm` zum Ansteuern des GSM-Moduls. Von den vielen M&ouml;glichkeiten werden in der Threadfunktion `fSendSms` bzw. in `myMQTTperiodicExec()` folgende Methoden zum Senden und Empfangen von SMS verwendet:   
`isReady()`, `begin()`, `sendSms(phone, text)`; `isSms1()`, `readSms1(smsIn)`, `deleteSms1()`   
&nbsp;
#### C_X232.cpp und C_X232.h
Die Dateien enthalten die Klasse `X232` zum einfachen Ansteuern der seriellen Schnittstelle unter Linux und Windows. Diese Klasse wird von der Klasse `Gsm` verwendet.   
&nbsp;
#### C_mosquitto.hpp
Wrapper-Klasse f&uuml;r den Zugriff auf MQTT. Sie wandelt die mosquitto_-Funktionen in Methoden der Klasse `Mosquitto` um.   
&nbsp;
#### myMosq.h
Die Datei enth&auml;lt die Klasse `MyMosquitto`, die die `Mosquitto`-Klasse erweitert. Sie definert die Methode `onMessage(topic, payload)`, die bei jedem Eintreffen einer MQTT-Nachricht aufgerufen und in der Datei sms2mqtt.cpp realisiert wird.    
Weiters wird ein globales Objekt `g_myMosq` erzeugt, mit dem alle MQTT-Aktionen durchgef&uuml;hrt werden k&ouml;nnen.   

&nbsp;
#### sms2mqtt.conf
Konfigurationsdatei zum Programm `sms2mqtt`. Details siehe oben.   

&nbsp;
#### sms2mqtt.cpp
Hauptdatei des Programms. Es enth&auml;lt einige Definitionen wie die Versionsnummer, den Namen der Konfigurationsdatei und MQTT-Eigenschaften, die Konstanten
```
Conf  conf=Conf();                     // all configuration data
bool  g_smsBusy;                       // true while sending sms
int   g_endByXXX=-1;                   // reason for prog end
pid_t g_pid_main=-1;                   // main process id to kill
```
sowie folgende Funktionen bzw. Methoden:   
| Funktion/Methode | Bedeutung  |   
|------------------|------------|   
| void terminate_program(); | Methode, die beim Programmende aufgerufen wird. Sie stellt fest, wodurch das Ende hervorgerufen wurde (&lt;Strg&gt;c, MQTT oder SMS), schickt die MQTT- und SMS-Ende-Nachrichten aus (falls dies gew&uuml;nscht wurde) und beendet das Programm. |   
| void my_signal_handler(int signum); | Beendet das Programm bei Dr&uuml;cken von &lt;Steuerung&gt; c |   
|  std::string getDateTime(); | Liefert einen String mit Datum und Uhrzeit in deutschem Format (`dd.mm.YYYY HH:MM:SS`) |   
| int  parseSmsIn( std::string smsText, std::string& topicIn, std::string& payloadIn, bool& retainIn); | Zerlegt den angegebenen SMS-Text in die drei MQTT-Teile Topic, Payload und Retain-Flag. |   
| void fSendSms(std::string phone, std::string text); | Thread-Funktion zum Senden einer SMS. |   
| void MyMosquitto::onMessage(std::string topic, std::string payload) | Methode der Klasse MyMosquitto, die bei jedem Eintreffen einer MQTT-Nachricht aufgerufen wird. Sie pr&uuml;ft, ob die Nachricht f&uuml;r das Programm `sms2mqtt` gedacht ist, ob ein Kommando vorliegt oder eine SMS zu senden ist. |   
| void myMQTTperiodicExec(std::string param) | Funktion, die eine Endlos-Schleife enth&auml;lt, die parallel zur MQTT-Empfangsschleife ausgef&uuml;hrt wird. In diesem Programm dient die Funktion nur zum Warten auf das Eintreffen einer SMS. |   
| int main(int argc, char **argv) | Hauptprogramm, das das Dr&uuml;cken von &lt;Strg&gt;c abf&auml;ngt, die Konfigurationsdatei einliest, das Empfangen von MQTT-Nachrichten einrichtet, die parallele Schleife startet, die Startmeldungen verschickt und in einer Endlosschleife auf MQTT-Nachrichten wartet. |   
