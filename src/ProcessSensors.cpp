#include <Arduino.h>
#include "Global.hpp"
#include "Structures.hpp"

//#pragma once

int processSensor(SensorStruct sensor) {
  int iValue = sensor.value;
  int nValue = analogRead(sensor.pin);
//  debugLineToSerial("Sensor " + String(aSensor) + " gives " + String(nValue));
  //Serial.print("read = " + String(nValue) + "    ");
  int maxValue = iValue + (sensor.hysteresis / 2);
  int minValue = iValue - (sensor.hysteresis / 2);
  if ((nValue < minValue) || (nValue > maxValue)) {
    iValue = nValue;
  }
  return iValue;
}

int processAnalogButtonArray(SensorStruct sensor) {
  //int iValue = aValue;
  int nValue = analogRead(sensor.pin);
  //Serial.println(String(A0) + "  == " + String(sensor.pin) + " read = " + String(nValue) + "    ");
  //Serial.print("read = " + String(nValue) + "    ");
  //debugLineToSerial("Sensor " + String(aSensor) + " gives " + String(nValue));
  //int maxValue = iValue + (aHysteresis / 2);
  //int minValue = iValue - (aHysteresis / 2);
  //if ((nValue < minValue) || (nValue > maxValue)) {
  //  iValue = nValue;
  //}
  return nValue;
}

int processDigitalInput(SensorStruct sensor) {
  int nValue = digitalRead(sensor.pin);
//  debugLineToSerial("Button " + String(aSensor) + " gives " + String(nValue));
  //Serial.print("read = " + String(nValue) + "    ");
  return nValue;
}