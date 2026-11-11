![Microchip Logo](assets/logo.png "Microchip Technology")

# AIoT Blue Wedge Platform

**Adaptive IoT reference design for smart building automation over 10BASE-T1S**

![Status](https://img.shields.io/badge/status-active-success)
![IDE](https://img.shields.io/badge/IDE-VS%20Code%20%2B%20MPLAB-blue)
![Platform](https://img.shields.io/badge/MCU-SAME51%20%7C%20PIC32CK%20%7C%20SAMD21%20%7C%20SAME54%20%7C%20dsPIC33AK-lightgrey)

> The legacy MPLAB X / IP500 (CNX200) version of this project is kept on the
> `MCHP-IP500-GitHub` remote (tag `v1.1`). This README describes the current
> VS Code based project.

---

## Table of Contents

- [About](#about)
- [Hardware](#hardware)
- [Repository Structure](#repository-structure)
- [Firmware Architecture](#firmware-architecture)
- [Getting Started (VS Code)](#getting-started-vs-code)
- [Network and MQTT](#network-and-mqtt)
- [Node-RED Dashboard and Raspberry Pi](#node-red-dashboard-and-raspberry-pi)
- [Further Documentation](#further-documentation)
- [Disclaimer](#disclaimer)

---

## About

This reference design showcases an Adaptive Internet of Things (AIoT) platform
for building automation. Multiple boards with sensors and actuators are
connected over **10BASE-T1S** Single Pair Ethernet (LAN865x MAC-PHY, lwIP,
MQTT). The T1S segments are bridged into a TSN Ethernet backbone, and a
Raspberry Pi runs the Node-RED dashboard that orchestrates the system.

| AIoT Blue Wedge | AIoT Sales Demo |
|-----------------|-----------------|
| <img src="docs/images/BlueWedge3.png" alt="AIoT Blue Wedge" width="90%"> | <img src="docs/images/sales-demo.png" alt="AIoT Sales Demo" width="60%"> |

The system consists of:

- Curiosity Nano boards, each with a dedicated role:
  - **Smart Sensor** – air quality / weather
  - **Smart HMI** – touch, OLED and BLE user interface (thermostat)
  - **Smart I/O** – actuators (relay, fan)
  - **Demo** – sales demo board
- **Smart Motor** board (motor control / predictive maintenance; SAM E54,
  to be replaced by dsPIC33AK), connected externally over a T1S adapter
- **SAMA7D65** edge gateway with display
  ([SAMA7D65 Curiosity Kit, EV63J76A](https://www.microchip.com/en-us/development-tool/ev63j76a)),
  bridging T1S network 1
- **LAN9662** 4-Port TSN Gigabit Ethernet Switch with End-Point (T1S network 2)
- **PD-IES008** Industrial TSN Ethernet Switch (Ethernet backbone)
- **Raspberry Pi** with 7" touch display running the Node-RED dashboard

> The **Test Me!** board (SAM-IoT WG, `AIoT_TestMe_SAMD21`) is still part of
> the software but is **not available on the current hardware**.

---

## Hardware

### Supported MCUs

| MCU | Board | Boards / roles |
|-----|-------|----------------|
| ATSAME51J20A | SAM E51 Curiosity Nano [(EV76S68A)](https://www.microchip.com/en-us/development-tool/ev76s68a) | Demo, Smart Sensor, Smart HMI, Smart I/O |
| PIC32CK2051SG01064 | PIC32CK SG01 Curiosity Nano (EV33V87A) | Demo, Smart Sensor, Smart HMI, Smart I/O |
| ATSAMD21G18A | SAM-IoT WG [(EV75S95A)](https://www.microchip.com/en-us/development-tool/ev75s95a) | Test Me! (software only, not on current hardware) |
| ATSAME54P20A | SAM E54 based motor control board | Smart Motor (current) |
| dsPIC33AK512MC510 | dsPIC33AK motor control board | Smart Motor (planned, replaces SAM E54 – branch `add_dspic33ak`) |

The Curiosity Nano boards are mounted on a
[Curiosity Nano Base for Click boards (AC164162)](https://www.microchip.com/en-us/development-tool/AC164162).
Every networked board uses a
[Two-Wire ETH Click (MIKROE-5543)](https://www.mikroe.com/two-wire-eth-click)
for 10BASE-T1S.

### Smart Sensor Board (air quality)

<img src="docs/images/AirQuality.jpg" alt="Smart Sensor Board" width="20%">

Sensors are detected at start-up; only the topics of detected sensors are published.

| Module | Description |
|--------|-------------|
| [HVAC Click (MIKROE-4290)](https://www.mikroe.com/hvac-click) | Sensirion SCD41 – CO2, temperature, humidity |
| [Sensirion SEN5x (SEN55)](https://sensirion.com/products/catalog/SEK-SEN5x) | PM1/2.5/4/10, VOC, NOx, temperature, humidity |
| MIKROE-5908 (APC1) | Temperature, humidity, VOC, CO2, AQI, PM |
| MIKROE-5761 (BME280) | Temperature, humidity, pressure |
| [OLED C Click (MIKROE-1585)](https://www.mikroe.com/oled-c-click) | Status display / alarm |

> **Smoke detector:** the RE46C42X smoke detector driver
> (`firmware/src/RE46C42X/`) is still part of the software, but the smoke
> detector is **not fitted on the current AIoT Blue Wedge hardware**.

### Smart HMI Board (thermostat)

<img src="docs/images/Thermostat.jpg" alt="Smart HMI Board" width="20%">

| Module | Description |
|--------|-------------|
| [CapSense 2 Click (MIKROE-4982)](https://www.mikroe.com/capsense-2-click) | Touch buttons and slider |
| [OLED C Click (MIKROE-1585)](https://www.mikroe.com/oled-c-click) | Display |
| [RNBD451 Add-On (EV25F14A)](https://www.microchip.com/en-us/development-tool/ev25f14a) | Bluetooth Low Energy bridge |

### Smart I/O Board (actuator)

<img src="docs/images/IOBoard.jpg" alt="Smart I/O Board" width="20%">

| Module | Description |
|--------|-------------|
| [Relay 2 Click (MIKROE-1899)](https://www.mikroe.com/relay-2-click) | Two relay channels (e.g. lamp) |
| [Fan Click (MIKROE-2004)](https://www.mikroe.com/fan-click) | 5 V PWM fan, RPM and stall detection |
| [Multimeter Click (MIKROE-3116)](https://www.mikroe.com/multimeter-click) | Electrical measurements |

Use the board pictures above to plug the Click boards into the correct sockets.

### Smart Motor Board

Motor control / predictive maintenance board
(`firmware/predictive_maintenance_mc/`). It is not mounted on the wedge; it is
connected externally over a T1S adapter to **T1S network 1** (Smart Sensor –
SAMA7D65).

> The current implementation runs on the **ATSAME54P20A**. It will be replaced
> by a **dsPIC33AK512MC510** implementation, which is being developed on branch
> `add_dspic33ak` and merged from there.

### Edge Gateway and Network Infrastructure

| Device | Part | Role |
|--------|------|------|
| SAMA7D65 | [SAMA7D65 Curiosity Kit (EV63J76A)](https://www.microchip.com/en-us/development-tool/ev63j76a) | Edge gateway with display, T1S network 1 bridge |
| LAN9662 | LAN9662 4-Port TSN Gigabit Ethernet Switch with End-Point | T1S network 2 bridge |
| PD-IES008 | Industrial TSN Ethernet Switch | Ethernet backbone |
| Raspberry Pi | Raspberry Pi with 7" touch display | Node-RED dashboard |

### Network Topology

The boards are split into two 10BASE-T1S multidrop segments, which are bridged
to a common Ethernet backbone by the SAMA7D65 and the LAN9662. The backbone is
the PD-IES008 Industrial TSN Ethernet Switch, which also connects the
Raspberry Pi.

```
                    ┌──────────────────────────────────────────┐
                    │ PD-IES008 Industrial TSN Ethernet Switch │
                    └──────┬──────────────────┬──────────────┬─┘
                           │ Ethernet         │ Ethernet     │ Ethernet
                ┌──────────┴─────┐   ┌────────┴─────────┐  ┌─┴────────────┐
                │ SAMA7D65       │   │ LAN9662 4-Port   │  │ Raspberry Pi │
                │ Edge Gateway   │   │ TSN GbE Switch   │  │ Node-RED     │
                └──────────┬─────┘   └────────┬─────────┘  └──────────────┘
                           │ 10BASE-T1S       │ 10BASE-T1S
           T1S network 1   │                  │   T1S network 2
        ┌──────────────────┴──┐            ┌──┴──────────────────┐
        │                     │            │                     │
 ┌──────┴───────┐   ┌─────────┴────┐  ┌────┴─────────┐   ┌───────┴──────┐
 │ Smart Sensor │   │ Smart Motor  │  │ Smart I/O    │   │ Smart HMI    │
 │              │   │ (external,   │  │              │   │              │
 │              │   │  T1S adapter)│  │              │   │              │
 └──────────────┘   └──────────────┘  └──────────────┘   └──────────────┘
```

| Segment | Bridge | Nodes |
|---------|--------|-------|
| T1S network 1 | SAMA7D65 | Smart Sensor, Smart Motor (external via T1S adapter) |
| T1S network 2 | LAN9662 | Smart I/O, Smart HMI |
| Ethernet backbone | PD-IES008 | SAMA7D65, LAN9662, Raspberry Pi |

---

## Repository Structure

```
aiot-bluewedge/
├── .vscode/                      # VS Code workspace
│   ├── AIoT_Demo.mplab.json          # MPLAB project manifests, one per board,
│   ├── AIoT_Smart_Sensor.mplab.json  # each with one configuration per MCU
│   ├── AIoT_Smart_HMI.mplab.json
│   ├── AIoT_Smart_IO.mplab.json
│   ├── AIoT_TestMe_SAMD21.mplab.json
│   ├── AIoT_Smart_Motor_same54.mplab.json
│   ├── AIoT_PIC32CK_Secure.mplab.json  # shared PIC32CK secure image
│   ├── liblwip.mplab.json        # library projects
│   ├── libtc6.mplab.json
│   ├── launch.json               # debug / run configurations
│   └── settings.json
├── harmony/                      # Harmony (MCC) projects, one per MCU architecture
│   ├── AIoT_SAME51/
│   ├── AIoT_SAMD21/
│   └── AIoT_PIC32CK/             # secure/ + non_secure/
├── firmware/
│   ├── src/                      # shared application source
│   │   ├── main.c, app.c, tasks.c, terminal.c, board_types.h
│   │   ├── hal/<board>_<mcu>/    # board specific HAL (one folder per configuration)
│   │   ├── MQTT/                 # topic abstraction layer
│   │   ├── T1S/                  # LAN865x / TC6 + lwIP glue
│   │   └── APC1/ BME280/ CAPSENSE2/ FAN/ MULTIMETER/ OLED/
│   │       PROXIMITY3/ RE46C42X/ RELAY2/ RNBD451/ sensirion/ ...
│   ├── libs/                     # liblwip.X, libtc6.X
│   └── predictive_maintenance_mc/  # Smart Motor (SAM E54) project
├── aiot-rp-img/                  # submodule: Raspberry Pi image / overlay
├── flows.json, package.json      # Node-RED project (dashboard)
├── docs/                         # documentation and images
└── assets/
```

`cmake/`, `_build/` and `out/` are generated by the MPLAB VS Code extension and
are not meant to be edited (except `cmake/**/user.cmake`, see the
[PIC32CK guide](docs/PIC32CK_Programming_Guide.md)).

---

## Firmware Architecture

### Shared Harmony project per MCU architecture

The `harmony/` folder contains **one Harmony project per MCU architecture**.
That Harmony project is **shared by all MPLAB projects** that run on this
architecture:

| Harmony project | Used by |
|-----------------|---------|
| `harmony/AIoT_SAME51`  | `demo_same51`, `smart_sensor_same51`, `smart_hmi_same51`, `smart_io_same51` |
| `harmony/AIoT_PIC32CK` | `pic32ck_secure`, `demo_pic32ck`, `smart_sensor_pic32ck`, `smart_hmi_pic32ck`, `smart_io_pic32ck` |
| `harmony/AIoT_SAMD21`  | `testme_samd21` |

### Common software, portable hardware layer

All boards and all architectures share **one common application and driver
code base** in `firmware/src/`. Harmony provides a **general hardware
abstraction layer** for each MCU architecture (SERCOM I2C/SPI/UART, EIC, ADC,
STDIO, ...). The common software is connected to it through **module-specific
HAL files**, one set per board configuration:

<img src="docs/images/HAL_DevTools.png" alt="Harmony and HAL" width="60%">

| Layer | Location | Content |
|-------|----------|---------|
| Application and module drivers | `firmware/src/`, `firmware/src/<MODULE>/` | Common, hardware independent code (app, MQTT, T1S, BME280, SEN5x, OLED, ...) |
| Module HAL files | `firmware/src/hal/<board>_<mcu>/` | One HAL file per module (e.g. `bme280_hal.c`, `oled_spi_hal.c`, `t1s_lwip_hal.c`) mapping the driver to the Harmony peripheral, plus `hal.h` with `BOARD_TYPE`, `BOARD_NODE_NAME`, pins and buses |
| General hardware abstraction | `harmony/AIoT_<MCU>/` | Harmony generated peripheral libraries, shared by all boards of the architecture |

Module drivers never call Harmony directly; they only use their HAL file.
This makes the software very flexible and portable:

- **New architecture:** create a Harmony project in `harmony/` and a set of
  HAL files – the application and module drivers stay unchanged. This is how
  the PIC32CK port was done next to SAM E51.
- **New board / role:** add a HAL folder and a configuration that selects the
  modules it uses.
- **New module:** add a driver in `firmware/src/<MODULE>/` and its HAL file per
  board (see `firmware/src/TEMPLATE_MODULEXYZ/`).

Note that because the Harmony configuration is shared, a change there affects
every board of that architecture, and pins cannot be configured differently per
board. Details, component list and example screenshots (SAM E51):
**[docs/harmony_setup.md](docs/harmony_setup.md)**.

### MPLAB projects and configurations

| Project (`.vscode/*.mplab.json`) | Configurations | Output |
|----------------------------------|----------------|--------|
| `AIoT_Demo` | `demo_pic32ck`, `demo_same51` | `out/AIoT_Demo/` |
| `AIoT_Smart_Sensor` | `smart_sensor_pic32ck`, `smart_sensor_same51` | `out/AIoT_Smart_Sensor/` |
| `AIoT_Smart_HMI` | `smart_hmi_pic32ck`, `smart_hmi_same51` | `out/AIoT_Smart_HMI/` |
| `AIoT_Smart_IO` | `smart_io_pic32ck`, `smart_io_same51` | `out/AIoT_Smart_IO/` |
| `AIoT_TestMe_SAMD21` | `testme_samd21` | `out/AIoT_TestMe_SAMD21/` |
| `AIoT_Smart_Motor_same54` | `smart_mc_same54` (to be replaced by dsPIC33AK512MC510) | `out/AIoT_Smart_Motor_SAME54/` |
| `AIoT_PIC32CK_Secure` | `pic32ck_secure` | `out/AIoT_PIC32CK_Secure/` |
| `liblwip`, `libtc6` | per-MCU `debug_*` / `release_*` | static libraries |

Switching a board between SAM E51 and PIC32CK is a configuration change in the
same project, not a different project.

---

## Getting Started (VS Code)

### Prerequisites

| Tool | Version | Notes |
|------|---------|-------|
| Visual Studio Code | latest | |
| MPLAB Extension Pack for VS Code | latest | Generates CMake/Ninja, build, debug, MCC |
| MPLAB XC32 | v5.00 | |
| MPLAB X IDE | v6.30 | Only needed for `mdb` / IPE command line programming |
| Device packs | SAME51_DFP, SAMD21_DFP, SAME54_DFP, PIC32CK-SG_DFP, CMSIS | Downloaded by the extension |

On Windows enable long paths if you hit path length errors (see
[MPLAB_to_VSCode_Migration.md](docs/MPLAB_to_VSCode_Migration.md#44-issue-windows-path-length-limit-260-characters)).

### Clone

```bash
git clone --recurse-submodules ssh://git@bitbucket.microchip.com/~m71906/aiot-bluewedge.git
cd aiot-bluewedge
```

### Build

1. Open the repository root folder in VS Code.
2. Open the **MPLAB** view and select the project and configuration
   (e.g. `AIoT_Smart_Sensor : smart_sensor_same51`).
3. Build with the MPLAB build action or `Ctrl+Shift+B`.

**PIC32CK:** build `AIoT_PIC32CK_Secure : pic32ck_secure` once first, and
always program the `*-unified.hex`. Read the
[PIC32CK Programming Guide](docs/PIC32CK_Programming_Guide.md) before flashing.

### Program and debug

Use the **Run and Debug** view. `.vscode/launch.json` provides `Debug ...` and
`Run ...` entries per configuration (debugger type `mplab-core-da`), each
building the configuration first.

### Serial terminal

Connect to the Curiosity Nano virtual COM port with **115200 8N1** before
resetting the board. Commands (terminate with newline):

| Key | Function |
|-----|----------|
| `w` | Print sensor readings |
| `v` | Toggle CSV output of published values |
| `c` | Redraw header |
| `d` | Debug mode |
| `b` | Bridge mode to the COMM module |

<img src="docs/images/terminal.png" alt="Terminal" width="100%">

---

## Network and MQTT

### IP addresses

Addresses are derived from `BOARD_TYPE` (`firmware/src/board_types.h`):

| Board | `BOARD_TYPE` | IP address | MQTT node name |
|-------|--------------|------------|----------------|
| Smart Sensor | `SMART_SENSOR_BOARD` | 192.168.0.100 | `smart_sensor` |
| Smart HMI | `SMART_HMI_BOARD` | 192.168.0.101 | `smart_hmi` |
| Smart I/O | `SMART_IO_BOARD` | 192.168.0.102 | `smart_io` |
| Test Me! (not on current hardware) | `TEST_ME_BOARD` | 192.168.0.103 | `testme` |
| Demo | `DEMO_BOARD` | 192.168.0.104 | `demo` |
| Smart Motor | `SMART_MOTOR_BOARD` | 192.168.0.105 | – |
| Gateway (SAMA7D65 / LAN9662) | – | 192.168.0.5 | – |

The gateway is always at **192.168.0.5**: the SAMA7D65 for T1S network 1 and
the LAN9662 TSN switch for T1S network 2 both use this address as the entry
point of their T1S segment. The firmware also uses 192.168.0.5 as the MQTT
broker address (`firmware/src/MQTT/mqtt_topics.c`).

### Topic structure

Topics are built as `<board>/<module>/<parameter>`, e.g.
`smart_sensor/mikroe4290/CO2`. Each board registers its topics in
`firmware/src/hal/<board>_<mcu>/mqtt_topics_hal.c`; the common layer is in
`firmware/src/MQTT/`.

<img src="docs/images/TopicHAL.png" alt="Topic Abstraction Layer" width="100%">

**All boards**

| Topic | Dir | Description |
|-------|-----|-------------|
| `<board>/curiosity/sw0` | UP | On-board button |
| `<board>/curiosity/led0` | DOWN | On-board LED |
| `<board>/curiosity/mcu_uid` | UP | MCU unique ID |
| `<board>/curiosity/firmware` | UP | Firmware revision |

**Smart Sensor / Demo** (`smart_sensor`, `demo`) – published only if the sensor is detected

| Module | Parameters | Dir |
|--------|------------|-----|
| `mikroe4290` (SCD41) | `CO2`, `temperature`, `moisture` | UP |
| `sen5x` | `temperature`, `moisture`, `voc`, `nox`, `pm1`, `pm25`, `pm4`, `pm10` | UP |
| `mikroe5908` (APC1) | `temperature`, `moisture`, `voc`, `co2`, `aqi`, `pm1`, `pm25`, `pm10` | UP |
| `mikroe5761` (BME280) | `temperature`, `moisture`, `pressure` | UP |
| `mikroe2801` (Demo only) | `distance`, `light` | UP |
| `mikroe5545` (Smart Sensor) | `alarm` | DOWN |

**Smart HMI** (`smart_hmi`)

| Module | Parameters | Dir |
|--------|------------|-----|
| `mikroe4982` (CapSense 2) | `s1`, `s2`, `level` | UP |
| `mikroe5545` | `oled` | DOWN |
| `rnbd451` | `mcu2bt` | DOWN |
| `rnbd451` | `bt2mcu` | UP |

**Smart I/O** (`smart_io`)

| Module | Parameters | Dir |
|--------|------------|-----|
| `mikroe2004` (Fan) | `state`, `speed` | DOWN |
| `mikroe2004` (Fan) | `stall`, `rpm` | UP |
| `mikroe1899` (Relay 2) | `ch1`, `ch2` | DOWN |

Boolean payloads accept `true`/`false` (or `ff`/`00`).

### Testing with Mosquitto

```bash
mosquitto_sub -h 192.168.0.5 -t "#" -v
mosquitto_pub -h 192.168.0.5 -t "smart_io/mikroe1899/ch1" -m "true"
```

---

## Node-RED Dashboard and Raspberry Pi

The repository root is a **Node-RED project**: `flows.json` contains the
dashboard flow and `package.json` lists its dependencies
(`@flowfuse/node-red-dashboard`, `node-red-contrib-ui-led`, ...). Open it with
Node-RED in Projects mode, or import `flows.json` manually.

<img src="docs/images/nodered_dashboard.png" alt="Node-RED Dashboard" width="30%">

The Raspberry Pi image (Mosquitto, Node-RED, T1S driver, web UI) is maintained
in the **`aiot-rp-img`** submodule. See
[aiot-rp-img/README.md](aiot-rp-img/README.md) for building and updating the
image.

<img src="docs/images/raspberry_touch.png" alt="Raspberry Pi with touch display" width="30%">

---

## Further Documentation

| Document | Content |
|----------|---------|
| [docs/harmony_setup.md](docs/harmony_setup.md) | Harmony/MCC organization and configuration screenshots |
| [docs/PIC32CK_Programming_Guide.md](docs/PIC32CK_Programming_Guide.md) | PIC32CK TrustZone build, flashing, debugging, pin map |
| [docs/MPLAB_to_VSCode_Migration.md](docs/MPLAB_to_VSCode_Migration.md) | History of the MPLAB X to VS Code migration |
| [firmware/src/sensirion](firmware/src/sensirion) | Sensirion driver notes |
| [firmware/src/OLED](firmware/src/OLED) | OLED C driver notes (MISO pin known issue) |

---

## Disclaimer

This project is a proof-of-concept:

- No industrial-ready software driver quality
- Simple error handling, not industry safe
- Read the Microchip Disclaimer [here](./docs/disclaimer.txt)

---

Copyright (c) Microchip Technology Inc. and its subsidiaries.
