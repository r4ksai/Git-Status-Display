#include "WiFi Manager/WiFiManager.h"

const byte DNS_PORT = 53;

DNSServer dnsServer;
ESP8266WebServer server(80);

WiFiMode WiFiManager::connect()
{
    // int x = WiFi.scanNetworks();
    // delay(2000);
    // while (x != 0)
    // {
    //     Serial.println(WiFi.SSID(x--));
    // }

    if (String(ssid).length() == 0)
    {
        // No Creds Available
        accessPoint();
        return WIFI_AP;
    }
    else
    {
        // Try Connecting
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        uint8 retries = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            if (retries > MAX_RETRIES)
            {
                accessPoint();
                return WIFI_AP;
            }
            retries++;
            delay(500);
        }
        return WIFI_STA;
    }
}

void WiFiManager::wipeEEPROM()
{
    EEPROM.begin(200);
    for (int i = 0; i < 200; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    EEPROM.end();
}

bool setToken(String _token)
{
    if (_token.length() < 50)
    {
        EEPROM.begin(200);
        for (unsigned int i = 0; i < 50; i++)
        {
            if (i < _token.length())
                EEPROM.write(i + 50, _token[i]);
            else
                EEPROM.write(i + 50, 0);
        }
        EEPROM.commit();
        EEPROM.end();
        return true;
    }
    return false;
}

bool setUsername(String _username)
{
    if (_username.length() < 50)
    {
        EEPROM.begin(200);
        for (unsigned int i = 0; i < 50; i++)
        {
            if (i < _username.length())
                EEPROM.write(i, _username[i]);
            else
                EEPROM.write(i, 0);
        }
        EEPROM.commit();
        EEPROM.end();
        return true;
    }
    return false;
}

bool setCreds(String _ssid, String _password)
{
    if (_ssid.length() < 50 && _ssid.length() > 1)
    {
        EEPROM.begin(200);
        for (unsigned int i = 0; i < 50; i++)
        {
            if (i < _ssid.length())
                EEPROM.write(i + 100, _ssid[i]);
            else
                EEPROM.write(i + 100, 0);

            if (i < _password.length())
                EEPROM.write(i + 150, _password[i]);
            else
                EEPROM.write(i + 150, 0);
        }
        EEPROM.commit();
        EEPROM.end();
        return true;
    }
    return false;
}

String getUsername()
{
    char username[50];
    EEPROM.begin(50);
    for (unsigned int i = 0; i < 50; i++)
    {
        username[i] = EEPROM.read(i);
    }
    EEPROM.end();

    return String(username);
}

WiFiManager::WiFiManager()
{
    EEPROM.begin(200);
    for (int i = 0; i < 50; i++)
    {
        ssid[i] = EEPROM.read(i + 100);
        password[i] = EEPROM.read(i + 150);
        username[i] = EEPROM.read(i);
        token[i] = EEPROM.read(i + 50);
    }
    EEPROM.end();
}

void WiFiManager::accessPoint()
{
    WiFi.mode(WIFI_AP);
    IPAddress IP(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    delay(100);

    dnsServer.start(DNS_PORT, "*", IP);
}

void handleHome()
{
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);
    page += FPSTR(HTTP_HOME);
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void handleWiFiCreds()
{
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("ssid") && server.hasArg("password"))
    {
        String ssid = server.arg("ssid");
        String password = server.arg("password");

        setCreds(ssid, password);

        page += FPSTR(HTTP_CREDS_SAVED);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(201, "text/html", page);

        delay(1000);
        ESP.reset();
    }
    else
    {
        page += FPSTR(HTTP_SET_CREDS);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
    }
}

void handleGitToken()
{
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("token"))
    {
        String token = server.arg("token");

        setToken(token);

        page += FPSTR(HTTP_DETAILS_SAVED);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(201, "text/html", page);

        delay(1000);
        ESP.reset();
    }
    else
    {
        page += FPSTR(HTTP_SET_TOKEN);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
    }
}

void handleGitUsername()
{
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("username"))
    {
        String username = server.arg("username");

        setUsername(username);

        page += FPSTR(HTTP_DETAILS_SAVED);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(201, "text/html", page);

        delay(1000);
        ESP.reset();
    }
    else
    {
        page += FPSTR(HTTP_SET_USERNAME);
        // Update old username here
        String storedUsername = getUsername();
        if (storedUsername.length() > 0)
            page.replace("{v}", storedUsername);
        else
            page.replace("{v}", "");
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
    }
}

void handleNotFound()
{
    String page = FPSTR(HTTP_HEADER);
    page.replace("Git Device", "Not Found");
    page += FPSTR(HTTP_HEADER_END);
    page += FPSTR("Not Found");
    page += FPSTR(HTTP_END);

    Serial.println(server.uri());

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void WiFiManager::intializeServer()
{
    server.on("/", handleHome);
    server.on("/hotspot-detect.html", handleWiFiCreds);
    server.on("/creds", handleWiFiCreds);
    server.on("/token", handleGitToken);
    server.on("/username", handleGitUsername);
    server.onNotFound(handleNotFound);
    server.begin();
}

void WiFiManager::handleClient()
{
    if (WiFi.getMode() == WIFI_AP)
        dnsServer.processNextRequest();
    server.handleClient();
}