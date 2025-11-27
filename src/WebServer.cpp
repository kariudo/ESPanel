#include "WebServer.h"

namespace ESPanel {
inline namespace WebServer {
static AsyncWebServer server(80);
static bool routesRegistered = false;
void registerRoutes() {
  // Only want to let that happen once
  if (routesRegistered) {
#ifdef DEBUG_OUTPUT
    Serial.println("Warning: Routes already registered.");
#endif // DEBUG_OUTPUT
    return;
  }
  routesRegistered = true;
#ifdef DEBUG_OUTPUT
  Serial.println("Registering routes");
#endif // DEBUG_OUTPUT
  server.serveStatic("/index.html", LittleFS, "/www/index.html");
  server.serveStatic("/sensors.json", LittleFS, "/sensors.json");
  server.serveStatic("/climate.json", LittleFS, "/climate.json");

  // Define POST route
  server.on(
      "/update_wifi", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        // Nothing here, body will be handled in onBody
      },
      NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total) {
        // Parse JSON body
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
          Serial.println("Failed to parse JSON");
          request->send(400, "application/json",
                        "{\"status\":\"error\",\"msg\":\"Invalid JSON\"}");
          return;
        }

        const char *ssid = doc["ssid"];
        const char *password = doc["password"];

        if (!ssid || !password) {
          request->send(400, "application/json",
                        "{\"status\":\"error\",\"msg\":\"Missing fields\"}");
          return;
        }

        // Write to file
        File file = LittleFS.open("/cl_conf.txt", "w");
        if (!file) {
          request->send(500, "application/json",
                        "{\"status\":\"error\",\"msg\":\"File open failed\"}");
          return;
        }

        file.println(ssid);
        file.println(password);
        file.close();
        doc.clear();

        Serial.printf("Saved SSID: %s, Password: %s\n", ssid, password);

        request->send(200, "application/json", "{\"status\":\"ok\"}");

        // Just reboot to load the settings.
        ESP.restart();
      });
  server.begin();
#ifdef DEBUG_OUTPUT
  Serial.println("Web server started");
#endif // DEBUG_OUTPUT
}
} // namespace WebServer
} // namespace ESPanel