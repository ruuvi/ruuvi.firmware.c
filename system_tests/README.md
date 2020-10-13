# System tests

These tests verify the end-to-end functionality of Ruuvi devices between apps. 

# Setting up
## Hardware
You'll need a nRF devkit to run BLE throughput test, Power profiler to run power measurements, 
Ruuvi DevKit to riun integration tests and SDK update tests. You'll also need 2 Ruuvi devices
of the variant under test. 

An Android phone with ADB connection is also needed.

## .test_env
Check the serial numbers of your devkits with `nrfjprog --ids`

# Running test
Run `initialize_devkits.sh`, then `test_sdk_dfu.sh`.

## Test stages

### SDK update test
Device on devkit is preprogrammed with nRF5 SDK 12.3 S132 v3.1.1 + bootloader.
As there is no firmware, bootloader will stay on, waiting for update. 

SDK update package is pushed on the DUT.

### DFU update test
