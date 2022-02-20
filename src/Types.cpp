#include <WString.h>
#include "Types.hpp"

#define IDNAME(name) #name

char* sceneTypeNames[]  = {IDNAME(scAllUpDown), IDNAME(scAllUpDownFollowing), IDNAME(scAllUpDownAlternating),
                           IDNAME(scOddUpDown), IDNAME(scEvenUpDown),         IDNAME(scAllRandom),
                           IDNAME(scNone)};

char* outputTypeNames[] = {IDNAME(oLED),     IDNAME(oPWM),    IDNAME(oNone)};

char* sensorTypeNames[] = {IDNAME(sDigital), IDNAME(sAnalog), IDNAME(sAnalogButtons), IDNAME(sNone)};

char* buttonTypeNames[] = {IDNAME(bPower), IDNAME(bUp), IDNAME(bDown), IDNAME(bProgram), IDNAME(bNone)};

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

sceneTypes getNextSceneType(sceneTypes scene) {
  sceneTypes newScene = scNone;
  switch (scene) {
  case scAllUpDown:
    newScene = scAllUpDownFollowing;
    break;
  case scAllUpDownFollowing:
    newScene = scAllUpDownAlternating;
    break;
    case scAllUpDownAlternating:
    newScene = scOddUpDown;
    break;
  case scOddUpDown:
    newScene = scEvenUpDown;
    break;
  case scEvenUpDown:
    newScene = scAllRandom;
    break;
  default:
    newScene = scAllUpDown;
    break;
  }
  return newScene;
}

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

String getButtonTypeName(buttonTypes button) {
  return String(buttonTypeNames[button]);
}

buttonTypes getButtonTypeFromName(String name) {
  for (buttonTypes button = bPower; button <= bNone; button = buttonTypes(button + 1)) {
    if (getButtonTypeName(button) == name) {
      return button;
    }
  }
  return bNone;
}
