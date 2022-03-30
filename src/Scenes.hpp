#ifndef Scenes_hpp
#define Scenes_hpp

#include <WString.h>
#include "Types.hpp"

void startScene();
void nextStepInScene();
void stopScene();

void startSceneSequence();
void nextStepInSceneSequence();

void allUpDown();
void allUpDownFollowing();
void allUpDownAlternating();
void oddUpDown();
void evenUpDown();
void allRandom();

void checkFirstRun(int index);
void checkFirstRunFollowing(int index);
void checkFirstRunAlternating(int index);

#endif