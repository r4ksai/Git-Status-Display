#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include "globals.h"
#include "git-api.h"
#include <EEPROM.h>

const char* host = "https://api.github.com/graphql";
const int port = 443;
const char* fingerprint = "29 70 30 74 CA 3C 48 F5 4A 79 C6 2D 11 57 A2 41 2A 2D 7D 5C";

static WiFiClientSecure client;
HTTPClient http;

void fetchData(){
    client.setFingerprint(fingerprint);
    char username[100];
    char token[100];

    EEPROM.begin(200);
    for(int i = 0; i < 100; i++)
    {
      username[i] = EEPROM.read(i);
      token[i] = EEPROM.read(i + 100);
    }

    http.begin(client, host);
    http.addHeader("Content-Type", "application/graphql");
    http.addHeader("Authorization", "bearer " + String(token));

    String payload = "{\"query\": \"query {user(login: \\\"" + String(username) + "\\\") {contributionsCollection{contributionCalendar{weeks{contributionDays{contributionCount}}}}}}\"}";
    int status = http.POST(payload);

    if (status != 200)
    {
      return;
    }

    boolean contributions[256] = {};

    int len = http.getSize();

    static char buff[11000] = {0};

    while (http.connected() && (len > 0 || len == -1)){

      size_t size = client.available();

      static char *buffer = buff;

      if (size) {
        int c = client.readBytes(buffer, ((size > sizeof(buff)) ? sizeof(buff) : size));
        if (len > 0) {
          len -= c;
          buffer = buffer + c;
        }
      }
      delay(1);
    }

    String responseString(buff);

    Serial.println("BUFFER :");
    Serial.println(responseString.length());
    Serial.println(responseString);
    // TODO: Data too large returning blank
    JSONVar response = JSON.parse(responseString);
    Serial.println("ALL LENGTH:");
    Serial.println(response.length());
    JSONVar weekData = response["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"];
    Serial.println("LENGTH :");
    Serial.println(weekData.length());
    int counter = 0;
    for (int i = weekData.length() - 1; i >= 0; i--){
      for (int j = weekData[i]["contributionDays"].length() - 1; j >= 0; j--) {
        if (counter >= 256)
        break;
        contributions[counter] = int(weekData[i]["contributionDays"][j]["contributionCount"]) > 0 ;
        counter++;
      }
    }

    int bufferCounter = 0;
    for (int i = 0; i < 32; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        if (j == 0)
        statusBuffer[i] = contributions[bufferCounter];
        else
        statusBuffer[i] |= contributions[bufferCounter] << j;

        bufferCounter ++;
      }
    }

    infiniteLoading = false;

}

ESP8266WebServer webServer(80);

  String HTMLHead = R"rawliteral(
  <!DOCTYPE HTML><html>
      <head><meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no ">
        <title>Git Display</title>
        <style>
          html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
          body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}
          form{display: flex; flex-direction: column;}
          input{display: block; padding: 10px 20px; margin-bottom: 5px; border-radius: 5px;}
          input.button{background: #669; color: white;}
        </style>
      </head>
      <body>
        <h1>Git Display</h1>
        )rawliteral";

void handle_onChangeToken() {

  String tokenBody = R"rawliteral(
        <h3>Set Git Token</h3>
        <form>
          <input placeholder="Token" name="token" maxlength="50" />
          <input type="submit" value="Save" class="button" />
        </form>
      </body>
  </html>
  )rawliteral";


  if (webServer.hasArg("token")) {
    String token = webServer.arg("token");

    EEPROM.begin(100);
    // Clear Memory
    for(int i = 0; i < 100; i++)
    {
      EEPROM.write(i, 0);
    }

    // Write
    for(unsigned int i = 0; i < token.length(); i++)
    {
      EEPROM.write(i, token[i]);
    }

    EEPROM.commit();

    webServer.send(200, "text/html", HTMLHead+tokenBody);
    // Wait before restarting !
    delay(1000);
    ESP.restart();
  }
  else {
    webServer.send(200, "text/html", HTMLHead+tokenBody);
  }
}

void handle_onConnect() {
  String HTMLBody = R"rawliteral(
        <h3>Configure the Device</h3>
        <form>
          <input placeholder="Username" name="username" maxlength="30" />
          <input placeholder="Token" name="token" maxlength="50" />
          <input type="submit" value="Save" class="button" />
        </form>
      </body>
  </html>
  )rawliteral";

  if (webServer.hasArg("username") && webServer.hasArg("token")) {
    String username = webServer.arg("username");
    String token = webServer.arg("token");

    EEPROM.begin(200);

    // Clear Memory
    for(int i = 0; i < 200; i++)
    {
      EEPROM.write(i, 0);
    }

    for(unsigned int i = 0; i < username.length(); i++)
    {
      EEPROM.write(i, username[i]);
    }

    for(unsigned int i = 0; i < token.length(); i++)
    {
      EEPROM.write(i+100, token[i]);
    }

    EEPROM.commit();

    webServer.send(200, "text/html", HTMLHead+HTMLBody);
    // Wait before restarting !
    delay(1000);
    ESP.restart();
  }
  else {
    webServer.send(200, "text/html", HTMLHead+HTMLBody);
  }
}

void runServer(){
  webServer.begin();
  webServer.on("/", handle_onConnect);
  webServer.on("/token", handle_onChangeToken);
}

void handleConnections(){
  webServer.handleClient();
}