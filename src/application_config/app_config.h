#ifndef APP_CONFIG_H
#define APP_CONFIG_H
#include "application_modes.h"
#include "ruuvi_boards.h"
/**
 * @defgroup Configuration Application configuration
 */
/*@{*/
/**
 * @defgroup APP Application configuration
 * @brief Configure application enabled modules and parameters.
 */
/*@}*/
/**
 * @addtogroup SDK15
 */
/*@{*/
/**
 * @file app_config.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-01-29
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 */

/** @brief If watchdog is not fed at this intercal or faster, reboot */
#define APP_WDT_INTERVAL_MS (2U*60U*1000U)

/** @brief enable nRF15 SDK implementation of drivers */
#define RUUVI_NRF5_SDK15_ENABLED 1
/** @brief enable Ruuvi Button tasks */
#define RT_BUTTON_ENABLED 1
/** @brief enable Ruuvi GPIO interface & tasks */
#define RT_GPIO_ENABLED 1
/** @brief enable Ruuvi GPIO interface & tasks */
#define RI_GPIO_ENABLED 1
/** @brief GPIOs are indexed by GPIO number starting from 1 */
#define RT_GPIO_INT_TABLE_SIZE (RB_GPIO_NUMBER+1)
/** @brief enable Ruuvi Flash interface on boards with enough RAM & Flash */
#define RI_FLASH_ENABLED (RB_APP_PAGES > 0)
/** @brief enable Ruuvi Yield interface */
#define RI_YIELD_ENABLED 1
/** @brief enable Ruuvi Watchdog interface */
#define RI_WATCHDOG_ENABLED 1

#ifndef APP_FW_NAME
#define APP_FW_NAME "Ruuvi FW"
#endif

// TODO: move under debug
#ifdef DEBUG
/** @brief Logs conserve lot of flash, enable only on debug builds */
#define RI_LOG_ENABLED 1
#define APP_LOG_LEVEL RI_LOG_LEVEL_INFO
#else
#define RI_LOG_ENABLED 0
#define APP_LOG_LEVEL RI_LOG_LEVEL_WARNING
#endif

/*@}*/
#endif
