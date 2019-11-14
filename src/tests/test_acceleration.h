/**
 * Ruuvi Firmware 3.x Acceleration tests.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TEST_ACCELERATION_H
#define  TEST_ACCELERATION_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"

/** Run tests on a sensor
 *
 *  Returns RUUVI_DRIVER_SUCCESS if no errors occured, RUUVI_DRIVER_ERROR_SELFTEST if there was an error.
 */
ruuvi_driver_status_t test_acceleration_run(void);

#endif