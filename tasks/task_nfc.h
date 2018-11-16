/**
 * Ruuvi Firmware 3.x NFC tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_NFC_H
#define  TASK_NFC_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_nfc.h"
#include "ruuvi_interface_log.h"

/**
 * Initializes NFC and configures FW, ADDR and ID records according to application_config.h constants
 *
 * return RUUVI_DRIVER_SUCCESS on success
 * return error code from stack on error
 */
ruuvi_driver_status_t task_nfc_init(void);

/**
 * Sets given message to NFC RAM buffer. Clears previous message
 *
 * return RUUVI_DRIVER_SUCCESS on success
 * return error code from stack on error
 */
ruuvi_driver_status_t task_nfc_send(ruuvi_interface_communication_message_t* message);

/**
 * Handle Ruuvi communication events from NFC driver
 *
 * parameter evt: Type of event
 * parameter p_data pointer to data received. NULL if data was not received
 * parameter data_len; length of data received. 0 if data was NULL
 *
 * return RUUVI_DRIVER_SUCCESS if no error occurred
 * return error code from stack on error.
 */
ruuvi_driver_status_t task_nfc_on_nfc(ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len);


#endif