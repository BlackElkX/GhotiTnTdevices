#ifndef Types_hpp
#define Types_hpp

#include <WString.h>

enum sceneTypes  {scNone, scAllUpDown, scAllUpDownFollowing, scAllUpDownAlternating, scOddUpDown, scEvenUpDown, scAllRandom, scProgSequence};
enum outputTypes {oNone, oLED, oPWM};
enum sensorTypes {sNone, sDigital, sAnalog, sAnalogButtons};
enum buttonTypes {bNone, bPower, bUp, bDown, bProgram};

String      getSceneTypeName(sceneTypes scene);
sceneTypes  getSceneTypeFromName(String  name);
sceneTypes  getNextSceneType(sceneTypes scene);
sceneTypes  getNextSceneSequence(sceneTypes scene);

String      getOutputTypeName(outputTypes outputType);
outputTypes getOutputTypeFromName(String  name);

String      getSensorTypeName(sensorTypes sensorType);
sensorTypes getSensorTypeFromName(String  name);

String      getButtonTypeName(buttonTypes button);
buttonTypes getButtonTypeFromName(String  name);
#endif