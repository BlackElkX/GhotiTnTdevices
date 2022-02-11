#include "Types.hpp"
#include "scenes.hpp"
#include "Global.hpp"
#include "variables.hpp"

#pragma once

//static scenes activeScene = scNone;

void startScene(int steps, sceneTypes scene, int aValues[], int oSceneDirection[]) {
  stopScene(aValues, oSceneDirection);
  nextStepInScene(steps, scene, aValues, oSceneDirection);
}

void nextStepInScene(int steps, sceneTypes scene, int aValues[], int oSceneDirection[]) {
  switch (scene) {
    case scAllUpDownFollowing:
      allUpDownFollowing(steps, aValues, oSceneDirection);
      break;
    case scAllUpDownAlternating:
      allUpDownAlternating(steps, aValues, oSceneDirection);
      break;
    case scOddUpDown:
      oddUpDown(steps, aValues, oSceneDirection);
      break;
    case scEvenUpDown:
      evenUpDown(steps, aValues, oSceneDirection);
      break;
    case scAllRandom:
      allRandom(steps, aValues);
      break;
    case scAllUpDown:
      allUpDown(steps, aValues, oSceneDirection);
      break;
    default:
      break;
  }
}

void stopScene(int oValues[], int oSceneDirection[]) {
  for (int index = 0; index < outputs; index++) {
    oValues[index] = 0;
    oSceneDirection[index] = 0;
  }
}

void allUpDown(int steps, int oValues[], int oSceneDirection[]) {
  for (int index = 0; index < outputs; index++) {
    checkFirstRun(index, oValues, oSceneDirection);
    oValues[index] = oValues[index] + (oSceneDirection[index] * steps);
    switchSceneDirection(index, oValues, oSceneDirection);
  }
}

void allUpDownFollowing(int steps, int oValues[], int oSceneDirection[]) {
  for (int index = 0; index < outputs; index++) {
    checkFirstRunFollowing(index, steps, oValues, oSceneDirection);
    oValues[index] = oValues[index] + (oSceneDirection[index] * steps);
    switchSceneDirection(index, oValues, oSceneDirection);
  }
}

void allUpDownAlternating(int steps, int oValues[], int oSceneDirection[]) {
  for (int index = 0; index < outputs; index++) {
    checkFirstRunAlternating(index, oValues, oSceneDirection);
    oValues[index] = oValues[index] + (oSceneDirection[index] * steps);
    switchSceneDirection(index, oValues, oSceneDirection);
  }
}

void oddUpDown(int steps, int oValues[], int oSceneDirection[]) {
  for (int index = 1; index < outputs; index = index + 2) {
    checkFirstRun(index, oValues, oSceneDirection);
    oValues[index] = oValues[index] + (oSceneDirection[index] * steps);
    switchSceneDirection(index, oValues, oSceneDirection);
  }
}

void evenUpDown(int steps, int oValues[], int oSceneDirection[]) {
  for (int index = 0; index < outputs; index = index + 2) {
    checkFirstRun(index, oValues, oSceneDirection);
    oValues[index] = oValues[index] + (oSceneDirection[index] * steps);
    switchSceneDirection(index, oValues, oSceneDirection);
  }
}

void allRandom(int steps, int oValues[]) {
  for (int index = 0; index < outputs; index++) {
    int value = random(256 / steps) * steps;
    if (value > 255) {
      value = 255;
    }
    oValues[index] = value;
  }
}

void checkFirstRun(int index, int oValues[], int oSceneDirection[]) {
  if (oSceneDirection[index] == 0) {
    oSceneDirection[index] = 1;
    oValues[index] = 0;
  }
}

void checkFirstRunFollowing(int index, int steps, int oValues[], int oSceneDirection[]) {
  if (index > 0) {
    if (oValues[index - 1] > ((256 / steps) / (outputs + 1))) {
      checkFirstRun(index, oValues, oSceneDirection);
    }
  } else {
    checkFirstRun(index, oValues, oSceneDirection);
  }
}

void checkFirstRunAlternating(int index, int oValues[], int oSceneDirection[]) {
  if (oSceneDirection[index] == 0) {
    if ((index % 2) != 0) {
      oSceneDirection[index] = 1;
      oValues[index] = 0;
    } else {
      oSceneDirection[index] = -1;
      oValues[index] = 255;
    }
  }
}

void switchSceneDirection(int index, int oValues[], int oSceneDirection[]) {
  if ((oValues[index] >= 255) || (oValues[index] <= 0)) {
    oSceneDirection[index] = -oSceneDirection[index];
  }
}
