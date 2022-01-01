/**
 * @file application_config/debug.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-04-16
 * @brief  predefine  defaults in  app_config
 * This is included by _modes followed by _default then _config
 *
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 *  EDIT THIS file to customize
 */ 
#ifndef APPLICATION_MODE_DEBUG_H
#define APPLICATION_MODE_DEBUG_H

#define APP_FW_VARIANT "+debug"
#define RI_LOG_ENABLED (true)
#define APP_LOG_LEVEL RI_LOG_LEVEL_INFO

// If RTT LOG buffer is full: block 2; trim 1; skip 0  
// block will stop application if no RealTimeTerminal (RTT) is connected.
#define SEGGER_RTT_CONFIG_DEFAULT_MODE (1U) 

#define APP_WDT_INTERVAL_MS (10U*60U*1000U)

/** @brief Communicate sensor data at this interval. 221 matches Apple guideline. */
#define APP_BLE_INTERVAL_MS (221U)

/** @brief repeat same data N times in advertisement, reduce sensor reads. */
#define APP_NUM_REPEATS (4U)

/** @brief Communicate sensor data at this interval. 221 matches Apple guideline. */
#define APP_HEARTBEAT_INTERVAL_MS (APP_BLE_INTERVAL_MS * APP_NUM_REPEATS)

/* Save history log every second rather than the default 5 minutes  */
/* This will result in writing FLASH log block every 7.36 minutes ie: 07:21 */
#define APP_LOG_INTERVAL_S (1U) //!< Gets limited to heartbeat rate.

/* Not enough flash to load debug with all drivers so don't enable rarely needed ones */
#define APP_SENSOR_PHOTO_ENABLED  (false)    
#define APP_SENSOR_NTC_ENABLED    (false)
#define APP_SENSOR_DPS310_ENABLE  (false)

#endif //APPLICATION_MODE_DEBUG_H
