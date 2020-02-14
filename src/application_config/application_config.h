/**
 * Application configuration constants
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H
#include "application_modes.h" // Includes different modes, such as long-life with low sampling rate and tx rate.
#include "ruuvi_boards.h"      // Includes information such as number of buttons and leds onboard

/**
 * @brief Version string, displayed in NFC read and GATT data on DIS.
 * The actual version shall be based on Git tag / commit and passed from makefile while compiling packages for distribution.
**/
#ifndef APPLICATION_FW_VERSION
#define APPLICATION_FW_VERSION "RuuviFW DEVEL"
#endif

/**
 * @brief enable compilation of NRF5 SDK15 implementation of driver interface functions.
 * Only one implementation can be enabled at a time.
 */
#define RUUVI_NRF5_SDK15_ENABLED              1



#if NRF52811_XXAA
#define APPLICATION_ENVIRONMENTAL_RAMBUFFER_SIZE 512
#else
#define APPLICATION_ENVIRONMENTAL_RAMBUFFER_SIZE 16384
#endif

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
#define APPLICATION_ADC_MODE               RUUVI_DRIVER_SENSOR_CFG_SLEEP // Note: call to task_adc_sample will leave the ADC in single-shot mode. Use data get only in continuous mode
#define APPLICATION_ADC_SAMPLE_INTERVAL_MS (60*1000) // Valid for single mode

/**
 * Bluetooth configuration
 */
#ifndef APPLICATION_ADVERTISING_POWER_DBM
#  define APPLICATION_ADVERTISING_POWER_DBM             RUUVI_BOARD_TX_POWER_MAX
#endif
#define APPLICATION_DATA_FORMAT                       0x05


// Scanning configuration
#define APPLICATION_BLE_SCAN_ENABLED     0     //!< Do not scan advertisements by default.
#define APPLICATION_BLE_SCAN_BUFFER_SIZE 31    //!< Bytes in BLE scanner buffer. 31 for normal advertisements, 256 for extended. 
#define APPLICATION_SCAN_INTERVAL_MS     10000
#define APPLICATION_SCAN_WINDOW_MS       100
#define APPLICATION_SCAN_TIMEOUT_MS      10000

// Apple connection parameter quidelines:
// Slave latency <= 30
// 2 seconds <= timeout <= 6 seconds
// Interval MIN % 15 ms == 0. Interval MIN >= 15 ms.
// Interval MIN + 15 ms <= Interval MAX OR Interval MIN == Interval MAX == 15
// Interval Max * (slave latency + 1) <= 2 seconds
// Interval MAX + (slave latency +1) * 3 < connSupervisionTimeout
#define APPLICATION_GATT_CONN_INTERVAL_MIN_MS      15
#define APPLICATION_GATT_CONN_INTERVAL_MAX_MS      15
#define APPLICATION_GATT_CONN_SLAVE_SKIP_INTERVALS 29   // Slave latency. How many intervals can be skipped.
#define APPLICATION_GATT_CONN_TIMEOUT_MS           5600 // 29 * 45 * 3 < 5600

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
 * @brief Task scheduler configuration
 * Data max size must accommodate ruuvi_standard_message_t
 * Queue must be long enough to handle extended GATT connection events
 * pushing tasks to queues.
 */

#define APPLICATION_TASK_DATA_MAX_SIZE    32
#if NRF52811
// Shorter queue is safe if there's no long GATT events
#define APPLICATION_TASK_QUEUE_MAX_LENGTH 5
#else
#define APPLICATION_TASK_QUEUE_MAX_LENGTH 20
#endif

/**
 * Flags which determine which c-modules are compiled in.
 * These modules may reserve some RAM and FLASH, so if you
 * do not need module you can disable it. The modules might also
 * have some dependencies between themselves.
 */
#define APPLICATION_ACCELERATION_ENABLED              1
#define APPLICATION_ADC_ENABLED                       1
#define APPLICATION_ATOMIC_ENABLED                    1
#define APPLICATION_BUTTON_ENABLED                    RUUVI_BOARD_BUTTONS_NUMBER
#define APPLICATION_COMMUNICATION_ENABLED             1 // Common functions for communication
#define APPLICATION_COMMUNICATION_ADVERTISING_ENABLED 1 // BLE Advertising
#if NRF52811_XXAA
#define APPLICATION_COMMUNICATION_GATT_ENABLED        0 // BLE GATT, requiresh Flash and advertising
#else
#define APPLICATION_COMMUNICATION_GATT_ENABLED        0 // BLE GATT, requiresh Flash and advertising
#endif
#define APPLICATION_COMMUNICATION_NFC_ENABLED         RUUVI_BOARD_NFC_INTERNAL_INSTALLED
#define APPLICATION_GPIO_ENABLED                      1
#define APPLICATION_GPIO_INTERRUPT_ENABLED            1
#define APPLICATION_ENVIRONMENTAL_ENABLED             1
#define APPLICATION_ENVIRONMENTAL_BME280_ENABLED      (1 && RUUVI_BOARD_ENVIRONMENTAL_BME280_PRESENT)
#define APPLICATION_ENVIRONMENTAL_MCU_ENABLED         (1 && RUUVI_BOARD_ENVIRONMENTAL_MCU_PRESENT)
#define APPLICATION_ENVIRONMENTAL_TMP117_ENABLED      (1 && RUUVI_BOARD_ENVIRONMENTAL_TMP117_PRESENT)
#define APPLICATION_ENVIRONMENTAL_SHTCX_ENABLED       (1 && RUUVI_BOARD_ENVIRONMENTAL_SHTCX_PRESENT)

#define APPLICATION_I2C_ENABLED                       1
#define APPLICATION_POWER_ENABLED                     1
#define APPLICATION_RTC_MCU_ENABLED                   (RUUVI_BOARD_RTC_INSTANCES > 2)
#define APPLICATION_SCHEDULER_ENABLED                 1
#define APPLICATION_SPI_ENABLED                       1
#define APPLICATION_TIMER_ENABLED                     1
#if NRF52811_XXAA
#define APPLICATION_TIMER_MAX_INSTANCES               5 ///< Timers are allocated statically on RAM
#else
#define APPLICATION_TIMER_MAX_INSTANCES               10 ///< Timers are allocated statically on RAM
#endif
#define APPLICATION_WATCHDOG_ENABLED                  1

#define APPLICATION_YIELD_ENABLED                   1

/** Number of GPIO interrupt lines */
#define APPLICATION_GPIO_INTERRUPT_NUMBER           (2 * APPLICATION_ACCELERATION_ENABLED \
                                                     + RUUVI_BOARD_BUTTONS_NUMBER)


#if DEBUG
#define APPLICATION_LOG_ENABLED                     1
#else
#define APPLICATION_LOG_ENABLED                     0
#endif
// RUUVI_INTERFACE_LOG_ ERROR, WARNING, INFO, DEBUG
#define APPLICATION_LOG_LEVEL                       RUUVI_INTERFACE_LOG_INFO
/** @brief Bytes of RAM to conserve for printed log messages
 *  Pick a power of 2 for nRF5 backend. at least 128 is recommended.
 */
#if NRF52811_XXAA
#define APPLICATION_LOG_BUFFER_SIZE                128
#else
#define APPLICATION_LOG_BUFFER_SIZE                256
#endif

// Choose one. RTT is recommended, but does not work on devices
// with readback protection enabled
#define APPLICATION_LOG_BACKEND_RTT_ENABLED         1
//#define APPLICATION_LOG_BACKEND_UART_ENABLED      0 // UART not implemented

#if NRF52811_XXAA
#define APPLICATION_FLASH_ENABLED                       0
#else
#define APPLICATION_FLASH_ENABLED                       1
#endif
#define APPLICATION_FLASH_DATA_PAGES_NUMBER             RUUVI_BOARD_APP_PAGES
#define APPLICATION_FLASH_ENVIRONMENTAL_FILE            0xFE
#define APPLICATION_FLASH_ENVIRONMENTAL_SHTCX_RECORD    0xE0
#define APPLICATION_FLASH_ENVIRONMENTAL_BME280_RECORD   0xE1
#define APPLICATION_FLASH_ENVIRONMENTAL_NTC_RECORD      0xE2
#define APPLICATION_FLASH_ENVIRONMENTAL_MCU_RECORD      0xE3
#define APPLICATION_FLASH_ENVIRONMENTAL_LIS2DH12_RECORD 0xE4
#define APPLICATION_FLASH_ENVIRONMENTAL_TMP117_RECORD   0xE5
#define APPLICATION_FLASH_ERROR_FILE                    0xEE
#define APPLICATION_FLASH_ERROR_RECORD                  0xEE

#endif