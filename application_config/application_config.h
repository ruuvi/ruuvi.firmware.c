/**
 * Application configuration constants
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H
#include "application_modes.h" // Includes different modes, such as long-life with low sampling rate and tx rate.

/** @brief Version string, displayed in NFC read and GATT data on DIS */
#define APPLICATION_FW_VERSION "RuuviFW 3.23.0"

/** @brief Bytes of RAM to conserve for printed log messages
 *  Pick a power of 2 for nRF5 backend. at least 128 is recommended.
 */
#define APPLICATION_LOG_BUFFER_SIZE           256

/**
 * @brief enable compilation of NRF5 SDK15 implementation of driver interface functions.
 * Only one implementation can be enabled at a time.
 */
#define RUUVI_NRF5_SDK15_ENABLED              1

/**
 * @brief Battery voltage measurement mode configuration
 */
#ifndef APPLICATION_BATTERY_VOLTAGE_MODE
  #define APPLICATION_BATTERY_VOLTAGE_SIMPLE   0  //<! Simple mode: Not synchronized to anything, sampled at regular interval
  #define APPLICATION_BATTERY_VOLTAGE_RADIO    1  //<! Radio mode: Voltage is sampled after radio tx, refreshed after interval has passed.
  #define APPLICATION_BATTERY_VOLTAGE_DROOP    0  //<! Droop mode: Battery is read after TX and after a brief recovery period. Droop is reported
#endif
#define APPLICATION_BATTERY_DROOP_DELAY_MS   2  //<! Milliseconds between active and recovered tx
#if(APPLICATION_BATTERY_VOLTAGE_SIMPLE + APPLICATION_BATTERY_VOLTAGE_RADIO + APPLICATION_BATTERY_VOLTAGE_DROOP != 1)
  #error "Select application battery voltage monitor mode by defining one constant as 1"
#endif

/**
 * Environmental sensor configuration
 **/
#ifndef APPLICATION_ENVIRONMENTAL_CONFIGURED
  /** @brief sample rate, in Hz. */
  #define APPLICATION_ENVIRONMENTAL_SAMPLERATE RUUVI_DRIVER_SENSOR_CFG_MIN

  /** @brief BME280 cannot adjust resolution, use default */
  #define APPLICATION_ENVIRONMENTAL_RESOLUTION RUUVI_DRIVER_SENSOR_CFG_DEFAULT

  /** @brief BME280 cannot adjust scale, use default */
  #define APPLICATION_ENVIRONMENTAL_SCALE      RUUVI_DRIVER_SENSOR_CFG_DEFAULT

  /** @brief DSP configuration of environmental sensor.
  * Valid values for BME280 are: (RUUVI_DRIVER_SENSOR_DSP_)LAST, LOW_PASS, OS
  * Low pass slows step response but lowers noise
  * Ooversampling (OS) increases power consumption but lowers noise.
  * @see https://blog.ruuvi.com/humidity-sensor-673c5b7636fc and https://blog.ruuvi.com/dsp-compromises-3f264a6b6344
  */
  #define APPLICATION_ENVIRONMENTAL_DSPFUNC    RUUVI_DRIVER_SENSOR_CFG_DEFAULT

  /** @brief Parameter to DSP function.
  * The parameter affects how agressively the DSP is applied, higher means
  * stronger effect. For example Oversampling with parameter 8 means 8 samples are averaged for one sample.
  *
  * Valid values are RUUVI_DRIVER_SENSOR_CFG_MAX, RUUVI_DRIVER_SENSOR_CFG_MIN, RUUVI_DRIVER_SENSOR_CFG_DEFAULT.
  * 1, 2, 4, 8, 16
  */
  #define APPLICATION_ENVIRONMENTAL_DSPPARAM   RUUVI_DRIVER_SENSOR_CFG_DEFAULT

  /**
  * @brief default mode of environmental sensor.
  * Valid values are RUUVI_DRIVER_SENSOR_CFG_SLEEP, RUUVI_DRIVER_SENSOR_CFG_SINGLE and RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS.
  * Sleep enters lowest-power mode possible.
  * Single commands sensor to take a new sample, waits sample to be available and then returns the data from sensor.
  * Continuous keeps the sensor running on the background regardless of how often data is read.
  * Continuous is recommended mode for most applications.
  */
  #define APPLICATION_ENVIRONMENTAL_MODE       RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS
#endif


/** @brief Enable compiling BME280 interface functions */
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_ENABLED 1

/** @brief Enable compiling BME280 SPI interface */
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_SPI_ENABLED 1

/** @brief Enable compiling BME280 I2C interface */
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_I2C_ENABLED 0

/** @brief Enable compiling SHTCX interface functions */
#define RUUVI_INTERFACE_ENVIRONMENTAL_SHTCX_ENABLED 1

/**
 * Accelerometer configuration
 **/
#ifndef APPLICATION_ACCELERATION_CONFIGURED
  /** @brief sample rate of accelerometer
  *
  * This controls only internal sampling of accelerometer, not reading frequency of data.
  * Valid values are 1, 10, 25, 50, 100, 200 for LIS2DH12
  * @c RUUVI_DRIVER_CFG_MIN, @c RUUVI_DRIVER_CFG_MAX, and @c RUUVI_DRIVER_CFG_DEFAULT, are valid for all sensors.
  */
  #define APPLICATION_ACCELEROMETER_SAMPLERATE 1

  /** @brief Resolution of accelerometer, in bits.
  *
  * This controls only internal sampling of accelerometer, not reading frequency of data.
  * Valid values are  8, 10, 12 for LIS2DH12
  * @c RUUVI_DRIVER_SENSOR_CFG_MIN, @c RUUVI_DRIVER_SENSOR_CFG_MAX, and @c RUUVI_DRIVER_SENSOR_CFG_DEFAULT, are valid for all sensors.
  */
  #define APPLICATION_ACCELEROMETER_RESOLUTION RUUVI_DRIVER_SENSOR_CFG_DEFAULT

  /** @brief Scale of accelerometer, in G (9.81 m / s^2).
  *
  * This controls only internal sampling of accelerometer, not reading frequency of data.
  * Valid values are  2, 4, 8, 16 for LIS2DH12
  * @c RUUVI_DRIVER_CFG_MIN, @c RUUVI_DRIVER_CFG_MAX, and @c RUUVI_DRIVER_CFG_DEFAULT, are valid for all sensors.
  */
  #define APPLICATION_ACCELEROMETER_SCALE   RUUVI_DRIVER_SENSOR_CFG_MIN

  /** @brief DPS function for accelerometer
  *  Enable high-pass to filter out the gravity.
  */
  #define APPLICATION_ACCELEROMETER_DSPFUNC RUUVI_DRIVER_SENSOR_DSP_LAST

  /** @brief DPS parameter for accelerometer
  *  Higher is more agressive.
  *  Valid values are  1, 2, 3, 4 for LIS2DH12
  *  @c RUUVI_DRIVER_SENSOR_CFG_MIN, @c RUUVI_DRIVER_SENSOR_CFG_MAX, and @c RUUVI_DRIVER_SENSOR_CFG_DEFAULT, are valid for all sensors.
  */
  #define APPLICATION_ACCELEROMETER_DSPPARAM RUUVI_DRIVER_SENSOR_CFG_DEFAULT

  /** @brief Operating mode for accelerometer
  * Valid values are RUUVI_DRIVER_SENSOR_CFG_SLEEP, RUUVI_DRIVER_SENSOR_CFG_SINGLE and RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS.
  * Sleep enters lowest-power mode possible.
  * Single commands sensor to take a new sample, waits sample to be available and then returns the data from sensor.
  * Continuous keeps the sensor running on the background regardless of how often data is read.
  * Continuous is recommended mode for most applications.
  */
  #define APPLICATION_ACCELEROMETER_MODE RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS

  /**
  * @brief activity interrupt threshold for accelerometer in G.
  * Maximum is the scale of accelerometer.
  */
  #define APPLICATION_ACCELEROMETER_ACTIVITY_THRESHOLD 0.100f
#endif

/** @brief enable compiling accelerometer interface */
#define RUUVI_INTERFACE_ACCELERATION_ENABLED 1

/** @brief Enable compiling lis2dh12 accelerometer support */
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
#define APPLICATION_ADC_SAMPLE_INTERVAL_MS 3000 // Valid for single mode

/**
 * Bluetooth configuration
 */
// Avoid "even" values such as 100 or 1000 to eventually drift apart from the devices transmitting at same interval
#ifndef APPLICATION_ADVERTISING_CONFIGURED
#if DEBUG
  #define APPLICATION_ADVERTISING_INTERVAL              211  //!< Apple guidelines, exact value would be 211.25  
#else
  #define APPLICATION_ADVERTISING_INTERVAL              1285 //!< Apple guidelines max interval
#endif
  #define APPLICATION_ADVERTISING_POWER                 RUUVI_BOARD_TX_POWER_MAX
  #define APPLICATION_DATA_FORMAT                       0x05
  #define APPLICATION_STANDBY_INTERVAL                  9900
  #define APPLICATION_ADVERTISEMENT_UPDATE_INTERVAL     APPLICATION_ADVERTISING_INTERVAL
#endif

// Apple connection parameter quidelines:
// Slave latency <= 30
// 2 seconds <= timeout <= 6 seconds
// Interval MIN % 15 ms == 0. Interval MIN >= 15 ms.
// Interval MIN + 15 ms <= Interval MAX OR Interval MIN == Interval MAX == 15
// Interval Max * (slave latency + 1) <= 2 seconds
// Interval MAX + (slave latency +1) * 3 < connSupervisionTimeout
#define APPLICATION_GATT_CONN_INTERVAL_MIN_MS      15
#define APPLICATION_GATT_CONN_INTERVAL_MAX_MS      15
#define APPLICATION_GATT_CONN_SLAVE_SKIP_INTERVALS 31   // Slave latency. How many intervals can be skipped. 31 * 45 < 2000
#define APPLICATION_GATT_CONN_TIMEOUT_MS           5600 // 31 * 45 * 3 < 5600

/**
 * NFC configuration
 */
// Longest text in a text field, i.e. "FW: ojousima.logger.c 3.10.0"
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
#define APPLICATION_TASK_DATA_MAX_SIZE    32   
#define APPLICATION_TASK_QUEUE_MAX_LENGTH 20

/**
 * Flags which determine which c-modules are compiled in.
 * These modules may reserve some RAM and FLASH, so if you
 * do not need module you can disable it. The modules might also
 * have some dependencies between themselves.
 */
#define APPLICATION_ADC_ENABLED                     1
#define APPLICATION_ATOMIC_ENABLED                  1
#define APPLICATION_BUTTON_ENABLED                  RUUVI_BOARD_BUTTONS_NUMBER
#define APPLICATION_COMMUNICATION_ENABLED           1 // Common functions for communication
#define APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED 1 // Advertising and GATT
#define APPLICATION_COMMUNICATION_NFC_ENABLED       1 // NFC
#define APPLICATION_FLASH_ENABLED                   1
#define APPLICATION_GPIO_ENABLED                    1
#define APPLICATION_GPIO_INTERRUPT_ENABLED          1
#define APPLICATION_ENVIRONMENTAL_MCU_ENABLED       1
#define APPLICATION_ENVIRONMENTAL_BME280_ENABLED    1
#define APPLICATION_I2C_ENABLED                     1
#define APPLICATION_POWER_ENABLED                   1
#define APPLICATION_RTC_MCU_ENABLED                 1
#define APPLICATION_SCHEDULER_ENABLED               1
#define APPLICATION_SPI_ENABLED                     1
#define APPLICATION_TIMER_ENABLED                   1
#define APPLICATION_TIMER_MAX_INSTANCES             10 ///< Timers are allocated statically on RAM
#define APPLICATION_WATCHDOG_ENABLED                1
#if DEBUG
  #define APPLICATION_WATCHDOG_INTERVAL_MS            1200000u
#else
  #define APPLICATION_WATCHDOG_INTERVAL_MS            12000u
#endif
#define APPLICATION_YIELD_ENABLED                   1
#define APPLICATION_LOG_ENABLED                     1
// RUUVI_INTERFACE_LOG_ ERROR, WARNING, INFO, DEBUG
#define APPLICATION_LOG_LEVEL                       RUUVI_INTERFACE_LOG_INFO

// Choose one. RTT is recommended, but does not work on devices
// with readback protection enabled
#define APPLICATION_LOG_BACKEND_RTT_ENABLED         1
//#define APPLICATION_LOG_BACKEND_UART_ENABLED        0 // UART not implemented

// 10 * 1024 words (4096 bytyes) = 40960 bytes = 40 kB.
#define  APPLICATION_FLASH_DATA_PAGES_NUMBER        10

#endif