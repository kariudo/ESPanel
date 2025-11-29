# ESPanel

Alarm panel integration firmware for the ESP8266 microcontroller.

Replaces a "old school" home alarm system, publishing inputs (motion, door, switches) to Home Assistant and MQTT. Allowing for the repurposing of existing home sensors.

## Configuration

Requires an `.env` or other means of setting the following required env vars:

```conf
CONFIG_MQTT_SERVER=192.168.1.110
CONFIG_MQTT_PORT=1883
CONFIG_MQTT_USER=YourUser
CONFIG_MQTT_PASSWORD=YourPassword
CONFIG_WIFI_SSID=YourWifiNetwork
CONFIG_WIFI_PASS=YourWifiPassword
```
