#include "Sensors.h"

#define ERR_MSG_MEM "ERROR: Insufficient Memory to return state message"

namespace ESPanel
{
inline namespace Sensors
{

const char *LocationNames[] = {
    "unknown",
    "front hall",
    "patio",
    "back hall",
    "basement"};

const char *SensorTypeNouns[] = {
    "sensor",
    "door",
    "window",
    "motion"};

const char *SensorStateTrueDescs[] = {
    "active",
    "open",
    "open",
    "detected"};

const char *SensorStateFalseDescs[] = {
    "inactive",
    "closed",
    "closed",
    "not detected"};

Sensor::Sensor() : pin(0), type(SensorType::Generic), location(Location::FrontHall), state(false) {}

Sensor::Sensor(int pin, SensorType type, Location location) : pin(pin), type(type), location(location), state(false)
{
    configurePin();
}

Sensor::Sensor(int pin, SensorType type, Location location, bool initialState) : pin(pin), type(type), location(location), state(initialState)
{
    configurePin();
}

const bool Sensor::updateState()
{
    return setState(digitalRead(pin));
}

const bool Sensor::getState()
{
    return state;
}

const char *Sensor::getLocation()
{
    return LocationNames[int(location)];
}

const char *Sensor::getType()
{
    return SensorTypeNouns[int(type)];
}

const bool Sensor::setState(const bool currentState)
{
    if (state != currentState)
    {
        state = currentState;
#ifdef rdebugAln
        // If the state has chaned, log it
        rdebugAln("(%d) %s %s is %s",
                  pin,
                  LocationNames[int(location)],
                  SensorTypeNouns[int(type)],
                  (state ? SensorStateTrueDescs[int(type)] : SensorStateFalseDescs[int(type)]));
#endif // REMOTE_DEBUG

        // todo: add mqtt update here
        return true;
    }
    else
    {
        state = currentState;
        return false;
    }
}

const char *Sensor::stateMessage()
{
    char *out;
    if ((out = (char *)malloc(strlen(LocationNames[int(location)]) + strlen(SensorTypeNouns[int(type)]) + (state ? strlen(SensorStateTrueDescs[int(type)]) : strlen(SensorStateFalseDescs[int(type)])) + 6)) != NULL)
    {
        strcpy(out, LocationNames[int(location)]);
        strcat(out, " ");
        strcat(out, SensorTypeNouns[int(type)]);
        strcat(out, " is ");
        strcat(out, (state ? SensorStateTrueDescs[int(type)] : SensorStateFalseDescs[int(type)]));
        return out;
    }
    else
    {
#ifdef DEBUG_OUTPUT
        Serial.println(ERR_MSG_MEM);
#endif // DEBUG_OUTPUT
#ifdef REMOTE_DEBUG
        rdebugEln(ERR_MSG_MEM);
#endif // REMOTE_DEBUG
        return ERR_MSG_MEM;
    }
}

void Sensor::configurePin()
{
    // Set pit to input with internal pullup resistor active
    pinMode(pin, INPUT_PULLUP);
}

} // namespace Sensors
} // namespace ESPanel