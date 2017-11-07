# RasPi: Raspberry-as-mqtt-broker

Info about this repository, Version 2017-11-07   
[Deutsche Version](./Raspberry-as-mqtt-broker_LIESMICH.md "Deutsche Version")

This repository demonstrates how to use a Raspberry Pi as access point and MQTT broker and contains some usefull programs.   

## rpi_mqtt_sub2file
This command line program subscribes all messages from a broker. For every message a file is written to the directory `/var/www/html/mqtt/data/` (filename = topic, file content = payload of the message).   
Replacements in the filename: `' ' -> '_'` and `'/' -> '&'`   
Program is running in an endless loop, so exit it by pressing &lt;Ctrl&gt;C.   
