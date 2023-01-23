#include <Arduino.h>
#include "WiFi Manager/WiFiManager.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

void WiFiStart(){
    // WiFi.begin()
}

const byte DNS_PORT = 53;
DNSServer dnsServer;

void accessPoint() {
    IPAddress IP(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    delay(100);

    dnsServer.start(DNS_PORT, "*", IP);
}

ESP8266WebServer server(80);

void handleNotFound(){
    String page = FPSTR(HTTP_HEADER);
    page.replace("Git Device", "Not Found");
    page += FPSTR(HTTP_HEADER_END);
    page += FPSTR("Not Found");
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(404, "text/html", page);
}

void handleWiFiCreds(){
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("ssid") && server.hasArg("password")) {
        String ssid = server.arg("ssid");
        String password = server.arg("password");

        page += FPSTR(HTTP_CREDS_SAVED);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(201, "text/html", page);
        
        delay(1000);
        ESP.reset();
    }
    else {
        page += FPSTR(HTTP_SET_CREDS);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
    }

}

void handleGitToken(){
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("token")) {
        String token = server.arg("token");

        page += FPSTR(HTTP_DETAILS_SAVED);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(201, "text/html", page);
        
        delay(1000);
        ESP.reset();
    }
    else {
        page += FPSTR(HTTP_SET_TOKEN);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
    }
}

void handleGitUsername(){
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);

    if (server.hasArg("token")) {
        String token = server.arg("token");

        page += FPSTR(HTTP_DETAILS_SAVED);
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(201, "text/html", page);
        
        delay(1000);
        ESP.reset();
    }
    else {
        page += FPSTR(HTTP_SET_USERNAME);
        // Update old username here
        page.replace("{v}", "");
        page += FPSTR(HTTP_END);

        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
    }
}

void handleHome(){
    String page = FPSTR(HTTP_HEADER);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);
    page += FPSTR(HTTP_HOME);
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void intializeServer() {
    server.on("/", handleHome);
    server.on("/hotspot-detect.html", handleWiFiCreds);
    server.on("/creds", handleWiFiCreds);
    server.on("/token", handleGitToken);
    server.on("/username", handleGitUsername);
    server.onNotFound(handleNotFound);
    server.begin();
}

void handleClient() {
    if (WiFi.getMode() == WIFI_AP)
    dnsServer.processNextRequest();
    server.handleClient();
}