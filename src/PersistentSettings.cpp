#include <LittleFS.h>
#include <WString.h>
#include <Global.hpp>
#include <ArduinoJson.h>
#include "Types.hpp"
#include "PersistentSettings.hpp"

JsonObject configJson;

//global used variables
extern DebugStruct debugInfo;
extern HtmlStruct  htmlInfo;
extern SceneStruct sceneInfo;
extern WifiStruct  wifiInfo;

extern int outputQuantity;
extern OutputStruct outputInfo[USED_OUTPUT_QTY];
extern int sensorQuantity;
extern SensorStruct sensorInfo[USED_SENSOR_QTY];

bool readConfig() {
  debugLineToSerial("start json doc");
  DynamicJsonDocument jsonDoc(MAX_FILE_SIZE);
  if (!LittleFS.begin()) {
    debugLineToSerial("An error occured while mounting file system");
    configJson["error"] = "An error occured while mounting file system";
    return false;
  }
  debugLineToSerial("start file");
  File file = LittleFS.open("/config.json", "r");
  if (!file) {
    debugLineToSerial("Failed to open config.json for reading");
    configJson["error"] = "Failed to open config.json for reading";
    return false;
  }
  debugLineToSerial("start reading");
  
  size_t size = file.size();
  debugToSerial("Size = ");
  debugLineToSerial(size);
  if (size > MAX_FILE_SIZE) {
    debugLineToSerial("Config file size is too large");
    configJson["error"] = "Config file size is too Large";
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  file.readBytes(buf.get(), size);
 
  debugLineToSerial("file is read:");
  DeserializationError error = deserializeJson(jsonDoc, buf.get());
  configJson = jsonDoc.as<JsonObject>();
  //configJson["error"] = "";
  //configJson.printTo(Serial);
  debugLineToSerial(String(configJson));
    
  file.close();
  
  if (error) {
    debugToSerial("deserializeJson() failed: ");
    debugLineToSerial(error.f_str());
    configJson["error"] = "deserializeJson() failed: " + String(error.f_str());
    return false;
  }
  debugInfo = getDebugSettings();
  wifiInfo  = getWifiSettings();
  sceneInfo = getScene();
  outputQuantity = getOutputQty();
  for (int index = 0; index < outputQuantity; index++) {
    outputInfo[index] = getOutput(index);
  }
 
  sensorQuantity = getSensorQty();
  for (int index = 0; index < sensorQuantity; index++) {
    sensorInfo[index] = getSensor(index);
  }

  return true;
}

bool saveConfig(JsonObject& configJson) {
  if (!LittleFS.begin()) {
    debugLineToSerial("An error occured while mounting file system");
    return false;
  }
  File file = LittleFS.open("/config.json", "w");
  if (!file) {
    debugLineToSerial("Failed to open config.json for writing");
    return false;
  }
  serializeJson(configJson, file);
  file.close();
  debugLineToSerial("config saved to file");
  return readConfig(/*configJson*/);
}

void printConfig() {
  Serial.println("configJson");
  //debugLineToSerial(String(*configJson));
}

String getDeviceName() {
  return String(configJson["deviceName"]);
}

SceneStruct getScene() {
  SceneStruct scenestruct;
  scenestruct.active   = getSceneTypeFromName(configJson["scene"]["active"]);
  scenestruct.steps_ms = configJson["scene"]["steps_ms"];
  scenestruct.pause_ms = configJson["scene"]["pause_ms"];
  return scenestruct;
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
  output.type           = getOutputTypeFromName(outputArray[index]["type"]);
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
  sensor.type       = getSensorTypeFromName(sensorArray[index]["type"]);
  sensor.hysteresis = sensorArray[index]["hysteresis"];
  sensor.minValue   = sensorArray[index]["minvalue"];
  sensor.maxValue   = sensorArray[index]["maxvalue"];
  sensor.minRemap   = sensorArray[index]["minremap"];
  sensor.maxRemap   = sensorArray[index]["maxremap"];
  sensor.multiplier = sensorArray[index]["multiplier"];
  sensor.buttonQty  = sensorArray[index]["buttonQty"];
  for (int btnIndex = 0; btnIndex < 4; btnIndex++) {
    sensor.buttontypes[btnIndex]  = getButtonTypeFromName(sensorArray[index]["buttonfunctions"][btnIndex]);
    sensor.buttonValues[btnIndex] = sensorArray[index]["buttonValues"][btnIndex];
    //Serial.println("button value for " + String(index) + " = " + String(sensor.buttonValues[btnIndex]));
  }
  sensor.buttonIncrease = sensorArray[index]["buttonIncrease"];
  sensor.buttonDecrease = sensorArray[index]["buttonDecrease"];
  sensor.readDelay      = sensorArray[index]["readDelay"];
  return sensor;
}

bool getDebug() {
  return debugInfo.enabled;
}

void setDebug(bool debug) {
  debugInfo.enabled = debug;
}

void setActiveScene(String aActiveScene) {
  sceneInfo.active = getSceneTypeFromName(aActiveScene);
}

void setOutputValue(int index, int value) {
  outputInfo[index].value = value;
}

void setOutputSceneDir(int index, int sceneDirection) {
  outputInfo[index].sceneDirection = sceneDirection;
}

void setSensorValue(int index, int value) {
  sensorInfo[index].value = value;
}
