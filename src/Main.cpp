#include <Arduino.h>

#include "Wireless.h"
#include "LED.h"
#include "Main.h"
#include "MQTT.h"
#include "Sensors.h"
#include "DHTesp.h"

// CONFIGURATION ==========================================

#ifdef REMOTE_DEBUG
#include "RemoteDebug.h"
RemoteDebug Debug;
#endif // REMOTE_DEBUG

// TODO: Perhaps configuration headers that are not .env should be in one header.
#define HOSTNAME STR_VALUE(CONFIG_HOSTNAME)
#define POLLING_SPEED 1000 // check sensors every second
#define REMOTE_DEBUG
#define DEBUG_OUTPUT
//#define READ_ALL_PINS // This is for debug experimentation only.
//#define BLINK_READS

// WiFi Configuration =====================================
const char *ap_default_ssid = "ESPanelSetup"; ///< Default SSID.
const char *ap_default_psk = "";              ///< Default PSK.

DHTesp dht;

// Define pins available for input usage, these will be initialized as input.
const int inputPins[] = {
    //16,  // wake, wants to be low so this wont work to pulldown
    14,    // HSPICLK
    12,    // HSPIQ
    13,    // HSPID & RXD2
    //1,   // TXD0 (break serial) Couldn't make this one work even with serial not enabled
    3,     // RXD0 (break serial)
    5,
    4,
    15,    // TXD2 & HSPICS, must be low at boot
    //2,   // LED & D4, must be high at boot
    //0    // flash mode, can't be low at boot
};  //9, 10 are questionable, 1 and 3 also a maybe

const int pinCount = sizeof(inputPins) / sizeof(int);

// Configure up to 7 sensors, one for each pin max
using namespace ESPanel::Sensors;

#define SENSOR_COUNT 6 // WARNING!! WARNING!! UPDATE THIS IF CHANGING THE SENSOR COUNT
static Sensor *SensorList[] = {
    new MotionSensor(5, Location::FrontHall), // Front room motion
    new DoorSensor(12, Location::FrontHall),  // Front door
    new DoorSensor(14, Location::Patio),      // Patio door
    new DoorSensor(4, Location::BackHall),    // Back Hall, needed a 220ohm resistor inline to compensate for some terminating resistor down the line
    new DoorSensor(13, Location::Basement),   // Basement door
    new SwitchSensor(3, Location::Basement),  // Switch in basement - Last one that can be used reliably is 3, but it should break serial rx
};

// END CONFIGURATION ======================================

using namespace ESPanel::Setup;
using namespace ESPanel::Wireless;
using namespace ESPanel::LED;
using namespace ESPanel::MQTT;

void setup()
{
  // Invert the state of the basement switch
  SensorList[5]->setInverted();

  // Enable the builtin led for blinking
  pinMode(D4, OUTPUT); 

#ifdef DEBUG_OUTPUT
  // Start Serial interface
  startSerial();
#endif // DEBUG_OUTPUT

  // Start WiFi and setup for OTA
  wifiStart(HOSTNAME, ap_default_ssid, ap_default_psk);

  // Start OTA server.
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();

  // MQTT connection config.
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(ESPanel::MQTT::mqttCallback);

#ifdef REMOTE_DEBUG
  // Start Telnet server based remote debug
  Debug.begin(HOSTNAME);
  Debug.setResetCmdEnabled(true);
#endif // REMOTE_DEBUG

  // Enable pin inputs for reading security sensors
  setPins();

  // DHT22 configuration. (Temperature & Humidity)
  dht.setup(D0, DHTesp::DHT22);

  // BLINK! 1 second to show that setup completed.
  blink(1000);
}


void loop()
{
  // Handle OTA server.
  ArduinoOTA.handle();

  // Loop the sensors for current state, and publish MQTT values for any with changed states.
  for (int i = 0; i < SENSOR_COUNT; i++)
  {
    if (SensorList[i]->updateState()) // If state changed
    {
      char topicBuf[50];
      snprintf(topicBuf, 50, "%s/%s/%s", BASE_TOPIC, SensorList[i]->getLocation(), SensorList[i]->getType());
      mqttClient.publish(topicBuf, SensorList[i]->getState() ? PAYLOAD_TRUE : PAYLOAD_FALSE, true); // publish retained states
    }
  }

#ifdef READ_ALL_PINS
  readAllPins();
#endif // READ_ALL_PINS

  // Evaluate the DHT22. (Only performed after 30 seconds.)
  // TODO: The Temp & Humidity deltas should be compared to require a minimum diff before publish.
  // TODO: It might be wise to take a rolling average per second and decide if that value changed enough.
  static unsigned long previousTime = 0;
  static const int dhtInterval = 30000; // 30 seconds.
  unsigned long currentMillis = millis();
  if (currentMillis - previousTime >= dhtInterval)
  {
    char buff[10];
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    if (std::isnan(humidity) || std::isnan(temperature))
    {
#ifdef DEBUG_OUTPUT
      Serial.println("Unable to get readings from DHT22, skipping submission.");
#endif
    } else {
      // If valid values are read, publish them to MQTT.
      dtostrf(dht.toFahrenheit(temperature), 2, 2, buff);
      mqttClient.publish(TEMPERATURE_TOPIC, buff, true);
      dtostrf(humidity, 2, 2, buff);
      mqttClient.publish(HUMIDITY_TOPIC, buff, true);
      previousTime = currentMillis;
    }
  }

#ifdef REMOTE_DEBUG
  Debug.handle();
#endif

  // Ensure MQTT connected, and do MQTT work loop.
  if (!mqttClient.connected())
  {
    ESPanel::MQTT::reconnectMQTT();
  }
  mqttClient.loop();

#ifdef REMOTE_DEBUG
  rdebugVln(".");
#endif

  // Blink LED to show polling cycle.
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
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);
//  delay(2000); // wait for serial
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
// This method is only here for testing and debug purposes. To allow for "feeling out" what pins are available and
// what devices are connected to what.
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
      // redundant, logged by remote
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
      // rdebugVln("Pin %d is pulled LOW.", inputPins[j]);
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