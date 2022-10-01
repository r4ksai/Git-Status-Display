#pragma once
#include <Arduino.h>

struct WifiCreds {
    String ssid;
    String password;
};

void reset();
void saveToken(String username, String token);
void saveCreds(String ssid, String password);
WifiCreds readCreds();