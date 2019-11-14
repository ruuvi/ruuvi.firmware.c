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
 * Populate advertisement buffer with initial data.
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 *
 */
ruuvi_driver_status_t advertisement_data_init(void);

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

/** @brief Send given message as a BLE advertisement.
 *
 *  This function configures the primary advertisement packet with the flags and manufacturer specific data.
 *  Payload of the msg will be sent as the manufacturer specific data payload.
 *  Manufacturer ID is defined by RUUVI_BOARD_BLE_MANUFACTURER_ID in ruuvi_board.h.
 *
 *  If the device is connectable, call @ref task_gatt_init to setup the scan response and flags to advertise
 *  connectability.
 *
 *  @param[in] msg message to be sent as manufacturer specific data payload
 *  @return    RUUVI_DRIVER_ERROR_NULL if msg is NULL
 *  @return    RUUVI_DRIVER_ERROR_INVALID_STATE if advertising isn't initialized or started.
 *  @return    error code from stack on other error.
 */
ruuvi_driver_status_t task_advertisement_send_data(
  ruuvi_interface_communication_message_t* const msg);

#endif