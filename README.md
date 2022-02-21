# GhotiTnTdevices

Self build esp device to transform simple LED-strip designligts into wifi controlled dimable devices.

A very simple ui is available in browser.
A rest api is available.

- A schema and pcb layout is available, in the directory fritzing
- I use IR540 mosfets to drive the led strips. They stay very cool, because they are overrated for these strips.
- As source, the original 24V power suply is powerfull enough.
- I use as development and debug a NodeMCU ESP8266 module.
- As production boards, D1 mini is used.
- For the button array, I use 4x 10k ohm resistors, whish give on the NodeMCU
   . values 10, 550, 730, 815
   . a hysteresis of 42
   . and maxvalue of 980
  I did some tests, with other resistors, but 10k is the most stable
   (dataExample/config.json is based on this tests)
  on D1 mini its more accurate, hyst is lower and values are different
  To calibrate, use serial console, uncomment Serial.println line in main::readAnalogSensor(...
    Serial.println("old = " + String(oldValue) + " new = " + String(newValue) + " max = " + String(maxValue) + " min = " + String(minValue));

Even Tasmota mentioned this board only suport up to 5 PWM channels, driving 6 PWM channels is not a problem.

Known bugs:

V 0.0.5 - Initial production version
Solved bugs:
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