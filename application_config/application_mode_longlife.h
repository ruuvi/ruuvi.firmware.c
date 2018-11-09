/**
 * Long-life mode for Ruuvi Firmware.
 * Broadcasts at 10 Hz interval and runs sensors at slower rate to conserve battery.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */
#ifndef APPLICATION_MODE_LONGLIFE_H
#define APPLICATION_MODE_LONGLIFE_H

/**
 * Environmental sensor configuration
 **/
// Sample rate is in Hz. This configures only the sensor, not transmission rate of data.
#define APPLICATION_ENVIRONMENTAL_CONFIGURED
#define APPLICATION_ENVIRONMENTAL_SAMPLERATE RUUVI_DRIVER_SENSOR_CFG_MIN

// Resolution and scale cannot be adjusted on BME280
#define APPLICATION_ENVIRONMENTAL_RESOLUTION RUUVI_DRIVER_SENSOR_CFG_DEFAULT
#define APPLICATION_ENVIRONMENTAL_SCALE      RUUVI_DRIVER_SENSOR_CFG_DEFAULT

// Valid values for BME280 are: (RUUVI_DRIVER_SENSOR_DSP_)LAST, IIR, OS
// IIR slows step response but lowers noise
// OS increases power consumption but lowers noise.
// See https://blog.ruuvi.com/humidity-sensor-673c5b7636fc and https://blog.ruuvi.com/dsp-compromises-3f264a6b6344
#define APPLICATION_ENVIRONMENTAL_DSPFUNC    RUUVI_DRIVER_SENSOR_DSP_IIR

// No effect on _LAST, use 1. On _OS and _IIR valid values are 2, 4, 8 and 16.
#define APPLICATION_ENVIRONMENTAL_DSPPARAM   RUUVI_DRIVER_SENSOR_CFG_MAX

// (RUUVI_DRIVER_SENSOR_CFG_)SLEEP, SINGLE or CONTINUOUS
#define APPLICATION_ENVIRONMENTAL_MODE       RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS


/**
 * Accelerometer configuration
 **/
#define APPLICATION_ACCELERATION_CONFIGURED
// 1, 10, 25, 50, 100, 200 for LIS2DH12
#define APPLICATION_ACCELEROMETER_SAMPLERATE RUUVI_DRIVER_SENSOR_CFG_MIN

// 8, 10, 12 for LIS2DH12
#define APPLICATION_ACCELEROMETER_RESOLUTION 10

// 2, 4, 8, 16 for LIS2DH12
#define APPLICATION_ACCELEROMETER_SCALE   RUUVI_DRIVER_SENSOR_CFG_MIN

// LAST or HIGH_PASS
#define APPLICATION_ACCELEROMETER_DSPFUNC RUUVI_DRIVER_SENSOR_DSP_LAST
#define APPLICATION_ACCELEROMETER_DSPPARAM 1

// SLEEP or CONTINUOUS
#define APPLICATION_ACCELEROMETER_MODE RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS

#define APPLICATION_ACCELEROMETER_ACTIVITY_THRESHOLD 0.100f

/**
 * Bluetooth configuration
 *
 */
// Avoid "even" values such as 100 or 1000 to eventually drift apart from the devices transmitting at same interval
// Min 100, Max 10000
#define APPLICATION_ADVERTISING_CONFIGURED
#define APPLICATION_ADVERTISING_INTERVAL              9990
#define APPLICATION_CONNECTION_ADVERTISEMENT_INTERVAL 1010
#define APPLICATION_ADVERTISING_POWER                 RUUVI_BOARD_TX_POWER_MAX
#define APPLICATION_DATA_FORMAT                       5

#endif