/**
 * @file application_config/application_mode_longmem.h
 * @copyright Ruuvi Innovations Ltd, License BSD-3-Clause
 * @author Otso Jousimaa <otso@ojousima.net>
 *         DG12 <DGerman@Real-World-Systems.com>
 * @date 2021-07-28 make it a real conf customization file. Use yes/no. set for SHTC and LIS2DH12 only
 *       2020-06-12 create empty file.
 * *
 * @brief override default configuration from app_config for longmem variant of firmware.
 * Increasing the interval between storing sensor readings(history) to save as many as possible.
 * Don't compile sensor modules for sensors that are not installed.
 * ToDo change loader directives to lower the flash boundry and increase pages for history.
 *
 *      Edit THIS file for customizations
 */

#define APP_FW_VARIANT  "+longmem"

#define APP_LOG_INTERVAL_S (5U * 60u)         // 5 seconds. Less often than the default variant of

#define no  0
#define yes 1

/* Explicitly enable the included sensors */
#define RT_SENSOR_ENABLED           yes
#deinfe APP_SENSOR_SHTCX_ENABLED    yes
#define APP_SENSOR_LIS2DH12_ENABLED yes

/* Disable unused sensors */

// It seems that if sensor_xxx is not enabled, it should imply that RI_sensor  is also disabled  ?? (_)

#define APP_SENSOR_BME280_ENABLED               no
#define         RI_BME280_ENABLED               no    // Prefer SHTC
#define APP_SENSOR_PHOTO_ENABLED                no
#define     RI_ADC_PHOTO_ENABLED                no
#define APP_SENSOR_NTC_ENABLED                  no
#define     RI_ADC_NTC_ENABLED                  no
#define APP_SENSOR_TMP117_ENABLED               no
#define         RI_TMP117_ENABLED               no
#define APP_SENSOR_DPS310_ENABLED               no
#define         RI_DPS310_SPI_ENABLED           no
#define APP_SENSOR_ENVIRONMENTAL_MCU_ENABLED    no

// END
