/**
 * Long-memory mode for Ruuvi Firmware.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */
#ifndef APPLICATION_MODE_LONGMEM_H
#define APPLICATION_MODE_LONGMEM_H

/**
 * Environmental sensor configuration
 **/
// Sample rate is in Hz. This configures only the sensor, not transmission rate of data.
#define APPLICATION_ENVIRONMENTAL_SAMPLERATE RUUVI_DRIVER_SENSOR_CFG_MIN

/* Logging configuration */
#define APPLICATION_ENVIRONMENTAL_LOG_INTERVAL_MS (12LU * 60LU * 60LU * 1000LU)

#endif