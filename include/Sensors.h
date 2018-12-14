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
  Motion
};

enum class Location
{
  Unknown = 0,
  FrontHall,
  Patio
};

// Sensor - Describes a sensor connected to the panel.
// Provides methods of updating and reading the state of the sensor.
class Sensor
{
protected:
  const SensorType type;
  const Location location;
  const int pin;
  bool state;

public:
  Sensor();
  Sensor(int pin, SensorType type, Location location);
  Sensor(int pin, SensorType type, Location location, bool initialState);
  // updateState - Set the current known state of a sensor, return True if the state changed.
  const bool updateState(const bool currentState);
  // getState - Return the current state of the sensor.
  const bool getState();
  // stateMessage - Return a human readable description of the state of the sensor.
  const char *stateMessage();
};

} // namespace Sensors
} // namespace ESPanel

#endif // !SENSORS_H_