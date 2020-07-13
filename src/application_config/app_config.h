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
/** @ }*/
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

/** @brief If watchdog is not fed at this interval or faster, reboot */
#ifndef APP_WDT_INTERVAL_MS
#   define APP_WDT_INTERVAL_MS (2U*60U*1000U)
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

#define APP_SENSOR_TMP117_ENABLED 0
#ifndef APP_SENSOR_TMP117_ENABLED
#   define APP_SENSOR_TMP117_ENABLED RB_ENVIRONMENTAL_TMP117_PRESENT
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
#   define APP_GATT_ENABLED (RB_APP_PAGES > 0U) //!< If Flash is at premium, cut GATT off by default
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

// ***** Flash storage constants *****/
// These constants can be any non-zero uint8, but two files and two records in same file can't have same ID.
#define APP_FLASH_SENSOR_FILE (0xCEU)
#define APP_FLASH_SENSOR_NTC_RECORD      (0xC1U)
#define APP_FLASH_SENSOR_PHOTO_RECORD    (0xC2U)
#define APP_FLASH_SENSOR_SHTCX_RECORD    (0xC3U)
#define APP_FLASH_SENSOR_LIS2DH12_RECORD (0x2DU)
#define APP_FLASH_SENSOR_BME280_RECORD   (0x28U)

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
 * @brief enable Ruuvi ADC interface.
 *
 * Required by sensor
 */
#ifndef RI_ADC_ENABLED
#   define RI_ADC_ENABLED (1U)
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

/**
 * @brief Enable Ruuvi Flash interface on boards with enough RAM & Flash
 */
#ifndef RI_FLASH_ENABLED
#   define RI_FLASH_ENABLED (RB_APP_PAGES > 0U)
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
#define RI_LOG_ENABLED (0U)
#define APP_LOG_LEVEL RI_LOG_LEVEL_NONE
#endif

/*@}*/
#endif
