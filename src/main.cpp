#include <Arduino.h>
#include <SPI.h>
#include "Display/display.h"
#include <WiFiManager.h>
#include "Git API/git-api.h"

#include "globals.h"

RawDisplay rawDisplay = RawDisplay();

// Variables for interval checking and animation
unsigned long lastBreak;
unsigned int breakAfter = 10; // 10 Minutes interval
unsigned int breakTime = 1; // 1 Minute break
boolean statusChecked = false;
int loadingCounter = 0;
int retryCounter = 0;

void setup(void)
{
  Serial.begin(115200);
  WiFiManager wifiManager;

  // wifiManager.resetSettings();

  rawDisplay.begin();

  // wifiManager.setDebugOutput(false);

  wifiManager.autoConnect(AP_SSID, AP_PASSWORD);

  runServer();

  fetchData();

}

void loop(void)
{
  handleConnections();

  // No need to handle millis overflow because we are taking the difference in unsigned long
  unsigned long timeNow = millis();
  unsigned long sinceLastBreak = timeNow - lastBreak;

  // Show loading after an user specified duration
  boolean showLoading = sinceLastBreak > (60000 * breakAfter);

  // Display loading animation after a specified duration and complete animation before showing the status display
  if (loadingCounter < 2 || showLoading) {
    // This variable controls animation to play till its over
    if (infiniteLoading || showLoading) 
    loadingCounter = 0;

    if (!statusChecked)
    {
      statusChecked = true;
    }

    // Reset last break for restarting the break timer
    if (sinceLastBreak > (60000 * (breakAfter + breakTime))){
      statusChecked = false;
      lastBreak = millis();
    }

    // Add counter when animaiton is over
    boolean animtionOver = rawDisplay.loading() ? 1 : 0;
    loadingCounter += animtionOver;
    retryCounter += animtionOver;
  }
  else {
    rawDisplay.gitStatus(7, 0, statusBuffer); // Row no, Col no, Status Array
  }
}

