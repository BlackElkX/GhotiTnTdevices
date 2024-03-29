#include <WString.h>
#include <Arduino.h>
#include "Global.hpp"
#include "Types.hpp"

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
  debugLineToSerial(iValue);
  return iValue;
}

int processRefresh(String request, int arefreshrate) {
  int result = arefreshrate;
  if (request.indexOf("/refresh=on") != -1) {
    result = 5;
  } else if (request.indexOf("/refresh=off") != -1) {
    result = 0;
  }
  return result;
}

sceneTypes processScene(String request) {
  sceneTypes sceneType = scNone;
  if (request.indexOf("GET /scene=") != -1) {
    String newScene1 = request.substring(request.indexOf("GET /scene=") + 11);
    String newScene2 = newScene1.substring(0, newScene1.indexOf(" "));
    sceneType = getSceneTypeFromName(newScene2);
  }
  return sceneType;
}
