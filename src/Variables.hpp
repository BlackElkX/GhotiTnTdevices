#ifndef variables_hpp
#define variables_hpp

#include <Arduino.h>
#include <WString.h>

  // led connected to GPIO  4 (D2)
  // led connected to GPIO  0 (D3)
  // led connected to GPIO  2 (D4)
  // led connected to GPIO 14 (D5)
  // led connected to GPIO 12 (D6)
  // led connected to GPIO 13 (D7)
  // led connected to GPIO 15 (D8)
  // analog sensor on ADC  17 (A0)
  // button connected to GPIO 16 (D0)
  // button connected to GPIO  5 (D1)
#pragma once

//#define TnTnevel
//#define TnTsmall
//#define TnTbig
#define develop
/*
// wifi stuff
#define ssid     "Lakota"
#define password "qwrffy365645DRGFdfr"
*/
#define webserver
#ifdef webserver
//  #define httpPort 80
  static int refreshrate;
#endif

#define restserver
#ifdef restserver
//  #define restPort 8080
#endif

#define serialdebug
#ifdef serialdebug
  #define serialBaudrate 115200
#endif
#define debugPin D0
static bool debug = false;

// types of in and output
#define typeOutLED  0
#define typeOutPWM  1
#define typeInAnal  2
#define typeInDigi  3 

#ifdef TnTnevel
  #define outputs 3
#endif
#ifdef TnTsmall
  #define outputs 3
#endif
#ifdef TnTbig
  #define outputs 6
  #define TnTsmall
#endif
#ifdef develop
  #define outputs 7
  #define sensors 2
  #define TnTbig
  #define TnTsmall
#endif

#ifdef outputs
  static int oPins[outputs] = {
    #ifdef TnTnevel
      D2,
      D3,
      D4
    #endif
    #ifdef TnTsmall
      D2,
      D3,
      D4,
    #endif
    #ifdef TnTbig
      D5,
      D6,
      D7,
    #endif
    #ifdef develop
      D8,
    #endif
  };
  static int oValues[outputs] = {
    #ifdef TnTnevel
      LOW,
      0,
      LOW
    #endif
    #ifdef TnTsmall
      127,
      127,
      127,
    #endif
    #ifdef TnTbig
      127,
      127,
      127,
    #endif
    #ifdef develop
      LOW,
    #endif
  };
  static String oNames[outputs] = {
    #ifdef TnTnevel
      "OUT0",
      "PWM1",
      "OUT1"
    #endif
    #ifdef TnTsmall
      "PWM1",
      "PWM2",
      "PWM3",
    #endif
    #ifdef TnTbig
      "PWM4",
      "PWM5",
      "PWM6",
    #endif
    #ifdef develop
      "LED0",
    #endif
  };
  static int oTypes[outputs] = {
    #ifdef TnTnevel
      typeOutLED,
      typeOutPWM,
      typeOutLED
    #endif
    #ifdef TnTsmall
      typeOutPWM,
      typeOutPWM,
      typeOutPWM,
    #endif
    #ifdef TnTbig
      typeOutPWM,
      typeOutPWM,
      typeOutPWM,
    #endif
    #ifdef develop
      typeOutLED,
    #endif
  };
  #ifndef TnTnevel
    static int oSceneDirection[outputs] = {
      #ifdef TnTsmall
        0,
        0,
        0,
      #endif
      #ifdef TnTbig
        0,
        0,
        0,
      #endif
      #ifdef develop
        0,
      #endif
    };
  #endif
#endif

#ifdef sensors
  static int sPins[sensors] = {
    A0,
    D1,
  };
  static int sValues[sensors] = {
    0,
    LOW,
  };
  static String sNames[sensors] = {
    "Humidity",
    "btn1",
  };
  static int sTypes[sensors] = {
    typeInAnal,
    typeInDigi,
  };
  static int sHyster[sensors] = {
    10,
    0,
  };
  static int sMin[sensors] = {
    0,
    NULL,
  };
  static int sMax[sensors] = {
    1024,
    NULL,
  };
  static int sMinRemap[sensors] = {
    0,
    NULL,
  };
  static int sMaxRemap[sensors] = {
    1024,
    NULL,
  };
#endif
#endif