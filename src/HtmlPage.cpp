#include <Arduino.h>
#include <ESP8266WiFi.h>

void htmlPageStart(WiFiClient client) {
  client.println("HTTP/1.1 200 OK"); //
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("  <head>");
}

void htmlPageHeadRefresh(WiFiClient client, int aRefreshRate) {
  if (aRefreshRate > 0) {
    client.println("    <meta http-equiv='refresh' content='" + String(aRefreshRate) + "'>");
  }
}

void htmlPageHeadEndBodyStart(WiFiClient client) {
  client.println("  </head>");
  client.println("  <body>");
}

void htmlPageBodyTableStart(WiFiClient client, String title, int columns, bool showRefresh, int aRefreshRate) {
  client.println("    <table border=1>");
  client.println("      <tr>");
  client.print("        <th colspan=" + String(columns) + " align='center'>" + title);
  if (showRefresh) {
    if (aRefreshRate > 0) {
      client.print(" <a href='/refresh=off'><button>auto refresh off</button></a>");
    } else {
      client.print(" <a href='/refresh=on'><button>auto refresh on</button></a>");
    }
  }
  client.println("</th>");
  client.println("      </tr>");
}

void htmlOutputTitle(WiFiClient client, int columns) {
  client.println("      <tr>");
  client.println("        <th>Output</th>");
  client.println("        <th colspan=" + String(columns - 2) + ">Buttons</th>");
  client.println("        <th>Value</th>");
  client.println("      </tr>");
}

void htmlLedLine(WiFiClient client, String obj, int value) {
  client.println("      <tr>");
  client.print("        <td>");
  if (value == HIGH) {
    client.print(obj + ": ON");
  } else {
    client.print(obj + ": OFF");
  }
  client.println("</td>");
  client.println("        <td><a href='/" + obj + "=ON'><button>"  + obj + " ON</button></a></td>");
  client.println("        <td></td>");
  client.println("        <td><a href='/" + obj + "=OFF'><button>" + obj + " OFF</button></a></td>");
  client.println("        <td>" + String(value) + "</td>");
  client.println("      </tr>");
}

void htmlPwmLine(WiFiClient client, String obj, int value) {
  client.println("      <tr>");
  client.print("        <td>");
  if (value == 255) {
    client.print(obj + ": full");
  } else if (value == 127) {
    client.print(obj + ": half");
  } else if (value == 0) {
    client.print(obj + ": off");
  } else {
    client.print(obj + ": " + value);
  }
  client.println("</td>");
  client.println("        <td><a href='/" + obj + "=255'><button>" + obj + " ON</button></a></td>");
  client.println("        <td><a href='/" + obj + "=127'><button>" + obj + " HALF</button></a></td>");
  client.println("        <td><a href='/" + obj + "=0'><button>"   + obj + " OFF</button></a></td>");
  client.println("        <td>" + String(value) + "</td>");
  client.println("      </tr>");
}

void htmlSensorTitle(WiFiClient client) {
  client.println("      <tr>");
  client.println("        <th>Sensor</th>");
  client.println("        <th>Value</th>");
  client.println("      </tr>");
}

void htmlSensorLine(WiFiClient client, String obj, int value) {
  client.println("      <tr>");
  client.println("        <td>" + obj + "</td>");
  client.println("        <td>" + String(value) + "</td>");
  client.println("      </tr>");
}

void htmlSceneTitle(WiFiClient client, String activeScene, String subSceneName) {
  client.println("    <table border=1>");
  client.println("      <tr>");
  client.print("        <th colspan=8 align='center'>Active scene is " + activeScene);
  if (subSceneName != "") {
    client.print(" with " + subSceneName + " as the running one.");
  }
  client.println("</th>");
  client.println("      </tr>");
  client.println("      <tr>");
}

void htmlSceneLine(WiFiClient client, String sceneName) {
  client.println("        <td><a href='/scene=" + sceneName + "'><button>" + sceneName + "</button></a></td>");
}

void htmlSceneTableEnd(WiFiClient client) {
  client.println("      </tr>");
}

void htmlPageTableEnd(WiFiClient client) {
  client.println("    </table>");
}

void htmlPageBodyEnd(WiFiClient client) {
  client.println("    </table>");
  client.println("  </body>");
  client.println("</html>");
}

void htmlPageEmptyLine(WiFiClient client) {
  client.println("    <br/><br/>");
}