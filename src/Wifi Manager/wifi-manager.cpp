#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <Memory/memory.h>
#include <Wifi Manager/wifi-manager.h>
#include "globals.h"
#include <Wifi Manager/git-api.h>

const char* ap_ssid = "Git Status Tracker";
const char* ap_password = "123456789";
const int gmt_offset = 19800;
const char* ntp_server = "pool.ntp.org";

boolean credsAvailable = false;
boolean noConnection = true;

WebServer server(80);
WiFiClientSecure client;

void handleNotFound(){
  server.send(404, "text/plain", "Not found");
}

void handleReset() {
  reset();
  server.send(200, "text/plain", "Device Reset !");
}

void handleUsername() {
  String usernamePage = R"rawliteral(
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
        <h3>Change Username</h3>
        <form>
          <input placeholder="Username" name="username" maxlength="30" />
          <input type="submit" value="Save" class="button" />
        </form>
      </body>
  </html>
  )rawliteral";

  if (server.hasArg("username")) {
    String username = server.arg("username");
    saveUsername(username);
    server.send(200, "text/html", usernamePage);
    // Wait before restarting !
    delay(1000);
    ESP.restart();
  }
  else {
    server.send(200, "text/html", usernamePage);
  }

}

void handleToken() {
  String tokenPage = R"rawliteral(
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
          <input placeholder="Username" name="username" maxlength="30" />
          <input placeholder="Token" name="token" maxlength="50" />
          <input type="submit" value="Save" class="button" />
        </form>
      </body>
  </html>
  )rawliteral";

  if (server.hasArg("username") && server.hasArg("token")) {
    String username = server.arg("username");
    String token = server.arg("token");
    saveToken(username, token);
    server.send(200, "text/html", tokenPage);
    // Wait before restarting !
    delay(1000);
    ESP.restart();
  }
  else {
    server.send(200, "text/html", tokenPage);
  }

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
    server.send(200, "text/html", homePage);
    // Wait before restarting !
    delay(1000);
    ESP.restart();
  }
  else {
    server.send(200, "text/html", homePage);
  }

}
void initializeServer() {
  server.on("/", handleHome);
  server.on("/token", handleToken);
  server.on("/username", handleUsername);
  server.on("/reset", handleReset);
  server.onNotFound(handleNotFound);
  server.begin();
}

void accessPoint() {
    WiFi.softAP(ap_ssid, ap_password);
    delay(100);
    IPAddress IP(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
}

void updateStatusPoints() {
    if (WiFi.isConnected())
    {
        // Get Git status
        UserCreds userData = readToken();

        char usernameBuffer[30];
        char tokenBuffer[50];

        userData.username.toCharArray(usernameBuffer, 30);
        userData.token.toCharArray(tokenBuffer, 50);

        if (userData.username.length() <= 0 && userData.token.length() <= 0) 
        return;

        #ifdef DEBUG
            Serial.println("Token Exists");
            Serial.print("Username : ");
            Serial.println(usernameBuffer);
            Serial.print("Token : ");
            Serial.println(tokenBuffer);
        #endif
        fetchData(usernameBuffer, tokenBuffer);
    }
}

void connectToWifi(char* ssid, char* password) {
  // TODO: Add NO WIFI Animation to the display
    int statusChecks = 0;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        if (statusChecks == 20)
        {
            accessPoint();
            noConnection = true;
            return;
        }
      statusChecks++;
      delay(500);
    }

    noConnection = false;

#ifdef DEBUG
    Serial.println("Connected to WIFI");
    Serial.print("Wifi Name : ");
    Serial.println(ssid);
#endif

    updateStatusPoints();

}


void accessWifi() {
  WifiCreds creds = readCreds();

  char ssidBuffer[30];
  char passwordBuffer[30];

  creds.ssid.toCharArray(ssidBuffer, 30);
  creds.password.toCharArray(passwordBuffer, 30);

  credsAvailable = !(creds.ssid.length() <= 0 && creds.password.length() <= 0)

  if (credsAvailable) 
    connectToWifi(ssidBuffer, passwordBuffer);
  else 
    accessPoint();
}

void retryConnection() {
  if (credsAvailable) {
    if (noConnection) {
      accessWifi();
    }
    }
}

void handleClient() {
    server.handleClient();
}