/**
 * Ruuvi Firmware 3.x Button tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_BUTTON_H
#define  TASK_BUTTON_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio_interrupt.h"

typedef ruuvi_driver_status_t(*task_button_fp_t)(void);

/**
 * Button initialization function. Configures GPIO as pulled-up interrupt input.
 *
 * parameter slope: slopes to interrupt on. (RUUVI_INTERFACE_GPIO_SLOPE_)HITOLO, LOTOHI or TOGGLE
 * parameter action: Function to be called when button interrupt occurs
 *
 * return: Status code from the stack. RUUVI_DRIVER_SUCCESS if no errors occured.
 **/
ruuvi_driver_status_t task_button_init(ruuvi_interface_gpio_slope_t slope, task_button_fp_t action);

/**
 * Calls other Button tasks.
 *
 * return: Status code from the stack. RUUVI_DRIVER_SUCCESS if no errors occured.
 */
ruuvi_driver_status_t task_button_on_press(void);

#endif