#include <Arduino.h>
#include <WString.h>
#include "variables.hpp"

#pragma once

bool getDebugPinState() {
  return !(digitalRead(debugPin) > 0);
}

void debugToSerial(const String message) {
  #ifdef serialdebug
    if (debug || getDebugPinState()) {
      Serial.print(message);
    }
  #endif
}

void debugLineToSerial(const String message) {
  #ifdef serialdebug
    if (debug || getDebugPinState()) {
      Serial.println(message);
    }
  #endif
}

void debugToSerial(const int message) {
  #ifdef serialdebug
  debugToSerial(String(message));
  #endif
}

void debugLineToSerial(const int message) {
  #ifdef serialdebug
    debugLineToSerial(String(message));
  #endif
}

bool getDebug() {
  #ifdef serialdebug
  return debug;
  #endif
  return false;
}

void setDebug(bool value) {
  #ifdef serialdebug
  debug  = value;
  #endif
}
