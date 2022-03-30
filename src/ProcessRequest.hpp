#ifndef ProcessRequest_hpp
#define ProcessRequest_hpp

#include <WString.h>

int processLED(String request, String obj, int iValue, int iLed);
int processHtmlPWM(String request, String obj, int iValue, int iPwm);
int processRefresh(String request, int arefreshrate);
sceneTypes processScene(String request);
#endif