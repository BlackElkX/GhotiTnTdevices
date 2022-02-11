#include <WString.h>
#include <Arduino.h>
#include "Global.hpp"

#pragma once

int processLED(String request, String obj, int iValue, int iLed) {
  int aValue = iValue;
  if (request.indexOf("/" + obj + "=ON") != -1) {
    aValue = HIGH;
  } else if (request.indexOf("/" + obj + "=OFF") != -1) {
    aValue = LOW;
  }
  digitalWrite(iLed, aValue);
  return aValue;
}

int processHtmlPWM(String request, String obj, int aValue, int iPwm) {
  debugToSerial(" received");
  int iValue = aValue;
  if (request.indexOf("/" + obj + "=255") != -1) {
    iValue = 255;
  } else if (request.indexOf("/" + obj + "=127") != -1) {
    iValue = 127;
  } else if (request.indexOf("/" + obj + "=0") != -1) {
    iValue = 0;
  } else if (request.indexOf("/" + obj + "=") != -1) {
    String pwmDummy = request.substring(request.indexOf("/" + obj + "="));
    debugToSerial(": " + pwmDummy);
    iValue = pwmDummy.toInt();
  }
  analogWrite(iPwm, iValue);
  debugToSerial(" - gives the value of: ");
  debugLineToSerial(iValue);
  return iValue;
}

int processRestPWM(String obj, int aValue, int iPwm) {
  debugToSerial(obj + " received");
  analogWrite(iPwm, aValue);
  debugToSerial(" - gives the value of: ");
  debugLineToSerial(aValue);
  return aValue;
}

void processRestPWMScene(String obj, int aValue, int iPwm) {
  analogWrite(iPwm, aValue);
}

int processRefresh(String request, int arefreshrate) {
  int result = arefreshrate;
  if (request.indexOf("/refresh=on") != -1) {
    debugLineToSerial("refresh set on");
    result = 5;
  } else if (request.indexOf("/refresh=off") != -1) {
    debugLineToSerial("refresh set off");
    result = 0;
  }
  debugToSerial("Refreshing at rate ");
  debugLineToSerial(result);
  return result;
}
