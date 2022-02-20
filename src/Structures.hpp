#ifndef Structures_hpp
#define Structures_hpp

#include <ESP8266WiFi.h>
#include <WString.h>
#include "Types.hpp"

//#pragma once

typedef struct {
  int    pin;
  String pinName;
  bool   enabled;
  int    baudrate;
} DebugStruct;

typedef struct {
  WiFiClient client;
  String request;
} HtmlStruct;

typedef struct {
  sceneTypes active;
  int steps_ms;
  int pause_ms;
} SceneStruct;

typedef struct {
  String nameSSID;
  String passphrase;
  String mode;
  bool   dhcp;
  String ipaddress;
  String netmask;
  String gateway;
  String username;
  String password;
  int    restPort;
  int    httpPort;
  int    httpRefreshRate;
} WifiStruct;

typedef struct {
  buttonTypes types;
  int resitor;
  unsigned long timePress;
  bool pressed;
} BtnArrayStruct;

typedef struct {
  int pin;
  int value;
  String name;
  String pinName;
  sensorTypes type;
  int hysteresis;
  int minValue;
  int maxValue;
  int minRemap;
  int maxRemap;
  int multiplier;
  int readDelay;
  int btnArrayIncrease;
  int btnArrayDecrease;
  int debounceCount;
  BtnArrayStruct btnArray[4];
} SensorStruct;

typedef struct {
  int pin;
  int value;
  String name;
  String pinName;
  outputTypes type;
  int sceneDirection;
} OutputStruct;

#endif