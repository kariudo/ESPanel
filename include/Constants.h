#ifndef CONSTANTS_H_
#define CONSTANTS_H_

//#define DEBUG
//#define BLINK_READS
#define REMOTE_DEBUG

const char *HOSTNAME = "ESPanel";
const char *ap_default_ssid = "ESPanelSetup"; ///< Default SSID.
const char *ap_default_psk = "";              ///< Default PSK.

const int inputPins[] = {
    //16, //wake, wants to be low so this wont work to pulldown
    14, //HSPICLK
    12, //HSPIQ
    13, //HSPID & RXD2
        //  1,  //RXD0 (break serial) Couldn't make this one work even with serial not enabled
    3,  //TXD0 (break serial)
    5,
    4,
    15, // TXD2 & HSPICS, must be low at boot
        // 2, // LED, must be high at boot
        // 0 // flash mode, can't be low at boot
};      //9, 10 are questionable, 1 and 3 also a maybe

const int pinCount = sizeof(inputPins) / sizeof(int);

#endif // !CONSTANTS_H_