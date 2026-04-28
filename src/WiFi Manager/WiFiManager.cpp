#include "WiFi Manager/WiFiManager.h"

const byte DNS_PORT = 53;

DNSServer dnsServer;
ESP8266WebServer server(80);

WiFiMode WiFiManager::connect()
{
    WiFi.persistent(false);

    #ifdef DEBUG
        DEBUG_PRINTLN("WM: Available Networks");
        DEBUG_PRINTLN("WM: ------------------");
        int l = WiFi.scanNetworks();
        while (l != 0)
        {
            Serial.print("WM: - ");
            Serial.print(WiFi.SSID(l));
            Serial.print(" : ");
            Serial.println(WiFi.RSSI(l));
            if (String(ssid) == WiFi.SSID(l))
            Serial.println("WM: WiFi Available !");

            --l;
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
        WiFi.disconnect();
        DEBUG_PRINT("WM: WiFi Mode ");
        DEBUG_PRINTLN(WiFi.getMode());
        DEBUG_PRINT("WM: Connecting to ");
        DEBUG_PRINT(ssid);
        WiFi.begin(ssid, password);
        uint8 retries = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            DEBUG_PRINT(".");
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

        DEBUG_PRINTLN();
        DEBUG_PRINT("WM: Connected with IP ");
        DEBUG_PRINTLN(WiFi.localIP());
        return WIFI_STA;
    }
}

void WiFiManager::wipeEEPROM()
{
    DEBUG_PRINTLN("WM: Wiping EEPROM");
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    EEPROM.end();
}

bool WiFiManager::setToken(String _token)
{
    if (_token.length() <= TOKEN_MAX_LEN)
    {
        EEPROM.begin(EEPROM_SIZE);
        for (unsigned int i = 0; i < TOKEN_MAX_LEN; i++)
        {
            EEPROM.write(EEPROM_TOKEN + i, i < _token.length() ? _token[i] : 0);
        }
        EEPROM.commit();
        EEPROM.end();
        return true;
    }
    return false;
}

bool WiFiManager::setUsername(String _username)
{
    if (_username.length() < EEPROM_FIELD_LEN)
    {
        EEPROM.begin(EEPROM_SIZE);
        for (unsigned int i = 0; i < EEPROM_FIELD_LEN; i++)
        {
            EEPROM.write(EEPROM_USERNAME + i, i < _username.length() ? _username[i] : 0);
        }
        EEPROM.commit();
        EEPROM.end();
        return true;
    }
    return false;
}

bool WiFiManager::setCreds(String _ssid, String _password)
{
    if (_ssid.length() > 1 && _ssid.length() < EEPROM_FIELD_LEN)
    {
        EEPROM.begin(EEPROM_SIZE);
        for (unsigned int i = 0; i < EEPROM_FIELD_LEN; i++)
        {
            EEPROM.write(EEPROM_SSID     + i, i < _ssid.length()     ? _ssid[i]     : 0);
            EEPROM.write(EEPROM_PASSWORD + i, i < _password.length() ? _password[i] : 0);
        }
        EEPROM.commit();
        EEPROM.end();
        return true;
    }
    return false;
}

String WiFiManager::getUsername()
{
    char username[EEPROM_FIELD_LEN];
    EEPROM.begin(EEPROM_SIZE);
    for (unsigned int i = 0; i < EEPROM_FIELD_LEN; i++)
    {
        username[i] = EEPROM.read(EEPROM_USERNAME + i);
    }
    EEPROM.end();
    return String(username);
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
    DEBUG_PRINTLN(WiFi.softAPIP());
}

void WiFiManager::handleHome()
{
    DEBUG_PRINT("WM: 200 - Home - ");
    DEBUG_PRINTLN(server.method() == 1 ? "GET" : "POST");
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
    DEBUG_PRINT("WM: 200 - WiFi Creds - ");
    DEBUG_PRINTLN(server.method() == 1 ? "GET" : "POST");
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
    DEBUG_PRINT("WM: 200 - Token - ");
    DEBUG_PRINTLN(server.method() == 1 ? "GET" : "POST");
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
    DEBUG_PRINT("WM: 200 - Username - ");
    DEBUG_PRINTLN(server.method() == 1 ? "GET" : "POST");
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
    server.send(404, "text/html", page);
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

WiFiClientSecure client;
HTTPClient http;

byte* WiFiManager::fetchData(){
    // Fingerprint-based verification breaks whenever GitHub rotates their cert.
    // On ESP8266 without a full CA bundle, setInsecure() is the practical tradeoff
    // for a read-only personal device with no sensitive writes.
    client.setInsecure();
    http.begin(client, host);
    http.addHeader("Content-Type", "application/graphql");
    http.addHeader("Authorization", "bearer " + String(token));

    DEBUG_PRINT("WM: Getting Git Contribution data for ");
    DEBUG_PRINTLN(username);
    String payload = "{\"query\": \"query {user(login: \\\"" + String(username) + "\\\") {contributionsCollection{contributionCalendar{weeks{contributionDays{contributionCount}}}}}}\"}";
    int status = http.POST(payload);

    if (status != 200)
    {
        DEBUG_PRINT("WM: Failed to fetch with status code ");
        DEBUG_PRINTLN(status);
        return NULL;
    }

    int len = http.getSize();
    int totalLen = len;
    DEBUG_PRINT("WM: Response Size - ");
    DEBUG_PRINTLN(len);

    DEBUG_PRINTLN("WM: Allocating Memory in Heap for Response");
    char* buff = (char*) malloc(len + 1);
    if (!buff) {
        http.end();
        return NULL;
    }

    char *buffer = buff;
    while (http.connected() && (len > 0 || len == -1)){
      size_t size = client.available();
      if (size) {
        int c = client.readBytes(buffer, size);
        if (len > 0) {
          len -= c;
          buffer += c;
        }
      }
      delay(1);
    }
    buff[totalLen] = '\0';

    int col = 0, row = 0;

    static byte statusBuffer[32] = {0};

    DEBUG_PRINTLN("WM: Converting JSON to Pixels");
    for (int i = totalLen - 1; i >= 0; i--){
        char c1 = buff[i];
        if (isDigit(c1))
        {
            int stat = atoi(&c1);

            // Check if its a 2 digit number
            char c2 = buff[--i];
            int tenMultiplier = 0;
            if (isDigit(c2))
                tenMultiplier = atoi(&c2);

            stat = stat + (tenMultiplier * 10);

            if(row == 0)
            statusBuffer[col] = stat == 0 ? 0 : 1;
            else
            statusBuffer[col] |= stat == 0 ? 0 : 1 << row;

            row ++;

            // Increment column after every 8 rows
            if (row == 8) {
                row = 0;
                col ++;
            }
            // If more data is available, exit after the last column is done
            if (col == 32) break;
        }
    }

    DEBUG_PRINTLN("WM: Freeing Memory in Heap allocated for Response");
    free(buff);

    return statusBuffer;
}

WiFiManager::WiFiManager()
{
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < EEPROM_FIELD_LEN; i++)
    {
        ssid[i]     = EEPROM.read(EEPROM_SSID     + i);
        password[i] = EEPROM.read(EEPROM_PASSWORD + i);
        username[i] = EEPROM.read(EEPROM_USERNAME + i);
    }
    for (int i = 0; i < TOKEN_MAX_LEN; i++)
    {
        token[i] = EEPROM.read(EEPROM_TOKEN + i);
    }
    token[TOKEN_MAX_LEN] = '\0';
    EEPROM.end();
}

WiFiManager& WiFiManager::getInstance() {
    static WiFiManager instance = WiFiManager();
    return instance;
}
