#  RasPi: SMS to MQTT converter
Sketch: sms2mqtt.cpp, Version 2021-05-23   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   

The program __`sms2mqtt`__ for the Raspberry Pi ("RasPi") is used to send and receive SMS via MQTT messages. The properties of the program are defined in the configuration file `sms2mqtt.conf`.   
As absolutely necessary parameters the phone numbers of the subscribers, who are authorized to send and receive messages, must be specified:   
```
# authorisized phone numbers
from: 6700,+43680xxxxx
to: 6700,+43680xxxxx
```
*Note*: The phone numbers must be separated by commas.   

If the GSM modem is not connected to the serial interface `ttyS0` (default in the program), this must also be specified, e.g:   
`device: /dev/ttyUSB0`

## Required hardware
1. Raspberry Pi 3B+ or 4 (with accessories: power supply, micro-SD card, display, keyboard, ...)
2. GSM module SIM800C or SIM808 etc.
3. connection cable for the GSM module (to the serial port of the Raspi or via a USB to TTL serial adapter)
4. SIM card with appropriate credit (in Austria e.g. from HOT = Hofer)   

## Commissioning
1. connect RasPi with the GSM module, start it up
2. install the MQTT broker "Mosquitto" on the RasPi and start it:   
`sudo apt-get install mosquitto`.   
`sudo apt-get install mosquitto-clients`   
`sudo apt-get install libmosquitto-dev`   
`sudo /etc/init.d/mosquitto start`.   
3. open a first terminal/putty/cmd window to __view__ all MQTT messages and start the mosquitto-subscribe-client:   
`mosquitto_sub -h 127.0.0.1 -t "#" -v`   
*Note*: after -h you have to specify the IP address of the host where the MQTT broker is running. If you display the messages on the same host, the specification of -h can be omitted.   
4. open a second terminal/putty/cmd window to __send__ MQTT messages.   
Sending is done with the Mosquitto publish client:   
`mosquitto_pub -h 127.0.0.1 -t topic -m payload -r`.   
The specification of -r (retain) can also be omitted if the message is not to be stored permanently.   

5. create or edit the configuration file `sms2mqtt.conf` (see above or for details below)   
The configuration file must be located in the program directory of sms2mqtt and can be created e.g. with the text editor nano:   
`nano sms2mqtt.conf`.   
6. start the program `sms2mqtt`:   
`./sms2mqtt`.   
In the display window (see point 3. above) the following MQTT start message appears (default):   
`info/start sms2mqtt (23.05.2021 21:50:27)`   


## Examples
### Example 1: Using MQTT to query the sms2mqtt program version   
Querying the program version is also used to determine if the program is running at all ;)   
``mosquitto_pub -h 127.0.0.1 -t sms2mqtt/send -m cmd_version``   
If the program `sms2mqtt` is running, the following MQTT messages appear in the display window:   
```
sms2mqtt/send cmd_version
sms2mqtt/send/ret 2021-05-23
```   
If only the first message appears, `sms2mqtt` is not running.   
* *Note 1*: Enter the correct host IP address at -h (or omit).   
* *Note 2*: The query message for the program version (cmd_version) can be changed in the configuration file, e.g. to "-version":   
`cmdversion=-version`   

&nbsp;
### Example 2: Using MQTT to send an SMS message
To send an SMS, only the phone number and the content of the SMS (separated by a space) must be specified in the payload (entered in the send window):   
`mosquitto_pub -h 127.0.0.1 -t sms2mqtt/send -m "+43680xxx Hello from sms2mqtt on RasPi"`.
* *Note 1*: The given phone number (+43680xxx) must be listed in the configuration file under "to:"!   
* *Note 2*: The payload must be set under apostrophe (""), because it contains spaces.   

The following MQTT messages appear in the display window:   
```   
sms2mqtt/send +43680xxx Hello from sms2mqtt on the RasPi
sms2mqtt/send/ret SMS sent (+43680xxx: Hello from sms2mqtt on the RasPi)
```   
If the GSM modem is not ready or the interface specification does not fit (default /dev/ttyS0), you get e.g. the following MQTT message:   
`sms2mqtt/send/ret SMS NOT sent #3: ERROR while open serial (+43680xxx: Hello from sms2mqtt on the RasPi).
`   

&nbsp;
### Example 3: Querying the provider credit with MQTT
If you use e.g. a prepaid card from HOT as SIM card, you can query the credit by sending an SMS to `6700` with the content `GUTHABEN`. To do this, the phone number `6700` must be listed in the configuration file under `from:` and `to:`!   
```mosquitto_pub -h 127.0.0.1 -t "sms2mqtt/send" -m "6700 GUTHABEN"```   
The following MQTT messages appear in the display window:   
```
sms2mqtt/send 6700 GUTHABEN
sms2mqtt/send/ret SMS sent (6700: GUTHABEN)
sms2mqtt/send/ret Lieber Kunde, Ihr aktuelles Guthaben beträgt 18,09 Euro. Ihr HoT Service Team
```
If you receive these messages, you know that sending and receiving SMS works.   


&nbsp;
### Example 4: Using SMS to query the sms2mqtt program version
Querying the sms2mqtt program version can also be done by SMS. For this, the phone number of the smartphone must be registered in the configuration file (at `from:` AND at `to:`, otherwise you won't get a reply SMS ;) )    
Content of the SMS:   
`cmd_version`   
As answer you get a SMS with the version number (e.g. `2021-05-23`).   
In the display window you can see that a (reply) SMS was sent by the RasPi:   
```
sms2mqtt/send/ret SMS sent (+43680xxx: 2021-05-23)
```
If you do not receive an SMS reply, you know that the program `sms2mqtt` is not running.   

&nbsp;
### Example 5: Using SMS to send an MQTT message
To send an MQTT message, the SMS is constructed the same as a Mosquitto publish message:   
`-t test -m Hello from smartphone to RasPi`.   
Furthermore the switch -r can be used if necessary.   
The MQTT message appears in the display window:   
`test hello from smartphone to the RasPi`   
* *Note*: The phone number of the (sending) smartphone must be listed in the configuration file under "from:"!   

&nbsp;
### Example 6: Reload the configuration file with MQTT
If you make changes in the configuration file (e.g. add a phone number), the running program `sms2mqtt` does not have to be stopped and restarted to make the changes effective. It is sufficient to send an MQTT message with the content `cmd_reload`:   
`mosquitto_pub -h 127.0.0.1 -t sms2mqtt/send -m cmd_reload`   
The MQTT messages appear in the display window:   
```
sms2mqtt/send cmd_reload
sms2mqtt/send/ret Config file read: OK
```
* *Note*: The reload command (cmd_reload) can be changed in the config file, e.g. to "-reload":   
`cmdreload=-reload`   

&nbsp;
### Example 7: Using MQTT or SMS to terminate the program
Quitting the program `sms2mqtt` is done with an MQTT message or SMS with the content `cmd_end`:   
`mosquitto_pub -h 127.0.0.1 -t sms2mqtt/send -m cmd_end`   
The MQTT messages appear in the display window:   
```
sms2mqtt/send cmd_end
info/end sms2mqtt (05/24/2021 11:15:21) - Program terminated by MQTT
```
* *Note*: The end command (cmd_end) and the end message (topic and payload) can be changed in the configuration file, for example:   
```
endmqtt: info/end sms2mqtt @ raspi
cmdend: -end
```

&nbsp;
## Configuration file details  
A complete configuration file contains e.g. the following lines:   
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
The meaning of the entries in detail:   
| Key | Default | Meaning |   
|-------------|---------------|-------------------------------------------------|   
| from:       | &nbsp; | Enumeration of all phone numbers (separated by commas) from which SMS may be received. |   
| to:         | &nbsp; | Enumeration of all phone numbers (separated by commas) to which SMS may be sent. |   
| device:     | ttyS0 | Interface to which the GSM module is connected. <br>Other possible values are `ttyACM0`, `ttyAMA0`, `ttyUSB0`, `ttyUSB1`, ...  |   
| sub: | sms2mqtt/send | Topic to which the program `sms2mqtt` "listens". All MQTT messages must be sent to this topic. |   
| pub: | sms2mqtt/send/ret | Topic under which the program `sms2mqtt` sends messages. |   
| addtime: | yes (=true) | The switch controls whether date and time as well as the reason for the program end should be appended to the start/end messages/SMS. |   
| startmqtt: | <nobr>info/start sms2mqtt</nobr> | Topic and payload (separated by a space) of the MQTT message sent when the program `sms2mqtt` is started. <br>If no MQTT startup message is to be sent, there must be no value after the colon: <br>`startmqtt: ` |   
| endmqtt: | <nobr>info/end sms2mqtt</nobr> | Topic and payload (separated by a space) of the MQTT message sent at the end of `sms2mqtt`. <br>If no MQTT end message is to be sent, there must be no value after the colon: <br>`endmqtt: ` |   
| startsms: | &nbsp; | phone number and message (separated by a space), which will be sent as SMS when the program `sms2mqtt` is started. Default is not to send SMS. |   
| endsms: | &nbsp; | Phone number and message (separated by a space) that will be sent as SMS when the program `sms2mqtt` ends. Default is not to send SMS. |   
| cmdend: | cmd_end | command to end the `sms2mqtt` program (both as MQTT message and SMS) |   
| cmdversion: | cmd_version | command to query the version number of the `sms2mqtt` program (both as MQTT message and SMS) |   
| cmdreload:  | cmd_reload | command to reload the configuration file. |    
 

## Software components  
#### C_Conf_s2m.cpp
The file contains the realization of the class `Conf`, which reads the configuration file with `readFile(filename)` and manages all parameters of the program.   
The methods `isAuthSmsFrom(phoneNumber)` and `isAuthSmsTo(phoneNumber)` can be used to check if the given phone number is included in the corresponding list.   
Furthermore, helper methods for removing spaces, replacing substrings, splitting strings, etc. are included.   
&nbsp;
#### C_Conf_s2m.h
The file contains the declaration of the class `Conf` and the definition of the default values of all parameters.   
&nbsp;
#### C_Gsm.cpp and C_Gsm.h
The files contain the class `Gsm` to control the GSM module. From the many possibilities the following methods are used in the thread function `fSendSms` or in `myMQTTperiodicExec()` to send and receive SMS:   
`isReady()`, `begin()`, `sendSms(phone, text)`; `isSms1()`, `readSms1(smsIn)`, `deleteSms1()`   
&nbsp;
#### C_X232.cpp and C_X232.h
The files contain the class `X232` for easy control of the serial port under Linux and Windows. This class is used by the class `Gsm`.   
&nbsp;
#### C_mosquitto.hpp
Wrapper class for accessing MQTT. It converts the mosquitto_ functions into methods of the `Mosquitto` class.   
&nbsp;
#### myMosq.h
The file contains the `MyMosquitto` class, which extends the `Mosquitto` class. It defines the method `onMessage(topic, payload)`, which is called each time an MQTT message arrives and is implemented in the file sms2mqtt.cpp.    
Furthermore, a global object `g_myMosq` is created, which can be used to perform all MQTT actions.   

&nbsp;
#### sms2mqtt.conf
Configuration file for the program `sms2mqtt`. See above for details.   

&nbsp;
#### sms2mqtt.cpp
Main file of the program. It contains some definitions like the version number, the name of the configuration file and MQTT properties, the constants
```
Conf conf=Conf(); // all configuration data
bool g_smsBusy; // true while sending sms
int g_endByXXX=-1; // reason for prog end
pid_t g_pid_main=-1; // main process id to kill
```
and the following functions or methods:   
| function/method | meaning |   
|------------------|------------|   
| void terminate_program(); | method which is called when the program ends. It determines what caused the end (&lt;Ctrl&gt;c, MQTT or SMS), sends out the MQTT and SMS end messages (if requested) and terminates the program. |   
| void my_signal_handler(int signum); | Terminates the program when &lt;control&gt; c is pressed |   
| std::string getDateTime(); | Returns a string with date and time in German format (`dd.mm.YYYY HH:MM:SS`) |   
| int parseSmsIn( std::string smsText, std::string& topicIn, std::string& payloadIn, bool& retainIn); | Splits the given SMS text into the three MQTT parts topic, payload and retain flag. |   
| void fSendSms(std::string phone, std::string text); | Thread function for sending an SMS. |   
| void MyMosquitto::onMessage(std::string topic, std::string payload) | Method of the MyMosquitto class that is called whenever an MQTT message arrives. It checks if the message is intended for the `sms2mqtt` program, if there is a command or if there is an SMS to be sent. |   
| void myMQTTperiodicExec(std::string param) | Function containing an infinite loop which is executed in parallel to the MQTT receive loop. In this program the function is only used to wait for the arrival of an SMS. |   
| int main(int argc, char **argv) | Main program that intercepts the press of &lt;Ctrl&gt;c, reads the configuration file, sets up receiving MQTT messages, starts the parallel loop, sends the startup messages, and waits in an infinite loop for MQTT messages. | 

