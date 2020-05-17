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

/** @brief If watchdog is not fed at this interval or faster, reboot */
#ifndef APP_WDT_INTERVAL_MS
#   define APP_WDT_INTERVAL_MS (2U*60U*1000U)
#endif

#define APP_SENSOR_TMP117_ENABLED 0
#ifndef APP_SENSOR_TMP117_ENABLED
#   define APP_SENSOR_TMP117_ENABLED RB_ENVIRONMENTAL_TMP117_PRESENT
#endif

#ifndef APP_SENSOR_BME280_ENABLED
#   define APP_SENSOR_BME280_ENABLED RB_ENVIRONMENTAL_BME280_PRESENT
#endif

#ifndef RI_BME280_ENABLED
#   define RI_BME280_ENABLED APP_SENSOR_BME280_ENABLED
#endif 

#ifndef APP_SENSOR_LIS2DH12_ENABLED
#   define APP_SENSOR_LIS2DH12_ENABLED RB_ACCELEROMETER_LIS2DH12_PRESENT
#endif

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
#   define APP_SENSOR_SHTCX_RESOLUTION RD_SENSOR_CFG_DEFAULT
#endif
#ifndef APP_SENSOR_SHTCX_SAMPLERATE
#   define APP_SENSOR_SHTCX_SAMPLERATE RD_SENSOR_CFG_DEFAULT //!< SHTC sample rate is defined by reads.
#endif
#ifndef APP_SENSOR_SHTCX_SCALE
#   define APP_SENSOR_SHTCX_SCALE RD_SENSOR_CFG_DEFAULT //!< Only default is valid.
#endif

#ifndef APP_NFC_ENABLED
#   define APP_NFC_ENABLED RB_NFC_INTERNAL_INSTALLED
#endif 

#ifndef APP_ADV_ENABLED
#   define APP_ADV_ENABLED 1
#endif

#define APP_GATT_ENABLED 0
#ifndef APP_GATT_ENABLED
#   define APP_GATT_ENABLED (RB_APP_PAGES > 0U) //!< If Flash is at premium, cut GATT off by default
#endif

// ***** Flash storage constants *****/
// These constants can be any non-zero uint8, but two files and two records in same file can't have same ID.
#define APP_FLASH_SENSOR_FILE (0xCEU)
#define APP_FLASH_SENSOR_SHTCX_RECORD    (0xC3U)
#define APP_FLASH_SENSOR_LIS2DH12_RECORD (0x2DU)
#define APP_FLASH_SENSOR_BME280_RECORD   (0x28U)

/** @brief enable nRF15 SDK implementation of drivers */
#define RUUVI_NRF5_SDK15_ENABLED (1U)

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
 * @brief Allocate RAM for the interrupt function pointers.
 *
 * GPIOs are indexed by GPIO number starting from 1, so size is GPIO_NUM + 1.
 */
#ifndef RT_GPIO_INT_TABLE_SIZE
#    define RT_GPIO_INT_TABLE_SIZE (RB_GPIO_NUMBER + 1U)
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

/** @brief Enable Ruuvi NFC tasks. */
#ifndef RT_NFC_ENABLED
#   define RT_NFC_ENABLED (1U)
#endif

/** @brief Enable Ruuvi Power interface. */
#ifndef RI_POWER_ENABLED
#   define RI_POWER_ENABLED (1U)
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
