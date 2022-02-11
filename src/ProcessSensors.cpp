#include <Arduino.h>
#include "Global.hpp"

#pragma once

int processSensor(int aHysteresis, int aValue, int aSensor) {
  int iValue = aValue;
  int nValue = analogRead(aSensor);
//  debugLineToSerial("Sensor " + String(aSensor) + " gives " + String(nValue));
  int maxValue = iValue + (aHysteresis / 2);
  int minValue = iValue - (aHysteresis / 2);
  if ((nValue < minValue) || (nValue > maxValue)) {
    iValue = nValue;
  }
  return iValue;
}

int processDigitalInput(int aValue, int aSensor) {
  int iValue = aValue;
  int nValue = digitalRead(aSensor);
//  debugLineToSerial("Button " + String(aSensor) + " gives " + String(nValue));
  if (nValue != iValue) {
    iValue = nValue;
  }
  return iValue;
}
