/**
 * Ruuvi Firmware 3.x Acceleration tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_ACCELERATION_H
#define  TASK_ACCELERATION_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"

/**
 * Auto-detects and initializes acceleration sensor in low-power state
 * Reads supported sensors from board.h and tries to initialize them.
 * Configures the sensor according to defaults in application_config.h
 */
ruuvi_driver_status_t task_acceleration_init(void);

/**
 * Prints Acceleration data to log at given severity
 */
ruuvi_driver_status_t task_acceleration_data_log(const ruuvi_interface_log_severity_t level);

/**
 * Get acceleration data and print it on console.
 */
ruuvi_driver_status_t task_acceleration_on_button(void);

#endif