# ble-sensor-node

Battery-powered BLE environmental sensor node on the nRF54L15 DK, built with Zephyr RTOS (nRF Connect SDK).

## Goal

Read temperature and humidity from an SHT30 over I2C using a driver written from the datasheet, and expose the data over BLE through a custom GATT service with notifications.

## Setup

- Board: nRF54L15 DK (`nrf54l15dk/nrf54l15/cpuapp`, revision: <z naklejki na płytce>)
- SDK: nRF Connect SDK v3.4.2 LTS
- Sensor: SHT30 (I2C, address 0x44)
- Tooling: VS Code with the nRF Connect extension

## Status

- [x] Environment, build and flash (blinky)
- [x] Devicetree overlay: `led0` alias redirected to `led1`
- [ ] SHT30 I2C node in Devicetree and own driver
- [ ] Unit tests (Twister / ZTest)
- [ ] BLE advertising and custom GATT service

#Notes and Sugestions

In Zephyr, C code looks up hardware through a devicetree alias, e.g. `DT_ALIAS(led0)`. 
A node label is the name of a specific node in the devicetree (e.g. `led1`), used in `.dts` files and overlays. 
The board file maps `led0` to the `led0` node by default; my overlay redirects the `led0` alias to the `led1` node, so the LED changes without touching `main.c`.
