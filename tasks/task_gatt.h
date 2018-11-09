/**
 * Ruuvi Firmware 3.x advertisement tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_GATT_H
#define  TASK_GATT_H

#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_ble4_gatt.h"

/**
 * Initializes GATT with DFU, DIF and NUS
 * Cannot be used with data advertising
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 */
ruuvi_driver_status_t task_gatt_init(void);

// ruuvi_driver_status_t task_gatt_on_accelerometer(ruuvi_interface_communication_evt_t evt);
// ruuvi_driver_status_t task_gatt_on_advertisement(ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len);
// ruuvi_driver_status_t task_gatt_on_button(ruuvi_interface_communication_evt_t evt);
// ruuvi_driver_status_t task_gatt_on_nfc(ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len);

/**
 * Event handler for NUS events
 */
ruuvi_driver_status_t task_gatt_on_gatt(ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len);

/**
 * Send given message via NUS
 *
 * returns RUUVI_DRIVER_SUCCESS if data was placed in send buffer
 * returns RUUVI_DRIVER_ERROR_INVALID_STATE if NUS is not connected
 * returns error code from stack on error
 *
 */
ruuvi_driver_status_t task_gatt_send(ruuvi_interface_communication_message_t* const msg);


#endif