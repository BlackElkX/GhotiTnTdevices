#ifndef ProcessRequest_hpp
#define ProcessRequest_hpp

#include <WString.h>

//#pragma once

int processLED(String request, String obj, int iValue, int iLed);
int processHtmlPWM(String request, String obj, int iValue, int iPwm);
int processRestPWM(String obj, int aValue, int iPwm);
void processRestPWMScene(String obj, int aValue, int iPwm) ;
int processRefresh(String request, int arefreshrate);

#endif