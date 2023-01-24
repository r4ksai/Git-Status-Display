#include "WiFi Manager/WiFiManager.h"

const byte DNS_PORT = 53;

DNSServer dnsServer;
ESP8266WebServer server(80);

WiFiMode WiFiManager::connect()
{

    #ifdef DEBUG
        DEBUG_PRINTLN("WM: Available Networks");
        DEBUG_PRINTLN("WM: ------------------");
        int l = WiFi.scanNetworks();
        while (l != 0)
        {
            Serial.print("WM: - ");
            Serial.println(WiFi.SSID(l--));
        }
    #endif

    if (String(ssid).length() == 0)
    {
        DEBUG_PRINTLN("WM: No Credentials Available");
        accessPoint();
        return WIFI_AP;
    }
    else
    {
        // Try Connecting
        WiFi.mode(WIFI_STA);
        DEBUG_PRINT("WM: Connecting to ");
        DEBUG_PRINT(ssid);
        WiFi.begin(ssid, password);
        uint8 retries = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            DEBUG_PRINT(".");
            WiFi.begin(ssid, password);
            if (retries > MAX_RETRIES)
            {
                DEBUG_PRINTLN();
                DEBUG_PRINTLN("WM: Failed to connect to WiFi");
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
    DEBUG_PRINTLN("WM: Wiping EEPROM");
    EEPROM.begin(200);
    for (int i = 0; i < 200; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    EEPROM.end();
}

bool WiFiManager::setToken(String _token)
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

bool WiFiManager::setUsername(String _username)
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

bool WiFiManager::setCreds(String _ssid, String _password)
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

String WiFiManager::getUsername()
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
    DEBUG_PRINTLN("WM: Starting Access Point");
    WiFi.mode(WIFI_AP);
    IPAddress IP(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    delay(100);

    dnsServer.start(DNS_PORT, "*", IP);
    DEBUG_PRINT("WM: Access Point started with IP ");
    DEBUG_PRINTLN(WiFi.localIP());
}

void WiFiManager::handleHome()
{
    DEBUG_PRINT("WM: 200 - home - ");
    DEBUG_PRINTLN(server.method());
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);
    page += FPSTR(HTTP_HOME);
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void WiFiManager::handleWiFiCreds()
{
    DEBUG_PRINT("WM: 200 - wifi creds - ");
    DEBUG_PRINTLN(server.method());
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("ssid") && server.hasArg("password"))
    {
        String _ssid = server.arg("ssid");
        String _password = server.arg("password");

        setCreds(_ssid, _password);

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

void WiFiManager::handleGitToken()
{
    DEBUG_PRINT("WM: 200 - token - ");
    DEBUG_PRINTLN(server.method());
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("token"))
    {
        String _token = server.arg("token");

        setToken(_token);

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

void WiFiManager::handleGitUsername()
{
    DEBUG_PRINT("WM: 200 - username - ");
    DEBUG_PRINTLN(server.method());
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("username"))
    {
        String _username = server.arg("username");

        setUsername(_username);

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
        String savedUsername = getUsername();
        if (savedUsername.length() > 0)
            page.replace("{v}", savedUsername);
        else
            page.replace("{v}", "");
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
    }
}

void WiFiManager::handleNotFound()
{
    DEBUG_PRINT("WM: 404 - ");
    DEBUG_PRINTLN(server.uri());
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
    DEBUG_PRINTLN("WM: Setting up the Server");
    server.on("/", handleHome);
    server.on("/hotspot-detect.html", handleWiFiCreds);
    server.on("/creds", handleWiFiCreds);
    server.on("/token", handleGitToken);
    server.on("/username", handleGitUsername);
    server.onNotFound(handleNotFound);
    server.begin();
    DEBUG_PRINTLN("WM: Server Started");
}

void WiFiManager::handleClient()
{
    if (WiFi.getMode() == WIFI_AP)
        dnsServer.processNextRequest();
    server.handleClient();
}