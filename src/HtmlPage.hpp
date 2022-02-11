#ifndef HtmlPage_hpp
#define HtmlPage_hpp

#include <ESP8266WiFi.h>

#pragma once

void htmlPageStart(WiFiClient client);
void htmlPageHeadRefresh(WiFiClient client, int aRefreshRate);
void htmlPageHeadEndBodyStart(WiFiClient client);
void htmlPageBodyTableStart(WiFiClient client, String title, int columns, bool showRefresh, int aRefreshRate);
void htmlOutputTitle(WiFiClient client, int columns);
void htmlLedLine(WiFiClient client, String obj, int value);
void htmlPwmLine(WiFiClient client, String obj, int value);
void htmlSensorTitle(WiFiClient client);
void htmlSensorLine(WiFiClient client, String obj, int value);
void htmlPageTableEnd(WiFiClient client);
void htmlPageBodyEnd(WiFiClient client);
void htmlPageEmptyLine(WiFiClient client);

#endif