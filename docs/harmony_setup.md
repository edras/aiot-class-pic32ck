# Harmony / MCC Setup

[Back to README](../README.md)

This document describes how the MPLAB Harmony (MCC) configuration is organized
in this repository and how to modify and regenerate it.

---

## Concept: one Harmony project per MCU architecture

The `harmony/` folder holds **one Harmony project per MCU architecture**. Each
Harmony project is **shared by every MPLAB project (board) that runs on that
architecture**.

The application and module drivers in `firmware/src/` are **common to all
boards and architectures**. Harmony provides the **general hardware
abstraction layer** of each architecture, and the common software is connected
to it through **module-specific HAL files** in
`firmware/src/hal/<board>_<arch>/` (e.g. `bme280_hal.c`, `oled_spi_hal.c`).
Board-specific behaviour is *not* configured in Harmony; it lives in these HAL
files. Porting to a new architecture therefore only needs a new Harmony project
and a new set of HAL files, while the application code stays unchanged.

```
harmony/
├── AIoT_SAME51/                 # ATSAME51J20A  (SAM E51 Curiosity Nano)
│   ├── mcc/                     # MCC store (edit through MCC)
│   │   ├── mcc.vscode           # links this config to the VS Code projects
│   │   └── AIoT_SAME51_default/ # component settings (*.yml, mcc-config.mc4)
│   └── src/                     # generated code: config/default, packs
├── AIoT_SAMD21/                 # ATSAMD21G18A  (SAM-IoT WG, Test Me!)
│   ├── mcc/
│   └── src/
└── AIoT_PIC32CK/                # PIC32CK2051SG01064 (TrustZone-M)
    ├── mcc/
    ├── secure/                  # secure image (fuses, PORT, trampoline)
    └── non_secure/              # non-secure application config
```

| Harmony project | Device | Used by MPLAB projects (configuration) |
|-----------------|--------|-----------------------------------------|
| `AIoT_SAME51`  | ATSAME51J20A | `AIoT_Demo` (`demo_same51`), `AIoT_Smart_Sensor` (`smart_sensor_same51`), `AIoT_Smart_HMI` (`smart_hmi_same51`), `AIoT_Smart_IO` (`smart_io_same51`) |
| `AIoT_SAMD21`  | ATSAMD21G18A | `AIoT_TestMe_SAMD21` (`testme_samd21`) |
| `AIoT_PIC32CK` | PIC32CK2051SG01064 | `AIoT_PIC32CK_Secure` (`pic32ck_secure`), `AIoT_Demo` (`demo_pic32ck`), `AIoT_Smart_Sensor` (`smart_sensor_pic32ck`), `AIoT_Smart_HMI` (`smart_hmi_pic32ck`), `AIoT_Smart_IO` (`smart_io_pic32ck`) |

Each MPLAB project manifest in `.vscode/*.mplab.json` pulls in the generated
sources of the matching Harmony project
(e.g. `harmony/AIoT_SAME51/src/config/default`) and excludes the other
architectures from its file set.

> The Smart Motor board (`AIoT_Smart_Motor_same54`, ATSAME54P20A) is a
> separate motor-control project imported from MPLAB X and keeps its own
> configuration in `firmware/predictive_maintenance_mc/AIoT_Smart_Motor_same54.X/`.
> It will be replaced by a dsPIC33AK512MC510 implementation (branch `add_dspic33ak`).

### Consequences of a shared configuration

- **A change in Harmony affects every board on that architecture.** Rebuild
  and check all dependent configurations after regenerating.
- **Pins are a shared resource.** A pin cannot be an input on one board and an
  output on another. Resolve conflicts in the HAL or by choosing another pin.
  Known conflicts are listed in
  [PIC32CK_Programming_Guide.md](PIC32CK_Programming_Guide.md#9-pins-are-a-shared-finite-resource-across-the-four-boards).
- **Generated files must not be edited alone.** A change made only in a
  generated `.c`/`.h` is lost on the next regeneration. Change it in MCC (or in
  the `.yml` store *and* the generated file).

---

## Components per architecture

| Component | AIoT_SAME51 | AIoT_SAMD21 | AIoT_PIC32CK |
|-----------|:-----------:|:-----------:|:------------:|
| SERCOM0 | x | x | x |
| SERCOM1 | x |   | x |
| SERCOM2 | x | x | x |
| SERCOM3 |   | x |   |
| SERCOM4 | x | x | x |
| SERCOM5 | x | x | x |
| ADC1    | x |   |   |
| EIC     | x | x | x |
| EVSYS   | x | x | x |
| NVMCTRL | x | x |   |
| PM      |   |   | x |
| STDIO   | x | x | x |

The ADC is intentionally not part of the PIC32CK non-secure configuration (see
[PIC32CK_Programming_Guide.md](PIC32CK_Programming_Guide.md#3-the-adc-cannot-be-used-from-non-secure-code)).

---

## Modifying the configuration in VS Code

1. Open the repository root in VS Code with the MPLAB extensions installed.
2. Launch **MCC** from the MPLAB extension and open the Harmony project of the
   architecture you want to change (`harmony/AIoT_<ARCH>/mcc`).
3. Apply the changes in the Project Graph / Pin Configuration.
4. Click **Generate**. Output is written to `harmony/AIoT_<ARCH>/src/config/default`
   (for PIC32CK: `secure/` and `non_secure/`).
5. Rebuild **all** MPLAB configurations that use this architecture.
6. If MCC added new source files, make sure they are listed in the file sets of
   the affected `.vscode/*.mplab.json` manifests.

---

## Example: SAM E51 configuration

> **Note:** the screenshots below show the `AIoT_SAME51` configuration
> (ATSAME51J20A) only. They are an **example** of how a Harmony project in
> this repository is set up. No screenshots are planned for `AIoT_SAMD21` or
> `AIoT_PIC32CK`. For those architectures, check the configuration directly in
> MCC.

### Project Graph

Components added to the project:

- Harmony > Peripherals > SERCOM > SERCOM0, 1, 2, 4 and 5
- Harmony > Peripherals > ADC > ADC1
- Harmony > Peripherals > EIC > EIC
- Harmony > Tools > STDIO

STDIO is connected to the SERCOM5 UART output (debug terminal, 115200 8N1).

<img src="images/ProjectGraph.png" alt="Harmony Project Graph" width="40%">

### Component settings

| SERCOM0 | SERCOM1 |
|---------|---------|
| <img src="images/SERCOM0.png" alt="SERCOM0" width="90%"> | <img src="images/SERCOM1.png" alt="SERCOM1" width="90%"> |

| SERCOM2 | SERCOM4 |
|---------|---------|
| <img src="images/SERCOM2.png" alt="SERCOM2" width="90%"> | <img src="images/SERCOM4.png" alt="SERCOM4" width="90%"> |

| SERCOM5 | STDIO |
|---------|-------|
| <img src="images/SERCOM5.png" alt="SERCOM5" width="90%"> | <img src="images/STDIO.png" alt="STDIO" width="60%"> |

| ADC1 | EIC |
|------|-----|
| <img src="images/ADC1.png" alt="ADC1" width="90%"> | <img src="images/EIC.png" alt="EIC" width="90%"> |

**System**

<img src="images/System.png" alt="System" width="30%">

### Pin configuration

Open the Pin Configuration from the Project Graph:

<img src="images/PinConfig.png" alt="Pin Configuration" width="25%">

and apply the following pin settings:

<img src="images/PIN1.png" alt="Pin configuration 1" width="70%">

<img src="images/PIN2.png" alt="Pin configuration 2" width="70%">

### Generate

<img src="images/Generate.png" alt="Generate" width="30%">

---

For the PIC32CK (TrustZone) configuration, pin map and SERCOM renumbering see
[PIC32CK_Programming_Guide.md](PIC32CK_Programming_Guide.md#board-pin-reference).
