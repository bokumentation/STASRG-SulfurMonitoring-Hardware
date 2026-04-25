# ESP-IDF Components Collection

A curated collection of Arduino-compatible libraries ported for the ESP-IDF environment for my projects.

### Features

- Decoupled Components: Libraries are stripped of original Git metadata to avoid submodule conflicts.
- Clean Workspace: Unnecessary files (examples, docs, tests) are removed via automation scripts to keep the project lightweight.
- Portability: Includes custom CMakeLists.txt (or porting logic) to bridge Arduino-dependent code to the ESP32 ESP-IDF toolchain.

## Included Components

| Component | Original Author / Source | License |
| :--- | :--- | :--- |
| **Adafruit_BME280** | [Adafruit](https://github.com/adafruit/Adafruit_BME280_Library) | MIT |
| **Adafruit_BusIO** | [Adafruit](https://github.com/adafruit/Adafruit_BusIO) | MIT |
| **Adafruit_INA219** | [Adafruit](https://github.com/adafruit/Adafruit_INA219) | MIT |
| **Adafruit_Sensor** | [Adafruit](https://github.com/adafruit/Adafruit_Sensor) | Apache 2.0 |
| **Adafruit_SSD1306** | [Adafruit](https://github.com/adafruit/Adafruit_SSD1306) | BSD |
| **Adafruit-GFX-Library** | [Adafruit](https://github.com/adafruit/Adafruit-GFX-Library) | BSD |
| **espsoftwareserial** | [Peter Lerup](https://github.com/plerup/espsoftwareserial) | LGPL-2.1 |
| **RadioLib** | [Jan Gromes](https://github.com/jgromes/RadioLib) | MIT |
| **RTClib** | [Adafruit](https://github.com/adafruit/RTClib) | MIT |
| **TinyGPSPlus** | [Mikal Hart](https://github.com/mikalhart/TinyGPSPlus) | MIT |
| **PicoMQTT** | [Marek Lesniewski](https://github.com/mlesniew/PicoMQTT) | MIT |

## Licensing

- Repository Logic: The porting scripts and configuration files are licensed under the MIT License.
- Components: Each library inside this folder remains the property of its original author. Original LICENSE files are preserved within each component subdirectory to ensure compliance and attribution.
