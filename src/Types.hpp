#ifndef Types_hpp
#define Types_hpp

#include <WString.h>

#pragma once

enum outputTypes {oLED, oPWM, oNone};
enum sensorTypes {sDigital, sAnalog, sNone};
enum sceneTypes  {scAllUpDown, scAllUpDownFollowing, scAllUpDownAlternating, scOddUpDown, scEvenUpDown, scAllRandom, scNone};

String      getOutputTypeName(outputTypes outputType);
outputTypes getOutputTypeFromName(String  name);

String      getSensorTypeName(sensorTypes sensorType);
sensorTypes getSensorTypeFromName(String  name);

String      getSceneTypeName(sceneTypes scene);
sceneTypes  getSceneTypeFromName(String  name);

#endif