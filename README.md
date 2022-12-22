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
