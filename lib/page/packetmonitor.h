/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/ESP32-DIV
   ________________________________________ */

#ifndef packetmonitor_H
#define packetmonitor_H

#include "arduinoFFT.h"

#include "esp_wifi.h"
// #include "esp_wifi_types.h"
// #include "esp_system.h"
// #include "esp_event.h"
#include "nvs_flash.h"
// #include <stdio.h>
// #include <string>
// #include <cstddef>
// #include <Wire.h>

#include <Arduino.h>
#include <BasePage.h>

class PacketMonitor : public BasePage
{
public:
   PacketMonitor(Arduino_GFX *gfx, const char *name) : BasePage(gfx, name) {}
   void draw() override;
   void update() override;
   void init() override;
   void exit() override;
   void click() override;
   void doubleClick() override;
   void loop() override;
   // void setEventLoop();

private:
   void packetMonitorSetup();
   void packetMonitorLoop();
   void do_sampling_FFT();
   void setChannel(uint8_t channel);
   unsigned int channel = 0;
   bool _isEventLoopSet = false;
};

#endif
