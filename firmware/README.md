# AIoT Blue Wedge Platform

## Overview

The **AIoT Blue Wedge Platform** is a comprehensive Microchip IoT demonstration ecosystem consisting of multiple interconnected embedded systems. The platform showcases industrial IoT capabilities, AI/ML integration, distributed sensor networks, and real-time communication over **10BASE-T1S (Single Pair Ethernet)**.

### Key Technologies
- **MCUs:** ATSAME51J20A (Cortex-M4F), ATSAMD21G18A (Cortex-M0+)
- **Networking:** 10BASE-T1S Ethernet (LAN8651), BLE (RNBD451)
- **Protocols:** MQTT over TCP/IP, Cornetix IP500
- **Stack:** lwIP (Lightweight TCP/IP)
- **Architecture:** Hardware Abstraction Layer (HAL) for multi-board support

---

## Table of Contents
- [System Architecture](#system-architecture)
- [Board Descriptions](#board-descriptions)
  - [Demo Board](#1-demo-board)
  - [Smart HMI Board](#2-smart-hmi-board)
  - [Smart I/O Board](#3-smart-io-board)
  - [Smart Sensor Board](#4-smart-sensor-board)
  - [Test Me Board](#5-test-me-board)
  - [Smart Motor Board](#6-smart-motor-board)
- [Network Configuration](#network-configuration)
- [Software Architecture](#software-architecture)
- [MQTT Topic Structure](#mqtt-topic-structure)
- [Getting Started](#getting-started)

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    10BASE-T1S Network                       │
│                   Gateway: 192.168.0.5                      │
└─────────────────────────────────────────────────────────────┘
           │                   │                   │
    ┌──────┴──────┐      ┌─────┴─────┐      ┌──────┴──────┐
    │   Sensor    │      │    HMI    │      │    I/O      │
    │   Boards    │      │   Board   │      │   Board     │
    └─────────────┘      └───────────┘      └─────────────┘
           │                   │                    │
           └───────────────────┴────────────────────┘
                               │
                        ┌──────┴──────┐
                        │ MQTT Broker │
                        └─────────────┘
```

### Communication Flow
1. **Physical Layer:** 10BASE-T1S Single Pair Ethernet
2. **Network Layer:** lwIP TCP/IP Stack (IPv4)
3. **Application Layer:** MQTT Pub/Sub, HTTP, Cornetix IP500
4. **Data Format:** JSON, CSV

---

## Board Descriptions

### 1. Demo Board
**Project:** `AIoT_Demo_SAME51.X`
**MCU:** ATSAME51J20A
**IP Address:** 192.168.0.104
**MQTT Node:** `demo`

#### Active Modules
| Module | Interface | Purpose |
|--------|-----------|---------|
| APC1 | I²C | Air Quality Click sensor |
| BME280 | I²C | Temperature, Humidity, Pressure |
| OLED-C | SPI | Color display (96x96) |
| Sensirion SEN5x | I²C | PM2.5, PM10, VOC, NOx, RH, Temp |
| Proximity3 (VCNL4200) | I²C | Proximity and ambient light |
| LAN8651 | SPI | 10BASE-T1S Ethernet PHY |

#### Purpose
Full-featured demonstration board showcasing comprehensive sensor integration with visual feedback. Primary use case: **Environmental monitoring with local visualization**.

---

### 2. Smart HMI Board
**Project:** `AIoT_Smart_HMI_SAME51.X`
**MCU:** ATSAME51J20A
**IP Address:** 192.168.0.101
**MQTT Node:** `smart_hmi`

#### Active Modules
| Module | Interface | Purpose |
|--------|-----------|---------|
| CapSense2 | I²C | Capacitive touch interface |
| CNX200 | UART | Cornetix IP500 industrial protocol |
| OLED-C | SPI | Display output |
| RNBD451 | UART | Bluetooth Low Energy 5.1 |
| LAN8651 | SPI | 10BASE-T1S Ethernet PHY |

#### Purpose
Human-Machine Interface with **dual connectivity** (wired T1S + wireless BLE). Acts as a thermostat/control panel with touch input and display output. Bridges wireless devices to the wired industrial network.

---

### 3. Smart I/O Board
**Project:** `AIoT_Smart_IO_SAME51.X`
**MCU:** ATSAME51J20A
**IP Address:** 192.168.0.102
**MQTT Node:** `smart_io`

#### Active Modules
| Module | Interface | Purpose |
|--------|-----------|---------|
| CNX200 | UART | Cornetix IP500 protocol |
| Fan Click | I²C | PWM motor control with tachometer |
| Multimeter Click | SPI | Voltage/current measurement |
| Relay 2 Click | GPIO | Dual relay for load switching |
| LAN8651 | SPI | 10BASE-T1S Ethernet PHY |

#### Purpose
**Industrial automation I/O node**. Provides actuator control (fan, relays) and electrical measurements. Use case: **HVAC control, load switching, power monitoring**.

---

### 4. Smart Sensor Board
**Project:** `AIoT_Smart_Sensor_SAME51.X`
**MCU:** ATSAME51J20A
**IP Address:** 192.168.0.100
**MQTT Node:** `smart_sensor`

#### Active Modules
| Module | Interface | Purpose |
|--------|-----------|---------|
| APC1 | I²C | Air Quality sensor |
| BME280 | I²C | Environmental sensor |
| CNX200 | UART | Cornetix IP500 protocol |
| OLED-C | SPI | Local display |
| RE46C42X | SPI | Photoelectric smoke detector |
| Sensirion SEN5x | I²C | Multi-sensor (PM, VOC, NOx) |
| LAN8651 | SPI | 10BASE-T1S Ethernet PHY |

#### Purpose
**Primary sensor aggregation node** with safety monitoring. Dedicated to environmental sensing and smoke detection with local display. Critical for **safety-critical applications**.

---

### 5. Test Me Board
**Project:** `AIoT_TestMe_SAMD21.X`
**MCU:** ATSAMD21G18A (Cortex-M0+)
**IP Address:** 192.168.0.103
**MQTT Node:** `testme`

#### Active Modules
| Module | Interface | Purpose |
|--------|-----------|---------|
| CNX200 | UART | Cornetix IP500 protocol |

#### Purpose
**Lightweight test/development board** with minimal configuration. Used for prototyping network communication without the full sensor suite. Lower-cost entry point for development.

---

### 6. Smart Motor Board
**Project:** `predictive_maintenance_mc/`
**MCU:** ATSAME51J20A (assumed)
**IP Address:** 192.168.0.105
**MQTT Node:** `smart_motor` (inferred)

#### Active Modules
| Module | Interface | Purpose |
|--------|-----------|---------|
| Motor Control | PWM/ADC | Variable speed motor control |
| Current Sensing | ADC | Motor current monitoring |
| LAN8651 | SPI | 10BASE-T1S Ethernet PHY |

#### Purpose
**Predictive maintenance** demonstration. Monitors motor health parameters (vibration, current, speed, temperature) and uses AI/ML algorithms to predict failures before they occur.

---

## Network Configuration

### IP Address Map
| Board Type | IP Address | MQTT Index | Enum Value |
|------------|------------|------------|------------|
| Smart Sensor | 192.168.0.100 | 0 | SMART_SENSOR_BOARD |
| Smart HMI | 192.168.0.101 | 1 | SMART_HMI_BOARD |
| Smart I/O | 192.168.0.102 | 2 | SMART_IO_BOARD |
| Test Me | 192.168.0.103 | 3 | TEST_ME_BOARD |
| Demo | 192.168.0.104 | 4 | DEMO_BOARD |
| Smart Motor | 192.168.0.105 | 5 | SMART_MOTOR_BOARD |

**Network Gateway:** 192.168.0.5
**Subnet:** 192.168.0.0/24

The IP addresses are automatically assigned based on the `BOARD_TYPE` enumeration defined in `src/board_types.h`. Each board's IP is calculated as: `192.168.0.100 + BOARD_TYPE`.

---

## Software Architecture

### Task Scheduler
Each board runs a **cooperative multitasking system** with 1ms timer ticks.

#### Registered Tasks (Example - Demo Board)
| Task | Interval | Function |
|------|----------|----------|
| `read_sensor_data` | 2000ms | Poll all sensors |
| `publish_sensor_data` | 1000ms | Publish to MQTT |
| `print_oled_data` | 1000ms | Update display |
| `publish_csv_data` | 1000ms | Publish CSV format |
| `fast_sensor_read` | 300ms | High-speed sensor polling |
| `blink_online_led` | 1000ms | Status indicator |
| `read_topics` | 1000ms | Check MQTT subscriptions |

### Task Registration
```c
// Example: Execute doSomething immediately, repeat every 50ms
Task_register(0, 50, doSomething);

// Execute after 100ms delay, repeat every 200ms
Task_register(100, 200, doSomething);

// Execute once after 200ms (one-shot)
Task_register(200, 0, doSomething);
```

### Hardware Abstraction Layer (HAL)
Each board has a dedicated HAL directory (`src/hal/<board_name>/`) containing:
- `hal.h` - Board-specific definitions and module enable/disable flags
- `*_hal.c/h` - Module-specific hardware interface implementations

This allows the **same application code** to run on different hardware configurations by simply changing compile-time defines.

---

## MQTT Topic Structure

### Topic Format
```
<nodename>/<topic_name>
```

### Examples
```
demo/temperature          → Temperature reading from demo board
smart_sensor/smoke        → Smoke detector status
smart_hmi/button_state    → Touch button state
smart_io/relay_status     → Relay on/off state
smart_io/fan_speed        → Fan RPM
curiosity/mcu_uid         → MCU unique identifier
curiosity/firmware        → Firmware version & build date
```

### Topic Types
1. **Publish Topics** - Sensor data, status updates (auto-published periodically)
2. **Subscribe Topics** - Commands, control signals (callback-based)
3. **System Topics** - MCU UID, firmware version (published once at startup)

### Topic Structure
```c
typedef struct {
    char*   nodename;                      // Node identifier
    char*   topic_name;                    // Topic name
    void    (*get_str_value)(char *data);  // Get data callback
    void    (*sub_callback)(char *data);   // Subscribe callback
    topic_t topic_nr;                      // CNX200 topic number
    bool    autoPublish;                   // Auto-publish flag
} TopicItem;
```

---

## Getting Started

### Prerequisites
- **MPLAB X IDE** (v6.0 or later)
- **XC32 Compiler** (v4.0 or later)
- **MPLAB Code Configurator (MCC)** for peripheral configuration
- **Hardware:** One or more AIoT Blue Wedge boards
- **Network:** 10BASE-T1S gateway/switch

### Building a Project
1. Open the desired `.X` project in MPLAB X IDE
2. Select the configuration matching your board
3. Build the project: `Production → Build Main Project`
4. Program the device: `Production → Make and Program Device`

### Project Configurations
Each project contains multiple configurations in `nbproject/configurations.xml`:
- `same51_demo`
- `same51_thermostat` (Smart HMI)
- `same51_ioboard` (Smart I/O)
- `same51_airquality` (Smart Sensor)
- `samd21_testme`

### Serial Terminal Setup
- **Baud Rate:** 115200
- **Data Bits:** 8
- **Parity:** None
- **Stop Bits:** 1

---

## Recent Updates

Based on recent git commits:

- ✅ **Feature auto update** - Automatic firmware update mechanism
- ✅ **New MQTT topics naming** - Improved topic structure and naming conventions
- ✅ **Motor Control integration** - Added motor control to AIoT platform
- ✅ **AIML variable updates** - Enhanced AI/ML integration for predictive features
- ✅ **Fan MQTT control** - Remote fan control via MQTT messages
- ✅ **Persistent variables** - Non-volatile storage for configuration

---

## Project Structure

```
firmware/
├── AIoT_Demo_SAME51.X/              # Demo board project
├── AIoT_Smart_HMI_SAME51.X/         # Smart HMI project
├── AIoT_Smart_IO_SAME51.X/          # Smart I/O project
├── AIoT_Smart_Sensor_SAME51.X/      # Smart Sensor project
├── AIoT_TestMe_SAMD21.X/            # Test board project
├── predictive_maintenance_mc/       # Smart Motor project
├── libs/
│   └── liblwip.X/                   # lwIP TCP/IP stack library
└── src/                             # Shared source code
    ├── APC1/                        # Air Quality Click driver
    ├── BME280/                      # Environmental sensor driver
    ├── CAPSENSE2/                   # Capacitive touch driver
    ├── CNX200/                      # Cornetix IP500 driver
    ├── FAN/                         # Fan Click driver
    ├── MQTT/                        # MQTT topic management
    ├── MULTIMETER/                  # Multimeter Click driver
    ├── OLED/                        # OLED-C display driver
    ├── PROXIMITY3/                  # Proximity sensor driver
    ├── RE46C42X/                    # Smoke detector driver
    ├── RELAY2/                      # Relay Click driver
    ├── RNBD451/                     # BLE module driver
    ├── T1S/                         # 10BASE-T1S + lwIP integration
    ├── sensirion/                   # Sensirion sensor driver
    ├── hal/                         # Hardware Abstraction Layer
    │   ├── demo_same51/
    │   ├── smart_hmi_same51/
    │   ├── smart_io_same51/
    │   ├── smart_sensor_same51/
    │   └── testme_samd21/
    ├── config/                      # MCC-generated configurations
    ├── packs/                       # CMSIS and DFP files
    ├── app.c/h                      # Application layer
    ├── main.c                       # Main entry point
    ├── tasks.c/h                    # Task scheduler
    ├── terminal.c/h                 # Terminal interface
    └── board_types.h                # Board type definitions
```

---

## Key Features

### 1. **Distributed Sensing**
Multiple boards collect different environmental and operational data, providing comprehensive system monitoring.

### 2. **Real-time Communication**
MQTT publish/subscribe over 10BASE-T1S Ethernet enables low-latency, deterministic communication.

### 3. **Modular Design**
Hardware Abstraction Layer allows the same codebase to run on different board configurations with compile-time switches.

### 4. **Industrial Protocol Support**
CNX200/Cornetix IP500 enables integration with legacy industrial systems.

### 5. **Wireless Bridging**
BLE (RNBD451) provides wireless access to the wired T1S network for mobile devices.

### 6. **Safety Monitoring**
Integrated smoke detection with automated reaction capabilities (alarms, fan control, notifications).

### 7. **Predictive Maintenance**
Motor monitoring with AI/ML algorithms for failure prediction and preventive maintenance scheduling.

### 8. **Edge Computing**
On-device processing of sensor data with local decision-making capabilities.

---

## Use Cases

- **Smart Building Automation** - HVAC control, occupancy sensing, air quality monitoring
- **Industrial IoT** - Predictive maintenance, process monitoring, machine health
- **Environmental Monitoring** - Multi-parameter sensing with cloud connectivity
- **Safety Systems** - Smoke/fire detection with automated response
- **Energy Management** - Power monitoring, load control, efficiency optimization

---

## Support

For questions or issues:
- **Company:** Microchip Technology Inc.
- **Platform:** Blue Wedge AIoT Platform
- **Documentation:** See individual project folders for module-specific docs

---

## License

Copyright © 2018-2026 Microchip Technology Inc. and its subsidiaries.

Subject to compliance with terms, you may use Microchip software and derivatives exclusively with Microchip products. See individual source files for complete license text.

---

**Last Updated:** March 2026
