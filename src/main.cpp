#include <Arduino.h>
#include <SPI.h>
#include "Display/display.h"
#include "Memory/memory.h"
#include "Wifi Manager/wifi-manager.h"
#include "globals.h"

RawDisplay rawDisplay = RawDisplay();

// Variables for interval checking and animation
unsigned long lastBreak;
unsigned int breakAfter = 10;
unsigned int breakTime = 1;
boolean statusChecked = false;
int loadingCounter = 0;

void setup(void)
{
#ifdef DEBUG
  Serial.begin(115200);
#endif

  lastBreak = 0;
  infiniteLoading = true;
  rawDisplay.begin();

  accessWifi();
  initializeServer();
}

void loop(void)
{
  // Handle connection to the server
  handleClient();

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
      updateStatusPoints();
      statusChecked = true;
    }

    // Reset last break for restarting the break timer
    if (sinceLastBreak > (60000 * (breakAfter + breakTime))){
      statusChecked = false;
      lastBreak = millis();
    }

    // Add counter when animaiton is over
    loadingCounter += rawDisplay.loading() ? 1 : 0;
  }
  else {
    rawDisplay.gitStatus(7, 0, statusBuffer);
  }
}

