#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#include <Arduino.h>
#include <FS.h>

#include "LED.h"

namespace ESPanel
{
inline namespace FileSystem
{
void initFS();
bool loadConfig(String *ssid, String *pass);
bool saveConfig(String *ssid, String *pass);
} // namespace OTA
} // namespace ESPanel

#endif // FILESYSTEM_H_