#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <WString.h>

#pragma once

bool getDebugPinState();
void debugToSerial(const String message);
void debugLineToSerial(const String message);
void debugToSerial(const int message);
void debugLineToSerial(const int message);
bool getDebug();
void setDebug(bool value);
