#ifndef Scenes_hpp
#define Scenes_hpp

#include <WString.h>
#include "Types.hpp"

//#pragma once

void startScene();
void nextStepInScene();
void stopScene();

void allUpDown();
void allUpDownFollowing();
void allUpDownAlternating();
void oddUpDown();
void evenUpDown();
void allRandom();

void checkFirstRun(int index);
void checkFirstRunFollowing(int index);
void checkFirstRunAlternating(int index);
void switchSceneDirection(int index);

#endif