/**
 * Ruuvi Firmware 3.x Flash tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_FLASH_H
#define  TASK_FLASH_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_flash.h"
#include "ruuvi_interface_log.h"

/**
 * Initializes flash storage
 * Configures the sensor according to defaults in application_config.h
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error.
 */
ruuvi_driver_status_t task_flash_init(void);

ruuvi_driver_status_t task_flash_store(const uint32_t page_id, const uint32_t record_id, const char* const message, const size_t message_length);

ruuvi_driver_status_t task_flash_load(const uint32_t page_id, const uint32_t record_id, char* const message, const size_t message_length);

ruuvi_driver_status_t task_flash_demo();

#endif