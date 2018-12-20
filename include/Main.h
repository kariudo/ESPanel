#ifndef MAIN_H_
#define MAIN_H_

namespace ESPanel
{
inline namespace Setup
{
#ifdef DEBUG_OUTPUT
void startSerial();
#endif // DEBUG_OUTPUT
void setPins();
void readAllPins();
void reconnectMQTT();
void startSerial();
void mqttCallback(char *topic, byte *payload, unsigned int length);
} // namespace Setup
} // namespace ESPanel

#endif // !MAIN_H_