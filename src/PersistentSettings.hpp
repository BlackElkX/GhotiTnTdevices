#ifndef PersistentSettings_hpp
#define PersistentSettings_hpp

#include <WString.h>
#include <ArduinoJson.h>
#include "Types.hpp"
#include "Structures.hpp"

//#pragma once

bool         readConfig();
bool         saveConfig(JsonObject& configJson);
void         printConfig();
String       getDeviceName();
SceneStruct  getScene();
DebugStruct  getDebugSettings();
WifiStruct   getWifiSettings();
int          getOutputQty();
OutputStruct getOutput(int index);
int          getSensorQty();
SensorStruct getSensor(int index);

bool getDebug();

void setDebug(bool debug);
void setActiveScene(String aActiveScene);
void setOutputValue(int index, int value);
void setOutputSceneDir(int index, int sceneDirection);
void setSensorValue(int index, int value);
#endif