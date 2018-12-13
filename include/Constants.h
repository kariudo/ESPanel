#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define REMOTE_DEBUG
//#define DEBUG
//#define BLINK_READS

#ifdef REMOTE_DEBUG
#include "RemoteDebug.h"
RemoteDebug Debug;
#endif // REMOTE_DEBUG

#include "Sensors.h"

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
const ESPanel::Sensor *AllSensors[] = {
    new ESPanel::Sensor (5, ESPanel::SensorType::Motion), // Front room motion
    new ESPanel::Sensor (12, ESPanel::SensorType::Door), // Front door
    new ESPanel::Sensor (14, ESPanel::SensorType::Door), // Patio door
}

#endif // !CONSTANTS_H_