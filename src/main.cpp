#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include "Display/display.h"

#define RESET_PIN 4
const int SHORT_PRESS_TIME = 1000;
int lastState = LOW;
int currentState;
unsigned long pressedTime = 0;
unsigned long realeasedTime = 0;

const char* ap_ssid = "Git Status Tracker";
const char* ap_password = "123456789";

WebServer server(80);
RawDisplay rawDisplay = RawDisplay();

void reset() {
  EEPROM.begin(512);
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();

  ESP.restart();
}

void saveCreds(String ssid, String password) {
  char buff1[30];
  char buff2[30];
  ssid.toCharArray(buff1, 30);
  password.toCharArray(buff2, 30);

  EEPROM.begin(400);
    EEPROM.writeString(100, buff1);
    EEPROM.writeString(200, buff2);
    EEPROM.commit();
  EEPROM.end();

}

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
          form{display: flex; flex-direction: column;}
          input{display: block; padding: 10px 20px; margin-bottom: 5px; border-radius: 5px;}
          input.button{background: #669; color: white;}
        </style>
      </head>
      <body>
        <h1>Git Status Tracker</h1>
        <h3>Configure the Device</h3>
        <form>
          <input placeholder="SSID" name="ssid" maxlength="30" />
          <input placeholder="Password" name="password" maxlength="30" />
          <input type="submit" value="Save" class="button" />
        </form>
      </body>
  </html>
  )rawliteral";

  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    saveCreds(ssid, password);
    ESP.restart();
  }
  else {
    server.send(200, "text/html", homePage);
  }

}

void accessPoint() {
    WiFi.softAP(ap_ssid, ap_password);
    delay(100);
    IPAddress IP(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
}

void connectToWifi(char* ssid, char* password) {
    int statusChecks = 0;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      statusChecks++;
      delay(500);
    }
}

void readCreds() {
  EEPROM.begin(400);
    String ssid = EEPROM.readString(100);
    String password = EEPROM.readString(200);
  EEPROM.end();

  char ssidBuffer[30];
  char passwordBuffer[30];

  ssid.toCharArray(ssidBuffer, 30);
  password.toCharArray(passwordBuffer, 30);

  if (ssid.length() <= 0 && password.length() <= 0) 
    accessPoint();
  else 
    connectToWifi(ssidBuffer, passwordBuffer);
}

void setup(void)
{

  // Set RESET PIN
  pinMode(RESET_PIN, INPUT_PULLUP);

  readCreds();

  server.on("/", handleHome);
  server.onNotFound(handleNotFound);
  server.begin();

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
  currentState = digitalRead(RESET_PIN);

  // if (lastState == HIGH && currentState == LOW)
  //   pressedTime = millis();
  // else if (lastState == LOW && currentState == HIGH) {
  //   realeasedTime = millis();

  //   long pressDuration = realeasedTime - pressedTime;

  //   if (pressDuration > SHORT_PRESS_TIME)
  //     reset();

  //   lastState = currentState;

  // }
  server.handleClient();
  // rawDisplay.loading();
  rawDisplay.gitStatus(7, 0, status);
}

