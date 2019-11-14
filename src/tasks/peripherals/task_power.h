/**
 * Ruuvi Firmware 3.x Power tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_POWER_H
#define  TASK_POWER_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_power.h"
#include "ruuvi_interface_log.h"

/**
 * Initializes installed DC/DC converters
 * May crash the program on error due to power glitch
 *
 * return RUUVI_DRIVER_SUCCESS
 */
ruuvi_driver_status_t task_power_dcdc_init(void);


#endif