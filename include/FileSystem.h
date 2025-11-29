#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#include "LED.h"
#include "LittleFS.h"
#include <Arduino.h>
#include <FS.h>

namespace ESPanel {
inline namespace FileSystem {
void initFS();
bool loadConfig(String *ssid, String *pass);
bool saveConfig(String *ssid, String *pass);
} // namespace FileSystem
} // namespace ESPanel

#endif // FILESYSTEM_H_