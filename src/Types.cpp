#include <WString.h>
#include "Types.hpp"

#define IDNAME(name) #name

char* sceneTypeNames[]  = {IDNAME(scNone),      IDNAME(scAllUpDown),  IDNAME(scAllUpDownFollowing), IDNAME(scAllUpDownAlternating),
                           IDNAME(scOddUpDown), IDNAME(scEvenUpDown), IDNAME(scAllRandom),          IDNAME(scProgSequence)};

char* outputTypeNames[] = {IDNAME(oNone), IDNAME(oLED), IDNAME(oPWM)};

char* sensorTypeNames[] = {IDNAME(sNone), IDNAME(sDigital), IDNAME(sAnalog), IDNAME(sAnalogButtons)};

char* buttonTypeNames[] = {IDNAME(bNone), IDNAME(bPower), IDNAME(bUp), IDNAME(bDown), IDNAME(bProgram)};

String getSceneTypeName(sceneTypes scene) {
  return String(sceneTypeNames[scene]);
}

sceneTypes getSceneTypeFromName(String name) {
  for (sceneTypes scene = scNone; scene <= scProgSequence; scene = sceneTypes(scene + 1)) {
    if (getSceneTypeName(scene) == name) {
      return scene;
    }
  }
  return scNone;
}

sceneTypes getNextSceneType(sceneTypes scene) {
  sceneTypes newScene = scNone;
  switch (scene) {
    case scNone:
      newScene = scAllUpDown;
      break;
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
    case scAllRandom:
      newScene = scProgSequence;
      break;
    case scProgSequence:
      newScene = scNone;
      break;
    default:
      newScene = scNone;
      break;
  }
  return newScene;
}

sceneTypes getNextSceneSequence(sceneTypes scene) {
  sceneTypes newScene = scNone;
  switch (scene) {
    case scAllRandom:
      newScene = scAllUpDown;
      break;
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
  for (outputTypes outputType = oNone; outputType <= oPWM; outputType = outputTypes(outputType + 1)) {
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
  for (sensorTypes sensorType = sNone; sensorType <= sAnalogButtons; sensorType = sensorTypes(sensorType + 1)) {
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
  for (buttonTypes button = bNone; button <= bProgram; button = buttonTypes(button + 1)) {
    if (getButtonTypeName(button) == name) {
      return button;
    }
  }
  return bNone;
}
