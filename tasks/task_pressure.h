/**
 * Ruuvi Firmware 3.x Pressure tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_PRESSURE_H
#define  TASK_PRESSURE_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_environmental.h"
#include "ruuvi_interface_log.h"

/**
 * Auto-detects and initializes pressure sensor in low-power state
 * Reads supported sensors from board.h and tries to initialize them.
 * Configures the sensor according to defaults in application_config.h
 */
ruuvi_driver_status_t task_pressure_init(void);

/**
 * Get latest sample from the sensor. Remember to trigger sampling in case you're in single-shot mode
 */
ruuvi_driver_status_t task_pressure_data_get(ruuvi_interface_environmental_data_t*
    const data);

/**
 * Return true if task pressure has been initialized
 */
bool task_pressure_is_init();

#endif