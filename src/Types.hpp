#ifndef Types_hpp
#define Types_hpp

#include <WString.h>

enum sceneTypes  {scAllUpDown, scAllUpDownFollowing, scAllUpDownAlternating, scOddUpDown, scEvenUpDown, scAllRandom, scNone};
enum outputTypes {oLED, oPWM, oNone};
enum sensorTypes {sDigital, sAnalog, sAnalogButtons, sNone};
enum buttonTypes {bPower, bUp, bDown, bProgram, bNone};

String      getSceneTypeName(sceneTypes scene);
sceneTypes  getSceneTypeFromName(String  name);
sceneTypes  getNextSceneType(sceneTypes scene);


String      getOutputTypeName(outputTypes outputType);
outputTypes getOutputTypeFromName(String  name);

String      getSensorTypeName(sensorTypes sensorType);
sensorTypes getSensorTypeFromName(String  name);

String      getButtonTypeName(buttonTypes button);
buttonTypes getButtonTypeFromName(String  name);
#endif