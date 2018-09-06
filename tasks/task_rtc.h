/**
 * Ruuvi Firmware 3.x RTC tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_RTC_H
#define  TASK_RTC_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"

/**
 * Initializes the RTC and sets up timestamp function on sensor interface
 */
ruuvi_driver_status_t task_rtc_init(void);

#endif