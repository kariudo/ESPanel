#include "MQTT.h"

namespace ESPanel
{
inline namespace MQTT
{

void reconnectMQTT()
{
  Serial.print("Attempting MQTT connection...");
  // Connect with a will messages
  if (mqttClient.connect(HOSTNAME, MQTT_USER, MQTT_PASSWORD, WILL_TOPIC, MQTT_QOS, true, PAYLOAD_FALSE))
  {
#ifdef DEBUG_OUTPUT
    Serial.println("MQTT Connected");
#endif
#ifdef REMOTE_DEBUG
    rdebugIln("Posting connected to will topic %s", __TIMESTAMP__, WILL_TOPIC);
#endif

    // Notify "will" that we are connected.
    mqttClient.publish(WILL_TOPIC, PAYLOAD_TRUE, true);
  }
  else
  {
#ifdef REMOTE_DEBUG
    rdebugEln("Waiting 5 seconds, failed to connect to MQTT server (%s:%s@%s:%d). Failed.", MQTT_USER, MQTT_PASSWORD, MQTT_SERVER, MQTT_PORT);
#endif
#ifdef DEBUG_OUTPUT
    Serial.print("failed, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" will try again...");
#endif
    delay(5000);
  }
}



// todo: relocate to a better place
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  // todo: handle any subscriptions (possibly to control config)
#ifdef DEBUG_OUTPUT
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
#endif // DEBUG_OUTPUT
  rdebugIln("[Message Received] %s", payload);
}

} // namespace MQTT
} // namespace ESPanel