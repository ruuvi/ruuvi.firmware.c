/**
 * Application configuration constants
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H
#include "application_modes.h" // Includes different modes, such as long-life with low sampling rate and tx rate.

#define APPLICATION_FW_VERSION "RuuviFW 3.16.0"

// Pick a power of 2 for nRF5 backend. 128 is recommended.
#define APPLICATION_LOG_BUFFER_SIZE              256

// Use nRF5 SDK15
#define NRF5_SDK15_PLATFORM_ENABLED              1

/**
 * Data format configuration
 */
// Voltage mode - select one
#ifndef APPLICATION_BATTERY_VOLTAGE_MODE
#define APPLICATION_BATTERY_VOLTAGE_SIMPLE   0  // Simple mode: Not synchronized to anything, sampled at regular interval
#define APPLICATION_BATTERY_VOLTAGE_RADIO    1  // Radio mode: Voltage is sampled after radio tx, refreshed after interval has passed.
#define APPLICATION_BATTERY_VOLTAGE_DROOP    0  // Droop mode: Battery is read after TX and after a brief recovery period. Droop is reported
#endif
#define APPLICATION_BATTERY_DROOP_DELAY_MS   2  // Milliseconds between active and recovered tx
#if(APPLICATION_BATTERY_VOLTAGE_SIMPLE + APPLICATION_BATTERY_VOLTAGE_RADIO + APPLICATION_BATTERY_VOLTAGE_DROOP != 1)
  #error "Select application battery voltage monitor mode by defining one constant as 1"
#endif

/**
 * Environmental sensor configuration
 **/
// Sample rate is in Hz. This configures only the sensor, not transmission rate of data.
#ifndef APPLICATION_ENVIRONMENTAL_CONFIGURED
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
#endif


// Allow BME280 support compilation
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_ENABLED 1

/**
 * Accelerometer configuration
 **/
 #ifndef APPLICATION_ACCELERATION_CONFIGURED
  // 1, 10, 25, 50, 100, 200 for LIS2DH12
  #define APPLICATION_ACCELEROMETER_SAMPLERATE 10

  // 8, 10, 12 for LIS2DH12
  #define APPLICATION_ACCELEROMETER_RESOLUTION 10

  // 2, 4, 8, 16 for LIS2DH12
  #define APPLICATION_ACCELEROMETER_SCALE   RUUVI_DRIVER_SENSOR_CFG_MIN

  // LAST or HIGH_PASS
  #define APPLICATION_ACCELEROMETER_DSPFUNC RUUVI_DRIVER_SENSOR_DSP_LAST
  #define APPLICATION_ACCELEROMETER_DSPPARAM 1

  // SLEEP or CONTINUOUS
  #define APPLICATION_ACCELEROMETER_MODE RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS

  // Up to scale
  #define APPLICATION_ACCELEROMETER_ACTIVITY_THRESHOLD 0.100f
#endif

// Allow LIS2DH12 support compilation
#define RUUVI_INTERFACE_ACCELERATION_LIS2DH12_ENABLED 1

/**
 *  ADC configuration
 */
#define APPLICATION_ADC_SAMPLERATE         RUUVI_DRIVER_SENSOR_CFG_MIN // Valid for continuous mode
#define APPLICATION_ADC_RESOLUTION         10
#define APPLICATION_ADC_SCALE              RUUVI_DRIVER_SENSOR_CFG_DEFAULT
#define APPLICATION_ADC_DSPFUNC            RUUVI_DRIVER_SENSOR_DSP_LAST
#define APPLICATION_ADC_DSPPARAM           1
#define APPLICATION_ADC_MODE               RUUVI_DRIVER_SENSOR_CFG_SINGLE // Note: call to task_adc_sample will leave the ADC in single-shot mode. Use data get only in continuous mode
#define APPLICATION_ADC_SAMPLE_INTERVAL_MS 30000 // Valid for single mode

/**
 * Bluetooth configuration
 *
 */
// Avoid "even" values such as 100 or 1000 to eventually drift apart from the devices transmitting at same interval
#ifndef APPLICATION_ADVERTISING_CONFIGURED
  #define APPLICATION_ADVERTISING_INTERVAL              1010
  #define APPLICATION_CONNECTION_ADVERTISEMENT_INTERVAL APPLICATION_ADVERTISING_INTERVAL
  #define APPLICATION_ADVERTISING_POWER                 RUUVI_BOARD_TX_POWER_MAX
  #define APPLICATION_DATA_FORMAT                       5
#endif

/**
 * NFC configuration
 */
// Longest text in a text field, i.e. "FW: ruuvi.firmware.c 3.10.0
#define APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE (32)
// Longest binary message
#define APPLICATION_COMMUNICATION_NFC_DATA_BUFFER_SIZE APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE
// 3 text records (version, address, id) and a data record
#define APPLICATION_COMMUNICATION_NFC_MAX_RECORDS      (4)
// 2 length bytes + 3 * text record + 1 * data record + 4 * 9 bytes for record header
// Conservers RAM for 3 * text buffer size + 1 * data buffer size + NDEF_FILE_SIZE
#define APPLICATION_COMMUNICATION_NFC_NDEF_FILE_SIZE   (166)

/**
 * Task scheduler configuration
 */
#define APPLICATION_TASK_DATA_MAX_SIZE 0
#define APPLICATION_TASK_QUEUE_MAX_LENGTH 10

/**
 * Flags which determine which c-modules are compiled in.
 * These modules may reserve some RAM and FLASH, so if you
 * do not need module you can disable it. The modules might also
 * have some dependencies between themselves.
 */
#define APPLICATION_ADC_ENABLED                     1
#define APPLICATION_COMMUNICATION_ENABLED           1 // Common functions for communication
#define APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED 1 // Advertising and GATT
#define APPLICATION_COMMUNICATION_NFC_ENABLED       1 // NFC
#define APPLICATION_GPIO_ENABLED                    1
#define APPLICATION_GPIO_INTERRUPT_ENABLED          1
#define APPLICATION_ENVIRONMENTAL_MCU_ENABLED       1
#define APPLICATION_ENVIRONMENTAL_BME280_ENABLED    1
#define APPLICATION_POWER_ENABLED                   1
#define APPLICATION_RTC_MCU_ENABLED                 1
#define APPLICATION_SCHEDULER_ENABLED               1
#define APPLICATION_SPI_ENABLED                     1
#define APPLICATION_TIMER_ENABLED                   1
#define APPLICATION_WATCHDOG_ENABLED                1
#define APPLICATION_WATCHDOG_INTERVAL_MS            120000u
#define APPLICATION_YIELD_ENABLED                   1
#define APPLICATION_LOG_ENABLED                     1
// RUUVI_INTERFACE_LOG_ ERROR, WARNING, INFO, DEBUG
#define APPLICATION_LOG_LEVEL                       RUUVI_INTERFACE_LOG_INFO

// Choose one. RTT is recommended, but does not work on devices
// with readback protection enabled
#define APPLICATION_LOG_BACKEND_RTT_ENABLED         1
//#define APPLICATION_LOG_BACKEND_UART_ENABLED        0 // UART not implemented

#endif