#ifndef APP_CONFIG_H
#define APP_CONFIG_H
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

/** @brief enable Ruuvi Yield interface */
#define RI_YIELD_ENABLED 1
/** @brief enable Ruuvi Watchdog interface */
#define RI_WATCHDOG_ENABLED 1

/*@}*/
#endif
