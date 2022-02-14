#include <Arduino.h>
#include <WString.h>
#include "PersistentSettings.hpp"

#pragma once

bool getDebugPinState() {
  return !(digitalRead(getDebugSettings().pin) > 0);
}

void debugToSerial(const String message) {
  if (getDebug() || getDebugPinState()) {
    Serial.print(message);
  }
}

void debugLineToSerial(const String message) {
  if (getDebug() || getDebugPinState()) {
    Serial.println(message);
  }
}

void debugToSerial(const int message) {
  debugToSerial(String(message));
}

void debugLineToSerial(const int message) {
  debugLineToSerial(String(message));
}
