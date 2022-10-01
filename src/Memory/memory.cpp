#include <Arduino.h>
#include <EEPROM.h>
#include "memory.h"

void reset() {
  EEPROM.begin(512);
  for (int i = 0; i < 250; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();

  delay(1000);

  ESP.restart();
}

void saveUsername(String username) {
  char buff1[30];
  username.toCharArray(buff1, 30);

  EEPROM.begin(512);
    EEPROM.writeString(250, buff1);
    boolean commitStatus = EEPROM.commit();
    #ifdef DEBUG
    if (commitStatus)
        Serial.println("Saved");
    else 
        Serial.println("Error Saving");
    #endif
    EEPROM.end();
}

void saveToken(String username, String token) {
  char buff1[30];
  char buff2[50];
  username.toCharArray(buff1, 30);
  token.toCharArray(buff2, 50);

#ifdef DEBUG
  Serial.print("Saving Username : ");
  Serial.println(username);

  Serial.print("Saving Buffer : ");
  Serial.println(buff1);
  Serial.print("Saving Token Buffer : ");
  Serial.println(buff2);
#endif

  EEPROM.begin(512);
    EEPROM.writeString(250, buff1);
    EEPROM.writeString(350, buff2);
    boolean commitStatus = EEPROM.commit();
    #ifdef DEBUG
    if (commitStatus)
        Serial.println("Saved");
    else 
        Serial.println("Error Saving");
    #endif

#ifdef DEBUG
    String user = EEPROM.readString(250);
    String tok = EEPROM.readString(350);

    Serial.print("Save Time Reading Username : ");
    Serial.println(user);
    Serial.println(tok);
#endif

  EEPROM.end();
}

UserCreds readToken() {
  EEPROM.begin(512);
    String username = EEPROM.readString(250);
    String token = EEPROM.readString(350);
  EEPROM.end();

  UserCreds userCreds;
  userCreds.username = username;
  userCreds.token = token;

#ifdef DEBUG
  Serial.print("Reading Username : ");
  Serial.println(username);
#endif

  return userCreds;
}

void saveCreds(String ssid, String password) {
  char buff1[30];
  char buff2[30];
  ssid.toCharArray(buff1, 30);
  password.toCharArray(buff2, 30);

  EEPROM.begin(512);
    EEPROM.writeString(50, buff1);
    EEPROM.writeString(150, buff2);
    EEPROM.commit();
  EEPROM.end();

}

WifiCreds readCreds() {
  EEPROM.begin(512);
    String ssid = EEPROM.readString(50);
    String password = EEPROM.readString(150);
  EEPROM.end();

  WifiCreds creds;
  creds.ssid = ssid;
  creds.password = password;

  return creds;
}