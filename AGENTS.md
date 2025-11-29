# ESPanel - Agent Documentation

## Project Overview

ESPanel is an experimental alarm panel integration firmware for the ESP8266 microcontroller. It reads security sensors (door, motion, switch) and publishes their states to MQTT, plus provides temperature/humidity monitoring via DHT22 sensor.

**Platform**: ESP8266 (NodeMCU v2)  
**Framework**: Arduino  
**Build System**: PlatformIO  
**Language**: C++11  

## Essential Commands

### Building and Deployment

```bash
# Build firmware
pio run

# Upload to device
pio run --target upload

# Clean build
pio run --target clean

# Monitor serial output
pio device monitor

# Build with specific environment
pio run -e nodemcuv2

# Upload with specific environment
pio run -e nodemcuv2 --target upload
```

### Testing

```bash
# Run tests (if available)
pio test

# Run specific test file
pio test --filter test_file.cpp
```

## Project Structure

```
ESPanel/
├── src/                 # Source code files
│   ├── Main.cpp         # Main application logic
│   ├── Sensors.cpp      # Sensor management
│   ├── Wireless.cpp     # WiFi and OTA handling
│   ├── WebServer.cpp    # Async web server
│   ├── LED.cpp          # LED control
│   └── FileSystem.cpp   # LittleFS file system
├── include/            # Header files
│   ├── Main.h
│   ├── Sensors.h
│   ├── Wireless.h
│   ├── WebServer.h
│   ├── LED.h
│   └── FileSystem.h
├── data/               # Static web content
│   └── www/            # Web server files
├── lib/                # Third-party libraries
│   ├── pubsubclient/   # MQTT client
│   └── RemoteDebug/    # Telnet debugging
├── platformio.ini      # PlatformIO configuration
├── env-extra.py        # Environment variable script
└── .hintrc            # HTML hint configuration
```

## Architecture and Design Patterns

### Namespace Structure
- **ESPanel** - Main namespace
  - **Setup** - Core setup functions (MQTT, serial, pins)
  - **Sensors** - Sensor management and types
  - **Wireless** - WiFi and OTA handling
  - **WebServer** - Async web server routes
  - **LED** - LED control functions

### Sensor System
- **Sensor Types**: Generic, Door, Window, Motion, Switch
- **Locations**: Unknown, FrontHall, Patio, BackHall, Basement
- **Configuration**: Define sensors in `Main.cpp` using `SENSOR_COUNT` array
- **Pins**: Currently configured for specific ESP8266 GPIO pins (3, 4, 5, 12, 13, 14, 15)

### Communication
- **MQTT**: Primary communication protocol for sensor data
- **Topics**: `sensors/{hostname}/{location}/{type}` or `test-sensors/{hostname}/{location}/{type}`
- **Web Server**: Async web server for configuration/status
- **Remote Debug**: Telnet-based debugging (RemoteDebug library)

## Configuration

### Environment Variables
Required `.env` file with:
```conf
CONFIG_MQTT_SERVER=192.168.1.110
CONFIG_MQTT_PORT=1883
CONFIG_MQTT_USER=YourUser
CONFIG_MQTT_PASSWORD=YourPassword
CONFIG_WIFI_SSID=YourWifiNetwork
CONFIG_WIFI_PASS=YourWifiPassword
```

### Build Flags
```ini
# platformio.ini
build_flags =
    -DEBUG_OUTPUT      # Enable serial debugging
    -DEV_TEST         # Use test MQTT topic prefix
    -std=gnu++11      # C++11 standard
```

## Code Conventions

### Naming
- **Classes**: PascalCase (e.g., `Sensor`, `RemoteDebug`)
- **Functions**: camelCase (e.g., `updateState`, `reconnectMQTT`)
- **Variables**: camelCase (e.g., `mqttClient`, `dht`)
- **Constants**: SCREAMING_SNAKE_CASE (e.g., `POLLING_SPEED`, `MQTT_QOS`)
- **Macros**: SCREAMING_SNAKE_CASE (e.g., `STRINGIZER`, `STR_VALUE`)

### File Structure
- **Headers**: Include guards with `#ifndef FILENAME_H_` / `#define FILENAME_H_` / `#endif`
- **Implementation**: Corresponding `.cpp` files in `src/`
- **Namespace**: All code within `ESPanel` namespace with inline sub-namespaces

### Error Handling
- Use `Serial.println()` for debug output when `DEBUG_OUTPUT` is defined
- Use `rdebugIln()`, `rdebugEln()` for remote debug when `REMOTE_DEBUG` is defined
- Graceful degradation for sensor read failures

### Memory Management
- Static allocation for sensors (no dynamic allocation after setup)
- Careful use of string buffers to prevent memory fragmentation
- Warning comments when memory limitations are approached

## Key Components

### Main Application (`Main.cpp`)
- **Setup()**: Initialize all systems, configure pins, start services
- **Loop()**: Main execution cycle with:
  - MQTT connection management
  - Sensor state polling and publishing
  - DHT22 temperature/humidity reading
  - OTA handling
  - Remote debug processing

### Sensor System (`Sensors.cpp/.h`)
- **Sensor base class**: Abstract interface for all sensors
- **Specialized sensors**: Motion, Door, Window, Switch (using macros)
- **State management**: Tracks changes and publishes MQTT updates
- **Location system**: Maps sensors to physical locations

### Communication
- **MQTT**: Will messages for connection status, retained messages for sensor states
- **WiFi**: Automatic reconnection with fallback AP mode for setup
- **Web Server**: AsyncESP for non-blocking operation
- **Remote Debug**: Telnet server for real-time debugging

## Important Gotchas

### ESP8266 Pin Limitations
- Some pins have special boot requirements (must be HIGH/LOW at boot)
- Pin 0 and 2 have boot mode constraints
- Serial pins (1, 3) conflict with other functions
- Current configuration uses pins: 3, 4, 5, 12, 13, 14, 15

### Memory Constraints
- Limited RAM on ESP8266 - careful with string operations
- Sensor array size fixed at compile time (update `SENSOR_COUNT`)
- DHT22 readings can fail - check for NaN values

### MQTT Configuration
- Will messages used for connection status
- Retained messages for sensor states
- QoS 1 for reliable delivery
- Topics include hostname for multi-unit support

### Debug Output
- Conditional compilation with `#ifdef DEBUG_OUTPUT`
- Remote debug via Telnet (port 23) when enabled
- Serial output at 9600 baud

### OTA Updates
- Enabled for remote firmware updates
- Hostname-based identification
- Requires proper WiFi connection

## Dependencies

### Core Libraries
- **Arduino Core**: Standard Arduino functions
- **ESP8266WiFi**: WiFi functionality
- **ESPAsyncTCP**: Async TCP for web server
- **ESPAsyncWebServer**: Async web server
- **PubSubClient**: MQTT client
- **DHT sensor library**: Temperature/humidity sensor
- **RemoteDebug**: Telnet debugging
- **LittleFS**: File system for web content

### PlatformIO Configuration
- **Platform**: espressif8266@2.6.3
- **Board**: nodemcuv2
- **Framework**: arduino
- **Filesystem**: littlefs

## Development Workflow

1. **Environment Setup**: Configure `.env` file with credentials
2. **Configuration**: Modify sensor array and constants in `Main.cpp`
3. **Building**: Use `pio run` to build firmware
4. **Testing**: Upload to device and monitor output
5. **Debugging**: Use serial monitor or remote debug telnet
6. **Deployment**: Upload OTA or via USB

## Security Considerations

- MQTT credentials stored in environment variables
- WiFi credentials in environment variables
- No authentication on web server (current limitation)
- Sensor pins use INPUT_PULLUP for security (HIGH = normal, LOW = triggered)

## Performance Notes

- Sensor polling every 1000ms (configurable via `POLLING_SPEED`)
- DHT22 reading every 10000ms to avoid overloading sensor
- MQTT reconnection attempts every 5000ms on failure
- Non-blocking design using async web server