# Electronic Load XY-FZ25 ESP32C3

A custom PCB and firmware add-on for the **XY-FZ25** electronic load, replacing the built-in measurement front-end with a more accurate solution based on the **Seeed XIAO ESP32C3** and an **INA228** high-precision power monitor.

## Overview

The XY-FZ25 is a low-cost programmable DC electronic load. This project adds a small companion board that sits alongside it and provides independent, higher-accuracy measurements of voltage, current, and power, plus a small OLED display and a self-contained LiFePO4 battery so the measurement circuit is fully isolated from the load's power rails.

### Key features

- **INA228** — 20-bit delta-sigma ADC for bus voltage, current, power, and die temperature
- **SH1106 128×64 OLED** — live display of current (mA), bus voltage (V), and die temperature (°C)
- **LiFePO4 cell** — galvanically isolated measurement supply; charged via the XIAO's USB-C port
- **Seeed XIAO ESP32C3** — compact, USB-C, Wi-Fi/BLE capable (Wi-Fi/BLE not used yet)
- Serial debug output at 115200 baud

## Repository structure

```
electronic-load-xy-fz25_esp32c3/
├── hardware/          # KiCad 7 project (schematic + PCB)
│   ├── *.kicad_pro
│   ├── *.kicad_sch
│   └── *.kicad_pcb
└── software/          # PlatformIO firmware (Arduino / ESP-IDF)
    ├── platformio.ini
    └── src/
        └── main.cpp
```

## Hardware

| Component | Description |
|-----------|-------------|
| Seeed XIAO ESP32C3 | MCU, USB-C charging, I²C host |
| INA228 | 20-bit power monitor, I²C addr `0x40` |
| SH1106 OLED 128×64 | I²C display, addr `0x3C` (or `0x3D`) |
| LiFePO4 cell | ~3.2 V nominal, ~3.6 V full |

The KiCad project is located in [`hardware/`](hardware/).

## Software

### Prerequisites

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- Board: **Seeed XIAO ESP32C3** (installed automatically by PlatformIO)

### Dependencies (installed automatically)

```
adafruit/Adafruit GFX Library
adafruit/Adafruit SH110X
adafruit/Adafruit INA228 Library
```

### Build & flash

```bash
# Build only
pio run

# Build and upload (USB-C)
pio run --target upload

# Open serial monitor at 115200 baud
pio device monitor
```

### Configuration

Edit the constants near the top of [`software/src/main.cpp`](software/src/main.cpp):

| Constant | Default | Purpose |
|----------|---------|---------|
| `OLED_ADDR` | `0x3C` | OLED I²C address |
| `INA_ADDR` | `0x40` | INA228 I²C address |
| `DISPLAY_UPDATE_MS` | `500` | Display refresh interval |
| Shunt resistor | `0.015 Ω` | Passed to `ina228.setShunt()` — **must match your actual shunt** |

The INA228 is configured for 256-sample averaging and 1052 µs conversion times for low-noise readings. Adjust `setAveragingCount()` and `setVoltageConversionTime()` / `setCurrentConversionTime()` in `setup()` to trade off noise vs. update rate.

## Serial output

When connected via USB, the firmware prints one line per refresh interval:

```
I=123.456 mA  Vbus=3.3012 V  Tdie=28.50 C
```

## License

MIT
