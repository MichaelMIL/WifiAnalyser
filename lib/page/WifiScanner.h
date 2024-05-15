
#ifndef wifiscanner_h
#define wifiscanner_h

#include <Arduino.h>
#include <BasePage.h>
#include "esp_wifi.h"
#include "nvs_flash.h"

class WifiScanner : public BasePage
{
public:
   WifiScanner(Arduino_GFX *gfx, const char *name) : BasePage(gfx, name) {}
   void draw() override;
   void update() override;
   void init() override;
   void exit() override;
   void click() override;
   void doubleClick() override;

private:
   void wifiScannerSetup();
   void wifiScannerLoop();
   void printScanningChannel();
   unsigned int channel = 0;
};

#endif
