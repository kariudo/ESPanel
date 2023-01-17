#ifndef MQTT_H_
#define MQTT_H_

#include <Arduino.h>
#include "RemoteDebug.h"
#include "PubSubClient.h"

// MQTT Configuration =====================================
#define HOSTNAME "ESPanel"
#define BASE_TOPIC "sensors/" HOSTNAME
#define WILL_TOPIC BASE_TOPIC "/connected"
#define MQTT_QOS 1

#define PAYLOAD_TRUE STR_VALUE(1)
#define PAYLOAD_FALSE STR_VALUE(0)

#define STRINGIZER(arg) #arg
#define STR_VALUE(arg) STRINGIZER(arg)
#define MQTT_SERVER STR_VALUE(CONFIG_MQTT_SERVER)
#define MQTT_PORT CONFIG_MQTT_PORT
#define MQTT_USER STR_VALUE(CONFIG_MQTT_USER)
#define MQTT_PASSWORD STR_VALUE(CONFIG_MQTT_PASSWORD)
#define TEMPERATURE_TOPIC BASE_TOPIC "/temperature"
#define HUMIDITY_TOPIC BASE_TOPIC "/humidity"

extern RemoteDebug Debug;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

namespace ESPanel
{
inline namespace MQTT
{

void reconnectMQTT();

void mqttCallback(char *topic, byte *payload, unsigned int length);

} // namespace MQTT
} // namespace ESPanel

#endif // !MQTT_H_