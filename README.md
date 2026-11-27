# ble-sensor-node

Battery-powered BLE environmental sensor node on the nRF54L15 DK, built with Zephyr RTOS (nRF Connect SDK).

## Goal

Read temperature and humidity from an SHT30 over I2C using a driver written from the datasheet, and expose the data over BLE through a custom GATT service with notifications.

## Hardware

- Board: nRF54L15 DK (PCA10156 v1.0.0), build target `nrf54l15dk/nrf54l15/cpuapp`
- Sensor: SHT30 breakout, I2C address `<0x44>`
- Wiring: SCL on P1.11, SDA on P1.12 (`i2c22`), VCC on VDD:IO, GND on GND
- VDD set to 3.3 V in nRF Connect Board Configurator

## Software

- nRF Connect SDK v3.4.2 LTS, Zephyr RTOS
- VS Code with the nRF Connect extension
- Custom devicetree binding for the sensor (`dts/bindings/nyxano,sht30.yaml`)
- Own CRC-8 implementation (polynomial 0x31, init 0xFF), verified against the datasheet test vector (0xBEEF gives 0x92)
- Measurement loop: one reading per second, integer math (values in hundredths of °C and %RH), frame rejected if CRC does not match
## Project layout

| Path | Purpose |
|---|---|
| `src/main.c` | Application code |
| `prj.conf` | Kconfig options (e.g. `CONFIG_I2C=y`) |
| `boards/nrf54l15dk_nrf54l15_cpuapp.overlay` | Devicetree overlay: LED alias, `i2c22` pins, SHT30 node |
| `dts/bindings/nyxano,sht30.yaml` | Devicetree binding for the SHT30 |

## Build and flash

1. Open the project in VS Code with the nRF Connect extension and SDK v3.4.2.
2. Build configuration: board target `nrf54l15dk/nrf54l15/cpuapp`, build directory `build`.
3. Build and flash. Console output is on the second virtual serial port of the DK (115200 baud).

## Status

- [x] Environment, build and flash (blinky)
- [x] Devicetree overlay: `led0` alias redirected to `led1`
- [x] SHT30 node on `i2c22` with own binding, ACK confirmed (soft reset)
- [x] CRC-8 with datasheet test vector
- [x] Temperature and humidity read (single shot, CRC check, conversion)
- [ ] Sensor code split into `sht30.c` / `sht30.h`
- [ ] Unit tests (Twister / ZTest)
- [ ] BLE advertising and custom GATT service

## Notes

- Zephyr code finds hardware through devicetree aliases (e.g. `DT_ALIAS(led0)`), while a node label (e.g. `led1`) names one specific node. My overlay points the `led0` alias at the `led1` node, so a different LED blinks without any change in `main.c`.
- I2C returned `-5` (`-EIO`) at first, which means the sensor did not ACK. The cause was SDA wired to the wrong header pin. After fixing it, the soft reset command returned 0.
- A new file (overlay, binding) needs a pristine build, because CMake only finds new files during configuration. Editing existing files is picked up by a normal build.