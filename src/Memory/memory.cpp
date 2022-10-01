#include <Arduino.h>
#include <EEPROM.h>
#include "memory.h"

void reset() {
  EEPROM.begin(250);
  for (int i = 0; i < 250; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();

  ESP.restart();
}

void saveToken(String username, String token) {
  char buff1[30];
  char buff2[50];
  username.toCharArray(buff1, 30);
  token.toCharArray(buff2, 50);

  EEPROM.begin(450);
    EEPROM.writeString(250, buff1);
    EEPROM.writeString(350, buff2);
    EEPROM.commit();
  EEPROM.end();

}

void saveCreds(String ssid, String password) {
  char buff1[30];
  char buff2[30];
  ssid.toCharArray(buff1, 30);
  password.toCharArray(buff2, 30);

  EEPROM.begin(250);
    EEPROM.writeString(50, buff1);
    EEPROM.writeString(150, buff2);
    EEPROM.commit();
  EEPROM.end();

}

WifiCreds readCreds() {
  EEPROM.begin(250);
    String ssid = EEPROM.readString(50);
    String password = EEPROM.readString(150);
  EEPROM.end();

  WifiCreds creds;
  creds.ssid = ssid;
  creds.password = password;

  return creds;
}