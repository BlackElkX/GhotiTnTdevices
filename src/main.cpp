#include "Types.hpp"
#include "ProcessRequest.hpp"
#include "HtmlPage.hpp"
#include "Global.hpp"
#include "Scenes.hpp"
#include "PersistentSettings.hpp"
#include "Types.hpp"
#include <ArduinoJson.h>
#include <Server.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include "main.hpp"

#define  VERSION_STRING "0.0.5"

extern DebugStruct debugInfo;
extern HtmlStruct  htmlInfo;
extern SceneStruct sceneInfo;
extern WifiStruct  wifiInfo;

extern int outputQuantity;
extern OutputStruct outputInfo[USED_OUTPUT_QTY];
extern int sensorQuantity;
extern SensorStruct sensorInfo[USED_SENSOR_QTY];

WiFiServer       httpSrv(80); //webserver
ESP8266WebServer restSrv(81); //restserver
unsigned long timestamp;

void initiateValues() {
  if (debugInfo.pin > -1) {
    pinMode(debugInfo.pin, INPUT);
  }
  for (int index = 0; index < outputQuantity; index++) {
    pinMode(outputInfo[index].pin, OUTPUT);
    digitalWrite(outputInfo[index].pin, outputInfo[index].value);
    debugLineToSerial("output " + String(index) + " (" + outputInfo[index].name + ") is set: pin " + String(outputInfo[index].pin) + " (" + outputInfo[index].pinName + ") = " + String(outputInfo[index].value));
  }
  for (int index = 0; index < sensorQuantity; index++) {
    pinMode(sensorInfo[index].pin, INPUT);
    debugLineToSerial("sensor " + String(index) + " (" + sensorInfo[index].name + ") is set: pin " + String(sensorInfo[index].pin) + " (" + sensorInfo[index].pinName + ") = " + String(sensorInfo[index].value));
  }
}

void setupSerialDebug() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Version " + String(VERSION_STRING));
}

void setupWiFi() {
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting to the Newtork");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiInfo.nameSSID, wifiInfo.passphrase);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected to ");
  Serial.println(wifiInfo.nameSSID);
  Serial.print("IP Address of network: ");
  Serial.println(WiFi.localIP());
}

// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += restSrv.uri();
  message += "\nMethod: ";
  message += (restSrv.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += restSrv.args();
  message += "\n";
  for (uint8_t i = 0; i < restSrv.args(); i++) {
    message += " " + restSrv.argName(i) + ": " + restSrv.arg(i) + "\n";
  }
  restSrv.send(404, "text/plain", message);
}

void setDebuging() {
  debugToSerial("set debugging to: ");
  debugToSerial(restSrv.argName(0));
  debugToSerial(restSrv.arg(0));
  String response = "{\"setDebug\":\"";
  if (restSrv.arg(0) == "ON") {
    setDebug(true);
    response += "ON";
  } else if (restSrv.arg(0) == "OFF") {
    setDebug(false);
    response += "OFF";
  } else {
    response += "\", \"error\":\"" + restSrv.arg(0);
  }
  response += "\"}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

String processOutput(String obj, String value) {
  String result = "";
  for (int index = 0; index < outputQuantity; index++) {
    if (obj.equals(outputInfo[index].name)) {
      String request = "/" + obj + "=" + value;
      debugLineToSerial(request);
      String lValue = "ON";
      switch (outputInfo[index].type) {
        case oLED:
          setOutputValue(index, processLED(request, outputInfo[index].name, outputInfo[index].value, outputInfo[index].pin));
          if (outputInfo[index].value == 0) {
            lValue = "OFF";
          }
          result = "{\"name\":\"" + outputInfo[index].name + "\", \"value\":\"" + lValue + "\"}";
          break;
        case oPWM:
          analogWrite(outputInfo[index].pin, value.toInt());
          setOutputValue(index, value.toInt());
          result = "{\"name\":\"" + outputInfo[index].name + "\", \"value\":\"" + String(outputInfo[index].value) + "\"}";
          break;
        default:
          result = "{\"name\":\"" + outputInfo[index].name + "\", \"value\":\"type unknown\"}";
          break;
      }
    }
  }
  return result;
}

void getSceneNames() {
  debugToSerial("getting scene names");
  String response = "{\"scenes\":[";
  int index = 0;
  for (sceneTypes scene = scAllUpDown; scene <= scNone; scene = sceneTypes(scene + 1)) {
    if (index > 0) {
      response += ", ";
    }
    index++;
    response += "\"" + getSceneTypeName(scene) + "\"";
  }
  response += "], \"default\":\"" + getSceneTypeName(scNone) + "\"}";
  
  debugLineToSerial(" => " + response);
  
  restSrv.send(200, "text/json", response);
}

void stopSceneNow() {
  if (sceneInfo.active != scNone) {
    stopScene();
    sceneInfo.active = scNone;
  }
}

void setOutput() {
  debugToSerial("setting output to: ");
  stopSceneNow();
  String response = "{\"outputs\":[";

  int argCount = restSrv.args();
  for (int index = 0; index < argCount - 1; index++) {
    debugToSerial(restSrv.argName(index) + " => " + restSrv.arg(index));
    String result = processOutput(restSrv.argName(index), restSrv.arg(index));
    if (index > 0) {
      response += ", ";
      debugToSerial(" - ");
    }
    response += result;
  }

  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setAllOffBtn() {
  stopSceneNow();
  for (int index = 0; index < outputQuantity; index++) {
    switch (outputInfo[index].type) {
      case oLED:
        digitalWrite(outputInfo[index].pin, LOW);
        setOutputValue(index, LOW);
        break;
      case oPWM:
        analogWrite(outputInfo[index].pin, 0);
        setOutputValue(index, 0);
        break;
      default:
        break;
    }
  }
}

void setAllOff() {
  debugToSerial("set all off: ");
  stopSceneNow();
  String response = "{\"outputs\":[";

  for (int index = 0; index < outputQuantity; index++) {
    debugToSerial(outputInfo[index].name + " => " + outputInfo[index].value);
    String lValue = "0";
    if (outputInfo[index].type == oLED) {
      lValue = "OFF";
    }
    String result = processOutput(outputInfo[index].name, lValue);
    if (index > 0) {
      response += ", ";
      debugToSerial(" - ");
    }
    response += result;
  }

  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setAllHalfBtn() {
  for (int index = 0; index < outputQuantity; index++) {
    if (outputInfo[index].type == oPWM) {
      analogWrite(outputInfo[index].pin, 127);
      setOutputValue(index, 127);
    }
  }
}

void setAllHalf() {
  debugToSerial("set all half:");
  stopSceneNow();
  String response = "{\"outputs\":[";

  int count = 0;
  for (int index = 0; index < outputQuantity; index++) {
    if (outputInfo[index].type == oPWM) {
      debugToSerial(outputInfo[index].name + " => " + outputInfo[index].value);
      String result = processOutput(outputInfo[index].name, "127");
      if (count > 0) {
        response += ", ";
        debugToSerial(" - ");
      }
      count++;
      response += result;
    }
  }
    
  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setAllFull() {
  debugToSerial("set all full:");
  stopSceneNow();
  String response = "{\"outputs\":[";

  for (int index = 0; index < outputQuantity; index++) {
    debugLineToSerial(outputInfo[index].name + " => " + outputInfo[index].value);
    String lValue = "255";
    if (outputInfo[index].type == oLED) {
      lValue = "ON";
    }
    String result = processOutput(outputInfo[index].name, lValue);
    if (index > 0) {
      response += ", ";
      debugToSerial(" - ");
    }
    response += result;
  }

  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setAllValueBtn(int amount) {
  for (int index = 0; index < outputQuantity; index++) {
    if (outputInfo[index].type == oPWM) {
      int newValue = outputInfo[index].value + amount;
      if (newValue < 0) {
        newValue = 0;
      }
      if (newValue > 255) {
        newValue = 255;
      }
      analogWrite(outputInfo[index].pin, newValue);
      setOutputValue(index, newValue);
    }
  }
}

bool checkAllOutputsOn() {
  bool result = false;
  for (int index = 0; index < outputQuantity; index++) {
    if (outputInfo[index].value > 0) {
      result = true;
    }
  }
  return result;
}

void setAllValue() {
  debugToSerial("set all:");
  stopSceneNow();
  String response = "{\"outputs\":[";

  int argCount = restSrv.args();
  debugLineToSerial(argCount);
  String aValue = restSrv.arg(0);
  int count = 0;
  for (int index = 0; index < outputQuantity; index++) {
    debugLineToSerial(outputInfo[index].name + " => " + outputInfo[index].value);
    String lValue = aValue;
    if (outputInfo[index].type == oLED) {
      if (aValue == "0") {
        lValue = "OFF";
      } else {
        lValue = "ON";
      }
    }
    String result = processOutput(outputInfo[index].name, lValue);
    if (count > 0) {
      response += ", ";
      debugToSerial(" - ");
    }
    count++;
    response += result;
  }
  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setNewScene() {
  debugToSerial("set scene to: ");
  String response = "{\"extra\":[";
  int argCount = restSrv.args();
  String sceneName;
  for (int index = 0; index < argCount; index++) {
    if (restSrv.argName(index) == "name") {
      sceneName = restSrv.arg(index);
    } else if (restSrv.argName(index) == "steps") {
      sceneInfo.steps_ms = restSrv.arg(index).toInt();
    } else if (restSrv.argName(index) == "pause") {
      sceneInfo.pause_ms = restSrv.arg(index).toInt();
    }
  }

  debugToSerial(sceneName);
  sceneTypes newscene = getSceneTypeFromName(sceneName);
  if (sceneInfo.active != newscene) {
    stopSceneNow();
    debugToSerial(" - selected scene = " + getSceneTypeName(newscene));
    sceneInfo.active = newscene;
    startScene();
  }

  response += "{\"name\":\"scene\", \"value\":\"";
  response += getSceneTypeName(sceneInfo.active);
  response += "\"}";
  response += ", {\"name\":\"steps\", \"value\":\"";
  response += String(sceneInfo.steps_ms);
  response += "\"}";
  response += ", {\"name\":\"pause\", \"value\":\"";
  response += String(sceneInfo.pause_ms);
  response += "\"}";
  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setStepsScene() {
  debugToSerial("set scene steps to: ");
  String response = "{\"extra\":[";

  int argCount = restSrv.args();
  debugLineToSerial(argCount);
  String sceneName;
  for (int index = 0; index < argCount; index++) {
    if (restSrv.argName(index) == "steps") {
      sceneInfo.steps_ms = restSrv.arg(index).toInt();
    } else {
      response += "\"error\":\"wrong rest call: " + restSrv.argName(index) + " is not valid for setSceneSteps\"";
    }
  }
  debugToSerial(sceneName);

  sceneTypes newscene = getSceneTypeFromName(sceneName);
  if (sceneInfo.active != newscene) {
    stopSceneNow();
    debugToSerial(" - selected scene = " + getSceneTypeName(newscene));
    sceneInfo.active = newscene;
    startScene();
  }

  response += "{\"name\":\"scene\", \"value\":\"";
  response += getSceneTypeName(sceneInfo.active);
  response += "\"}";
  response += ", {\"name\":\"steps\", \"value\":\"";
  response += String(sceneInfo.steps_ms);
  response += "\"}";
  response += ", {\"name\":\"pause\", \"value\":\"";
  response += String(sceneInfo.pause_ms);
  response += "\"}";
  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);

}

void setPauseScene() {
  debugToSerial("set scene pause to: ");
  String response = "{\"extra\":[";

  int argCount = restSrv.args();
  debugLineToSerial(argCount);
  String sceneName;
  for (int index = 0; index < argCount; index++) {
    if (restSrv.argName(index) == "name") {
      sceneName = restSrv.arg(index);
    } else if (restSrv.argName(index) == "steps") {
      sceneInfo.steps_ms = restSrv.arg(index).toInt();
    } else if (restSrv.argName(index) == "pause") {
      sceneInfo.pause_ms = restSrv.arg(index).toInt();
    }
  }
  debugToSerial(sceneName);
  sceneTypes newscene = getSceneTypeFromName(sceneName);
  if (sceneInfo.active != newscene) {
    stopSceneNow();
    debugToSerial(" - selected scene = " + getSceneTypeName(newscene));
    sceneInfo.active = newscene;
    startScene();
  }

  response += "{\"name\":\"scene\", \"value\":\"";
  response += getSceneTypeName(sceneInfo.active);
  response += "\"}";
  response += ", {\"name\":\"steps\", \"value\":\"";
  response += String(sceneInfo.steps_ms);
  response += "\"}";
  response += ", {\"name\":\"pause\", \"value\":\"";
  response += String(sceneInfo.pause_ms);
  response += "\"}";
  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

void startActionOfAnalogButton(int index, int btnIndex) {
  switch (sensorInfo[index].btnArray[btnIndex].types) {
    case bPower:
      if (checkAllOutputsOn()) {
        setAllOffBtn();
      } else {
        setAllHalfBtn();
      }
      break;
    case bUp:
      setAllValueBtn(sensorInfo[index].btnArrayIncrease);
      break;
    case bDown:
      setAllValueBtn(-sensorInfo[index].btnArrayDecrease);
      break;
    case bProgram:
      sceneInfo.active = getNextSceneType(sceneInfo.active);
      startScene();
      break;
    default:
      break;
  }
}

void processAnalogButtonArray(int index) {
  for (int btnIndex = 0; btnIndex < 4; btnIndex++) {
    int low  = sensorInfo[index].btnArray[btnIndex].resitor - sensorInfo[index].hysteresis;
    int high = sensorInfo[index].btnArray[btnIndex].resitor + sensorInfo[index].hysteresis;
    if ((sensorInfo[index].value > low) && (sensorInfo[index].value < high)) {
      if (!sensorInfo[index].btnArray[btnIndex].pressed) {
        sensorInfo[index].btnArray[btnIndex].pressed = true;
        startActionOfAnalogButton(index, btnIndex);
      }
    } else {
      sensorInfo[index].btnArray[btnIndex].pressed = false;
    }
  }
}

String getAnalogButtonArrayDetails(int index) {
  String result = "";
  for (int btnIndex = 0; btnIndex < 4; btnIndex++) {
    if (btnIndex > 0) {
      result += ", ";
    }
    result += "{\"name\": \"" + sensorInfo[index].name + "-" + String(btnIndex + 1) + "\", \"value\":";
    if (sensorInfo[index].btnArray[btnIndex].pressed) {
      result += "\"ON\"";
    } else {
      result += "\"OFF\"";
    }
    result += ", \"type\": \"" + getButtonTypeName(sensorInfo[index].btnArray[btnIndex].types) + "\"}";
  }
  debugLineToSerial(" " + result);
  return result;
}

void getStatus() {
  debugToSerial("getting status from rest api: ");
  String response = "{";

  //outputs
  response += "\"outputs\":[";
  int  lLEDValue    = -1;
  bool LEDDifferent = false;
  int  lPWMValue    = -1;
  bool PWMDifferent = false;
  for (int index = 0; index < outputQuantity; index++) {
    if (index > 0) {
      response += ", ";
    }
    String lValue = "ON";
    switch (outputInfo[index].type) {
      case oLED:
        if (outputInfo[index].value == 0) {
          lValue = "OFF";
        }
        if (lLEDValue != outputInfo[index].value) {
          if (lLEDValue > -1) {
            LEDDifferent = true;
          }
          lLEDValue = outputInfo[index].value;
        }
        response += "{\"name\":\"" + outputInfo[index].name + "\", \"value\":\"" + lValue + "\", \"type\":\"" + getOutputTypeName(outputInfo[index].type) + "\"}";
        break;
      case oPWM:
        response += "{\"name\":\"" + outputInfo[index].name + "\", \"value\":\"" + String(outputInfo[index].value) + "\", \"type\":\"" + getOutputTypeName(outputInfo[index].type) + "\"}";
        if (lPWMValue != outputInfo[index].value) {
          if (lPWMValue > -1) {
            PWMDifferent = true;
          }
          lPWMValue = outputInfo[index].value;
        }
        break;
      default:
        response += "{\"name\":\"" + outputInfo[index].name + "\", \"value\":\"n/a\",\"type\":\"type unknown\"}";
        break;
    }
  }
  response += "]";

  //sensors
  response += ", \"sensors\":[";
  for (int index = 0; index < sensorQuantity; index++) {
    if (index > 0) {
      response += ", ";
    }
    String lValue = "ON";
    switch (sensorInfo[index].type) {
      case sDigital:
        if (sensorInfo[index].value == 0) {
          lValue = "OFF";
        }
        response += "{\"name\":\"" + sensorInfo[index].name + "\", \"value\":\"" + lValue + "\", \"type\":\"" + getSensorTypeName(sensorInfo[index].type) + "\"}";
        break;
      case sAnalog:
        response += "{\"name\":\"" + sensorInfo[index].name + "\", \"value\":\"" + String(sensorInfo[index].value) + "\", \"type\":\"" + getSensorTypeName(sensorInfo[index].type) + "\"}";
        break;
      case sAnalogButtons:
        response += "{\"name\":\"" + sensorInfo[index].name + "\", \"value\":\"" + String(sensorInfo[index].value) + "\", \"type\":\"" + getSensorTypeName(sensorInfo[index].type) + "\", \"buttons\":[";
        response += getAnalogButtonArrayDetails(index);
        response += "]}";
        break;
      default:
        response += "{\"name\":\"" + sensorInfo[index].name + "\", \"value\":\"type unknown\"}";
        break;
    }    
  }
  response += "]";

  //gezamelijk
  response += ", \"generals\":[";

  if (!LEDDifferent) {
    String lValue = "ON";
    if (lLEDValue == 0) {
      lValue = "OFF";
    }
    response += "{\"name\":\"LEDS\", \"value\":\"" + lValue + "\"}";
  }
  if (!LEDDifferent && !PWMDifferent) {
    response += ", ";
  }
  if (!PWMDifferent) {
    response += "{\"name\":\"PWMS\", \"value\":\"" + String(lPWMValue) + "\"}";
  }
  response += "]";

  //extras, like scene, debug status, ...
  response += ", \"extra\":[{\"name\":\"scene\", \"value\":\"";
  response += getSceneTypeName(sceneInfo.active);
  response += "\"}";
  response += ", {\"name\":\"steps\", \"value\":\"";
  response += String(sceneInfo.active);
  response += "\"}";
  response += ", {\"name\":\"pause\", \"value\":\"";
  response += String(sceneInfo.active);
  response += "\"}";
  response += ", {\"name\":\"debug\", \"value\":\"";
  if (getDebug()) {
    response += "ON";
  } else {
    response += "OFF";
  }
  response += "\"}, {\"name\":\"debugPin\", \"value\":\"";
  if (getDebugPinState()) {
    response += "ON";
  } else {
    response += "OFF";
  }
  response += "\"}";
  response += "]}";

  debugLineToSerial(response);

  restSrv.send(200, "text/json", response);
}

// Define routing
void restServerRouting() {
  restSrv.on("/", HTTP_GET, []() {
    restSrv.send(200, F("text/html"),
    F("Welcome to the REST Web Server"));
  });
  restSrv.on(F("/status"),        HTTP_GET, getStatus);
  restSrv.on(F("/getSceneNames"), HTTP_GET, getSceneNames);
  restSrv.on(F("/setOutput"),     HTTP_PUT, setOutput);
  restSrv.on(F("/setDebug"),      HTTP_PUT, setDebuging);
  restSrv.on(F("/setAllOff"),     HTTP_PUT, setAllOff);
  restSrv.on(F("/setAllHalf"),    HTTP_PUT, setAllHalf);
  restSrv.on(F("/setAllFull"),    HTTP_PUT, setAllFull);
  restSrv.on(F("/setAllValue"),   HTTP_PUT, setAllValue);
  restSrv.on(F("/setScene"),      HTTP_PUT, setNewScene);
  restSrv.on(F("/setSceneSteps"), HTTP_PUT, setStepsScene);
  restSrv.on(F("/setScenePause"), HTTP_PUT, setPauseScene);
}

void setupRestSrv() {
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  restServerRouting();
  // Set not found response
  restSrv.onNotFound(handleNotFound);
  // Start rest server
  restSrv.begin(wifiInfo.restPort);
  Serial.print("REST Server started, use URL: ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(wifiInfo.restPort);
  Serial.println("/status to get the status of everything.");
}

void loopREST() {
  restSrv.handleClient();
}
/*  end rest server  */

/*  start regular html page server  */
void setupHtmlSrv() {
  httpSrv.begin(wifiInfo.httpPort);  // Starts the Server
  Serial.print("HTTP Server started on port ");
  Serial.println(wifiInfo.httpPort);
  Serial.print("Copy and paste the following URL: https://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(wifiInfo.httpPort);
  Serial.println("/");
}

void loopHTTPpage(WiFiClient client) {
  htmlPageStart(client);
  htmlPageHeadRefresh(client,  wifiInfo.httpRefreshRate);
  htmlPageHeadEndBodyStart(client);

  htmlPageBodyTableStart(client, "Control outputs", 5, true, wifiInfo.httpRefreshRate);
  htmlOutputTitle(client, 5);
  for (int index = 0; index < outputQuantity; index++) {
    switch (outputInfo[index].type) {
      case oLED:
        htmlLedLine(client, outputInfo[index].name, outputInfo[index].value);
        break;
      case oPWM:
        htmlPwmLine(client, outputInfo[index].name, outputInfo[index].value);
        break;
      default:
        break;
    }
  }
  htmlPageTableEnd(client);
  htmlPageEmptyLine(client);

  htmlPageBodyTableStart(client, "Sensors and buttons", 2, false, wifiInfo.httpRefreshRate);
  htmlSensorTitle(client);
  for (int index = 0; index < sensorQuantity; index++) {
    switch (sensorInfo[index].type) {
      case sDigital:
        htmlSensorLine(client, sensorInfo[index].name, sensorInfo[index].value);
        break;
      case sAnalog:
        htmlSensorLine(client, sensorInfo[index].name, sensorInfo[index].value);
        break;
      case sAnalogButtons:
        htmlSensorLine(client, sensorInfo[index].name, sensorInfo[index].value);
        break;
      default:
        break;
    }
  }
  htmlPageTableEnd(client);

  htmlPageBodyEnd(client);
}
/*  end regular html page server  */

void readDigitalSensor(int index) {
  int newSensorValue = digitalRead(sensorInfo[index].pin);
  if ((newSensorValue == sensorInfo[index].value)
   && (sensorInfo[index].counter > 0)) {
     sensorInfo[index].counter--;
   }
  if (newSensorValue == sensorInfo[index].value) {
    sensorInfo[index].counter++;
  }
  if (sensorInfo[index].counter >= sensorInfo[index].debounceCount) {
    sensorInfo[index].counter = 0;
    sensorInfo[index].value = newSensorValue;
  }
}

void readAnalogSensor(int index) {
  int oldValue = sensorInfo[index].value;
  int newValue = analogRead(sensorInfo[index].pin);
  int maxValue = oldValue + (sensorInfo[index].hysteresis / 2);
  int minValue = oldValue - (sensorInfo[index].hysteresis / 2);
  if (maxValue > sensorInfo[index].maxValue) {
    maxValue = sensorInfo[index].maxValue;
  }
  if (minValue > sensorInfo[index].maxValue) {
    minValue = sensorInfo[index].maxValue - (sensorInfo[index].hysteresis / 2);
  } else if (minValue < sensorInfo[index].minValue) {
    minValue = sensorInfo[index].minValue;
  }
  if (newValue > sensorInfo[index].maxValue) {
    newValue = sensorInfo[index].maxValue;
  }
  if ((newValue < minValue) || (newValue > maxValue)) {
    oldValue = newValue;
    if ((newValue == sensorInfo[index].value)
     && (sensorInfo[index].counter > 0)) {
      sensorInfo[index].counter--;
     }
    if (newValue == sensorInfo[index].value) {
      sensorInfo[index].counter++;
    }
    if (sensorInfo[index].counter >= sensorInfo[index].debounceCount) {
//      Serial.println("old = " + String(oldValue) + " new = " + String(newValue) + " max = " + String(maxValue) + " min = " + String(minValue));
      sensorInfo[index].counter = 0;
      sensorInfo[index].value = newValue;
      if (sensorInfo[index].type == sAnalogButtons) {
        //check whish button
        processAnalogButtonArray(index);
      } else {
        //process value from sensor
      }
    }
  }
}

void loopBOARDsensors() {
  if (millis() != timestamp) {
    for (int index = 0; index < sensorQuantity; index++) {
      if (sensorInfo[index].type == sDigital) {
        readDigitalSensor(index);
      } else {
        readAnalogSensor(index);
      }
      delay(sensorInfo[index].readDelay);
    }
    timestamp = millis();
  }
}

void loopHTTPBoardOutputs(String request) {
  for (int index = 0; index < outputQuantity; index++) {
    switch (outputInfo[index].type) {
      case oLED:
        setOutputValue(index, processLED(request, outputInfo[index].name, outputInfo[index].value, outputInfo[index].pin));
        break;
      case oPWM:
        setOutputValue(index, processHtmlPWM(request, outputInfo[index].name, outputInfo[index].value, outputInfo[index].pin));
        break;
      default:
        break;
    }
  }

  wifiInfo.httpRefreshRate = processRefresh(request, wifiInfo.httpRefreshRate);
}

void loopHTTPDebug() {
  delay(1);
  debugLineToSerial("Client disonnected");
  debugLineToSerial("");
  debugToSerial("Main - Refreshing at rate ");
  debugLineToSerial(wifiInfo.httpRefreshRate);
  debugLineToSerial("");
}

void initiate() {
  Serial.println("config read");
  initiateValues();
  Serial.println("values initiated");
  setupWiFi();
  Serial.println("WiFi started");
  setupRestSrv();
  Serial.println("REST server started");
  setupHtmlSrv();
  Serial.println("HTML server started");
  Serial.println("end setup");
}

void setup() {
  setupSerialDebug();
  Serial.println("started");
  Serial.println("get config file");
  
  if (readConfig()) {
    initiate();
  } else {
    Serial.println("config file not read...");
  }
}

void loopScene() {
  if (sceneInfo.active != scNone) {
    nextStepInScene();
    for (int index = 0; index < outputQuantity; index++) {
      switch (outputInfo[index].type) {
        case oPWM:
          analogWrite(outputInfo[index].pin, outputInfo[index].value);
          break;
        default:
          break;
      }
    }
    delay(sceneInfo.pause_ms);
  }
}

void loop() {
  loopREST();
  loopScene();
  loopBOARDsensors();
  HtmlStruct httpData;
  httpData.client = httpSrv.available();
  if (httpData.client) {
    debugLineToSerial("Waiting for new client");
    while(!httpData.client.available()) {
      delay(1);
    }
    httpData.request = httpData.client.readStringUntil('\r');
    debugLineToSerial(httpData.request);
    httpData.client.flush();
    loopHTTPBoardOutputs(httpData.request);
    loopHTTPpage(httpData.client);
    loopHTTPDebug();
  }  
}