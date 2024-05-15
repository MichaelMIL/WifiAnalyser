/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/ESP32-DIV
   ________________________________________ */

#include <Arduino.h>
#include "packetmonitor.h"

/* Create FFT object */

#define MAX_CH 14
#define SNAP_LEN 2324

#define MAX_X 128
#define MAX_Y 51

// esp_err_t event_handler(void *ctx, system_event_t *event)
// {
//   return ESP_OK;
// }
const uint16_t samples = 256;
const double samplingFrequency = 5000;

double attenuation = 10;

unsigned int sampling_period_us;
unsigned long microseconds;

double vReal[samples];
double vImag[samples];
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

byte palette_red[128], palette_green[128], palette_blue[128];

uint32_t tmpPacketCounter;
uint32_t pkts[MAX_X]; // here the packets per second will be saved
uint32_t deauths = 0; // deauth frames per second
unsigned int ch = 1;  // current 802.11 channel
int rssiSum;

unsigned int epoch = 0;
unsigned int color_cursor = 2016;

double getMultiplicator()
{
  uint32_t maxVal = 1;
  for (int i = 0; i < MAX_X; i++)
  {
    if (pkts[i] > maxVal)
      maxVal = pkts[i];
  }
  if (maxVal > MAX_Y)
    return (double)MAX_Y / (double)maxVal;
  else
    return 1;
}

void wifi_promiscuous(void *buf, wifi_promiscuous_pkt_type_t type)
{
  Serial.println("Packet received");

  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

  if (type == WIFI_PKT_MGMT && (pkt->payload[0] == 0xA0 || pkt->payload[0] == 0xC0))
    deauths++;

  if (type == WIFI_PKT_MISC)
    return;
  if (ctrl.sig_len > SNAP_LEN)
    return;
  uint32_t packetLength = ctrl.sig_len;
  if (type == WIFI_PKT_MGMT)
    packetLength -= 4;
  // Serial.print(".");
  tmpPacketCounter++;
  rssiSum += ctrl.rssi;
}

void PacketMonitor::setChannel(uint8_t newChannel)
{
  Serial.println("Setting channel to " + String(newChannel));
  channel = newChannel;
  if (channel > MAX_CH || channel < 1)
    channel = 1;

  esp_wifi_set_promiscuous(false);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous);
  esp_wifi_set_promiscuous(true);
}

void draw()
{
  double multiplicator = getMultiplicator();
  int len;
  int rssi;

  if (pkts[MAX_X - 1] > 0)
    rssi = rssiSum / (int)pkts[MAX_X - 1];
  else
    rssi = rssiSum;
}

void PacketMonitor::do_sampling_FFT()
{

  microseconds = micros();
  for (int i = 0; i < samples; i++)
  {
    vReal[i] = tmpPacketCounter * 300;
    vImag[i] = 1;
    while (micros() - microseconds < sampling_period_us)
    {
    }
    microseconds += sampling_period_us;
  }

  double mean = 0;
  for (uint16_t i = 0; i < samples; i++)
    mean += vReal[i];
  mean /= samples;
  for (uint16_t i = 0; i < samples; i++)
    vReal[i] -= mean;

  microseconds = micros();

  FFT.windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, samples, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, samples);

  unsigned int top_y = 155;
  int max_k = 0;
  for (int j = 0; j < samples >> 1; j++)
  {
    int k = vReal[j] / attenuation;
    if (k > max_k)
      max_k = k;
    if (k > 127)
      k = 127;
    unsigned int color = palette_red[k] << 11 | palette_green[k] << 5 | palette_blue[k];
    gfx->drawPixel(epoch, top_y - j, color);
  }
  double tattenuation = max_k / 127.0;
  if (tattenuation > attenuation)
    attenuation = tattenuation;

  gfx->fillRect(0, 0, 128, 10, ORANGE);
  // gfx->fillScreen(BLACK);
  gfx->setTextWrap(false);
  gfx->setCursor(20, 2);
  gfx->setTextColor(BLACK);
  gfx->setTextSize(1);
  gfx->print(ch);

  gfx->setCursor(2, 2);
  gfx->setTextColor(BLACK);
  gfx->print("Ch:");

  // gfx->fillRect(50,0,15,15, BLACK);
  gfx->setTextWrap(true);
  gfx->setCursor(80, 2);
  gfx->setTextColor(BLACK);
  gfx->setTextSize(1);
  gfx->print(tmpPacketCounter);

  gfx->setCursor(50, 2);
  gfx->setTextColor(BLACK);
  gfx->print("Pkts:");

  delay(10);
}

void PacketMonitor::packetMonitorSetup()
{
  Serial.println("Packet Monitor Setup");
  // gfx->initR(INITR_BLACKTAB);
  // gfx->setRotation(0);
  gfx->fillScreen(BLACK);

  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));

  for (int i = 0; i < 32; i++)
  {
    palette_red[i] = i / 2;
    palette_green[i] = 0;
    palette_blue[i] = i;
  }
  for (int i = 32; i < 64; i++)
  {
    palette_red[i] = i / 2;
    palette_green[i] = 0;
    palette_blue[i] = 63 - i;
  }
  for (int i = 64; i < 96; i++)
  {
    palette_red[i] = 31;
    palette_green[i] = (i - 64) * 2;
    palette_blue[i] = 0;
  }
  for (int i = 96; i < 128; i++)
  {
    palette_red[i] = 31;
    palette_green[i] = 63;
    palette_blue[i] = i - 96;
  }

  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
  Serial.println("Sampling period: " + String(sampling_period_us) + "us");
  nvs_flash_init();
  // tcpip_adapter_init();
  esp_netif_init();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  // setEventLoop();

  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  // ESP_ERROR_CHECK(esp_wifi_set_country(WIFI_COUNTRY_EU));
  // ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());

  Serial.println("Setting channel to " + String(ch));
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous);
  esp_wifi_set_promiscuous(true);
  gfx->fillScreen(BLACK);
}

// void PacketMonitor::setEventLoop()
// {
//   if (!_isEventLoopSet)
//   {
//     ESP_ERROR_CHECK(esp_event_init(event_handler, NULL));
//     _isEventLoopSet = true;
//   }
// }

void PacketMonitor::packetMonitorLoop()
{
  gfx->drawPixel(epoch, 0, color_cursor);
  do_sampling_FFT();
  delay(10);

  gfx->drawPixel(epoch, 0, 0);
  epoch++;
  if (epoch >= gfx->width())
    epoch = 0;

  pkts[127] = tmpPacketCounter;

  // Serial.println((String)pkts[MAX_X - 1]);

  tmpPacketCounter = 0;
  deauths = 0;
  rssiSum = 0;
}

// f (digitalRead(BUTTON_PIN) == LOW)
//   {
//     if (buttonEnabled)
//     {
//       if (!buttonPressed)
//       {
//         buttonPressed = true;
//       }
//     }

//     if (buttonPressed)
//     {
//       setChannel(ch + 1);
//     }
//     buttonPressed = false;
//     buttonEnabled = true;
//   }

void PacketMonitor::loop()
{
  packetMonitorLoop();
}

void PacketMonitor::draw()
{
  packetMonitorLoop();
}

void PacketMonitor::update()
{
  packetMonitorLoop();
}

void PacketMonitor::init()
{
  packetMonitorSetup();
}

void PacketMonitor::exit()
{
  gfx->fillScreen(BLACK);
  // return wifi settings to normal
  esp_wifi_stop();
  Serial.println("WiFi stop called");
  esp_wifi_set_promiscuous(false);
  Serial.println("WiFi promiscuous mode set to false");

  delay(100);
}

void PacketMonitor::click()
{
  ch++;
  if (ch > 14)
  {
    ch = 0;
  }
  setChannel(ch);
}

void PacketMonitor::doubleClick()
{
  ch = 0;
  setChannel(ch);
}
