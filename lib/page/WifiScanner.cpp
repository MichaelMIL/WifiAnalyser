/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/ESP32-DIV
   ________________________________________ */

#include <Arduino.h>
#include "WiFi.h"
#include "WifiScanner.h"

void WifiScanner::draw()
{
  wifiScannerSetup();
  wifiScannerLoop();
}

void WifiScanner::click()
{
  channel++;
  if (channel > 14)
  {
    channel = 0;
  }
  wifiScannerLoop();
}

void WifiScanner::doubleClick()
{
  channel = 0;
  wifiScannerLoop();
}

void WifiScanner::update()
{
  wifiScannerLoop();
}

void WifiScanner::init()
{
  wifiScannerSetup();
}

void WifiScanner::exit()
{
  gfx->fillScreen(BLACK);
}

void WifiScanner::wifiScannerSetup()
{

  gfx->fillScreen(BLACK);
  // WiFi.mode(WIFI_STA);
  // WiFi.disconnect();
  // setEventLoop();
  nvs_flash_init();
  esp_netif_init();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  // setEventLoop();

  // ESP_ERROR_CHECK(esp_wifi_set_country(WIFI_COUNTRY_EU));
  // ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  // ESP_ERROR_CHECK(esp_wifi_deinit());
  // Serial.println("WiFi deinitialized");
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  Serial.println("WiFi initialized");
  wifi_mode_t mode;
  ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));
  Serial.print("Current WiFi mode: ");
  Serial.println(mode);

  Serial.println("Setting WiFi mode to STA");
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));
  Serial.print("Current WiFi mode: ");
  Serial.println(mode);
  Serial.println(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_start());
  Serial.println("WiFi started");
  uint16_t nAPs = 0;

  ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
  Serial.println("WiFi scan start called");
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&nAPs));
  Serial.println("WiFi scan get AP num called");
  wifi_ap_record_t *list = (wifi_ap_record_t *)malloc(nAPs * sizeof(wifi_ap_record_t));
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&nAPs, list));
  Serial.println("WiFi scan get AP records called");
  ESP_ERROR_CHECK(esp_wifi_scan_stop());
  Serial.println("WiFi scan stop called");

  Serial.println("WiFi scanNetworks called");

  Serial.println("Debugging prints added");

  delay(100);

  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(0, 15);
  gfx->println("/ | SSID | RSSI | CH ");
}

void WifiScanner::printScanningChannel()
{
  gfx->fillRect(0, 0, 128, 10, ORANGE);
  gfx->setCursor(2, 2);
  gfx->setTextColor(BLACK);
  gfx->print("scanning channel");

  gfx->setTextWrap(false);
  gfx->setCursor(100, 2);
  gfx->setTextColor(BLACK);
  gfx->setTextSize(1);
  gfx->print("");
  if (channel == 0)
  {
    gfx->print("1-14");
  }
  else
  {
    gfx->print(channel);
  }
  gfx->println("");
}

void WifiScanner::wifiScannerLoop()
{
  printScanningChannel();
  // if (channel == 0)
  // {
  //   int n = WiFi.scanNetworks();
  // }
  // else
  // {
  //   int n = WiFi.scanNetworks(false, true, true, 500, channel);
  // }
  int n = 0;
  gfx->fillRect(0, 30, 128, 160, BLACK);

  gfx->setTextColor(ORANGE);
  gfx->setTextSize(1);
  for (int i = 1; i <= 14; i++)
  {
    gfx->setCursor(0, 30 + (i - 1) * 10);
    gfx->print(String(i, 10) + ":");
  }
  int maxLength = 9; // Set the maximum length you want to display

  for (int i = 0; i < 13; i++)
  {
    gfx->setCursor(20, 30 + i * 10); // Adjust the starting y-coordinate
    String ssid = WiFi.SSID(i);
    String truncatedSSID = ssid.substring(0, maxLength);

    bool isOpen = WiFi.encryptionType(i) == WIFI_AUTH_OPEN;

    if (isOpen)
    {
      gfx->setTextColor(GREEN); // Set the text color to green for open networks
    }
    else
    {
      gfx->setTextColor(ORANGE); // Set the text color to white for other networks
    }

    gfx->print(truncatedSSID.c_str());

    // Reset the text color to white for the next iteration
    gfx->setTextColor(ORANGE);
  }

  for (int i = 0; i < 13; i++)
  {
    gfx->setCursor(80, 30 + i * 10); // Adjust the starting y-coordinate
    gfx->println(WiFi.RSSI(i));
  }

  for (int i = 0; i < 13; i++)
  {
    gfx->setCursor(110, 30 + i * 10); // Adjust the starting y-coordinate
    gfx->println(WiFi.channel(i));
  }

  WiFi.scanDelete();
}
