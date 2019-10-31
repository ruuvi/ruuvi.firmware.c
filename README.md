# ruuvi.firmware.c
[![Build Status](http://jenkins.ruuvi.com/buildStatus/icon?job=ruuvi.firmware.c)](http://jenkins.ruuvi.com/job/ruuvi.firmware.c)

Ruuvi Firmware version 3. Built on top of Nordic SDK 15, uses both Ruuvi and external repositories as submodules.
Under development, please follow [Ruuvi Blog](https://blog.ruuvi.com) for details. The project is in alpha stage,
build maybe broken in obvious ways and not build at all or in non-obvious ways and fail on some Ruuvi boards. 

# Setting up
## SDK 15.3
Download [Nordic SDK15.3](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/) and install it one level above the project root. 

## Submodules
Run `git submodule update --init --recursive`. This will search for and install the other git repositories referenced by this project. If any of the submodules has a changed remote, you'll need to run `git submodule sync --recursive` and again `git submodule update --init --recursive` to update the modules from new remotes. 

## Toolchain
ARMGCC is used for [Jenkins builds](http://jenkins.ruuvi.com/job/ruuvi.firmware.c/), it's recommended to use SES for developing. You can make the project and all variants by runnning "make" at top level of this repository. 
 
Segger Embedded Studio can be set up by installing [nRF Connect for Desktop](https://www.nordicsemi.com/?sc_itemid=%7BB935528E-8BFA-42D9-8BB5-83E2A5E1FF5C%7D) 
and following Getting Started plugin instructions.

Start SES and open `ruuvi.firmware.c.emProject` at root level, each of the target boards is in their own project.

# Usage
Compile and flash the project to your board using SES. Instructions on how to use a bootloader will be added later on.
The project is not yet in a useful state for any practical purpose other than learning. 

Note: You should erase your board entirely in case there is a bootloader from a previous firmware.

# How to contribute
Please let us know your thoughts on the direction and structure of the project. Does the project help you to understand how to build code on RuuviTag?
Is the structure of the project sensible to you? Pull requests and GitHub issues are welcome, but new features will not be accepted at this time. 

# Licensing
Ruuvi code is BSD-3 licensed. Submodules and external dependencies have their own licenses, which are BSD-compatible.

# Changelog
## 3.28.0
  - Fix storing and loading sensor configurations
  - Store fatal error sources to flash before reboot.

## 3.27.2
  - Use busyloop in log flush to avoid getting stuck if logs are flushed in interrupt context
  - Initialize variables in environmental logging to zero. Explicit U64->U32 timestamp typecast.

## 3.27.1
  - Fix errors in data format 5 encoding
  - Known issue: timestamps are occasionally wrong in log data

## 3.27.0
  - Rework task structure
  - Add "hearbeat" transmissions to GATT

## 3.26.5
 - Add SD+BL+APP DFU builds

## 3.26.4
 - Do not block if RTT logs are not read in debug mode

## 3.26.3
 - Fix build script in case of out-of-master submodule commits

## 3.26.2
 - Fix Keijo build

## 3.26.1
 - Improve logging sent elements. 
 - Send 0xFFFFFFFF payload at the end of log data
 - Block until RTT logs are sent in debug mode

## 3.26.0
 - Add Humidity, pressure logging PoC

## 3.25.1
 - Add Temperature logging PoC

## 3.24.2
 - Switch Keijo to S140 for Coded PHY
 - Discard Keijo GATT, bootlaoder

## 3.24.1 
 - Fit Keijo application in nRF52811

## 3.24.0 
 - Initial support for "Keijo"-project
 - Port to SDK15.3

## 3.23.0 
 - Initial support for "Kaarle"-project

## 3.22.1 
 - Use acceleration interrupts for movement counter

## 3.22.0 
 - Add low-power delay to drivers
 - Try to initialize a separate pressure sensor if using SHTC as environmental sensor
 - Add pressure sensor tasks

## 3.19.0 ... # 3.21.0
 - Various reworks and refactoring.

## 3.18.0
 - Use refactored drivers.

## 3.17.0
- Add NFC writes to tag
- Disable GATT
- Use activity interrupts, advertise data format 5
- Trigger watchdog if data is not sent via NFC, GATT or advertisement for 120 seconds.
- 27 uA current consumption at 3V

## 3.16.0
 - Add GATT profile
 - Stream accelerometer data over GATT / NUS
 - Tigger watchdog if data is not sent over NUS for 120 seconds
 - Disable data advertising
 - Disable activity interrupts from LIS2DH12

## 3.15.0
 - Add watchdog, reset if button is not pressed.

## 3.14.0
 - Add interrupts to accelerometer
 - Add data format 5 support

## 3.13.0
 - Synchronize battery measurements to radio

## 3.12.0
 - Bootloader in a [separate repository](https://github.com/ruuvi/ruuvi.nrf5_sdk15_bootloader.c)

## 3.11.0
 * Add scheduler and timer, update transmitted data once per second.
 * Environmental sensor is now in continuous mode and has IIR filtering at 16x (was oversampling)
 * ADC is now read once per minute

## 3.10.0
Adds NFC tag functionality - read only

## 3.9.0
Adds BLE broadcasts

## 3.8.0
Adds RTC

## 3.7.0
Adds battery measurement

## 3.6.0 
Adds Accelerometer polling

## 3.5.0
Adds environmental sensing

## 3.4.0
Adds button control to blink leds

## 3.3.0
Adds Led blinking example

## 3.2.0
Adds logging and error code handling

## 3.1.1
Put external sensors to sleep and then put MCU to sleep.

