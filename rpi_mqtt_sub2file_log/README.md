# RasPi: Save all MQTT messages in files and a log file
Program: rpi_mqtt_sub2file_log, Version 2018-03-30   
[Deutsche Version](./LIESMICH.md "Deutsche Version")

This command line program subscribes all MQTT messages and makes one file for each topic with the file content as payload.   
Additionally every topic is added to a log-file named `mqttyymm.log`, where yy is the year and mm is the month e.g. `mqtt1803.log` for March 2018. Default path for the log files is `/var/www/html/mqtt/log/`.   
Example for a data line in log-file:   
`31.03.18 23:19:07 | weather2 | 997.5, 24.8, 25.1, 44.4, 203, 4.107`
* optional args: -q = quiet (no output to screen), ip broker
* A topic `rpi_mqtt_sub2file_log` with payload ? effects a message with same topic and payload version number of this program. (So you can check, if the program ist running ;)
* Exit program by pressing &lt;ctrl&gt; c

Compile file:   
`gcc -o rpi_mqtt_sub2file_log rpi_mqtt_sub2file_log.c utils_file.c -lmosquitto`   
(Needs sudo apt-get install libmosquitto-dev !)  

### Single message files
* Filename = topic (replacements: ' ' -> '_' and '/' -> '@')   
* File content = payload   
* Directory = \_PATH\_ (default `/var/www/html/mqtt/data/`)   
* MQTT Broker: ip = _HOST_ (default 127.0.0.1)   

These files can be used by a web page for example. To get all topics, pipe list of filenames into a file, name it like _:   
`ls -ghxG --full-time ./data > ./data/_`

Example for content of file _:
```
insgesamt 16K
-rw-r--r-- 1 0 2017-11-06 18:33:20.612348029 +0100 _
-rw-r--r-- 1 2 2017-11-06 18:18:44.491054227 +0100 button&01
-rw-r--r-- 1 3 2017-11-06 18:18:44.491054227 +0100 button&01&ok
-rw-r--r-- 1 6 2017-11-06 18:18:44.491054227 +0100 button&01&volt
-rw-r--r-- 1 4 2017-11-06 18:18:44.491054227 +0100 Test1
```

## Hardware
* Raspberry Pi, libmosquitto-dev installed
* (PC with putty.exe and WinSCP)

## Software
Required:   
Install library `libmosquitto-dev`:   
`sudo apt-get install libmosquitto-dev`   
Compile file:   
`gcc -o rpi_mqtt_sub2file_log rpi_mqtt_sub2file_log.c utils_file.c -lmosquitto`   


## RasPi: Automatic start of a compiled c-program   
If you want, that the file `rpi_mqtt_sub2file_log` is started on every RasPi power on, do the following steps:
### Copy file to `/usr/local/bin/` and change file's owner and mode
```
sudo cp ~/src_c/mqtt/rpi_mqtt_sub2file_log /usr/local/bin
sudo chown root /usr/local/bin/rpi_mqtt_sub2file_log
sudo chmod u+s /usr/local/bin/rpi_mqtt_sub2file_log
sudo chmod 777 /usr/local/bin/rpi_mqtt_sub2file_log
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
printf "start mqtt log file writer (rpi_mqtt_sub2file_log)\n"
/usr/local/bin/rpi_mqtt_sub2file_log -q &
printf "start date and time publisher (rpi_mqtt_date)\n"
/usr/local/bin/rpi_mqtt_sub2file_log -q &
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

