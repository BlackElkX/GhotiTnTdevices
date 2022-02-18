#include <ArduinoJson.h>
//#include "Variables.hpp"
//#include "Types.hpp"
#include "Structures.hpp"

//JsonObject configJson;
DebugStruct  debugInfo;
HtmlStruct   htmlInfo;
SceneStruct  sceneInfo;
WifiStruct   wifiInfo;

//Change the allocated space according to the ammount of outputs and sensors you have.
int outputQuantity;
OutputStruct outputInfo[USED_OUTPUT_QTY];
int sensorQuantity;
SensorStruct sensorInfo[USED_SENSOR_QTY];