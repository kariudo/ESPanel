// CONFIGURATION ============
#ifndef POLLING_SPEED
#define POLLING_SPEED 1000 // check sensors every second
#endif

#include <Arduino.h>

#include "DHTesp.h"
#include "LED.h"
#include "Main.h"
#include "PubSubClient.h"
#include "Sensors.h"
#include "WebServer.h"
#include "Wireless.h"

// #define BLINK_READS
// #define READ_ALL_PINS

#ifdef REMOTE_DEBUG
#include "RemoteDebug.h"
RemoteDebug Debug;
#endif // REMOTE_DEBUG

#define HOSTNAME STR_VALUE(CONFIG_HOSTNAME)
#ifdef DEV_TEST
#define BASE_TOPIC "test-sensors/" HOSTNAME
#else
#define BASE_TOPIC "sensors/" HOSTNAME
#endif
#define WILL_TOPIC BASE_TOPIC "/connected"
#define MQTT_QOS 1

#define STRINGIZER(arg) #arg
#define STR_VALUE(arg) STRINGIZER(arg)
#define MQTT_SERVER STR_VALUE(CONFIG_MQTT_SERVER)
#define MQTT_PORT CONFIG_MQTT_PORT
#define MQTT_USER STR_VALUE(CONFIG_MQTT_USER)
#define MQTT_PASSWORD STR_VALUE(CONFIG_MQTT_PASSWORD)
#define PAYLOAD_TRUE STR_VALUE(1)
#define PAYLOAD_FALSE STR_VALUE(0)
#define TEMPERATURE_TOPIC BASE_TOPIC "/temperature"
#define HUMIDITY_TOPIC BASE_TOPIC "/humidity"

const char *ap_default_ssid = "ESPanelSetup"; ///< Default SSID.
const char *ap_default_psk = "";              ///< Default PSK.

WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHTesp dht;

const int inputPins[] = {
    // 16, //wake, wants to be low so this wont work to pulldown
    14, // HSPICLK
    12, // HSPIQ
    13, // HSPID & RXD2
    //  1,  //TXD0 (break serial) Couldn't make this one work even with serial
    //  not enabled
    3, // RXD0 (break serial)
    5,
    4,
    15, // TXD2 & HSPICS, must be low at boot
        // 2, // LED & D4, must be high at boot
        // 0 // flash mode, can't be low at boot
}; // 9, 10 are questionable, 1 and 3 also a maybe

const int pinCount = sizeof(inputPins) / sizeof(int);

// Configure up to 7 sensors, one for each pin max
using namespace ESPanel::Sensors;

#define SENSOR_COUNT                                                           \
  6 // WARNING!! WARNING!! UPDATE THIS IF CHANGING THE SENSOR COUNT
static Sensor *SensorList[] = {
    new MotionSensor(5, Location::FrontHall), // Front room motion
    new DoorSensor(12, Location::FrontHall),  // Front door
    new DoorSensor(14, Location::Patio),      // Patio door
    new DoorSensor(4,
                   Location::BackHall), // Back Hall, needed a 220ohm resistor
                                        // inline to compensate for some
                                        // terminating resistor down the line
    new DoorSensor(13, Location::Basement), // Basement door
    new SwitchSensor(
        3,
        Location::Basement), // Switch in basement - Last one that can be used
                             // reliably is 3, but it should break serial rx
};

// END CONFIGURATION ============

using namespace ESPanel::Setup;
using namespace ESPanel::Wireless;
using namespace ESPanel::LED;
using namespace ESPanel::WebServer;

void setup() {
  // Invert the state of the basement switch
  SensorList[5]->setInverted();

  // Enable the builtin led for blinking
  pinMode(D4, OUTPUT);

#ifdef DEBUG_OUTPUT
  // Start Serial interface
  startSerial();
//  delay(2000); // wait for serial
#endif // DEBUG_OUTPUT

  // Start WiFi and setup for OTA
  wifiStart(HOSTNAME, ap_default_ssid, ap_default_psk);

  // Start OTA server.
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();

  // MQTT connection config.
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(ESPanel::mqttCallback);

#ifdef REMOTE_DEBUG
  // Start Telnet server based remote debug
  Debug.begin(HOSTNAME);
  Debug.setResetCmdEnabled(true);
#endif // REMOTE_DEBUG

  setPins(); // Enable pin inputs for reading security sensors

  // DHT22 configuration.
  dht.setup(D0, DHTesp::DHT22);

// Start webserverWebServer
#ifdef DEBUG_OUTPUT
  Serial.println("Initialize webserver");
#endif // DEBUG_OUTPUT
  registerRoutes();

  // BLINK!
  blink(1000);
}

void loop() {
  // Handle OTA server.
  ArduinoOTA.handle();

  // Loop the sensors
  // Serial.println("Checking for updated sensor states.");
  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (SensorList[i]->updateState()) // If state changed
    {
      char topicBuf[50];
      snprintf(topicBuf, 50, "%s/%s/%s", BASE_TOPIC,
               SensorList[i]->getLocation(), SensorList[i]->getType());
      mqttClient.publish(
          topicBuf, SensorList[i]->getState() ? PAYLOAD_TRUE : PAYLOAD_FALSE,
          true); // publish retained states
    }
  }

#ifdef READ_ALL_PINS
  readAllPins();
#endif // READ_ALL_PINS

  // Evaluate the DHT22.
  static unsigned long previousTime = 0;
  static const int dhtInterval = 10000; // 10 seconds.
  unsigned long currentMillis = millis();
  if (currentMillis - previousTime >= dhtInterval) {
    char buff[10];
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    if (std::isnan(humidity) || std::isnan(temperature)) {
#ifdef DEBUG_OUTPUT
//      Serial.println("Unable to get readings from DHT22, skipping
//      submission.");
#endif
    } else {
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

  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

#ifdef REMOTE_DEBUG
  rdebugVln(".");
#endif

  blink(int(POLLING_SPEED /
            2)); // yield() if we don't delay, keep those esp juices flowing
}

namespace ESPanel {
inline namespace Setup {

// todo: move to a better namespace
void reconnectMQTT() {
  // while (!mqttClient.connected()) // looping here will get us stuck
  // {
  Serial.print("Attempting MQTT connection...");
  // Connect with a will messages
  if (mqttClient.connect(HOSTNAME, MQTT_USER, MQTT_PASSWORD, WILL_TOPIC,
                         MQTT_QOS, true, PAYLOAD_FALSE)) {
#ifdef DEBUG_OUTPUT
    Serial.println("MQTT Connected");
    Serial.print("Posting connected to will topic ");
    Serial.println(WILL_TOPIC);
#endif
#ifdef REMOTE_DEBUG
    rdebugIln("Posting connected to will topic %s", __TIMESTAMP__, WILL_TOPIC);
#endif

    // notify connected
    mqttClient.publish(WILL_TOPIC, PAYLOAD_TRUE, true);
  } else {
#ifdef REMOTE_DEBUG
    rdebugEln("Waiting 5 seconds, failed to connect to MQTT server "
              "(%s:%s@%s:%d). Failed.",
              MQTT_USER, MQTT_PASSWORD, MQTT_SERVER, MQTT_PORT);
#endif
#ifdef DEBUG_OUTPUT
    Serial.print("failed, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" will try again...");
#endif
    delay(5000);
  }
  // }
}

// todo: relocate to a better place
void mqttCallback(char *topic, byte *payload, unsigned int length) {
// todo: handle any subscriptions (possibly to control config)
#ifdef DEBUG_OUTPUT
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (uint i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
#endif // DEBUG_OUTPUT
  rdebugIln("[Message Received] %s", payload);
}

#ifdef DEBUG_OUTPUT
void startSerial() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial
  }
  Serial.println();
  Serial.println(); // Get clear of the boot noise
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);
}
#endif // DEBUG_OUTPUT

void setPins() {
  int i;
  for (i = 0; i < pinCount - 1; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
#ifdef DEBUG_OUTPUT
    Serial.print("Set pin ");
    Serial.print(inputPins[i]);
    Serial.println(" to input");
#endif // DEBUG_OUTPUT
  }
}

#ifdef READ_ALL_PINS
void readAllPins() {
  int found;
  int readVal;

  // Read the pins
  found = 0;
  for (int j = 0; j < pinCount - 1; j++) {
    readVal = digitalRead(inputPins[j]);

#ifdef REMOTE_DEBUG
    rdebugVln("Pin %d is %s.", inputPins[j], readVal == HIGH ? "HIGH" : "LOW");
#endif // REMOTE_DEBUG

#ifdef DEBUG_OUTPUT
    if (readVal == LOW) {
      if (found == 0) {
        Serial.println();
      }
      // redundant, logged by remote
      Serial.print("Pin ");
      Serial.print(inputPins[j]);
      Serial.println(" is pulled LOW.");
#ifdef BLINK_READS
      for (int k = 0; k < inputPins[j] - 1; k++) {
        blink(200);
      }
#endif
#ifdef REMOTE_DEBUG
      // rdebugVln("Pin %d is pulled LOW.", inputPins[j]);
#endif
      found++;
    }
#endif // DEBUG_OUTPUT
  } // END pin loop
#ifdef DEBUG_OUTPUT
  if (found == 0) {
    Serial.print(".");
  }
#endif
}
#endif // READ_ALL_PINS
} // namespace Setup
} // namespace ESPanel