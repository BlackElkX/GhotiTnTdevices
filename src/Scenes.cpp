#include "Types.hpp"
#include "scenes.hpp"
#include "Global.hpp"
#include "Types.hpp"
#include "PersistentSettings.hpp"

extern SceneStruct sceneInfo;
extern int outputQuantity;
extern OutputStruct outputInfo[USED_OUTPUT_QTY];

//#pragma once

void startScene() {
  stopScene();
  nextStepInScene();
}

void nextStepInScene() {
  switch (sceneInfo.active) {
    case scAllUpDownFollowing:
      allUpDownFollowing();
      break;
    case scAllUpDownAlternating:
      allUpDownAlternating();
      break;
    case scOddUpDown:
      oddUpDown();
      break;
    case scEvenUpDown:
      evenUpDown();
      break;
    case scAllRandom:
      allRandom();
      break;
    case scAllUpDown:
      allUpDown();
      break;
    default:
      break;
  }
}

void stopScene() {
  for (int index = 0; index < outputQuantity; index++) {
    setOutputValue(index, 0);
    setOutputSceneDir(index, 0);
  }
}

void allUpDown() {
  for (int index = 0; index < outputQuantity; index++) {
    checkFirstRun(index);
    setOutputValue(index, outputInfo[index].value + (outputInfo[index].sceneDirection * sceneInfo.steps_ms));
    switchSceneDirection(index);
  }
}

void allUpDownFollowing() {
  for (int index = 0; index < outputQuantity; index++) {
    checkFirstRunFollowing(index);
    setOutputValue(index, outputInfo[index].value + (outputInfo[index].sceneDirection * sceneInfo.steps_ms));
    switchSceneDirection(index);
  }
}

void allUpDownAlternating() {
  for (int index = 0; index < outputQuantity; index++) {
    checkFirstRunAlternating(index);
    setOutputValue(index, outputInfo[index].value + (outputInfo[index].sceneDirection * sceneInfo.steps_ms));
    switchSceneDirection(index);
  }
}

void oddUpDown() {
  for (int index = 1; index < outputQuantity; index + 2) {
    checkFirstRun(index);
    setOutputValue(index, outputInfo[index].value + (outputInfo[index].sceneDirection * sceneInfo.steps_ms));
    switchSceneDirection(index);
  }
}

void evenUpDown() {
  for (int index = 0; index < outputQuantity; index + 2) {
    checkFirstRun(index);
    setOutputValue(index, outputInfo[index].value + (outputInfo[index].sceneDirection * sceneInfo.steps_ms));
    switchSceneDirection(index);
  }
}

void allRandom() {
  for (int index = 0; index < outputQuantity; index++) {
    int value = random(256 / sceneInfo.steps_ms) * sceneInfo.steps_ms;
    if (value > 255) {
      value = 255;
    }
    setOutputValue(index, value);
  }
}

void checkFirstRun(int index) {
  if (outputInfo[index].sceneDirection == 0) {
    setOutputSceneDir(index, 1);
    setOutputValue(index, 0);
  }
}

void checkFirstRunFollowing(int index) {
  if (index > 0) {
    if (getOutput(index - 1).value > ((256 / sceneInfo.steps_ms) / (outputQuantity + 1))) {
      checkFirstRun(index);
    }
  } else {
    checkFirstRun(index);
  }
}

void checkFirstRunAlternating(int index) {
  if (outputInfo[index].sceneDirection == 0) {
    if ((index % 2) != 0) {
      setOutputSceneDir(index, 1);
    } else {
      setOutputSceneDir(index, -1);
    }
    setOutputValue(index, 0);
  }
}

void switchSceneDirection(int index) {
  if ((outputInfo[index].value >= 255) || (outputInfo[index].value <= 0)) {
    outputInfo[index].sceneDirection = -outputInfo[index].sceneDirection;
  }
}