# GhotiTnTdevices

Self build esp device to transform simple LED-strip designligts into wifi controlled dimable devices.

A very simple ui is available in browser.
A rest api is available.

- A schema and pcb layout is available, in the directory fritzing
- I use IR540 mosfets to drive the led strips. They stay very cool, because they are overrated for these strips.
- As source, the original 24V power suply is powerfull enough.
- I use as development and debug a NodeMCU ESP8266 module.
- As production boards, is used.

Even Tasmota mentioned this board only suport up to 5 PWM channels, driving 6 PWM channels is not a problem.

Known bugs:
- programs not working except scnone, scAllUpDown and scRandom

Todo:
- a switch to disable wifi functionality (only when power is off, you can use it. If powered, the switch will not be checked)
- a power switch (only on hardware level)
- an ap wifi mode with fixed ip address
- saving the config into the config.json file, from code.
- split up the values of the sensors and the outputs in different json files, in stead of the config.json file.
- a secure connection
- a decent web ui
- checking memory usage
- ...

Started:

Done:
- adding documentation about the rest api.
- 4 buttons using the A0 analog pin to use it stand-alone

To build this, I use platformio in visual studio code on Mac, Linux and Windows.

Upload is as follow:
 1 in platformio.ini, select the correct board.
 2 edit the data/config.json file, to meet your desired configuration
   tip: set debug.enabled on true, for first attempt to use it.
        also, put your ssid and passphrase here before building the file system.
 3 in the platformio tab -> general -> Build to build it. (or below the screen, the V)
 4 in the platformio tab -> platform -> Build Filesystem Image
 5 in the platformio tab -> platform -> Upload Filesystem Image
 6 in the platformio tab -> general -> Upload. (or below the screen, the ->)
 
 you can check the serial monitor if it is working and to get the ip address.