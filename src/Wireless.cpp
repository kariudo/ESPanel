#include "Wireless.h"
#define REMOTE_DEBUG
#define DEBUG_OUTPUT
namespace ESPanel
{
inline namespace Wireless
{
void wifiStart(const char *hostname, const char *ap_ssid, const char *ap_psk)
{
    initFS();
    String _hostname = String(hostname);
    String station_ssid = "";
    String station_psk = "";

    WiFi.hostname(_hostname);
#ifdef DEBUG_OUTPUT
    Serial.println("Hostname: " + _hostname);
#endif // DEBUG_OUTPUT

    // Load wifi connection information.
    if (!loadConfig(&station_ssid, &station_psk))
    {
        station_ssid = "";
        station_psk = "";
#ifdef DEBUG_OUTPUT
        Serial.println("No WiFi connection information available.");
#endif // DEBUG_OUTPUT
    }

    // Check WiFi connection
    // ... check mode
    if (WiFi.getMode() != WIFI_STA)
    {
        WiFi.mode(WIFI_STA);
        delay(10);
    }

    // ... Compare file config with sdk config.
    if (WiFi.SSID() != station_ssid || WiFi.psk() != station_psk)
    {
#ifdef DEBUG_OUTPUT
        Serial.println("WiFi config changed.");
#endif // DEBUG_OUTPUT

        // ... Try to connect to WiFi station.
        WiFi.begin(station_ssid.c_str(), station_psk.c_str());

#ifdef DEBUG_OUTPUT
        // ... Print new SSID
        Serial.print("new SSID: ");
        Serial.println(WiFi.SSID());
        // ... Uncomment this for debugging output.
        //WiFi.printDiag(Serial);
#endif // DEBUG_OUTPUT
    }
    else
    {
        // ... Begin with sdk config.
        WiFi.begin();
    }

#ifdef DEBUG_OUTPUT
    Serial.println("Wait for WiFi connection.");
#endif // DEBUG_OUTPUT

    // ... Give ESP 10 seconds to connect to station.
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)
    {
#ifdef DEBUG_OUTPUT
        Serial.write('.');
#endif // DEBUG_OUTPUT

        //Serial.print(WiFi.status());
        delay(500);
    }

    // Check connection
    if (WiFi.status() == WL_CONNECTED)
    {
#ifdef DEBUG_OUTPUT
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
#endif // DEBUG_OUTPUT
    }
    else
    {
#ifdef DEBUG_OUTPUT
        Serial.println("Can not connect to WiFi station. Go into AP mode.");
#endif // DEBUG_OUTPUT

        // Go into software AP mode.
        WiFi.mode(WIFI_AP);
        delay(10);
        WiFi.softAP(ap_ssid, ap_psk);

#ifdef DEBUG_OUTPUT
        Serial.print("IP address: ");
        Serial.println(WiFi.softAPIP());
#endif // DEBUG_OUTPUT
    }
}
} // namespace Wireless
} // namespace ESPanel