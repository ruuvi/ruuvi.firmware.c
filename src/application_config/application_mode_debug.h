/**
 * Debug and test mode for Ruuvi Firmawre
 * Broadcasts at 5 Hz interval and logs at 6 s interval.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */
#ifndef APPLICATION_MODE_DEBUG_H
#define APPLICATION_MODE_DEBUG_H

/**
 * Bluetooth configuration
 *
 * Be sure to follow Apple guidelines on advertisement periods and connection parameters
 * for reliable operation with iOS devices.
 */
#define APPLICATION_ADVERTISING_INTERVAL_MS              211
#define APPLICATION_CONNECTION_ADVERTISEMENT_INTERVAL_MS 211
#define APPLICATION_ADVERTISEMENT_UPDATE_INTERVAL_MS     210
#define APPLICATION_ADVERTISING_STARTUP_PERIOD_MS        1000
#define APPLICATION_ADVERTISING_STARTUP_INTERVAL_MS      211

#define APPLICATION_ENVIRONMENTAL_LOG_INTERVAL_MS (6*1000)
#define APPLICATION_WATCHDOG_INTERVAL_MS          (120 * 1000)

#endif