#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <Memory/memory.h>
#include <Wifi Manager/wifi-manager.h>

const char* ap_ssid = "Git Status Tracker";
const char* ap_password = "123456789";

WebServer server(80);
WiFiClientSecure client;

void handleNotFound(){
  server.send(404, "text/plain", "Not found");
}

void handleReset() {
  reset();
  server.send(200, "text/plain", "Device Reset !");
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
    delay(50);
    ESP.restart();
  }
  else {
    server.send(200, "text/html", homePage);
  }

}
void initializeWifi() {
  server.on("/", handleHome);
  server.on("/token", handleToken);
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

void connectToWifi(char* ssid, char* password) {
  // TODO: Add NO WIFI Animation to the display
    int statusChecks = 0;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        if (statusChecks == 20)
        {
            accessPoint();
            break;
        }
      statusChecks++;
      delay(500);
    }
}


void accessWifi() {
  WifiCreds creds = readCreds();

  char ssidBuffer[30];
  char passwordBuffer[30];

  creds.ssid.toCharArray(ssidBuffer, 30);
  creds.password.toCharArray(passwordBuffer, 30);

  if (creds.ssid.length() <= 0 && creds.password.length() <= 0) 
    accessPoint();
  else 
    connectToWifi(ssidBuffer, passwordBuffer);
}

void handleClient() {
    server.handleClient();
}