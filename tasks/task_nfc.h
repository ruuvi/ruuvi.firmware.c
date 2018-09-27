/**
 * Ruuvi Firmware 3.x ADC tasks.
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


#endif