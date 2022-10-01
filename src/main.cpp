#include <Arduino.h>
#include <SPI.h>
#include "Display/display.h"
#include "Memory/memory.h"
#include "Wifi Manager/wifi-manager.h"

int loadingCounter = 0;
RawDisplay rawDisplay = RawDisplay();

void setup(void)
{
  Serial.begin(9600);
  rawDisplay.begin();
  accessWifi();
  initializeWifi();
}

byte status[] = {
  0xae, 0x04, 0x00, 0x78, 0xca, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

void loop(void)
{
  handleClient();
  if (loadingCounter < 2) {
    loadingCounter += rawDisplay.loading() ? 1 : 0;
  }
  else {
    rawDisplay.gitStatus(7, 0, status);
  }
}

