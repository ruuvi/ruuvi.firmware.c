/**
 * Ruuvi Firmware 3.x advertisement tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_ADVERTISEMENT_H
#define  TASK_ADVERTISEMENT_H

#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication_ble4_advertising.h"

/**
 * Initializes data advertising. 
 *
 * After calling this function radio module has been reserved to ble4 advertisements.
 * Parameters such as advertisement intercal and power are defined in application_config.h
 * Requires that timers have been initialized.
 * 
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 */
ruuvi_driver_status_t task_advertisement_init(void);

/**
 * Uninitializes data advertising. 
 *
 * After calling this function radio module has been released if it was reserved to ble4 advertisements.
 * Can be called even if advertising was not initialized.
 * 
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 */
ruuvi_driver_status_t task_advertisement_uninit(void);

/**
 * Starts advertising. Reads sensors for latest data, but does not initialize sensors themselves
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns RUUVI_DRIVER_ERROR_INVALID_STATE if advertising is not initialized.
 * returns error code from stack on error
 *
 */
ruuvi_driver_status_t task_advertisement_start(void);

/**
 * Stops advertising.
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 */
ruuvi_driver_status_t task_advertisement_stop(void);

/**
 * Reads sensors and encodes sensor data into Ruuvi format 3 (RAWv1).
 * Data is buffered and will be sent in next BLE advertisement as manufacturer specific data.
 * Data is transmitted until something else is written to buffer
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 */
ruuvi_driver_status_t task_advertisement_send_3(void);

/**
 * Reads sensors and encodes sensor data into Ruuvi format 3 (RAWv1).
 * Data is buffered and will be sent in next BLE advertisement as manufacturer specific data.
 * Data is transmitted until something else is written to buffer
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 */
ruuvi_driver_status_t task_advertisement_send_5(void);

#endif