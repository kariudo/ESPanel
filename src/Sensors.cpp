#include "Sensors.h"

#define ERR_MSG_MEM "ERROR: Insufficient Memory to return state message"

namespace ESPanel
{
inline namespace Sensors
{

const char *LocationNames[] = {
    "unknown",
    "front hall",
    "patio"};

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

Sensor::Sensor() : pin(0), location(Location::FrontHall), type(SensorType::Generic), state(false) {}

Sensor::Sensor(int pin, SensorType type, Location location) : pin(pin), type(type), location(location), state(false) {}

Sensor::Sensor(int pin, SensorType type, Location location, bool initialState) : pin(pin), type(type), location(location), state(initialState) {}

const bool Sensor::getState()
{
    return state;
}

const bool Sensor::updateState(const bool currentState)
{
    bool stateChanged;
    if (state != currentState)
    {
        stateChanged = true;
    }
    state = currentState;
    return stateChanged;
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
#ifdef DEBUG
        Serial.println(ERR_MSG_MEM);
#endif // DEBUG
#ifdef REMOTE_DEBUG
        rdebugEln(ERR_MSG_MEM);
#endif // REMOTE_DEBUG
        return ERR_MSG_MEM;
    }
}

} // namespace Sensors
} // namespace ESPanel