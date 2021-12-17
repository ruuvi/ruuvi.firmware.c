/**
 * @file application_config /app_config.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-04-16
 *       2021-12-10 DG12 correct _FLASH_PAGES from 16 to 0x15000/4096-1 see loader configuration.
 * @brief Master configuration base
 * @details Variables which can be adjusted by application_mode_xxxxx.h are preceeded by #ifdef
 *
 * Types of configuration items:
 *  unconfigurable constatnts that should be moved elsewhere example:
 *                          LONG_PRESS_TIME,FLASH_*_FILE , FLASH_*_RECORD, _SENSOR_FILE, _SENSOR_ENVI_RECORD ..
 *  define the hardware present 
 *   Currently they are compile time defines which can be defined in _mode_xxx
 *
 *  Do not make customizations here. Use application_mode_xxxx.h
 *
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* Select and include a customization file for example:
   application_mode_longlife,h       
  Then always includes  application_default.h  */
#include "application_modes.h"
#include "ruuvi_boards.h"         // include appropriate ruuvi_board_xxxxx_b.h and RUUVI_BOARDS_SEMVER
#include "ruuvi_driver_sensor.h"  // Provide functions to init/uninit,
/*                                         set/get samplerate, dsp, scale, resolution. mode & get data  */

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

#ifndef APP_FW_NAME
#   define APP_FW_NAME "Ruuvi FW"
#endif

#ifndef APP_FW_VERSION
#define APP_FW_VERSION  "v3.0.0"  // Overridden by 'make' provided define: -DAPP_FW_VERSION ; from git describe --tags
#endif

/* @brief *******  Logging  i.e. history ********* */
//   The values sent from history log are:
#ifndef APP_LOG_TEMPERATURE_ENABLED
#define APP_LOG_TEMPERATURE_ENABLED (true)
#endif
#ifndef APP_LOG_HUMIDITY_ENABLED
#define APP_LOG_HUMIDITY_ENABLED    (true)
#endif
#ifndef APP_LOG_PRESSURE_ENABLED
#define APP_LOG_PRESSURE_ENABLED    (true)
#endif
#ifndef APP_LOG_X_ENABLED
#define APP_LOG_X_ENABLED           (false)
#endif
#ifndef APP_LOG_Y_ENABLED
#define APP_LOG_Y_ENABLED           (false)
#endif
#ifndef APP_LOG_Z_ENABLED
#define APP_LOG_Z_ENABLED           (false)
#endif
#ifndef APP_LOG_MOVEMENT_ENABLED
#define APP_LOG_MOVEMENT_ENABLED    (false)
#endif
#ifndef APP_LOG_BATTVOLT_ENABLED
#define APP_LOG_BATTVOLT_ENABLED    (false)
#endif

#ifndef APP_LOG_INTERVAL_S
#define APP_LOG_INTERVAL_S                  (5U * 60U)                           // 5 MINUTES!
#endif

// Not yet implemented 3.31.1 2021-12-14
#ifndef APP_LOG_OVERFLOW          //<! When history log fills flash: OVERFLOW true wrap around i.e. save most recent readings
#define APP_LOG_OVERFLOW true     //                                          false  save oldest reading and loose recent ones
#endif

/* brief LED blinking heart beat timing */
#ifndef APP_HEARTBEAT_OVERDUE_INTERVAL_MS
#   define APP_HEARTBEAT_OVERDUE_INTERVAL_MS (5U * 60U * 1000U)
#endif

/* @brief ******* Sensor avalibility and parameters ******** */

/** @brief photosensor */
#ifndef APP_SENSOR_PHOTO_ENABLED
#   define APP_SENSOR_PHOTO_ENABLED RB_ENVIRONMENTAL_PHOTO_PRESENT
#endif
/** @brief Photodiode driver */
#ifndef RI_ADC_PHOTO_ENABLED
#    define RI_ADC_PHOTO_ENABLED APP_SENSOR_PHOTO_ENABLED
#endif

/** @brief NTC precision temperature sensor */
#ifndef APP_SENSOR_NTC_ENABLED
#   define APP_SENSOR_NTC_ENABLED RB_ENVIRONMENTAL_NTC_PRESENT
#endif
/** @brief NTC driver */
#ifndef RI_ADC_NTC_ENABLED
#   define RI_ADC_NTC_ENABLED APP_SENSOR_NTC_ENABLED
#endif

/** @brief BME280 temperature, humidity, pressure sensor */
#ifndef APP_SENSOR_BME280_ENABLED
#   define APP_SENSOR_BME280_ENABLED RB_ENVIRONMENTAL_BME280_PRESENT
#endif

/** @brief BME280 driver */
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

/** @brief LIS2DH12 Accelerometer ensor */
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
/** @brief LIS2DH12 driver */
#ifndef RI_LIS2DH12_ENABLED
#    define RI_LIS2DH12_ENABLED APP_SENSOR_LIS2DH12_ENABLED
#endif

/** @brief SHTCX Humidity and Temperature sensor */
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
/** @brief SHTCX driver */
#ifndef RI_SHTCX_ENABLED
#   define RI_SHTCX_ENABLED APP_SENSOR_SHTCX_ENABLED
#endif

/** @brief TMP117 temperature sensor */
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
/** @brief TMP117 driver */
#ifndef RI_TMP117_ENABLED
#   define RI_TMP117_ENABLED APP_SENSOR_TMP117_ENABLED
#endif

/** @brief nRF52 on chip temperature sensor */
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
#   define APP_SENSOR_NRF52_DSP_PARAM (1u) //!< Only 1 is valid with LAST
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

/** @brief DPS310 barometric pressure sensor */
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
/** @brief DPS310 driver */
#ifndef RI_DPS310_ENABLED
#   define RI_DPS310_ENABLED APP_SENSOR_DPS310_ENABLED
#endif

/* @brief ******  Communications Features ******* */
/** @brief Ruuvi NFC. */
#ifndef APP_NFC_ENABLED
#   define APP_NFC_ENABLED RB_NFC_INTERNAL_INSTALLED
#endif
/** @brief Ruuvi NFC tasks. */
#ifndef RT_NFC_ENABLED
#   define RT_NFC_ENABLED APP_NFC_ENABLED
#endif

/** @brief BLE advertising in application. */
#ifndef APP_ADV_ENABLED
#   define APP_ADV_ENABLED (true)
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

/** @brief Advertising tasks. */
#ifndef RT_ADV_ENABLED
#   define RT_ADV_ENABLED APP_ADV_ENABLED
#endif

#ifndef APP_GATT_ENABLED
//!< If Flash is at premium, cut GATT off by default.
#   define APP_GATT_ENABLED (RB_FLASH_SPACE_AVAILABLE > RB_FLASH_SPACE_SMALL)
#endif
/** @brief GATT tasks */
#ifndef RT_GATT_ENABLED
#   define RT_GATT_ENABLED APP_GATT_ENABLED
#endif

/** @brief communication tasks */
#ifndef RT_COMMUNICATION_ENABLED
#   define RT_COMMUNICATION_ENABLED (RT_NFC_ENABLED | RI_RADIO_ENABLED)
#endif

/** @brief communication interface */
#ifndef RI_COMM_ENABLED
#   define RI_COMM_ENABLED RT_COMMUNICATION_ENABLED
#endif

/** @brief bidirectional communication */
#ifndef APP_COMMS_BIDIR_ENABLED
#   define APP_COMMS_BIDIR_ENABLED ((APP_GATT_ENABLED) + (APP_NFC_ENABLED))
#endif

/* @brief  ****** Flash Data Storage parameters ******* */
/** @brief Flash tasks if there is storage space */
#ifndef RT_FLASH_ENABLED
#   define RT_FLASH_ENABLED (RB_FLASH_SPACE_AVAILABLE > RB_FLASH_SPACE_SMALL)
#endif
/** @brief Ruuvi Flash interface. */
#define RI_FLASH_ENABLED RT_FLASH_ENABLED

/* ***** logging flash storage  **** */

/* @ brief Caculate _RECORDS_NUM (actually blocks of log elements)  */
//  From .ld:      stop_storage_flash =.+0x15000
#define APP_FLASH_PAGES                 (0x15000/4096)-1             //!<  page size is 4 kB
#define APP_FLASH_LOG_DATA_RECORDS_NUM  (APP_FLASH_PAGES - 1U -1U )  //!< GarbageCollection swap page; settings and config

//                                                  As seen in memory:
#define APP_FLASH_SENSOR_FILE         (0xCEU)       //         0000000CE
//                                                  // #W  key      #W =: number of words
#define APP_FLASH_SENSOR_BME280_RECORD   (0x28U)    // 00020028      Temperature Humidity Atmospheric Air Pressure
#define APP_FLASH_SENSOR_DPS310_RECORD   (0x31U)    // 000?0031      Tempearture and Barometric Air Pressure
#define APP_FLASH_SENSOR_ENVI_RECORD     (0x52U)    // 00020052
#define APP_FLASH_SENSOR_LIS2DH12_RECORD (0x2DU)    // 0002002D      Acceleromoter
#define APP_FLASH_SENSOR_NTC_RECORD      (0xC1U)    // 000?00C1      Thermistor
#define APP_FLASH_SENSOR_PHOTO_RECORD    (0xC2U)    // 000?00C2
#define APP_FLASH_SENSOR_SHTCX_RECORD    (0xC3U)    // 000200C3      SHTC3  I2C_ADDRESS  70
#define APP_FLASH_SENSOR_TMP117_RECORD   (0x17U)    // 000?0017      TMP117 temperature sensor

/** @brief Flash tasks for history log if there is storage space */
#define APP_FLASH_LOG_FILE             (0xF0U)      //           000000F0
#define APP_FLASH_LOG_CONFIG_RECORD      (0x01U)    //  000?0001
#define APP_FLASH_LOG_DATA_RECORD_PREFIX (0xF0U)    //  03EDF0nn          !< Prefix, F0xx append with U8 number

#define APP_FLASH_LOG_BOOT_COUNTER_RECORD (0xBC00U) //  0001BC00

// ** Logging parameters ** //
#ifndef APP_LOG_INTERVAL_S
#   define APP_LOG_INTERVAL_S (5U * 60U)
#endif
#ifndef APP_LOG_OVERFLOW
#   define APP_LOG_OVERFLOW (false)
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
#   define APP_FLASH_LOG_CONFIG_NVM_ENABLED  (false)
#endif

/** @brief ADC tasks */
#ifndef RT_ADC_ENABLED
#   define RT_ADC_ENABLED (true)
#endif

/** @brief Ruuvi Button tasks. Reset button works regardless of this setting. */
#ifndef RT_BUTTON_ENABLED
#   define RT_BUTTON_ENABLED (true)
#endif

/**
 * @brief Ruuvi ADC interface  Required by sensor
 */
#ifndef RI_ADC_ENABLED
#   define RI_ADC_ENABLED (true)
#endif

/**
 * @brief all possible dataformats for unit testing.
 */
#ifdef CEEDLING
#  define ENABLE_ALL_DATAFORMATS (true)
#else
#  define ENABLE_ALL_DATAFORMATS (false)
#endif

/**
 * @brief legacy raw dataformat
 */
#ifndef RE_3_ENABLED
#   define RE_3_ENABLED  (0U + ENABLE_ALL_DATAFORMATS)
#endif

/**
 * @brief official raw dataformat
 */
#ifndef RE_5_ENABLED
#   define RE_5_ENABLED  (1U + ENABLE_ALL_DATAFORMATS)
#endif

/**
 * @brief official encrypted dataformat
 */
#ifndef RE_8_ENABLED
#   define RE_8_ENABLED  (0U + ENABLE_ALL_DATAFORMATS)
#endif

/**
 * @brief  legacy encrypted dataformat
 */
#ifndef RE_FA_ENABLED
#   define RE_FA_ENABLED (0U + ENABLE_ALL_DATAFORMATS)
#endif

/**
 * @brief  Ruuvi AES interface.
 *
 * Required by Dataformats 8, FA. Boards with small flash can't support mbed tls.
 */
#ifndef RI_AES_ENABLED
#   define RI_AES_ENABLED (RE_8_ENABLED | RE_FA_ENABLED)
#endif

/**
 * @brief Allocate RAM for the interrupt function pointers.
 * GPIOs are indexed by GPIO number starting from 1, so size is GPIO_NUM + 1.
 */
#ifndef RT_GPIO_INT_TABLE_SIZE
#    define RT_GPIO_INT_TABLE_SIZE (RB_GPIO_NUMBER + 1U)
#endif

/** @brief  Ruuvi I2C interface. */
#ifndef RI_I2C_ENABLED
#   define RI_I2C_ENABLED (true)
#endif

/** @brief  Ruuvi SPI interface. */
#ifndef RI_SPI_ENABLED
#   define RI_SPI_ENABLED (true)
#endif

/** @brief  Ruuvi LED tasks. */
#ifndef RT_LED_ENABLED
#   define RT_LED_ENABLED (true)
#endif

/** @brief Allocate memory for LED pins. */
#ifndef RT_MAX_LED_CFG
#   define RT_MAX_LED_CFG RB_LEDS_NUMBER
#endif

/* @brief features required by Ruuvi Application */

#   define RT_SENSOR_ENABLED (true)
#   define RT_GPIO_ENABLED (true)
#   define RI_GPIO_ENABLED (true)   /* Required by SPI, LED, Button and GPIO tasks. */
#   define RI_RADIO_ENABLED APP_ADV_ENABLED
#   define RUUVI_NRF5_SDK15_ENABLED (true)
#   define RI_POWER_ENABLED (true)
#   define RI_RTC_ENABLED (true)
#   define RI_SCHEDULER_ENABLED (true)
#   define RI_TIMER_ENABLED (true)
#   define RI_ATOMIC_ENABLED (true)
#   define RI_YIELD_ENABLED (true)

/** @brief watchdog prevents software hangups. 
   Wait a really long time then eventually reboot   */
#   define RI_WATCHDOG_ENABLED  (true)
#ifndef APP_WDT_INTERVAL_MS // never make it shorter, for DEBUG maybe make it longer
#   define APP_WDT_INTERVAL_MS (APP_HEARTBEAT_OVERDUE_INTERVAL_MS + (1U*60U*1000U)) 
#endif

/** @brief LOG display functions currently to Segger RealTimeTerminal */
#ifndef RI_LOG_ENABLED
#   define RI_LOG_ENABLED (false)
#   define APP_LOG_LEVEL RI_LOG_LEVEL_NONE
#endif

/** @}*/
#endif  // APP_CONFIG_H
