#ifndef WIRELESS_H_
#define WIRELESS_H_
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "FileSystem.h"

namespace ESPanel
{
inline namespace Wireless
{
void wifiStart(const char *hostname, const char *ap_ssid, const char *ap_psk);
} // namespace Wireless
} // namespace ESPanel

#endif // !WIRELESS_H_