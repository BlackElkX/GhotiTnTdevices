#include "Types.hpp"
#include "ProcessSensors.hpp"
#include "ProcessRequest.hpp"
#include "HtmlPage.hpp"
#include "Global.hpp"
#include "Scenes.hpp"
#include "Variables.hpp"
#include "PersistentSettings.hpp"
#include <Server.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

//WiFiServer       httpSrv(httpPort);
WiFiServer  httpSrv(80);
//ESP8266WebServer restSrv(restPort);
ESP8266WebServer restSrv(81);

static sceneTypes activeScene = scNone;
static int stepsScene = 5;
static int pauseScene = 100;


void initiateValues() {
  #ifdef outputs
    for (int index = 0; index < outputs; index++) {
      pinMode(oPins[index], OUTPUT);
      digitalWrite(oPins[index], oValues[index]);
    }
  #endif
  #ifdef sensors
    for (int index = 0; index < sensors; index++) {
      pinMode(sPins[index], INPUT);
    }
  #endif
}

void setupSerialDebug() {
  #ifdef serialdebug
    Serial.begin(serialBaudrate);
  #endif
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

  #ifdef webserver
    refreshrate = wifiSettings.httpRefreshRate;
  #endif
  debugLineToSerial("");
  debugToSerial("WiFi connected to ");
  debugLineToSerial(wifiSettings.nameSSID);
  debugToSerial("IP Address of network: ");
  #ifdef serialdebug
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.println(wifiSettings.httpPort);
  #endif
  return wifiSettings;
}

// Manage not found URL
void handleNotFound() {
  #ifdef restserver
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
  #endif
}

void getStatus() {
  debugToSerial("getting status from rest api: ");
  String response = "{";
  //outputs
  response += "\"outputs\":[";
  #ifdef outputs
    for (int index = 0; index < outputs; index++) {
      if (index > 0) {
        response += ", ";
      }
      String lValue = "ON";
      switch (oTypes[index]) {
        case typeOutLED:
          if (oValues[index] == 0) {
            lValue = "OFF";
          }
          response += "{\"name\":\"" + oNames[index] + "\", \"value\":\"" + lValue + "\"}";
          break;
        case typeOutPWM:
          response += "{\"name\":\"" + oNames[index] + "\", \"value\":\"" + String(oValues[index]) + "\"}";
          break;
        default:
          response += "{\"name\":\"" + oNames[index] + "\", \"value\":\"type unknown\"}";
          break;
      }
    }
  #endif
  response += "]";
  
  //sensors
  response += ", \"sensors\":[";
  #ifdef sensors
    for (int index = 0; index < sensors; index++) {
      if (index > 0) {
        response += ", ";
      }
      String lValue = "ON";
      switch (sTypes[index]) {
        case typeInDigi:
          if (sValues[index] == 0) {
            lValue = "OFF";
          }
          response += "{\"name\":\"" + sNames[index] + "\", \"value\":\"" + lValue + "\"}";
          break;
        case typeInAnal:
          response += "{\"name\":\"" + sNames[index] + "\", \"value\":\"" + String(sValues[index]) + "\"}";
          break;
        default:
          response += "{\"name\":\"" + sNames[index] + "\", \"value\":\"type unknown\"}";
          break;
      }    
    }
  #endif
  response += "]";
  
  //gezamelijk
  response += ", \"generals\":[";
  #ifdef outputs
    int  lLEDValue    = -1;
    bool LEDDifferent = false;
    int  lPWMValue    = -1;
    bool PWMDifferent = false;

    for (int index = 0; index < outputs; index++) {
      switch (oTypes[index]) {
        case typeOutLED:
          if (lLEDValue != oValues[index]) {
            if (lLEDValue > -1) {
              LEDDifferent = true;
            }
            lLEDValue = oValues[index];
          }
          break;
        case typeOutPWM:
          if (lPWMValue != oValues[index]) {
            if (lPWMValue > -1) {
              PWMDifferent = true;
            }
            lPWMValue = oValues[index];
          }
          break;
        default:
          break;
      }    
    }
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
  #endif
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
  #ifdef serialdebug
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
  #endif
  response += "]}";
  
  debugLineToSerial(response);
  
  #ifdef restserver
    restSrv.send(200, "text/json", response);
  #endif
}

void setDebuging() {
  debugToSerial("set debugging to: ");
  debugToSerial(restSrv.argName(0));
  debugToSerial(restSrv.arg(0));
  #ifdef restserver
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
  #endif
}

String processOutput(String obj, String value) {
  String result = "";
  for (int index = 0; index < outputs; index++) {
    if (obj.equals(oNames[index])) {
      String request = "/" + obj + "=" + value;
      debugLineToSerial(request);
      String lValue = "ON";
      switch (oTypes[index]) {
        case typeOutLED:
          oValues[index] = processLED(request, oNames[index], oValues[index], oPins[index]);
          if (oValues[index] == 0) {
            lValue = "OFF";
          }
          result = "{\"name\":\"" + oNames[index] + "\", \"value\":\"" + lValue + "\"}";
          break;
        case typeOutPWM:
          oValues[index] = processRestPWM(oNames[index], value.toInt(), oPins[index]);
          result = "{\"name\":\"" + oNames[index] + "\", \"value\":\"" + String(oValues[index]) + "\"}";
          break;
        default:
          result = "{\"name\":\"" + oNames[index] + "\", \"value\":\"type unknown\"}";
          break;
      }
    }
  }
  return result;
}

void stopSceneNow() {
  if (activeScene != scNone) {
    stopScene(oValues, oSceneDirection);
    activeScene = scNone;
  }
}

void setOutput() {
  debugToSerial("setting output to: ");
  #ifdef restserver
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
  #endif
}

void setAllOff() {
  debugToSerial("set all off: ");
  #ifdef restserver
    stopSceneNow();
    String response = "{\"outputs\":[";

    for (int index = 0; index < outputs; index++) {
      debugToSerial(oNames[index] + " => " + oValues[index]);
      String lValue = "0";
      if (oTypes[index] == typeOutLED) {
        lValue = "OFF";
      }
      String result = processOutput(oNames[index], lValue);
      if (index > 0) {
        response += ", ";
        debugToSerial(" - ");
      }
      response += result;
    }

    response += "]}";
    debugLineToSerial(" => " + response);
    restSrv.send(200, "text/json", response);
  #endif
}

void setAllHalf() {
  debugToSerial("set all half:");
  #ifdef restserver
    stopSceneNow();
    String response = "{\"outputs\":[";

    int count = 0;
    for (int index = 0; index < outputs; index++) {
      if (oTypes[index] == typeOutPWM) {
        debugToSerial(oNames[index] + " => " + oValues[index]);
        String result = processOutput(oNames[index], "127");
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
  #endif
}

void setAllFull() {
  debugToSerial("set all full:");
  #ifdef restserver
    stopSceneNow();
    String response = "{\"outputs\":[";

    for (int index = 0; index < outputs; index++) {
      debugLineToSerial(oNames[index] + " => " + oValues[index]);
      String lValue = "255";
      if (oTypes[index] == typeOutLED) {
        lValue = "ON";
      }
      String result = processOutput(oNames[index], lValue);
      if (index > 0) {
        response += ", ";
        debugToSerial(" - ");
      }
      response += result;
    }

    response += "]}";
    debugLineToSerial(" => " + response);
    restSrv.send(200, "text/json", response);
  #endif
}

void setAllValue() {
  debugToSerial("set all:");
  #ifdef restserver
    stopSceneNow();
    String response = "{\"outputs\":[";

    int argCount = restSrv.args();
    debugLineToSerial(argCount);
    String aValue = restSrv.arg(0);
    int count = 0;
    for (int index = 0; index < outputs; index++) {
      debugLineToSerial(oNames[index] + " => " + oValues[index]);
      String lValue = aValue;
      if (oTypes[index] == typeOutLED) {
        if (aValue == "0") {
          lValue = "OFF";
        } else {
          lValue = "ON";
        }
      }
      String result = processOutput(oNames[index], lValue);
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
  #endif
}

void setNewScene() {
  debugToSerial("set scene to: ");
  #ifdef restserver
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
      startScene(5, newscene, oValues, oSceneDirection);
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
  #endif
}

void setStepsScene() {
  debugToSerial("set scene steps to: ");
  #ifdef restserver
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
      startScene(5, newscene, oValues, oSceneDirection);
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
  #endif
}

void setPauseScene() {
  debugToSerial("set scene pause to: ");
  #ifdef restserver
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
      startScene(5, newscene, oValues, oSceneDirection);
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
  #endif
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
  
  #ifdef restserver
    restSrv.send(200, "text/json", response);
  #endif
}

// Define routing
void restServerRouting() {
  #ifdef restserver
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
  #endif
}

void setupRestSrv(WifiStruct wifiSettings) {
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  #ifdef restserver
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
  #endif
}

void loopREST() {
  #ifdef restserver
    restSrv.handleClient();
  #endif
}
/*  end rest server  */

/*  start regular html page server  */
void setupHtmlSrv(WifiStruct wifiSettings) {
  #ifdef webserver
    httpSrv.begin(wifiSettings.httpPort);  // Starts the Server
    debugToSerial("HTTP Server started on port ");
    debugLineToSerial(getWifiSettings().httpPort);
    debugToSerial("Copy and paste the following URL: https://");
    #ifdef serialdebug
      Serial.print(WiFi.localIP());
    #endif
    debugLineToSerial("/");
  #endif
}

void loopHTTPpage(WiFiClient client) {
  #ifdef webserver
    htmlPageStart(client);
    htmlPageHeadRefresh(client,    refreshrate);
    htmlPageHeadEndBodyStart(client);

    #ifdef outputs
      htmlPageBodyTableStart(client, "Control outputs", 5, true, refreshrate);
      htmlOutputTitle(client, 5);
      for (int index = 0; index < outputs; index++) {
        switch (oTypes[index]) {
          case typeOutLED:
            htmlLedLine(client, oNames[index], oValues[index]);
            break;
          case typeOutPWM:
            htmlPwmLine(client, oNames[index], oValues[index]);
            break;
          default:
            break;
        }
      }
      htmlPageTableEnd(client);
      htmlPageEmptyLine(client);
    #endif

    #ifdef sensors
      htmlPageBodyTableStart(client, "Sensors and buttons", 2, false, refreshrate);
      htmlSensorTitle(client);
      for (int index = 0; index < sensors; index++) {
        switch (sTypes[index]) {
          case typeInDigi:
            htmlSensorLine(client, sNames[index], sValues[index]);
            break;
          case typeInAnal:
            htmlSensorLine(client, sNames[index], sValues[index]);
            break;
          default:
            break;
        }
      }
      htmlPageTableEnd(client);
    #endif

    htmlPageBodyEnd(client);
  #endif
}
/*  end regular html page server  */

void loopBOARDsensors() {
  #ifdef sensors
    for (int index = 0; index < sensors; index++) {
      switch (sTypes[index]) {
        case typeInDigi:
          sValues[index] = processDigitalInput(sValues[index], sPins[index]);
          break;
        case typeInAnal:
          sValues[index] = processSensor(sHyster[index], sValues[index], sPins[index]);
          break;
        default:
          break;
      }
    }
  #endif
}

void loopBOARDoutputs(String request) {
  #ifdef outputs
    for (int index = 0; index < outputs; index++) {
      switch (oTypes[index]) {
        case typeOutLED:
          oValues[index] = processLED(request, oNames[index], oValues[index], oPins[index]);
          break;
        case typeOutPWM:
          oValues[index] = processHtmlPWM(request, oNames[index], oValues[index], oPins[index]);
          break;
        default:
          break;
      }
    }
  #endif

  #ifdef webservice
    refreshrate = processRefresh(request, refreshrate);
  #endif
}

void loopSerialDebug() {
  delay(1);
  #ifdef serialdebug
    debugLineToSerial("Client disonnected");
    debugLineToSerial("");

    debugToSerial("Main - Refreshing at rate ");
    debugLineToSerial(refreshrate);
    debugLineToSerial("");
  #endif
}

void setup() {
  setupSerialDebug();
  getConfig();
  initiateValues();
  WifiStruct wifiSettings = setupWiFi();
  #ifdef webserver
    setupRestSrv(wifiSettings);
  #endif
  #ifdef restserver
    setupHtmlSrv(wifiSettings);
  #endif
}

void loopScene(int steps, int pause) {
  if (activeScene != scNone) {
    nextStepInScene(steps, activeScene, oValues, oSceneDirection);
    for (int index = 0; index < outputs; index++) {
      switch (oTypes[index]) {
        case typeOutPWM:
          processRestPWMScene(oNames[index], oValues[index], oPins[index]);
          break;
        default:
          break;
      }
    }
    delay(pause);
  }
}

void loop() {
  #ifdef restserver
    loopREST();
  #endif
  loopScene(stepsScene, pauseScene);
  loopBOARDsensors();

  #ifdef webserver
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
  #else
    delay(1);
    String request = "";
    loopBOARDoutputs(request);
  #endif
} 