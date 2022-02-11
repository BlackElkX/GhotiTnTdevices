#ifndef Scenes_hpp
#define Scenes_hpp

#include <WString.h>
#include "types.hpp"

#pragma once

void startScene(int steps, sceneTypes scene, int aValues[], int oSceneDirection[]);
void nextStepInScene(int steps, sceneTypes scene, int aValues[], int oSceneDirection[]);
void stopScene(int oValues[], int oSceneDirection[]);

void allUpDown(int steps, int oValues[], int oSceneDirection[]);
void allUpDownFollowing(int steps, int oValues[], int oSceneDirection[]);
void allUpDownAlternating(int steps, int oValues[], int oSceneDirection[]);
void oddUpDown(int steps, int oValues[], int oSceneDirection[]);
void evenUpDown(int steps, int oValues[], int oSceneDirection[]);
void allRandom(int steps, int oValues[]);

void checkFirstRun(int index, int oValues[], int oSceneDirection[]);
void checkFirstRunFollowing(int index, int steps, int oValues[], int oSceneDirection[]);
void checkFirstRunAlternating(int index, int oValues[], int oSceneDirection[]);
void switchSceneDirection(int index, int oValues[], int oSceneDirection[]);

#endif