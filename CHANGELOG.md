# Changelog
## 3.30.4
 - Fix overflow in timer causing tag to stop logging in 50 days. 
 - Fix overflow in data encoding, causing pressure to jump 115 534 Pa -> 50 000 Pa 
 - Switch to faster connection parameters when transferring logged data to avoid timeouts

## 3.30.3
 - Use 400 kHz I2C to save power
 - Fix TMP117 single sample occasionally returning invalid data 
 - Fix nRF52 not returning temperature data due to default configuration of app
 - Fix device switching to slow advertising 60 seconds after a GATT connection, now 5 seconds
 - Fix Leds not lighting up on button press

## 3.30.2
 - Fix history log rotation

## 3.30.1 
 - Fix VDD sampling for the first data broadcast

## 3.30.0
 - Fix Kalervo support
 - Configure measurement and broadcast interval separately
 - Add GPIO-sensor power support
 - Add support for TMP117 temperature sensor
 - Add support for DPS310 pressure/temperature sensor
 - Fix log rotation drifing which caused gaps in data

## 3.29.3
 - Fix factory reset not erasing entire flash storage of app.
 - Fix GATT connection loss during log read causing a reboot + log erase. 
 - Increase release watchdog to 6 minutes (was 2).

## 3.29.2
 - Internal test version.

## 3.29.1 
 - Remove slave latency to fix race condition when iOS switches to 2 MBit/s PHY
 - Add +3% offset to BME280

## 3.29.0
 - Refactor, move tasks under drivers.
 - Refactor, store log data to flash.
 - Improve GATT power consumption.
 - Improve GATT data rate.

## 3.28.13
 - Fix leds not blinking

## 3.28.12
 - Automatically release a tag

## 3.28.11
 - Fix battery voltage monitoring
 - Fix advertisements not restarting after GATT connection

## 3.28.10
 - Fix issue in drivers preventing build 

## 3.28.9 
 - Fix 811 builds
 - Fix movement interrupts / counter

## 3.28.8
 - Refactor communication
 - Do not send heartbeats while processing log.

## 3.28.7
  - Fix long-life sensors not sampling.

## 3.28.6
  - Add Travis CI and SonarCloud support

## 3.28.5
  - Convert project to use Ceedling for unit tests.

## 3.28.1 ... 3.28.4
  - Bugfixes
  - Add longlife-logging

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

