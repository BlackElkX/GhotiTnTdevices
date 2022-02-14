#ifndef PersistentSettings_hpp
#define PersistentSettings_hpp

#include <WString.h>
#include "Types.hpp"
#include "Structures.hpp"

#pragma once

//1732 (2048) alles bijeen
// 411  (512) enkel config
// 868 (1024) enkel output
// 421  (512) enkel sensor
//1700 (2048) alle bestanden appart

void         getConfig();
void         saveConfig();
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