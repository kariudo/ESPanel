#include "Wireless.h"
namespace ESPanel
{
inline namespace Wireless
{
void wifiStart(const char *hostname, const char *ap_default_ssid, const char *ap_default_psk)
{
    initFS();
    String _hostname = String(hostname);
    String station_ssid = "";
    String station_psk = "";

    WiFi.hostname(_hostname);
#ifdef DEBUG
    Serial.println("Hostname: " + _hostname);
#endif // DEBUG

    // Load wifi connection information.
    if (!loadConfig(&station_ssid, &station_psk))
    {
        station_ssid = "";
        station_psk = "";
#ifdef DEBUG
        Serial.println("No WiFi connection information available.");
#endif // DEBUG
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
#ifdef DEBUG
        Serial.println("WiFi config changed.");
#endif // DEBUG

        // ... Try to connect to WiFi station.
        WiFi.begin(station_ssid.c_str(), station_psk.c_str());

#ifdef DEBUG
        // ... Print new SSID
        Serial.print("new SSID: ");
        Serial.println(WiFi.SSID());
        // ... Uncomment this for debugging output.
        //WiFi.printDiag(Serial);
#endif // DEBUG
    }
    else
    {
        // ... Begin with sdk config.
        WiFi.begin();
    }

#ifdef DEBUG
    Serial.println("Wait for WiFi connection.");
#endif // DEBUG

    // ... Give ESP 10 seconds to connect to station.
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)
    {
#ifdef DEBUG
        Serial.write('.');
#endif // DEBUG

        //Serial.print(WiFi.status());
        delay(500);
    }

#ifdef DEBUG
    Serial.println();
#endif // DEBUG

    // Check connection
    if (WiFi.status() == WL_CONNECTED)
    {
#ifdef DEBUG
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
#endif // DEBUG
    }
    else
    {
#ifdef DEBUG
        Serial.println("Can not connect to WiFi station. Go into AP mode.");
#endif // DEBUG

        // Go into software AP mode.
        WiFi.mode(WIFI_AP);
        delay(10);
        WiFi.softAP(ap_default_ssid, ap_default_psk);

#ifdef DEBUG
        Serial.print("IP address: ");
        Serial.println(WiFi.softAPIP());
#endif // DEBUG
    }
}
} // namespace Wireless
} // namespace ESPanel