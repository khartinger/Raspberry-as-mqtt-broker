# RasPi-MQTT: date and time publisher
Program: rpi_mqtt_date, Version 2018-03-30   
[Deutsche Version](./LIESMICH.md "Deutsche Version")

This command line program subscribes MQTT topic 'getData' and publishes a topic 'date' with message YYYYmmdd HHMMSS (year-month-day hour-minute-seconds).   
* When message 'version' is received, program sends its version number.
* When message 'end' is received, the program ends.
* Exit program by pressing &lt;ctrl&gt; c

Compile file:   
`gcc rpi_mqtt_date.c -o rpi_mqtt_date -lmosquitto`   
(Needs `sudo apt-get install libmosquitto-dev` before!)   
LIB-Info: https://mosquitto.org/api/files/mosquitto-h.html   
You can start this publisher automatically on every RasPi start by doing the steps below. 

## RasPi: Automatic start of a compiled c-program   
### Copy file to `/usr/local/bin/` and change file's owner and mode
```
sudo cp ~/src_c/mqtt/rpi_mqtt_date /usr/local/bin
sudo chown root /usr/local/bin/rpi_mqtt_date
sudo chmod u+s /usr/local/bin/rpi_mqtt_date
sudo chmod 777 /usr/local/bin/rpi_mqtt_date
```   
### Make your own autostart.sh file
`sudo nano /usr/local/bin/autostart.sh`   
Content of this file (for example):
```
#!/bin/bash
#...change text color to yellow...
echo -e "\\033[01;33m"
printf "_____autostart.sh_____14.4.2018_____Karl Hartinger_____\n"
#printf "copy RTC-time to date (rtc2date)\n"
#/usr/local/bin/rtc2date &
#printf "start mqtt log file writer (rpi_mqtt_sub2file_log)\n"
#/usr/local/bin/rpi_mqtt_sub2file_log -q &
printf "start date and time publisher (rpi_mqtt_date)\n"
/usr/local/bin/rpi_mqtt_date -q &
printf "_______________________________________________________\n"
#...change text color to white again...
echo -e "\\033[00m"
exit 0
```
Save and exit file: &lt;Strg&gt; o &lt;Enter&gt; &lt;Strg&gt; x   

Make script executable:   
`sudo chmod 777 /usr/local/bin/autostart.sh`   

### Execute `autostart.sh` file on every system start
Add the script name at the end of file `/etc/rc.local`:   
`sudo nano /etc/rc.local`   
Add before line exit 0:
```
#-----Call a script with your own "commands"-----
/usr/local/bin/autostart.sh
exit 0
```
Save and exit file: &lt;Strg&gt; o &lt;Enter&gt; &lt;Strg&gt; x   
