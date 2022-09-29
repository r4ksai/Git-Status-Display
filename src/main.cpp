#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include "Display/display.h"

const char* ap_ssid = "Git Status Tracker";
const char* ap_password = "123456789";

WebServer server(80);
RawDisplay rawDisplay = RawDisplay();

void handleNotFound(){
  server.send(404, "text/plain", "Not found");
}

void handleHome() {
  String homePage = R"rawliteral(
  <!DOCTYPE HTML><html>
      <head><meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no ">
        <title>Git Status Tracker</title>
        <style>
          html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
          body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}
          p {font-size: 14px;color: #888;margin-bottom: 10px;}
        </style>
      </head>
      <body>
        <h1>Git Status Tracker</h1>
        <h3>Configure the Device</h3>
        <form>
          <input placeholder="SSID" />
          <input placeholder="Password" />
          <input type="submit" value="Submit" />
        </form>
      </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", homePage);
}

void accessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    delay(100);
    IPAddress IP(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
}

void connectToWifi(char* ssid, char* password) {
  // TODO: Check if credentials exists if not then go with accesspoint or else try wifi
    int statusChecks = 0;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        if (statusChecks > 2) {
        accessPoint();
        return;
        }
        delay(500);
        statusChecks ++;
    }
}

void setup(void)
{

  // EEPROM.begin(4);
  // uint next = EEPROM.readUInt(0);

  // String ssid = EEPROM.readString(2);
  // String password = EEPROM.readString(next);

  // EEPROM.end();

  // server.on("/", handleHome);
  // server.onNotFound(handleNotFound);
  // server.begin();

  rawDisplay.begin();
}

byte status[] = {
  0xff, 0xfa, 0xff, 0xfe, 0xff, 0xaa, 0xff, 0xff,
  0xff, 0xfa, 0xff, 0xfe, 0xff, 0xaa, 0xff, 0xff,
  0xff, 0xfa, 0xff, 0xfe, 0xff, 0xaa, 0xff, 0xff,
  0xff, 0xfa, 0xff, 0xfe, 0xff, 0xaa, 0xff, 0xff,
};

void loop(void)
{
  // server.handleClient();
  // rawDisplay.loading();
  rawDisplay.gitStatus(7, 0, status);
}

