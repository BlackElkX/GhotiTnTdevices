#ifndef ProcessSensors_hpp
#define ProcessSensors_hpp

#include "Structures.hpp"

//#pragma once

int processSensor(SensorStruct sensor);
int processAnalogButtonArray(SensorStruct sensor);
int processDigitalInput(SensorStruct sensor);

#endif