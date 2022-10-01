#include <Arduino.h>
#include <SPI.h>
#include "Display/display.h"
#include "Memory/memory.h"
#include "Wifi Manager/wifi-manager.h"
#include "globals.h"

int loadingCounter = -1;
RawDisplay rawDisplay = RawDisplay();

void setup(void)
{
#ifdef DEBUG
  Serial.begin(9600);
#endif
  infiniteLoading = true;
  rawDisplay.begin();
  accessWifi();
  initializeWifi();
}

void loop(void)
{
  handleClient();
  if (loadingCounter < 2) {
    if (infiniteLoading) 
    loadingCounter = -1;

    loadingCounter += rawDisplay.loading() ? 1 : 0;
  }
  else {
    rawDisplay.gitStatus(7, 0, statusBuffer);
  }
}

