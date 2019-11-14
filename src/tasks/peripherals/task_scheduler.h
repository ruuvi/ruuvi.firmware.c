/**
 * Ruuvi Firmware 3.x Scheduler tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_SCHEDULER_H
#define  TASK_SCHEDULER_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_log.h"

/**
 * Initializes scheduler
 *
 * returns RUUVI_DRIVER_SUCCESS on success, error code from stack on error
 */
ruuvi_driver_status_t task_scheduler_init(void);

#endif