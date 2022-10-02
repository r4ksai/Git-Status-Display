#include <Arduino.h>
#include <SPI.h>
#include "Display/display.h"
#include "Memory/memory.h"
#include "Wifi Manager/wifi-manager.h"
#include "globals.h"

int loadingCounter = 0;
RawDisplay rawDisplay = RawDisplay();

unsigned long lastBreak;
unsigned int breakAfter = 10;
unsigned int breakTime = 1;
boolean checkStatus = false;

void setup(void)
{
#ifdef DEBUG
  Serial.begin(9600);
#endif
  lastBreak = 0;
  infiniteLoading = true;
  rawDisplay.begin();
  accessWifi();
  initializeWifi();
}

void loop(void)
{
  handleClient();
  unsigned long sinceLastBreak = millis() - lastBreak;
  boolean showLoading = sinceLastBreak > (60000 * breakAfter);

  if (loadingCounter < 2 || showLoading) {
    if (infiniteLoading || showLoading) 
    loadingCounter = 0;

    if (!checkStatus)
    {
      updateStatusPoints();
      checkStatus = true;
    }

    if (sinceLastBreak > (60000 * (breakAfter + breakTime))){
      checkStatus = false;
      lastBreak = millis();
    }

    loadingCounter += rawDisplay.loading() ? 1 : 0;
  }
  else {
    rawDisplay.gitStatus(7, 0, statusBuffer);
  }
}

