#include <WString.h>
#include "Types.hpp"

#pragma once

#define IDNAME(name) #name

char* outputTypeNames[] = {IDNAME(oLED),     IDNAME(oPWM),    IDNAME(oNone)};

char* sensorTypeNames[] = {IDNAME(sDigital), IDNAME(sAnalog), IDNAME(sNone)};

char* sceneTypeNames[]  = {IDNAME(scAllUpDown), IDNAME(scAllUpDownFollowing), IDNAME(scAllUpDownAlternating),
                           IDNAME(scOddUpDown), IDNAME(scEvenUpDown),         IDNAME(scAllRandom),
                           IDNAME(scNone)};


String getOutputTypeName(outputTypes outputType) {
  return String(outputTypeNames[outputType]);
}

outputTypes getOutputTypeFromName(String name) {
  for (outputTypes outputType = oLED; outputType <= oNone; outputType = outputTypes(outputType + 1)) {
    if (getOutputTypeName(outputType) == name) {
      return outputType;
    }
  }
  return oNone;
}

String getSensorTypeName(sensorTypes sensorType) {
  return String(sensorTypeNames[sensorType]);
}

sensorTypes getSensorTypeFromName(String name) {
  for (sensorTypes sensorType = sDigital; sensorType <= sNone; sensorType = sensorTypes(sensorType + 1)) {
    if (getSensorTypeName(sensorType) == name) {
      return sensorType;
    }
  }
  return sNone;
}

String getSceneTypeName(sceneTypes scene) {
  return String(sceneTypeNames[scene]);
}

sceneTypes getSceneTypeFromName(String name) {
  for (sceneTypes scene = scAllUpDown; scene <= scNone; scene = sceneTypes(scene + 1)) {
    if (getSceneTypeName(scene) == name) {
      return scene;
    }
  }
  return scNone;
}
