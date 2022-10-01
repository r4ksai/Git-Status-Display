#pragma once
#include <Arduino.h>

struct WifiCreds {
    String ssid;
    String password;
};

struct UserCreds {
    String username;
    String token;
};

void reset();
void saveToken(String username, String token);
void saveUsername(String username);
void saveCreds(String ssid, String password);
WifiCreds readCreds();
UserCreds readToken();