#ifndef Scenes_hpp
#define Scenes_hpp

#include <WString.h>
#include "Types.hpp"

#pragma once

void startScene(int steps, sceneTypes scene);
//void startScene(int steps, sceneTypes scene, int aValues[], int oSceneDirection[]);
void nextStepInScene(int steps, sceneTypes scene);
//void nextStepInScene(int steps, sceneTypes scene, int aValues[], int oSceneDirection[]);
void stopScene();
//void stopScene(int oValues[], int oSceneDirection[]);

void allUpDown(int steps);
//void allUpDown(int steps, int oValues[], int oSceneDirection[]);
void allUpDownFollowing(int steps);
//void allUpDownFollowing(int steps, int oValues[], int oSceneDirection[]);
void allUpDownAlternating(int steps);
//void allUpDownAlternating(int steps, int oValues[], int oSceneDirection[]);
void oddUpDown(int steps);
//void oddUpDown(int steps, int oValues[], int oSceneDirection[]);
void evenUpDown(int steps);
//void evenUpDown(int steps, int oValues[], int oSceneDirection[]);
void allRandom(int steps);
//void allRandom(int steps, int oValues[]);

void checkFirstRun(int index);
//void checkFirstRun(int index, int oValues[], int oSceneDirection[]);
void checkFirstRunFollowing(int index, int steps);
//void checkFirstRunFollowing(int index, int steps, int oValues[], int oSceneDirection[]);
void checkFirstRunAlternating(int index);
//void checkFirstRunAlternating(int index, int oValues[], int oSceneDirection[]);
int switchSceneDirection(int index);
//void switchSceneDirection(int index, int oValues[], int oSceneDirection[]);

#endif