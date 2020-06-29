# ruuvi.firmware.c
[![Build Status](https://jenkins.ruuvi.com/buildStatus/icon?job=ruuvi.firmware.c+-+deploy)](https://jenkins.ruuvi.com/job/ruuvi.firmware.c%20-%20deploy/)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=alert_status)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=bugs)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=code_smells)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=coverage)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=duplicated_lines_density)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=ncloc)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=reliability_rating)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=sqale_index)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)

Ruuvi Firmware version 3. Built on top of Nordic SDK 15, uses both Ruuvi and external repositories as submodules.
Under development, please follow [Ruuvi Blog](https://blog.ruuvi.com) for details. The project is in alpha stage,
build maybe broken in obvious ways and not build at all or in non-obvious ways and fail on some Ruuvi boards. 

# Setting up
## SDK 15.3
Download [Nordic SDK15.3](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/) and install it at the project root.
If you're working on multiple nRF projects, you can (and should) use softlinking instead.

## Submodules
Run `git submodule update --init --recursive`. This will search for and install the other git repositories referenced by this project. If any of the submodules has a changed remote, you'll need to run `git submodule sync --recursive` and again `git submodule update --init --recursive` to update the modules from new remotes. 

## Toolchain
ARMGCC is used for [Jenkins builds](http://jenkins.ruuvi.com/job/ruuvi.firmware.c/), it's recommended to use SES for developing. You can make the project and all variants by runnning "make" at top level of this repository. 
 
Segger Embedded Studio can be set up by installing [nRF Connect for Desktop](https://www.nordicsemi.com/?sc_itemid=%7BB935528E-8BFA-42D9-8BB5-83E2A5E1FF5C%7D) 
and following Getting Started plugin instructions.

Start SES and open `ruuvi_ruuvi.firmware.c.emProject` at root level, each of the target boards is in their own project.

## Code style
Code is formatted with [Artistic Style](http://astyle.sourceforge.net). 
Run `astyle --project=.astylerc ./target_file`. To format the entire project,
```
astyle --project=.astylerc "src/main.c"
astyle --project=.astylerc --recursive "src/application_config/*.h"
astyle --project=.astylerc --recursive "src/tasks/*.c"
astyle --project=.astylerc --recursive "src/tasks/*.h"
astyle --project=.astylerc --recursive "src/tests/*.c"
astyle --project=.astylerc --recursive "src/tests/*.h"
```

## Static analysis
The code can be checked with PVS Studio and Sonarcloud for some common errors, style issues and potential problems. [Here](https://ruuvi.github.io/ruuvi.firmware.c/fullhtml/index.html) is a link to generated report which gets pushed to GitHub.


### PVS
Obtain license and software from [Viva64](https://www.viva64.com/en/pvs-studio/).

Make runs PVS Studio scan and outputs results under doxygen/html/fullhtml. 

This results into hundreds of warnings, it is up to you to filter the data you're interested in. For example you probably want to filter out warnings related to 64-bit systems. 

### Sonar scan
Travis pushes the results to [SonarCloud.IO](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c).
SonarCloud uses access token which is private to Ruuvi, you'll need to fork the project and setup
the SonarCloud under your own account if you wish to run Sonar Scan on your own code.

# Running unit tests
## Ceedling
Unit tests are implemented with Ceedling. Run the tests with
`ceedling test:all`

### Gcov
Ceedling can also generate Gcov reports with `ceedling gcov:all utils:gcov`.
The report can be found under _build/artifacts/gcov_.

## Unit test continuous integration
Travis will fail the build if unit test fails and Gcov results will get pushed to SonarCloud.

# Running integration tests
Integration tests are run on actual hardware and they verify that sensors are present 
and reply to traffic on buses. To run integration test, you need to have a SWD connection on 
target board and JLinkRTTViewer on to read the logs. The logs are JSON and contain 
string "fail" if a test does not pass. Example log:
```
Version: v3.28.14
'mcu_tests':{
'library':{
'peak2peak':{
'valid_data':'pass',
'nan_data':'pass',
'overflow_data':'pass',
'input_validation':'pass',
},
'rms':{
'valid_data':'pass',
'nan_data':'pass',
'overflow_data':'pass',
'input_validation':'pass',
},
'variance':{
'valid_data':'pass',
'nan_data':'pass',
'overflow_data':'pass',
'input_validation':'pass',
},
'total_tests':'15',
'passed_tests':'15'
}
}
```

# Running system tests
System test verifies that SDK is upgradeable over DFU and application can be re-updated after SDK update
using Nordic's buttonless DFU Service. To run system tests you need to have RuuviTag on a devkit and power profiler
and update `system_tests/.test_env` to match the serial numbers and MAC addresses of your RuuviTags.
Automated power profiling is still on ToDo, right now you need to check the profile manually.

You also need an Android phone with nRF Connect and ADB connction to run the automated tests. 

After system tests are complete, test log is pulled from the Android and you need to manually review
the log. See `system_tests/test_sdk_update_result.txt.sample` for an example of what to expect. 

# Usage
Compile and flash the project to your board using SES. Instructions on how to use a bootloader will be added later on.
The project is not yet in a useful state for any practical purpose other than learning. 

Note: You should erase your board entirely in case there is a bootloader from a previous firmware.

# How to contribute
Please let us know your thoughts on the direction and structure of the project. Does the project help you to understand how to build code on RuuviTag?
Is the structure of the project sensible to you? 

If you want to assist in the project maintenance by fixing some issues _doxygen.error_ is
a good place to look for code which needs better commenting. Project badges at the top of the
readme point to issues which range from trivial clarifications to complex refactoring. 

If you want to add new features, please discuss the feature first, and then create ceedling
unit tests for the functionality. Once the functionality is agreed and testable in can be integrated
into project.

# Licensing
Ruuvi code is BSD-3 licensed. Submodules and external dependencies have their own licenses, which are BSD-compatible.

# Documentation
Document is generated with Doxygen. Run `make doxygen` to generate the docs locally, or
browse to [Travis built docs](ruuvi.github.io/ruuvi.firmware.c)

# Changelog
## 3.28.14
 - Fix LIS2DH12 reboot occasionally failing

## 3.28.13
 - Fix LEDs not blinking

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

