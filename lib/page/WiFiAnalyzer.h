#include <Arduino.h>
#include <BasePage.h>

// POWER SAVING SETTING
#define SCAN_INTERVAL 10000
// #define SCAN_COUNT_SLEEP 3
#include "WiFi.h"

// RSSI RANGE
#define RSSI_CEILING -40
#define RSSI_FLOOR -100

class WiFiAnalyzer : public BasePage
{
public:
    WiFiAnalyzer(Arduino_GFX *gfx, const char *name) : BasePage(gfx, name) {}
    void draw() override;
    void update() override;
    void init() override;
    void exit() override;
    void loop() override;

private:
    void initWiFiAnalyzer();
    void wiFiAnalyzerLoop();
    bool matchBssidPrefix(uint8_t *a, uint8_t *b);
    unsigned long lastScanTime = 0;
    uint8_t scan_count = 0;
    int16_t w, h, text_size, banner_height, graph_baseline, graph_height, channel_width, signal_width;
    uint16_t channel_color[14] = {
        RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
        RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
};
