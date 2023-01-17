# ESPanel [![Build Status](https://travis-ci.com/kariudo/ESPanel.svg?branch=master)](https://travis-ci.com/kariudo/ESPanel)

Experimental alarm panel integration firmware for the ESP8266 microcontroller.


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


## TODO

There are a few currently outstanding tasks:

- Add proper "WiFi Manager" library support, to avoid static config. Whatever implementation I had, I seem to have broken at some point.
- Switch to ESP32 for bluetooth bridge support?
- Define elements for HomeAssistant auto-config via MQTT JSON objects.
- Move DHT reads to sensors namespace.
- Switch travis for another build system.
- Web based flashing.
- Break additional configuration elements out to .env from .h files.
- Smooth out climate reads, instead of just waiting 30 seconds.
- Only send climate on delta.