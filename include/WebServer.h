#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include <Arduino.h>
// #if defined(ESP32) || defined(LIBRETINY)
// #include <AsyncTCP.h>
// #include <WiFi.h>
// #elif defined(ESP8266)
// #include <ESP8266WiFi.h>
// #include <ESPAsyncTCP.h>
// #endif

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

namespace ESPanel {
inline namespace WebServer {
void registerRoutes();
}
} // namespace ESPanel

#endif