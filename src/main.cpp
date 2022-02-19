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
  Serial.print/*debugToSerial*/("set debugging to: ");
  Serial.print/*debugToSerial*/(restSrv.argName(0));
  Serial.print/*debugToSerial*/(restSrv.arg(0));
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
  Serial.println/*debugLineToSerial*/(" => " + response);
  restSrv.send(200, "text/json", response);
}

String processOutput(String obj, String value) {
  String result = "";
  for (int index = 0; index < outputQuantity; index++) {
    if (obj.equals(outputInfo[index].name)) {
      String request = "/" + obj + "=" + value;
      Serial.println/*debugLineToSerial*/(request);
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
          setOutputValue(index, processPWM(value.toInt(), outputInfo[index].pin));
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
        setOutputValue(index, processPWM(0, outputInfo[index].pin));
        break;
      default:
        break;
    }
  }
}

void setAllOff() {
  Serial.print/*debugToSerial*/("set all off: ");
  stopSceneNow();
  String response = "{\"outputs\":[";

  for (int index = 0; index < outputQuantity; index++) {
    Serial.print/*debugToSerial*/(outputInfo[index].name + " => " + outputInfo[index].value);
    String lValue = "0";
    if (outputInfo[index].type == oLED) {
      lValue = "OFF";
    }
    String result = processOutput(outputInfo[index].name, lValue);
    if (index > 0) {
      response += ", ";
      Serial.print/*debugToSerial*/(" - ");
    }
    response += result;
  }

  response += "]}";
  Serial.println/*debugLineToSerial*/(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setAllHalfBtn() {
  for (int index = 0; index < outputQuantity; index++) {
    if (outputInfo[index].type == oPWM) {
      setOutputValue(index, processPWM(127, outputInfo[index].pin));
    }
  }
}

void setAllHalf() {
  Serial.print/*debugToSerial*/("set all half:");
  stopSceneNow();
  String response = "{\"outputs\":[";

  int count = 0;
  for (int index = 0; index < outputQuantity; index++) {
    if (outputInfo[index].type == oPWM) {
      Serial.print/*debugToSerial*/(outputInfo[index].name + " => " + outputInfo[index].value);
      String result = processOutput(outputInfo[index].name, "127");
      if (count > 0) {
        response += ", ";
        Serial.print/*debugToSerial*/(" - ");
      }
      count++;
      response += result;
    }
  }
    
  response += "]}";
  Serial.println/*debugLineToSerial*/(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setAllFull() {
  Serial.print/*debugToSerial*/("set all full:");
  stopSceneNow();
  String response = "{\"outputs\":[";

  for (int index = 0; index < outputQuantity; index++) {
    Serial.println/*debugLineToSerial*/(outputInfo[index].name + " => " + outputInfo[index].value);
    String lValue = "255";
    if (outputInfo[index].type == oLED) {
      lValue = "ON";
    }
    String result = processOutput(outputInfo[index].name, lValue);
    if (index > 0) {
      response += ", ";
      Serial.print/*debugToSerial*/(" - ");
    }
    response += result;
  }

  response += "]}";
  Serial.println/*debugLineToSerial*/(" => " + response);
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
      setOutputValue(index, processPWM(newValue, outputInfo[index].pin));
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
  Serial.print/*debugToSerial*/("set scene steps to: ");
  String response = "{\"extra\":[";

  int argCount = restSrv.args();
  //Serial.println/*debugLineToSerial*/(argCount);
  String sceneName;
  for (int index = 0; index < argCount; index++) {
    if (restSrv.argName(index) == "steps") {
      sceneInfo.steps_ms = restSrv.arg(index).toInt();
    } else {
      response += "\"error\":\"wrong rest call: " + restSrv.argName(index) + " is not valid for setSceneSteps\"";
    }
  }
  Serial.print/*debugToSerial*/(sceneName);

  sceneTypes newscene = getSceneTypeFromName(sceneName);
  if (sceneInfo.active != newscene) {
    stopSceneNow();
    Serial.print/*debugToSerial*/(" - selected scene = " + getSceneTypeName(newscene));
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
  Serial.println/*debugLineToSerial*/(" => " + response);
  restSrv.send(200, "text/json", response);

}

void setPauseScene() {
  Serial.print/*debugToSerial*/("set scene pause to: ");
  String response = "{\"extra\":[";

  int argCount = restSrv.args();
  //Serial.println/*debugLineToSerial*/(argCount);
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
  Serial.print/*debugToSerial*/(sceneName);
  sceneTypes newscene = getSceneTypeFromName(sceneName);
  if (sceneInfo.active != newscene) {
    stopSceneNow();
    Serial.print/*debugToSerial*/(" - selected scene = " + getSceneTypeName(newscene));
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
  Serial.println/*debugLineToSerial*/(" => " + response);
  restSrv.send(200, "text/json", response);
}

void processAnalogButton(SensorStruct sensor) {
  switch (sensor.type) {
    case bPower:
      if (checkAllOutputsOn()) {
        setAllOffBtn();
      } else {
        setAllHalfBtn();
      }
      break;
    case bProgram:
      stopSceneNow();
      sceneInfo.active = sceneTypes(sceneInfo.active + 1);
      if (sceneInfo.active == scNone) {
        sceneInfo.active = scAllUpDown;
      }
      startScene();
      break;
    case bUp:
      setAllValueBtn(sensor.buttonIncrease);
      break;
    case bDown:
      setAllValueBtn(-sensor.buttonDecrease);
      break;
    default:
      break;
  }
}

String processAnalogButtonArrayJson(SensorStruct sensor) {
  String result = "";
  //int divider = (sensor.maxValue - sensor.minValue) / sensor.buttonQty;
  //debugToSerial("divider = " + String(divider));
  for (int index = 0; index < sensor.buttonQty; index++) {
    if (index > 0) {
      result += ", ";
    }
    result += "{\"name\": \"" + sensor.name + "-" + String(index + 1);
    int low  = sensor.buttonValues[index] - sensor.hysteresis;
    int high = sensor.buttonValues[index] + sensor.hysteresis;
    Serial.print("     index = "  + String(index) + " low = "    + String(low) + " high = "   + String(high) + " value = "  + String(sensor.value));
    if ((sensor.value > low) && (sensor.value < high)) {
      result += "\", \"value\":\"ON\"";
      processAnalogButton(sensor);
      Serial.println("    OM");
    } else {
      result += "\", \"value\":\"OFF\"";
      Serial.println("    OFF");
    }
    result += ", \"type\": \"" + getButtonTypeName(sensor.buttontypes[index]) + "\"}";
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
        response += processAnalogButtonArrayJson(sensorInfo[index]);
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

void loopBOARDsensors() {
  for (int index = 0; index < sensorQuantity; index++) {
    int iValue = sensorInfo[index].value;
    int nValue;
    int maxValue = iValue + (sensorInfo[index].hysteresis / 2);
    int minValue = iValue - (sensorInfo[index].hysteresis / 2);
    switch (sensorInfo[index].type) {
      case sDigital:
        setSensorValue(index, digitalRead(sensorInfo[index].pin));
        break;
      case sAnalog:
        nValue = analogRead(sensorInfo[index].pin);
        //debugLineToSerial("Sensor " + String(aSensor) + " gives " + String(nValue));
        //Serial.print("read = " + String(nValue) + "    ");
        if ((nValue < minValue) || (nValue > maxValue)) {
          iValue = nValue;
        }
        setSensorValue(index, iValue);
        break;
      case sAnalogButtons:
        setSensorValue(index, analogRead(sensorInfo[index].pin));
        break;
      default:
        break;
    }
    delay(sensorInfo[index].readDelay);
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
    printConfig();
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
          processPWM(outputInfo[index].value, outputInfo[index].pin);
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