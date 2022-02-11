#include <LittleFS.h>
#include <WString.h>
#include <Global.hpp>
#include <ArduinoJson.h>
#include "types.hpp"
#include "PersistentSettings.hpp"

#pragma once
JsonObject configJson;

void getConfig() {
  //StaticJsonDocument<2048> jsonDoc;
  DynamicJsonDocument jsonDoc(2048);
  if (!LittleFS.begin()) {
    debugLineToSerial("An error occured while mounting file system");
    return;
  }
  File file = LittleFS.open("/config.json", "r");
  if (!file) {
    debugLineToSerial("Failed to open config.json for reading");
    return;
  }
  String configString = "";
  while (file.available()) {
    configString += file.read();
  }
  file.close();
  debugLineToSerial(configString);

  DeserializationError error = deserializeJson(jsonDoc, configString);

  if (error) {
    debugLineToSerial(F("deserializeJson() failed: "));
    debugLineToSerial(error.f_str());
    return;
  }
  configJson  = jsonDoc.as<JsonObject>();
}

void saveConfig() {
  if (!LittleFS.begin()) {
    debugLineToSerial("An error occured while mounting file system");
    return;
  }
  File file = LittleFS.open("/config.json", "r");
  if (!file) {
    debugLineToSerial("Failed to open config.json for reading");
    return;
  }
  serializeJson(configJson, file);
  file.close();
  debugLineToSerial("config saved to file");
  #ifdef serialdebug
    getConfig();
  #endif
}

String getDeviceName() {
  return String(configJson["deviceName"]);
}

SceneStruct getScene() {
  SceneStruct scenestruct;
  scenestruct.active   = String(configJson["scene"]["active"]);
  scenestruct.steps_ms = configJson["scene"]["steps_ms"];
  scenestruct.pause_ms = configJson["scene"]["pause_ms"];
  return scenestruct;
}

void setActiveScene(String aActiveScene) {
  configJson["scene"]["active"] = aActiveScene;
}

DebugStruct getDebugSettings() {
  DebugStruct debugstruct;
  debugstruct.pin      = configJson["debug"]["pin"];
  debugstruct.pinName  = String(configJson["debug"]["pinName"]);
  debugstruct.enabled  = configJson["debug"]["enabled"];
  debugstruct.baudrate = configJson["debug"]["baudrate"];
  return debugstruct;
}

WifiStruct getWifiSettings() {
  WifiStruct wifistruct;
  wifistruct.nameSSID        = String(configJson["wifi"]["ssid"]);
  wifistruct.passphrase      = String(configJson["wifi"]["passphrase"]);
  wifistruct.mode            = String(configJson["wifi"]["mode"]);
  wifistruct.restPort        = configJson["wifi"]["restport"];
  wifistruct.httpPort        = configJson["wifi"]["httpport"];
  wifistruct.httpRefreshRate = configJson["wifi"]["httprefreshrate"];
  wifistruct.dhcp            = configJson["wifi"]["dhcp"];
  wifistruct.ipaddress       = String(configJson["wifi"]["ipaddress"]);
  wifistruct.netmask         = String(configJson["wifi"]["netmask"]);
  wifistruct.gateway         = String(configJson["wifi"]["gateway"]);
  return wifistruct;
}

int getOutputQty() {
  JsonArray outputArray = configJson["outputs"].as<JsonArray>();
  return outputArray.size();
}

OutputStruct getOutput(int index) {
  JsonArray outputArray = configJson["outputs"].as<JsonArray>();
  OutputStruct output;
  output.value          = outputArray[index]["value"];
  output.name           = String(outputArray[index]["name"]);
  output.pin            = outputArray[index]["pin"];
  output.pinName        = String(outputArray[index]["pinName"]);
  output.type           = outputArray[index]["type"];
  output.sceneDirection = outputArray[index]["sceneDirection"];
  return output;
}

int getSensorQty() {
  JsonArray sensorArray = configJson["sensors"].as<JsonArray>();
  return sensorArray.size();
}

SensorStruct getSensor(int index) {
  JsonArray sensorArray = configJson["sensors"].as<JsonArray>();
  SensorStruct sensor;
    sensor.value      = sensorArray[index]["value"];
    sensor.name       = String(sensorArray[index]["name"]);
    sensor.pin        = sensorArray[index]["pin"];         
    sensor.pinName    = String(sensorArray[index]["pinName"]);
    sensor.type       = sensorArray[index]["type"];           
    sensor.hysteresis = sensorArray[index]["sceneDirection"]; 
    sensor.minValue   = sensorArray[index]["minvalue"];
    sensor.maxValue   = sensorArray[index]["maxvalue"];
    sensor.minRemap   = sensorArray[index]["minremap"];
    sensor.maxRemap   = sensorArray[index]["maxremap"];
    sensor.multiplier = sensorArray[index]["multiplier"];
  return sensor;
}
