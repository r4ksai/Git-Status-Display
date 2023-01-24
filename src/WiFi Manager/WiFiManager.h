#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>

#define AP_SSID "Git Device"
#define AP_PASSWORD "123456789"

#define MAX_RETRIES 20 // 500ms Delay between each tries

const char HTTP_HEADER[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>Git Device</title> <link rel=\"icon\" class=\"js-site-favicon\" type=\"image/svg+xml\" href=\"https://github.githubassets.com/favicons/favicon.svg\"> ";
const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} h1{text-transform: uppercase;} div,input{padding:5px;font-size:1em;} input{width:95%; margin-bottom: 0.5rem;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_HEADER_END[] PROGMEM      = "</head><body><div style='text-align:center;position:absolute;top:50%;left:50%;transform:translate(-50%, -50%);min-width:260px;'>";
const char HTTP_HOME[] PROGMEM            = "<h1>Git Device</h1><form action=\"/creds\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/token\" method=\"get\"><button>Configure Git API</button></form><br/><form action=\"/username\" method=\"get\"><button>Configure Git Username</button></form><br/>";
const char HTTP_SET_CREDS[] PROGMEM       = "<h1>WiFi Credentials</h1><form action=\"/creds\" method=\"post\"><input name=\"ssid\"  placeholder=\"SSID\" maxlength=\"30\"/><br/><input name=\"password\" placeholder=\"Password\" maxlength=\"30\" type=\"password\" /><br/><button>SET CREDENTIALS</button></form>";
const char HTTP_SET_TOKEN[] PROGMEM       = "<h1>Github Token</h1><form action=\"/token\" method=\"post\"><input name=\"token\"  placeholder=\"API Token\" maxlength=\"50\"/><br/><button>SET TOKEN</button></form>";
const char HTTP_SET_USERNAME[] PROGMEM    = "<h1>Github Username</h1><form action=\"/username\" method=\"post\"><input name=\"username\"  placeholder=\"Github Username\" value=\"{v}\" maxlength=\"30\"/><br/><button>SET USERNAME</button></form>";
const char HTTP_CREDS_SAVED[] PROGMEM     = "<div>Credentials Saved<br />Trying to connect ESP to network.<br />If it fails reconnect to AP to try again</div>";
const char HTTP_DETAILS_SAVED[] PROGMEM   = "<div>Saved Succesfully<br />Restarting ESP</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

// TODO: Make this a Singleton
class WiFiManager{
    private:
        // void handleHome();
        // void handleGitUsername();
        // void handleGitToken();
        // void handleWiFiCreds();
        // void handleNotFound();
        char ssid[50];
        char password[50];
        char username[50];
        char token[50];

        void accessPoint();
    public:
        WiFiManager();
        WiFiMode connect();
        void wipeEEPROM();
        void intializeServer();
        void handleClient();
};