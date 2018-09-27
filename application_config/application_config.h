/**
 * Application configuration constants
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H

#define APPLICATION_FW_VERSION "3.10.0"

// Pick a power of 2 for nRF5 backed. 128 is recommended.
#define APPLICATION_LOG_BUFFER_SIZE              128

// Use nRF5 SDK15
#define NRF5_SDK15_PLATFORM_ENABLED              1

/**
 * Environmental sensor configuration
 **/
// Sample rate is in Hz. This configures only the sensor, not transmission rate of data.
#define APPLICATION_ENVIRONMENTAL_SAMPLERATE RUUVI_DRIVER_SENSOR_CFG_MIN

// Resolution and scale cannot be adjusted on BME280
#define APPLICATION_ENVIRONMENTAL_RESOLUTION RUUVI_DRIVER_SENSOR_CFG_DEFAULT
#define APPLICATION_ENVIRONMENTAL_SCALE      RUUVI_DRIVER_SENSOR_CFG_DEFAULT

// Valid values for BME280 are: (RUUVI_DRIVER_SENSOR_DSP_)LAST, IIR, OS
// IIR slows step response but lowers noise
// OS increases power consumption but lowers noise.
// See https://blog.ruuvi.com/humidity-sensor-673c5b7636fc and https://blog.ruuvi.com/dsp-compromises-3f264a6b6344
#define APPLICATION_ENVIRONMENTAL_DSPFUNC    RUUVI_DRIVER_SENSOR_DSP_OS

// No effect on _LAST, use 1. On _OS and _IIR valid values are 2, 4, 8 and 16.
#define APPLICATION_ENVIRONMENTAL_DSPPARAM   RUUVI_DRIVER_SENSOR_CFG_MAX

// (RUUVI_DRIVER_SENSOR_CFG_)SLEEP, SINGLE or CONTINUOUS
#define APPLICATION_ENVIRONMENTAL_MODE       RUUVI_DRIVER_SENSOR_CFG_SINGLE

// Allow BME280 support compilation
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_ENABLED 1

/**
 * Accelerometer configuration
 **/

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

// Allow LIS2DH12 support compilation
#define RUUVI_INTERFACE_ACCELERATION_LIS2DH12_ENABLED 1

/**
 *  ADC configuration
 */
#define APPLICATION_ADC_SAMPLERATE RUUVI_DRIVER_SENSOR_CFG_MIN
#define APPLICATION_ADC_RESOLUTION 10
#define APPLICATION_ADC_SCALE      RUUVI_DRIVER_SENSOR_CFG_DEFAULT
#define APPLICATION_ADC_DSPFUNC    RUUVI_DRIVER_SENSOR_DSP_LAST
#define APPLICATION_ADC_DSPPARAM    1
#define APPLICATION_ADC_MODE       RUUVI_DRIVER_SENSOR_CFG_SINGLE

/**
 * Bluetooth configuration
 *
 */
// Avoid "even" values such as 100 or 1000 to eventually drift apart from the devices transmitting at same interval
#define APPLICATION_ADVERTISING_INTERVAL 1010
#define APPLICATION_ADVERTISING_POWER    RUUVI_BOARD_TX_POWER_MAX

/**
 * Flags which determine which c-modules are compiled in.
 * These modules may reserve some RAM and FLASH, so if you
 * do not need module you can disable it.
 */
#define APPLICATION_ADC_ENABLED                     1
#define APPLICATION_COMMUNICATION_ENABLED           1 // Common functions for communication
#define APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED 1 // Advertising and GATT
#define APPLICATION_COMMUNICATION_NFC_ENABLED       1
#define APPLICATION_GPIO_ENABLED                    1
#define APPLICATION_GPIO_INTERRUPT_ENABLED          1
#define APPLICATION_ENVIRONMENTAL_MCU_ENABLED       1
#define APPLICATION_ENVIRONMENTAL_BME280_ENABLED    1
#define APPLICATION_POWER_ENABLED                   1
#define APPLICATION_RTC_MCU_ENABLED                 1
#define APPLICATION_SPI_ENABLED                     1
#define APPLICATION_YIELD_ENABLED                   1
#define APPLICATION_LOG_ENABLED                     1
// RUUVI_INTERFACE_LOG_ ERROR, WARNING, INFO, DEBUG
#define APPLICATION_LOG_LEVEL                       RUUVI_INTERFACE_LOG_INFO

// Choose one. RTT is recommended, but does not work on devices
// with readback protection enabled
#define APPLICATION_LOG_BACKEND_RTT_ENABLED         1
#define APPLICATION_LOG_BACKEND_UART_ENABLED        0

#endif