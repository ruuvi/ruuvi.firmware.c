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

ruuvi_driver_status_t task_advertisement_init(void);

ruuvi_driver_status_t task_advertisement_send_3(void);

ruuvi_driver_status_t task_advertisement_send_5(void);

#endif