#include "Types.hpp"
#include "ProcessSensors.hpp"
#include "ProcessRequest.hpp"
#include "HtmlPage.hpp"
#include "Global.hpp"
#include "Scenes.hpp"
#include "PersistentSettings.hpp"
#include "Types.hpp"
#include <Server.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

static int refreshrate;
WiFiServer  httpSrv(80);
ESP8266WebServer restSrv(81);

static sceneTypes activeScene = scNone;
static int stepsScene = 5;
static int pauseScene = 100;

void initiateValues() {
  int outputqty = getOutputQty();
  int sensorqty = getSensorQty();
  for (int index = 0; index < outputqty; index++) {
    pinMode(getOutput(index).pin, OUTPUT);
    digitalWrite(getOutput(index).pin, getOutput(index).value);
  }
  for (int index = 0; index < sensorqty; index++) {
    pinMode(getSensor(index).pin, INPUT);
  }
}

void setupSerialDebug() {
  Serial.begin(getDebugSettings().baudrate);
}

WifiStruct setupWiFi() {
  debugLineToSerial("");
  debugLineToSerial("");
  debugLineToSerial("");
  debugToSerial("Connecting to the Newtork");
  WifiStruct wifiSettings = getWifiSettings();
  //httpSrv.begin(wifiSettings.httpPort);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSettings.nameSSID, wifiSettings.passphrase);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    debugToSerial(".");
  }

  refreshrate = wifiSettings.httpRefreshRate;
  debugLineToSerial("");
  debugToSerial("WiFi connected to ");
  debugLineToSerial(wifiSettings.nameSSID);
  debugToSerial("IP Address of network: ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(wifiSettings.httpPort);
  return wifiSettings;
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

void getStatus() {
  debugToSerial("getting status from rest api: ");
  String response = "{";

  //outputs
  response += "\"outputs\":[";
  int outputqty     = getOutputQty();
  int  lLEDValue    = -1;
  bool LEDDifferent = false;
  int  lPWMValue    = -1;
  bool PWMDifferent = false;
  for (int index = 0; index < outputqty; index++) {
    if (index > 0) {
      response += ", ";
    }
    String lValue = "ON";
    switch (getOutput(index).type) {
      case oLED:
        if (getOutput(index).value == 0) {
          lValue = "OFF";
        }
        if (lLEDValue != getOutput(index).value) {
          if (lLEDValue > -1) {
            LEDDifferent = true;
          }
          lLEDValue = getOutput(index).value;
        }
        response += "{\"name\":\"" + getOutput(index).name + "\", \"value\":\"" + lValue + "\"}";
        break;
      case oPWM:
        response += "{\"name\":\"" + getOutput(index).name + "\", \"value\":\"" + String(getOutput(index).value) + "\"}";
        if (lPWMValue != getOutput(index).value) {
          if (lPWMValue > -1) {
            PWMDifferent = true;
          }
          lPWMValue = getOutput(index).value;
        }
        break;
      default:
        response += "{\"name\":\"" + getOutput(index).name + "\", \"value\":\"type unknown\"}";
        break;
    }
  }
  response += "]";
  
  //sensors
  int sensorqty = getSensorQty();
  response += ", \"sensors\":[";
  for (int index = 0; index < sensorqty; index++) {
    if (index > 0) {
      response += ", ";
    }
    String lValue = "ON";
    switch (getSensor(index).type) {
      case sDigital:
        if (getSensor(index).value == 0) {
          lValue = "OFF";
        }
        response += "{\"name\":\"" + getSensor(index).name + "\", \"value\":\"" + lValue + "\"}";
        break;
      case sAnalog:
        response += "{\"name\":\"" + getSensor(index).name + "\", \"value\":\"" + String(getSensor(index).value) + "\"}";
        break;
      default:
        response += "{\"name\":\"" + getSensor(index).name + "\", \"value\":\"type unknown\"}";
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
  response += getSceneTypeName(activeScene);
  response += "\"}";
  response += ", {\"name\":\"steps\", \"value\":\"";
  response += String(stepsScene);
  response += "\"}";
  response += ", {\"name\":\"pause\", \"value\":\"";
  response += String(pauseScene);
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
  for (int index = 0; index < getOutputQty(); index++) {
    if (obj.equals(getOutput(index).name)) {
      String request = "/" + obj + "=" + value;
      debugLineToSerial(request);
      String lValue = "ON";
      switch (getOutput(index).type) {
        case oLED:
          setOutputValue(index, processLED(request, getOutput(index).name, getOutput(index).value, getOutput(index).pin));
          if (getOutput(index).value == 0) {
            lValue = "OFF";
          }
          result = "{\"name\":\"" + getOutput(index).name + "\", \"value\":\"" + lValue + "\"}";
          break;
        case oPWM:
          setOutputValue(index, processRestPWM(getOutput(index).name, value.toInt(), getOutput(index).pin));
          result = "{\"name\":\"" + getOutput(index).name + "\", \"value\":\"" + String(getOutput(index).value) + "\"}";
          break;
        default:
          result = "{\"name\":\"" + getOutput(index).name + "\", \"value\":\"type unknown\"}";
          break;
      }
    }
  }
  return result;
}

void stopSceneNow() {
  if (activeScene != scNone) {
    stopScene();
    activeScene = scNone;
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

void setAllOff() {
  debugToSerial("set all off: ");
  stopSceneNow();
  String response = "{\"outputs\":[";

  for (int index = 0; index < getOutputQty(); index++) {
    debugToSerial(getOutput(index).name + " => " + getOutput(index).value);
    String lValue = "0";
    if (getOutput(index).type == oLED) {
      lValue = "OFF";
    }
    String result = processOutput(getOutput(index).name, lValue);
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

void setAllHalf() {
  debugToSerial("set all half:");
  stopSceneNow();
  String response = "{\"outputs\":[";

  int count = 0;
  for (int index = 0; index < getOutputQty(); index++) {
    if (getOutput(index).type == oPWM) {
      debugToSerial(getOutput(index).name + " => " + getOutput(index).value);
      String result = processOutput(getOutput(index).name, "127");
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

  for (int index = 0; index < getOutputQty(); index++) {
    debugLineToSerial(getOutput(index).name + " => " + getOutput(index).value);
    String lValue = "255";
    if (getOutput(index).type == oLED) {
      lValue = "ON";
    }
    String result = processOutput(getOutput(index).name, lValue);
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

void setAllValue() {
  debugToSerial("set all:");
  stopSceneNow();
  String response = "{\"outputs\":[";

  int argCount = restSrv.args();
  debugLineToSerial(argCount);
  String aValue = restSrv.arg(0);
  int count = 0;
  for (int index = 0; index < getOutputQty(); index++) {
    debugLineToSerial(getOutput(index).name + " => " + getOutput(index).value);
    String lValue = aValue;
    if (getOutput(index).type == oLED) {
      if (aValue == "0") {
        lValue = "OFF";
      } else {
        lValue = "ON";
      }
    }
    String result = processOutput(getOutput(index).name, lValue);
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
  //debugLineToSerial(argCount);
  String sceneName;
  for (int index = 0; index < argCount; index++) {
    if (restSrv.argName(index) == "name") {
      sceneName = restSrv.arg(index);
    } else if (restSrv.argName(index) == "steps") {
      stepsScene = restSrv.arg(index).toInt();
    } else if (restSrv.argName(index) == "pause") {
      pauseScene = restSrv.arg(index).toInt();
    }
  }

  debugToSerial(sceneName);
  sceneTypes newscene = getSceneTypeFromName(sceneName);
  if (activeScene != newscene) {
    stopSceneNow();
    debugToSerial(" - selected scene = " + getSceneTypeName(newscene));
    activeScene = newscene;
    startScene(5, newscene);
  }

  response += "{\"name\":\"scene\", \"value\":\"";
  response += getSceneTypeName(activeScene);
  response += "\"}";
  response += ", {\"name\":\"steps\", \"value\":\"";
  response += String(stepsScene);
  response += "\"}";
  response += ", {\"name\":\"pause\", \"value\":\"";
  response += String(pauseScene);
  response += "\"}";
  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
}

void setStepsScene() {
  debugToSerial("set scene steps to: ");
  String response = "{\"extra\":[";

  int argCount = restSrv.args();
  //debugLineToSerial(argCount);
  String sceneName;
  for (int index = 0; index < argCount; index++) {
    if (restSrv.argName(index) == "steps") {
      stepsScene = restSrv.arg(index).toInt();
    } else {
      response += "\"error\":\"wrong rest call: " + restSrv.argName(index) + " is not valid for setSceneSteps\"";
    }
  }
  debugToSerial(sceneName);

  sceneTypes newscene = getSceneTypeFromName(sceneName);
  if (activeScene != newscene) {
    stopSceneNow();
    debugToSerial(" - selected scene = " + getSceneTypeName(newscene));
    activeScene = newscene;
    startScene(5, newscene);
  }

  response += "{\"name\":\"scene\", \"value\":\"";
  response += getSceneTypeName(activeScene);
  response += "\"}";
  response += ", {\"name\":\"steps\", \"value\":\"";
  response += String(stepsScene);
  response += "\"}";
  response += ", {\"name\":\"pause\", \"value\":\"";
  response += String(pauseScene);
  response += "\"}";
  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);

}

void setPauseScene() {
  debugToSerial("set scene pause to: ");
  String response = "{\"extra\":[";

  int argCount = restSrv.args();
  //debugLineToSerial(argCount);
  String sceneName;
  for (int index = 0; index < argCount; index++) {
    if (restSrv.argName(index) == "name") {
      sceneName = restSrv.arg(index);
    } else if (restSrv.argName(index) == "steps") {
      stepsScene = restSrv.arg(index).toInt();
    } else if (restSrv.argName(index) == "pause") {
      pauseScene = restSrv.arg(index).toInt();
    }
  }
  debugToSerial(sceneName);
  sceneTypes newscene = getSceneTypeFromName(sceneName);
  if (activeScene != newscene) {
    stopSceneNow();
    debugToSerial(" - selected scene = " + getSceneTypeName(newscene));
    activeScene = newscene;
    startScene(5, newscene);
  }

  response += "{\"name\":\"scene\", \"value\":\"";
  response += getSceneTypeName(activeScene);
  response += "\"}";
  response += ", {\"name\":\"steps\", \"value\":\"";
  response += String(stepsScene);
  response += "\"}";
  response += ", {\"name\":\"pause\", \"value\":\"";
  response += String(pauseScene);
  response += "\"}";
  response += "]}";
  debugLineToSerial(" => " + response);
  restSrv.send(200, "text/json", response);
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

void setupRestSrv(WifiStruct wifiSettings) {
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    debugLineToSerial("MDNS responder started");
  }
  restServerRouting();
  // Set not found response
  restSrv.onNotFound(handleNotFound);
  // Start rest server
  restSrv.begin(wifiSettings.restPort);
  debugToSerial("REST Server started on port ");
  debugLineToSerial(getWifiSettings().restPort);
}

void loopREST() {
  restSrv.handleClient();
}
/*  end rest server  */

/*  start regular html page server  */
void setupHtmlSrv(WifiStruct wifiSettings) {
  httpSrv.begin(wifiSettings.httpPort);  // Starts the Server
  debugToSerial("HTTP Server started on port ");
  debugLineToSerial(getWifiSettings().httpPort);
  debugToSerial("Copy and paste the following URL: https://");
  Serial.print(WiFi.localIP());
  debugLineToSerial("/");
}

void loopHTTPpage(WiFiClient client) {
  htmlPageStart(client);
  htmlPageHeadRefresh(client,  refreshrate);
  htmlPageHeadEndBodyStart(client);

  htmlPageBodyTableStart(client, "Control outputs", 5, true, refreshrate);
  htmlOutputTitle(client, 5);
  for (int index = 0; index < getOutputQty(); index++) {
    switch (getOutput(index).type) {
      case oLED:
        htmlLedLine(client, getOutput(index).name, getOutput(index).value);
        break;
      case oPWM:
        htmlPwmLine(client, getOutput(index).name, getOutput(index).value);
        break;
      default:
        break;
    }
  }
  htmlPageTableEnd(client);
  htmlPageEmptyLine(client);

  htmlPageBodyTableStart(client, "Sensors and buttons", 2, false, refreshrate);
  htmlSensorTitle(client);
  for (int index = 0; index < getSensorQty(); index++) {
    switch (getSensor(index).type) {
      case sDigital:
        htmlSensorLine(client, getSensor(index).name, getSensor(index).value);
        break;
      case sAnalog:
        htmlSensorLine(client, getSensor(index).name, getSensor(index).value);
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
  for (int index = 0; index < getSensorQty(); index++) {
    switch (getSensor(index).type) {
      case sDigital:
        setSensorValue(index, processDigitalInput(getSensor(index).value, getSensor(index).pin));
        break;
      case sAnalog:
        setSensorValue(index, processSensor(getSensor(index).hysteresis, getSensor(index).value, getSensor(index).pin));
        break;
      default:
        break;
    }
  }
}

void loopBOARDoutputs(String request) {
  for (int index = 0; index < getOutputQty(); index++) {
    switch (getOutput(index).type) {
      case oLED:
        setOutputValue(index, processLED(request, getOutput(index).name, getOutput(index).value, getOutput(index).pin));
        break;
      case oPWM:
        setOutputValue(index, processHtmlPWM(request, getOutput(index).name, getOutput(index).value, getOutput(index).pin));
        break;
      default:
        break;
    }
  }

  refreshrate = processRefresh(request, refreshrate);
}

void loopSerialDebug() {
  delay(1);
  debugLineToSerial("Client disonnected");
  debugLineToSerial("");

  debugToSerial("Main - Refreshing at rate ");
  debugLineToSerial(refreshrate);
  debugLineToSerial("");
}

void setup() {
  getConfig();
  setupSerialDebug();
  initiateValues();
  WifiStruct wifiSettings = setupWiFi();
  setupRestSrv(wifiSettings);
  setupHtmlSrv(wifiSettings);
}

void loopScene(int steps, int pause) {
  if (activeScene != scNone) {
    nextStepInScene(steps, activeScene);
    for (int index = 0; index < getOutputQty(); index++) {
      switch (getOutput(index).type) {
        case oPWM:
          processRestPWMScene(getOutput(index).name, getOutput(index).value, getOutput(index).pin);
          break;
        default:
          break;
      }
    }
    delay(pause);
  }
}

void loop() {
  loopREST();
  loopScene(stepsScene, pauseScene);
  loopBOARDsensors();

    HtmlStruct httpData;
    httpData.client = httpSrv.available();
    if (!httpData.client) {
      return;
    }
    debugLineToSerial("Waiting for new client");
    while(!httpData.client.available()) {
      delay(1);
    }
    httpData.request = httpData.client.readStringUntil('\r');
    debugLineToSerial(httpData.request);
    httpData.client.flush();
    loopBOARDoutputs(httpData.request);
    loopHTTPpage(httpData.client);
    loopSerialDebug();
} 