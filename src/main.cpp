#include <Arduino.h>
#include <SPI.h>
#include "Display/Display.h"
#include "WiFi Manager/WiFiManager.h"
#include "Debug.h"
#include <Ticker.h>

RawDisplay rawDisplay = RawDisplay();
WiFiManager wifiManager = WiFiManager();

// Variables for interval checking and animation
unsigned long lastBreak;
unsigned int breakAfter = 10; // 10 Minutes interval
unsigned int breakTime = 1; // 1 Minute break
boolean statusChecked = false;
int loadingCounter = 0;
int retryCounter = 0;

boolean showLoading = true;

byte* statusBuffer;

WiFiMode mode;

void setup(void)
{
  #ifdef DEBUG
  Serial.begin(115200);
  delay(2000);
  Serial.println();
  #endif

  rawDisplay.begin();

  mode = wifiManager.connect();
  wifiManager.intializeServer();

  statusBuffer = wifiManager.fetchData();
  showLoading = false;
}

void loop(void)
{
  wifiManager.handleClient();

  // No need to handle millis overflow because we are taking the difference in unsigned long
  unsigned long timeNow = millis();
  unsigned long sinceLastBreak = timeNow - lastBreak;

  // Show loading after an user specified duration
  showLoading = sinceLastBreak > (60000 * breakAfter);

  // Display loading animation after a specified duration and complete animation before showing the status display
  if (loadingCounter < 2 || showLoading) {
    // This variable controls animation to play till its over
    if (mode == WIFI_AP || showLoading) 
    loadingCounter = 0;

    if (!statusChecked)
    {
      statusBuffer = wifiManager.fetchData();
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
    rawDisplay.gitStatus(statusBuffer); 
  }
}

