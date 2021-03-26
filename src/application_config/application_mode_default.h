#ifndef APPLICATION_MODE_DEFAULT_H
#define APPLICATION_MODE_DEFAULT_H

/**
 * @file application_mode_default.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-06-12
 * @copyright Ruuvi Innovations Ltd, License BSD-3-Clause
 *
 * Default configuration for Ruuvi Firmware.
 */

/**
 * @brief name for Device Information Service.
 */
#ifndef APP_FW_NAME
#define APP_FW_NAME "Ruuvi FW "
#endif

#ifndef APP_FW_VERSION
#define APP_FW_VERSION "v0.0.1"
#endif

#ifndef APP_FW_VARIANT
#define APP_FW_VARIANT "+default"
#endif

/** @brief Communicate sensor data at this interval. 1285 matches Apple guideline. */
#ifndef APP_BLE_INTERVAL_MS
#   define APP_BLE_INTERVAL_MS (1285U)
#endif

/** @brief repeat same data N times in advertisement, reduce sensor reads. */
#ifndef APP_NUM_REPEATS
#   define APP_NUM_REPEATS 2
#endif

/** @brief Refresh sensor data at this interval.*/
#ifndef APP_HEARTBEAT_INTERVAL_MS
#   define APP_HEARTBEAT_INTERVAL_MS (APP_BLE_INTERVAL_MS * APP_NUM_REPEATS)
#endif

/** @brief Resample battery voltage at this interval */
#ifndef APP_BATTERY_SAMPLE_MS
#   define APP_BATTERY_SAMPLE_MS (60ULL*1000ULL)
#endif

/** @brief Time to long press of a button. */
#ifndef APP_BUTTON_LONG_PRESS_TIME_MS
#   define APP_BUTTON_LONG_PRESS_TIME_MS (5000U)
#endif

/** @brief Configuration mode disabled on this timeout. */
#ifndef APP_CONFIG_ENABLED_TIME_MS
#   define APP_CONFIG_ENABLED_TIME_MS (1U * 60U * 1000U)
#endif

#ifndef APP_MOTION_THRESHOLD
#   define APP_MOTION_THRESHOLD (0.064F)
#endif

#ifndef APP_LOCKED_AT_BOOT
#   define APP_LOCKED_AT_BOOT (true)
#endif
#endif
