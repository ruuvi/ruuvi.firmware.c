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

/** @brief name for Device Information Service.
 *
 * This should be provided by build script
 */
#ifndef APP_FW_NAME
#define APP_FW_NAME "Ruuvi FW-development"
#endif

/** @brief Communicate sensor data at this interval. 1285 matches Apple guideline. */
#ifndef APP_HEARTBEAT_INTERVAL_MS
#   define APP_HEARTBEAT_INTERVAL_MS (1285U)
#endif

#endif
