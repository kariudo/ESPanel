#include <Arduino.h>

#include "Constants.h"
#include "Wireless.h"
#include "LED.h"
#include "Main.h"

using namespace ESPanel;

void setup()
{
  // Enable the builtin led for blinking
  pinMode(2, OUTPUT);

#ifdef DEBUG
  // Start Serial interface
  startSerial();
#endif // DEBUG

  // Start WiFi and setup for OTA
  wifiStart(HOSTNAME, ap_default_ssid, ap_default_psk);

  // Start OTA server.
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();

#ifdef REMOTE_DEBUG
  // Start Telnet server based remote debug
  Debug.begin(HOSTNAME);
  Debug.setResetCmdEnabled(true);
#endif // REMOTE_DEBUG

  setPins(); // Enable pin inputs for reading security sensors
}

void loop()
{
  static int found;
  static int readVal;
  // Handle OTA server.
  ArduinoOTA.handle();
  // Read the pins
  found = 0;
  for (int j = 0; j < pinCount - 1; j++)
  {
    readVal = digitalRead(inputPins[j]);
    switch (inputPins[j])
    {
    case 5: // Front Room Motion (NC)
      if (readVal == HIGH)
      {
#ifdef REMOTE_DEBUG
        rdebugDln("Pin %d is HIGH, circuit is open.", inputPins[j]);
        rdebugIln("Front room motion detected.");
#endif // REMOTE_DEBUG
      }
      break;
    case 12: // Front Door
      if (readVal == HIGH)
      {
#ifdef REMOTE_DEBUG
        rdebugDln("Pin %d is HIGH, circuit is open.", inputPins[j]);
        rdebugIln("Front door is open.");
#endif // REMOTE_DEBUG
      }
      break;
//     case 4: // Family Door, doesn't seem to be working, no idea where this actually is
//       if (readVal == HIGH)
//       {
// #ifdef REMOTE_DEBUG
//         rdebugDln("Pin %d is HIGH, circuit is open.", inputPins[j]);
//         rdebugIln("Den door is open.");
// #endif // REMOTE_DEBUG
//       }
//       break;
    case 14: // Kitchen Door
      if (readVal == HIGH)
      {
#ifdef REMOTE_DEBUG
        rdebugDln("Pin %d is HIGH, circuit is open.", inputPins[j]);
        rdebugIln("Patio door is open.");
#endif // REMOTE_DEBUG
      }
      break;
    }
#ifdef REMOTE_DEBUG
    rdebugVln("Pin %d is %s.", inputPins[j], readVal == HIGH ? "HIGH" : "LOW");
#endif // REMOTE_DEBUG

#ifdef DEBUG
    if (readVal == LOW)
    {
      if (found == 0)
      {
        Serial.println();
      }
      Serial.print("Pin ");
      Serial.print(inputPins[j]);
      Serial.println(" is pulled LOW.");
#ifdef BLINK_READS
      for (int k = 0; k < inputPins[j] - 1; k++)
      {
        blink(200);
      }
#endif
#ifdef REMOTE_DEBUG
      rdebugVln("Pin %d is pulled LOW.", inputPins[j]);
#endif
      found++;
    }
#endif // DEBUG
  }
#ifdef DEBUG
  if (found == 0)
  {
    Serial.print(".");
  }
#endif
#ifdef REMOTE_DEBUG
  Debug.handle();
#endif
#ifdef BLINK_READS
  blink(1000);
#else
  delay(1000); // yield() if we don't delay, keep those esp juices flowing
#endif
}

namespace ESPanel
{
#ifdef DEBUG
void startSerial()
{
  Serial.begin(9600);
  while (!Serial)
  {
    delay(10); // wait for serial
  }
  Serial.println();
  Serial.println(); // Get clear of the boot noise
  Serial.println("\r\n");
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);
}
#endif // DEBUG

void setPins()
{
  int i;
  for (i = 0; i < pinCount - 1; i++)
  {
    pinMode(inputPins[i], INPUT_PULLUP);
#ifdef DEBUG
    Serial.print("Set pin ");
    Serial.print(inputPins[i]);
    Serial.println(" to input");
#endif // DEBUG
  }
}
} // namespace ESPanel