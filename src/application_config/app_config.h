#ifndef APP_CONFIG_H
#define APP_CONFIG_H
#include "application_modes.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_sensor.h"

/**
 * @defgroup Configuration Application configuration
 */

/** @{ */
/**
 * @defgroup app_config Application configuration
 * @brief Configure application enabled modules and parameters.
 */
/** @}*/
/**
 * @addtogroup SDK15
 */
/** @{ */
/**
 * @file app_config.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-04-16
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 */

/** @brief enable nRF15 SDK implementation of drivers */
#define RUUVI_NRF5_SDK15_ENABLED (1U)

#ifndef APP_HEARTBEAT_OVERDUE_INTERVAL_MS
#   define APP_HEARTBEAT_OVERDUE_INTERVAL_MS (5U * 60U * 1000U)
#endif

/** @brief If watchdog is not fed at this interval or faster, reboot */
#ifndef APP_WDT_INTERVAL_MS
#   define APP_WDT_INTERVAL_MS (APP_HEARTBEAT_OVERDUE_INTERVAL_MS + (1U*60U*1000U))
#endif

/** @brief Enable sensor tasks */
#ifndef RT_SENSOR_ENABLED
#   define RT_SENSOR_ENABLED (1U)
#endif

/** @brief Enable photosensor */
#ifndef APP_SENSOR_PHOTO_ENABLED
#   define APP_SENSOR_PHOTO_ENABLED RB_ENVIRONMENTAL_PHOTO_PRESENT
#endif

/** @brief Enable Photodiode driver */
#ifndef RI_ADC_PHOTO_ENABLED
#    define RI_ADC_PHOTO_ENABLED APP_SENSOR_PHOTO_ENABLED
#endif

/** @brief Enable NTC sensor */
#ifndef APP_SENSOR_NTC_ENABLED
#   define APP_SENSOR_NTC_ENABLED RB_ENVIRONMENTAL_NTC_PRESENT
#endif

/** @brief Enable NTC driver */
#ifndef RI_ADC_NTC_ENABLED
#   define RI_ADC_NTC_ENABLED APP_SENSOR_NTC_ENABLED
#endif

/** @brief Enable BME280 temperature, humidity, pressure sensor */
#ifndef APP_SENSOR_BME280_ENABLED
#   define APP_SENSOR_BME280_ENABLED RB_ENVIRONMENTAL_BME280_PRESENT
#endif

/** @brief Enable BME280 driver */
#ifndef RI_BME280_ENABLED
#   define RI_BME280_ENABLED APP_SENSOR_BME280_ENABLED
#   define RI_BME280_SPI_ENABLED RB_ENVIRONMENTAL_BME280_SPI_USE
#   define RI_BME280_I2C_ENABLED RB_ENVIRONMENTAL_BME280_I2C_USE
#endif

#ifndef APP_SENSOR_BME280_DSP_FUNC
#   define APP_SENSOR_BME280_DSP_FUNC RD_SENSOR_DSP_LOW_PASS
#endif
#ifndef APP_SENSOR_BME280_DSP_PARAM
#   define APP_SENSOR_BME280_DSP_PARAM (16U)
#endif
#ifndef APP_SENSOR_BME280_MODE
#   define APP_SENSOR_BME280_MODE RD_SENSOR_CFG_CONTINUOUS
#endif
#ifndef APP_SENSOR_BME280_RESOLUTION
#   define APP_SENSOR_BME280_RESOLUTION RD_SENSOR_CFG_DEFAULT
#endif
#ifndef APP_SENSOR_BME280_SAMPLERATE
#   define APP_SENSOR_BME280_SAMPLERATE (1U)
#endif
#ifndef APP_SENSOR_BME280_SCALE
#   define APP_SENSOR_BME280_SCALE RD_SENSOR_CFG_DEFAULT //!< Only default is valid.
#endif

/** @brief Enable LIS2DH12 sensor */
#ifndef APP_SENSOR_LIS2DH12_ENABLED
#   define APP_SENSOR_LIS2DH12_ENABLED RB_ACCELEROMETER_LIS2DH12_PRESENT
#endif
#ifndef APP_SENSOR_LIS2DH12_DSP_FUNC
#   define APP_SENSOR_LIS2DH12_DSP_FUNC RD_SENSOR_DSP_LAST
#endif
#ifndef APP_SENSOR_LIS2DH12_DSP_PARAM
#   define APP_SENSOR_LIS2DH12_DSP_PARAM (1U)
#endif
#ifndef APP_SENSOR_LIS2DH12_MODE
#   define APP_SENSOR_LIS2DH12_MODE RD_SENSOR_CFG_CONTINUOUS
#endif
#ifndef APP_SENSOR_LIS2DH12_RESOLUTION
#   define APP_SENSOR_LIS2DH12_RESOLUTION (10U) //!< bits
#endif
#ifndef APP_SENSOR_LIS2DH12_SAMPLERATE
#   define APP_SENSOR_LIS2DH12_SAMPLERATE (10U) //!< Hz
#endif
#ifndef APP_SENSOR_LIS2DH12_SCALE
#   define APP_SENSOR_LIS2DH12_SCALE (2U) //!< G
#endif

/** @brief Enable LIS2DH12 driver */
#ifndef RI_LIS2DH12_ENABLED
#    define RI_LIS2DH12_ENABLED APP_SENSOR_LIS2DH12_ENABLED
#endif

/** @brief Enable SHTCX sensor */
#ifndef APP_SENSOR_SHTCX_ENABLED
#   define APP_SENSOR_SHTCX_ENABLED RB_ENVIRONMENTAL_SHTCX_PRESENT
#endif

#ifndef APP_SENSOR_SHTCX_DSP_FUNC
#   define APP_SENSOR_SHTCX_DSP_FUNC RD_SENSOR_DSP_LAST //!< DSP function to use, only LAST is supported.
#endif
#ifndef APP_SENSOR_SHTCX_DSP_PARAM
#   define APP_SENSOR_SHTCX_DSP_PARAM 1 //!< Only 1 is valid with LAST
#endif
#ifndef APP_SENSOR_SHTCX_MODE
#   define APP_SENSOR_SHTCX_MODE RD_SENSOR_CFG_CONTINUOUS //!< SHTC runs in single-shot mode internally, update data automatically on fetch.
#endif
#ifndef APP_SENSOR_SHTCX_RESOLUTION
#   define APP_SENSOR_SHTCX_RESOLUTION RD_SENSOR_CFG_DEFAULT //!< Only default resolution supported.
#endif
#ifndef APP_SENSOR_SHTCX_SAMPLERATE
#   define APP_SENSOR_SHTCX_SAMPLERATE RD_SENSOR_CFG_DEFAULT //!< SHTC sample rate is defined by reads.
#endif
#ifndef APP_SENSOR_SHTCX_SCALE
#   define APP_SENSOR_SHTCX_SCALE RD_SENSOR_CFG_DEFAULT //!< Only default is valid.
#endif

/** @brief Enable SHTCX driver */
#ifndef RI_SHTCX_ENABLED
#   define RI_SHTCX_ENABLED APP_SENSOR_SHTCX_ENABLED
#endif

/** @brief Enable TMP117 temperature sensor */
#ifndef APP_SENSOR_TMP117_ENABLED
#   define APP_SENSOR_TMP117_ENABLED RB_ENVIRONMENTAL_TMP117_PRESENT
#endif

#ifndef APP_SENSOR_TMP117_DSP_FUNC
#   define APP_SENSOR_TMP117_DSP_FUNC RD_SENSOR_DSP_LAST //!< Do not use DSP by default
#endif
#ifndef APP_SENSOR_TMP117_DSP_PARAM
#   define APP_SENSOR_TMP117_DSP_PARAM 1 //!< Only 1 is valid with LAST
#endif
#ifndef APP_SENSOR_TMP117_MODE
#   define APP_SENSOR_TMP117_MODE RD_SENSOR_CFG_CONTINUOUS    //!< TMP117 runs continuously internally.
#endif
#ifndef APP_SENSOR_TMP117_RESOLUTION
#   define APP_SENSOR_TMP117_RESOLUTION RD_SENSOR_CFG_DEFAULT //!< Only default resolution supported.
#endif
#ifndef APP_SENSOR_TMP117_SAMPLERATE
#   define APP_SENSOR_TMP117_SAMPLERATE RD_SENSOR_CFG_DEFAULT //!<  sample rate is defined by reads.
#endif
#ifndef APP_SENSOR_TMP117_SCALE
#   define APP_SENSOR_TMP117_SCALE RD_SENSOR_CFG_DEFAULT //!< Only default is valid.
#endif

/** @brief Enable TMP117 driver */
#ifndef RI_TMP117_ENABLED
#   define RI_TMP117_ENABLED APP_SENSOR_TMP117_ENABLED
#endif

/** @brief Enable nRF52 temperature sensor */
#ifndef APP_SENSOR_ENVIRONMENTAL_MCU_ENABLED
#   define APP_SENSOR_ENVIRONMENTAL_MCU_ENABLED RB_ENVIRONMENTAL_MCU_PRESENT
#endif

#ifndef RUUVI_NRF5_SDK15_NRF52832_ENVIRONMENTAL_ENABLED
#   define RUUVI_NRF5_SDK15_NRF52832_ENVIRONMENTAL_ENABLED APP_SENSOR_ENVIRONMENTAL_MCU_ENABLED
#endif

#ifndef APP_SENSOR_NRF52_DSP_FUNC
#   define APP_SENSOR_NRF52_DSP_FUNC RD_SENSOR_DSP_LAST //!< DSP function to use, only LAST is supported.
#endif
#ifndef APP_SENSOR_NRF52_DSP_PARAM
#   define APP_SENSOR_NRF52_DSP_PARAM (1U) //!< Only 1 is valid with LAST
#endif
#ifndef APP_SENSOR_NRF52_MODE
#   define APP_SENSOR_NRF52_MODE RD_SENSOR_CFG_CONTINUOUS //!< SHTC runs in single-shot mode internally, update data automatically on fetch.
#endif
#ifndef APP_SENSOR_NRF52_RESOLUTION
#   define APP_SENSOR_NRF52_RESOLUTION RD_SENSOR_CFG_DEFAULT //!< Only default resolution supported.
#endif
#ifndef APP_SENSOR_NRF52_SAMPLERATE
#   define APP_SENSOR_NRF52_SAMPLERATE RD_SENSOR_CFG_DEFAULT //!< NRF52 sample rate is defined by reads.
#endif
#ifndef APP_SENSOR_NRF52_SCALE
#   define APP_SENSOR_NRF52_SCALE RD_SENSOR_CFG_DEFAULT //!< Only default is valid.
#endif



/** @brief Enable DPS310 sensor */
#ifndef APP_SENSOR_DPS310_ENABLED
#   define APP_SENSOR_DPS310_ENABLED RB_ENVIRONMENTAL_DPS310_PRESENT
#   ifndef RI_DPS310_SPI_ENABLED
#       define RI_DPS310_SPI_ENABLED (1U)
#   endif
#endif

#ifndef APP_SENSOR_DPS310_DSP_FUNC
#   define APP_SENSOR_DPS310_DSP_FUNC RD_SENSOR_DSP_LAST //!< DSP function to use, LAST and OVERSAMPLING supported.
#endif
#ifndef APP_SENSOR_DPS310_DSP_PARAM
#   define APP_SENSOR_DPS310_DSP_PARAM 1 //!< Only 1 is valid with LAST
#endif
#ifndef APP_SENSOR_DPS310_MODE
#   define APP_SENSOR_DPS310_MODE RD_SENSOR_CFG_CONTINUOUS //!< Run in background
#endif
#ifndef APP_SENSOR_DPS310_RESOLUTION
#   define APP_SENSOR_DPS310_RESOLUTION RD_SENSOR_CFG_DEFAULT //!< Only default resolution supported.
#endif
#ifndef APP_SENSOR_DPS310_SAMPLERATE
#   define APP_SENSOR_DPS310_SAMPLERATE RD_SENSOR_CFG_DEFAULT //!< 1 Hz by default
#endif
#ifndef APP_SENSOR_DPS310_SCALE
#   define APP_SENSOR_DPS310_SCALE RD_SENSOR_CFG_DEFAULT //!< Only default is valid.
#endif


/** @brief Enable DPS310 driver */
#ifndef RI_DPS310_ENABLED
#   define RI_DPS310_ENABLED APP_SENSOR_DPS310_ENABLED
#endif

/** @brief Enable atomic operations */
#ifndef RI_ATOMIC_ENABLED
#   define RI_ATOMIC_ENABLED (1U)
#endif

/** @brief Enable Ruuvi NFC in application. */
#ifndef APP_NFC_ENABLED
#   define APP_NFC_ENABLED RB_NFC_INTERNAL_INSTALLED
#endif

/** @brief Enable Ruuvi NFC tasks. */
#ifndef RT_NFC_ENABLED
#   define RT_NFC_ENABLED APP_NFC_ENABLED
#endif

/** @brief Enable BLE advertising in application. */
#ifndef APP_ADV_ENABLED
#   define APP_ADV_ENABLED 1
#endif

/** @brief Enable Radio interface. */
#ifndef RI_RADIO_ENABLED
#   define RI_RADIO_ENABLED APP_ADV_ENABLED
#endif

/** @brief Select modulation used in application */
#ifndef APP_MODULATION
#   if(RB_BLE_CODED_SUPPORTED)
#       define APP_MODULATION RI_RADIO_BLE_125KBPS
#   elif(RB_BLE_2MBPS_SUPPORTED)
#       define APP_MODULATION RI_RADIO_BLE_2MBPS
#   else
#       define APP_MODULATION RI_RADIO_BLE_1MBPS
#   endif
#endif

/** @brief Enable Advertising tasks. */
#ifndef RT_ADV_ENABLED
#   define RT_ADV_ENABLED APP_ADV_ENABLED
#endif

#ifndef APP_GATT_ENABLED
//!< If Flash is at premium, cut GATT off by default.
#   define APP_GATT_ENABLED (RB_FLASH_SPACE_AVAILABLE > RB_FLASH_SPACE_SMALL)
#endif

/** @brief Enable GATT tasks */
#ifndef RT_GATT_ENABLED
#   define RT_GATT_ENABLED APP_GATT_ENABLED
#endif

/** @brief Enable communication tasks */
#ifndef RT_COMMUNICATION_ENABLED
#   define RT_COMMUNICATION_ENABLED (RT_NFC_ENABLED | RI_RADIO_ENABLED)
#endif

/** @brief Enable communication interface */
#ifndef RI_COMM_ENABLED
#   define RI_COMM_ENABLED RT_COMMUNICATION_ENABLED
#endif

/** @brief Enable bidirectional communication */
#ifndef APP_COMMS_BIDIR_ENABLED
#   define APP_COMMS_BIDIR_ENABLED ((APP_GATT_ENABLED) + (APP_NFC_ENABLED))
#endif

/** @brief Enable Flash tasks if there is storage space */
#ifndef RT_FLASH_ENABLED
#   define RT_FLASH_ENABLED (RB_FLASH_SPACE_AVAILABLE > RB_FLASH_SPACE_SMALL)
#endif

/** @brief Enable Ruuvi Flash interface. */
#define RI_FLASH_ENABLED RT_FLASH_ENABLED

// ***** Flash storage constants *****/

#define APP_FLASH_PAGES (16U) //!< 64 kB flash storage if page size is 4 kB.
#define APP_FLASH_LOG_DATA_RECORDS_NUM   (APP_FLASH_PAGES - 2U) //!< swap page + settings.

// File constants can be any non-zero uint8.
// Record constants can be any non-zero uint16
// Two files and two records in same file can't have same ID.
#define APP_FLASH_SENSOR_FILE (0xCEU)
#define APP_FLASH_SENSOR_BME280_RECORD   (0x28U)
#define APP_FLASH_SENSOR_DPS310_RECORD   (0x31U)
#define APP_FLASH_SENSOR_ENVI_RECORD     (0x52U)
#define APP_FLASH_SENSOR_LIS2DH12_RECORD (0x2DU)
#define APP_FLASH_SENSOR_NTC_RECORD      (0xC1U)
#define APP_FLASH_SENSOR_PHOTO_RECORD    (0xC2U)
#define APP_FLASH_SENSOR_SHTCX_RECORD    (0xC3U)
#define APP_FLASH_SENSOR_TMP117_RECORD   (0x17U)



#define APP_FLASH_LOG_FILE                (0xF0U)
#define APP_FLASH_LOG_CONFIG_RECORD       (0x01U)
#define APP_FLASH_LOG_BOOT_COUNTER_RECORD (0xEFU)
#define APP_FLASH_LOG_DATA_RECORD_PREFIX  (0xF0U) //!< Prefix, append with U8 number


// ** Logging constants ** //
#ifndef APP_LOG_INTERVAL_S
#   define APP_LOG_INTERVAL_S (5U * 60U)
#endif
#ifndef APP_LOG_OVERFLOW
#   define APP_LOG_OVERFLOW (true)
#endif
#ifndef APP_LOG_TEMPERATURE_ENABLED
#   define APP_LOG_TEMPERATURE_ENABLED (true)
#endif
#ifndef APP_LOG_HUMIDITY_ENABLED
#   define APP_LOG_HUMIDITY_ENABLED (true)
#endif
#ifndef APP_LOG_PRESSURE_ENABLED
#   define APP_LOG_PRESSURE_ENABLED (true)
#endif
#ifndef APP_FLASH_LOG_CONFIG_NVM_ENABLED
#   define APP_FLASH_LOG_CONFIG_NVM_ENABLED  (0U)
#endif

/** @brief Enable ADC tasks */
#ifndef RT_ADC_ENABLED
#   define RT_ADC_ENABLED (1U)
#endif

/** @brief enable Ruuvi Button tasks. Reset button works regardless of this setting. */
#ifndef RT_BUTTON_ENABLED
#   define RT_BUTTON_ENABLED (1U)
#endif

/** @brief enable Ruuvi GPIO tasks. */
#ifndef RT_GPIO_ENABLED
#   define RT_GPIO_ENABLED (1U)
#endif

/**
 * @brief enable Ruuvi GPIO interface.
 *
 * Required by SPI, LED, Button and GPIO tasks.
 */
#ifndef RI_GPIO_ENABLED
#   define RI_GPIO_ENABLED (1U)
#endif



/**
 * @brief Enable Ruuvi ADC interface.
 *
 * Required by sensor
 */
#ifndef RI_ADC_ENABLED
#   define RI_ADC_ENABLED (1U)
#endif

/**
 * @brief Enable all possible dataformats for unit testing.
 */
#ifdef CEEDLING
#  define ENABLE_ALL_DATAFORMATS (1U)
#else
#  define ENABLE_ALL_DATAFORMATS (0U)
#endif

/**
 * @brief Enable legacy raw dataformat
 */
#ifndef RE_3_ENABLED
#   define RE_3_ENABLED  (0U + ENABLE_ALL_DATAFORMATS)
#endif

/**
 * @brief Enable official raw dataformat
 */
#ifndef RE_5_ENABLED
#   define RE_5_ENABLED  (1U + ENABLE_ALL_DATAFORMATS)
#endif

/**
 * @brief Enable official encrypted dataformat
 */
#ifndef RE_8_ENABLED
#   define RE_8_ENABLED  (0U + ENABLE_ALL_DATAFORMATS)
#endif

/**
 * @brief Enable legacy encrypted dataformat
 */
#ifndef RE_FA_ENABLED
#   define RE_FA_ENABLED (0U + ENABLE_ALL_DATAFORMATS)
#endif

/**
 * @brief Enable Ruuvi AES interface.
 *
 * Required by Dataformats 8, FA. Boards with little flash can't support mbed tls.
 */
#ifndef RI_AES_ENABLED
#   define RI_AES_ENABLED (RE_8_ENABLED | RE_FA_ENABLED)
#endif

/**
 * @brief Allocate RAM for the interrupt function pointers.
 *
 * GPIOs are indexed by GPIO number starting from 1, so size is GPIO_NUM + 1.
 */
#ifndef RT_GPIO_INT_TABLE_SIZE
#    define RT_GPIO_INT_TABLE_SIZE (RB_GPIO_NUMBER + 1U)
#endif

/** @brief Enable Ruuvi I2C interface. */
#ifndef RI_I2C_ENABLED
#   define RI_I2C_ENABLED (1U)
#endif

/** @brief Enable Ruuvi led tasks. */
#ifndef RT_LED_ENABLED
#   define RT_LED_ENABLED (1U)
#endif

/** @brief Allocate memory for LED pins. */
#ifndef RT_MAX_LED_CFG
#   define RT_MAX_LED_CFG RB_LEDS_NUMBER
#endif

/** @brief Enable Ruuvi Power interface. */
#ifndef RI_POWER_ENABLED
#   define RI_POWER_ENABLED (1U)
#endif

/** @brief Enable Ruuvi RTC interface. */
#ifndef RI_RTC_ENABLED
#   define RI_RTC_ENABLED (1U)
#endif

/** @brief Enable Ruuvi Scheduler interface. */
#ifndef RI_SCHEDULER_ENABLED
#   define RI_SCHEDULER_ENABLED (1U)
#endif

/** @brief Enable Ruuvi SPI interface. */
#ifndef RI_SPI_ENABLED
#   define RI_SPI_ENABLED (1U)
#endif

/** @brief Enable Ruuvi Timer interface. */
#ifndef RI_TIMER_ENABLED
#   define RI_TIMER_ENABLED (1U)
#endif

/** @brief Enable Ruuvi Yield interface. */
#ifndef RI_YIELD_ENABLED
#   define RI_YIELD_ENABLED (1U)
#endif

/** @brief Enable Ruuvi Watchdog interface. */
#ifndef RI_WATCHDOG_ENABLED
#   define RI_WATCHDOG_ENABLED (1U)
#endif

#ifndef APP_FW_NAME
#   define APP_FW_NAME "Ruuvi FW"
#endif

/** @brief Logs reserve lot of flash, enable only on debug builds */
#ifndef RI_LOG_ENABLED
#   define RI_LOG_ENABLED (0U)
#   define APP_LOG_LEVEL RI_LOG_LEVEL_NONE
#endif

/** @}*/
#endif
