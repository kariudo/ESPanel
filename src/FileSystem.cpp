#include "FileSystem.h"

namespace ESPanel
{
inline namespace FileSystem
{
void initFS()
{
  // Initialize file system.
  if (!SPIFFS.begin())
  {
#ifdef DEBUG
    Serial.println("Failed to mount file system");
#endif // DEBUG
    // blink 3 times for a failure warning, then restart
    for (int i = 0; i < 2; i++)
    {
      blink(1500);
    }
    ESP.restart();
  }
}
/**
   @brief Read WiFi connection information from file system.
   @param ssid String pointer for storing SSID.
   @param pass String pointer for storing PSK.
   @return True or False.

   The config file have to containt the WiFi SSID in the first line
   and the WiFi PSK in the second line.
   Line seperator can be \r\n (CR LF) \r or \n.
*/
bool loadConfig(String *ssid, String *pass)
{
  // open file for reading.
  File configFile = SPIFFS.open("/cl_conf.txt", "r");
  if (!configFile)
  {
    Serial.println("Failed to open cl_conf.txt.");

    return false;
  }

  // Read content from config file.
  String content = configFile.readString();
  configFile.close();

  content.trim();

  // Check if ther is a second line available.
  int8_t pos = content.indexOf("\r\n");
  uint8_t le = 2;
  // check for linux and mac line ending.
  if (pos == -1)
  {
    le = 1;
    pos = content.indexOf("\n");
    if (pos == -1)
    {
      pos = content.indexOf("\r");
    }
  }

  // If there is no second line: Some information is missing.
  if (pos == -1)
  {
    Serial.println("Infvalid content.");
    Serial.println(content);

    return false;
  }

  // Store SSID and PSK into string vars.
  *ssid = content.substring(0, pos);
  *pass = content.substring(pos + le);

  ssid->trim();
  pass->trim();

#ifdef DEBUG
  Serial.println("----- file content -----");
  Serial.println(content);
  Serial.println("----- file content -----");
  Serial.println("ssid: " + *ssid);
  Serial.println("psk:  " + *pass);
#endif

  return true;
} // loadConfig

/**
   @brief Save WiFi SSID and PSK to configuration file.
   @param ssid SSID as string pointer.
   @param pass PSK as string pointer,
   @return True or False.
*/
bool saveConfig(String *ssid, String *pass)
{
  // Open config file for writing.
  File configFile = SPIFFS.open("/cl_conf.txt", "w");
  if (!configFile)
  {
    Serial.println("Failed to open cl_conf.txt for writing");

    return false;
  }

  // Save SSID and PSK.
  configFile.println(*ssid);
  configFile.println(*pass);

  configFile.close();

  return true;
} // saveConfig
} // namespace FileSystem
} // namespace ESPanel