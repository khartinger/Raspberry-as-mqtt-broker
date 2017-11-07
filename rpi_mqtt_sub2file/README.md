# RasPi: Save MQTT messages as files
Program: rpi_mqtt_sub2file, Version 2017-11-06   
[Deutsche Version](./LIESMICH.md "Deutsche Version")

This command line program subscribes all messages from a broker. For every message a file is written to the directory `/var/www/html/mqtt/data/` (filename = topic, file content = payload of the message).   
Replacements in the filename: `' ' -> '_'` and `'/' -> '&'`   
Program is running in an endless loop, so exit it by pressing &lt;Ctrl&gt;C.   

Data can be used by a web page for example. To get all topics, pipe list of filenames into a file, name like _:   
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
Compile:   
`gcc -o rpi_mqtt_sub2file rpi_mqtt_sub2file.c utils_file.c -lmosquitto`
