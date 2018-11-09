# ruuvi.firmware.c
[![Build Status](http://jenkins.ruuvi.com:8080/buildStatus/icon?job=ruuvi.firmware.c)](http://jenkins.ruuvi.com:8080/job/ruuvi.firmware.c)

Ruuvi Firmware version 3. Built on top of Nordic SDK 15, uses both Ruuvi and external repositories as submodules.
Under development, please follow [Ruuvi Blog](https://blog.ruuvi.com) for details. 

# Setting up
## SDK 15.2
Download [Nordic SDK15.2](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/) and install it one level above the project root. 

## Submodules
Run `git submodule update --init --recursive`. This will search for and install the other git repositories referenced by this project.

## Toolchain
ARMGCC is used for [Jenkins builds](http://jenkins.ruuvi.com:8080/job/ruuvi.firmware.c/), it's recommended to use SES for developing.

Segger Embedded Studio can be set up by following this [RuuviLab Tool post](https://lab.ruuvi.com/ses/).

# Usage
Compile and flash the project to your board using SES. Instructions on how to use a bootloader will be added later on.
The project is not yet in a useful state for any practical purpose other than learning. 

Note: You should erase your board entirely in case there is a bootloader from a previous firmware.

# How to contribute
Please let us know your thoughts on the direction and structure of the project. Does the project help you to understand how to build code on RuuviTag?
Is the structure of the project sensible to you? Pull requests and GitHub issues are welcome, but new features will not be accepted at this time. 

# Licensing
Ruuvi code is BSD-3 licensed. Submodules and external dependencies have their own licenses, which generally are BSD-compatible.

# Changelog
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

