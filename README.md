# Superhet Radio ESP32 + Si5351

Firmware for a shortwave radio receiver built on ESP32. Combines a classic dual-conversion superheterodyne receiver covering LW/MW/SW bands with internet radio streaming over WiFi.

## Features

### SW/MW/LW Reception
- Si5351 clock generator provides local oscillators for a dual-conversion superhet:
  - **1st mixing** — RF signal is converted to a **10.7 MHz** first IF
  - **2nd mixing** — 10.7 MHz IF is converted to a **455 kHz** second IF
- Continuous tuning via rotary encoder with selectable step size (500 Hz, 1 kHz, 5 kHz, 100 kHz, 1 MHz)
- Coverage from LW (153 kHz) up to the 6m band (55 MHz)
- Band switching via buttons — jumps between broadcast and amateur radio bands
- S-meter using AD8307 logarithmic detector — displayed as dBm value and bar graph
- Last tuned frequency is saved to SPIFFS and restored on power-up

### Internet Radio
- MP3/AAC streaming over WiFi with I2S output to an external DAC
- Preset stations (CRo Plus, CRo Vltava, Jarviradio, Radio Devin)
- Amplifier enable/disable control — Si5351 outputs are disabled when entering internet mode

### Time
- NTP synchronization from `pool.ntp.org` at boot
- Time backed up to DS3231 RTC module
- Clock displayed on screen, updated every second

### Display
- Color TFT 320x240 (ST7789, SPI) in landscape orientation
- Shows: frequency in kHz, band name, wavelength, tuning step, S-meter, clock, WiFi status
- In internet mode — scrollable station list with selection
- Computer Modern Unicode monospace fonts

## Operating Modes

| Mode | Description |
|------|-------------|
| **LADENI** (Tuning) | Tune with encoder, switch bands with buttons |
| **PAMET** (Memory) | Preset frequencies (work in progress) |
| **INTERNET** | Browse and play internet radio stations |

Modes are cycled with a button. In INTERNET mode, Si5351 outputs are turned off and a station menu is shown.

## Hardware

| Component | Role |
|-----------|------|
| ESP32 (esp32dev) | Main MCU |
| Si5351 | Local oscillator generator (I2C, 25 MHz crystal) |
| ST7789 TFT 320x240 | Color display (SPI) |
| DS3231 | RTC module (I2C) |
| AD8307 | Logarithmic detector for S-meter |
| I2S DAC (e.g. PCM5102) | Audio output for internet radio |
| Rotary encoder | Frequency tuning |
| 4x push button | Band up/down, mode, go |

## GPIO Pinout

| GPIO | Function |
|------|----------|
| 2 | TFT DC |
| 4 | TFT RST |
| 5 | TFT CS |
| 12 | Encoder CLK |
| 13 | Encoder DT |
| 14 | Amplifier enable |
| 15 | I2S BCLK |
| 16 | I2S LRC |
| 17 | I2S DOUT |
| 18 | TFT SCLK (SPI) |
| 23 | TFT MOSI (SPI) |
| 25 | Band up button |
| 26 | Band down button |
| 27 | Mode button |
| 32 | Go button |
| 33 | ADC input (S-meter, AD8307) |
| 21/22 | I2C SDA/SCL (Si5351 + DS3231) |

## Building

The project uses [PlatformIO](https://platformio.org/) with the Arduino framework.

1. Clone the repository
2. Copy `src/secrets.h.example` to `src/secrets.h` and fill in your WiFi credentials:
   ```cpp
   #define WIFI_SSID "your_ssid"
   #define WIFI_PASSWORD "your_password"
   ```
3. Build and upload:
   ```bash
   pio run -t upload
   ```

## Dependencies

- `Adafruit ST7735 and ST7789 Library` — display driver
- `Adafruit GFX Library` — graphics primitives and fonts
- `DS3231` — RTC driver
- `NTPClient` — time synchronization
- `ESP32-audioI2S` — internet audio streaming

The Si5351 driver (Jason Milldrum, GPL v3) is included directly in the source tree.

## License

Si5351 driver: GPL v3 (see `src/si5351.cpp`)
