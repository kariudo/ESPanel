#ifndef SENSORS_H_
#define SENSORS_H_

#include <Arduino.h>
#include "RemoteDebug.h"

extern RemoteDebug Debug;

namespace ESPanel
{
inline namespace Sensors
{

enum class SensorType
{
  Generic = 0,
  Door,
  Window,
  Motion,
  Switch
};

enum class Location
{
  Unknown = 0,
  FrontHall,
  Patio,
  BackHall,
  Basement
};

// Sensor - Describes a sensor connected to the panel.
// Provides methods of updating and reading the state of the sensor.
class Sensor
{
protected:
  int pin;
  const SensorType type;
  const Location location;
  bool state;
  bool inverted;

public:
  Sensor();
  Sensor(int pin, SensorType type, Location location);
#define MotionSensor(pin, location) Sensor(pin, SensorType::Motion, location)
#define DoorSensor(pin, location) Sensor(pin, SensorType::Door, location)
#define SwitchSensor(pin, location) Sensor(pin, SensorType::Switch, location)
  Sensor(int pin, SensorType type, Location location, bool initialState);
  // updateState - Read pin for the current state of the sensor, return true if changed.
  const bool updateState();
  // setState - Set the current known state of a sensor, return True if the state changed.
  const bool setState(const bool currentState);
  // getState - Return the current state of the sensor.
  const bool getState();
  // stateMessage - Return a human readable description of the state of the sensor.
  const char *stateMessage();
  // getLocation - Return the location of the sensor
  const char *getLocation();
  // getType - Return the sensor type
  const char *getType();
  // setInverted - Invert the logic of the state on read
  void setInverted();

private:
  void configurePin();
};

} // namespace Sensors
} // namespace ESPanel

#endif // !SENSORS_H_