#include <Arduino.h>

#include "Config.h"
#include "Wireless.h"
#include "LED.h"
#include "Main.h"
#include "Sensors.h"

// CONFIGURATION ============

#define POLLING_SPEED 1000 // check sensors every second
#define REMOTE_DEBUG
//#define DEBUG_OUTPUT
//#define BLINK_READS
//#define READ_ALL_PINS

#ifdef REMOTE_DEBUG
#include "RemoteDebug.h"
RemoteDebug Debug;
#endif // REMOTE_DEBUG

const char *HOSTNAME = "ESPanel";
const char *ap_default_ssid = "ESPanelSetup"; ///< Default SSID.
const char *ap_default_psk = "";              ///< Default PSK.

const int inputPins[] = {
    //16, //wake, wants to be low so this wont work to pulldown
    14, //HSPICLK
    12, //HSPIQ
    13, //HSPID & RXD2
    //  1,  //RXD0 (break serial) Couldn't make this one work even with serial not enabled
    3, //TXD0 (break serial)
    5,
    4,
    15, // TXD2 & HSPICS, must be low at boot
        // 2, // LED, must be high at boot
        // 0 // flash mode, can't be low at boot
};      //9, 10 are questionable, 1 and 3 also a maybe

const int pinCount = sizeof(inputPins) / sizeof(int);

// Configure up to 7 sensors, one for each pin max
using namespace ESPanel::Sensors;

#define SENSOR_COUNT 3
static Sensor *SensorList[] = {
    new MotionSensor(5, Location::FrontHall), // Front room motion
    new DoorSensor(12, Location::FrontHall),  // Front door
    new DoorSensor(14, Location::Patio),      // Patio door
                                              // Sensor wired for 4, family room door isn't responding so it is not defined here until thats sorted
};

// END CONFIGURATION ============

using namespace ESPanel::Setup;
using namespace ESPanel::Wireless;
using namespace ESPanel::LED;

void setup()
{
  // Enable the builtin led for blinking
  pinMode(2, OUTPUT);

#ifdef DEBUG_OUTPUT
  // Start Serial interface
  startSerial();
#endif // DEBUG_OUTPUT

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
  // Handle OTA server.
  ArduinoOTA.handle();

  // Loop the sensors
  for (int i = 0; i < SENSOR_COUNT; i++)
  {
    SensorList[i]->updateState();
//    rdebugVln("%s", SensorList[i]->stateMessage());
  }

#ifdef READ_ALL_PINS
  readAllPins();
#endif // READ_ALL_PINS

#ifdef REMOTE_DEBUG
  Debug.handle();
#endif

  blink(int(POLLING_SPEED / 2)); // yield() if we don't delay, keep those esp juices flowing
}

namespace ESPanel
{
inline namespace Setup
{
#ifdef DEBUG_OUTPUT
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
#endif // DEBUG_OUTPUT

void setPins()
{
  int i;
  for (i = 0; i < pinCount - 1; i++)
  {
    pinMode(inputPins[i], INPUT_PULLUP);
#ifdef DEBUG_OUTPUT
    Serial.print("Set pin ");
    Serial.print(inputPins[i]);
    Serial.println(" to input");
#endif // DEBUG_OUTPUT
  }
}

#ifdef READ_ALL_PINS
void readAllPins()
{
  int found;
  int readVal;

  // Read the pins
  found = 0;
  for (int j = 0; j < pinCount - 1; j++)
  {
    readVal = digitalRead(inputPins[j]);

#ifdef REMOTE_DEBUG
    rdebugVln("Pin %d is %s.", inputPins[j], readVal == HIGH ? "HIGH" : "LOW");
#endif // REMOTE_DEBUG

#ifdef DEBUG_OUTPUT
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
#endif // DEBUG_OUTPUT
  }    // END pin loop
#ifdef DEBUG_OUTPUT
  if (found == 0)
  {
    Serial.print(".");
  }
#endif
}
#endif // READ_ALL_PINS
} // namespace Setup
} // namespace ESPanel