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
  server.begin();
#ifdef DEBUG_OUTPUT
  Serial.println("Web server started");
#endif // DEBUG_OUTPUT
}
} // namespace WebServer
} // namespace ESPanel