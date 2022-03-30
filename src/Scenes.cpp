#include "Types.hpp"
#include "Scenes.hpp"
#include "Global.hpp"
#include "Types.hpp"
#include "PersistentSettings.hpp"

extern SceneStruct sceneInfo;
extern int outputQuantity;
extern OutputStruct outputInfo[USED_OUTPUT_QTY];

void startScene() {
  stopScene();
  if (sceneInfo.active == scProgSequence) {
    startSceneSequence();
  } else {
    nextStepInScene();
  }
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
    case scProgSequence:
      nextStepInSceneSequence();
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

void startSceneSequence() {
    sceneInfo.sequence_timestamp = millis();
    sceneInfo.sequence_active    = scAllUpDown;
    nextStepInSceneSequence();
}

void nextStepInSceneSequence() {
  long newTimestamp = millis();
  if ((sceneInfo.sequence_timestamp + sceneInfo.sequence_ms) < newTimestamp) {
    stopScene();
    sceneTypes newSceneSequence  = getNextSceneSequence(sceneInfo.sequence_active);
    sceneInfo.sequence_active    = newSceneSequence;
    sceneInfo.sequence_timestamp = newTimestamp;
  }
  switch (sceneInfo.sequence_active) {
    case scAllUpDown:
      allUpDown();
      break;
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
    default:
      break;
  }
}

void processNewValue(int index) {
  int value = outputInfo[index].value + (outputInfo[index].sceneDirection * sceneInfo.steps_ms);
  if (value <= 0) {
    value = 0;
    outputInfo[index].sceneDirection = 1;
  }
  if (value >= 255) {
    value = 255;
    outputInfo[index].sceneDirection = -1;
  }
  setOutputValue(index, value);
}

void allUpDown() {
  for (int index = 0; index < outputQuantity; index++) {
    checkFirstRun(index);
    processNewValue(index);
  }
}

void allUpDownFollowing() {
  for (int index = 0; index < outputQuantity; index++) {
    checkFirstRunFollowing(index);
    processNewValue(index);
  }
}

void allUpDownAlternating() {
  for (int index = 0; index < outputQuantity; index++) {
    checkFirstRunAlternating(index);
    processNewValue(index);
  }
}

void oddUpDown() {
  for (int index = 0; index < outputQuantity; index++) {
    if ((index % 2) != 0) {
      int usedIndex = index;
      if (usedIndex >= outputQuantity) {
        usedIndex = 0;
      }
      checkFirstRun(index);
      processNewValue(index);
    }
  }
}

void evenUpDown() {
  for (int index = 0; index < outputQuantity; index++) {
    if ((index % 2) == 0) {
      int usedIndex = index;
      if (usedIndex >= outputQuantity) {
        usedIndex = 0;
      }
      checkFirstRun(index);
      processNewValue(index);
    }
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
  if (outputInfo[index].sceneDirection == 0) {
    setOutputSceneDir(index, 1);
    int part = 256 / (outputQuantity);
    int newValue = part * (index + 1);
    setOutputValue(index, newValue);
  }
}

void checkFirstRunAlternating(int index) {
  if (outputInfo[index].sceneDirection == 0) {
    if ((index % 2) != 0) {
      setOutputSceneDir(index, 1);
      setOutputValue(index, 0);
    } else {
      setOutputSceneDir(index, -1);
      setOutputValue(index, 255);
    }
  }
}