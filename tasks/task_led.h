/**
 * Ruuvi Firmware 3.x LED tasks. 
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_LED_H
#define  TASK_LED_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"

typedef enum{
  TASK_LED_ON  =  RUUVI_BOARD_LEDS_ACTIVE_STATE,
  TASK_LED_OFF = !RUUVI_BOARD_LEDS_ACTIVE_STATE
}task_led_state_t;

/**
 * LED initialization function. Configures GPIOs as high-drive output and sets LEDs as inactive.
 *
 * return: Status code from the stack. RUUVI_DRIVER_SUCCESS if no errors occured. 
 **/
ruuvi_driver_status_t task_led_init(void);

/**
 * LED write function. Set given LED ON or OFF.
 *
 * parameter led: LED to change, use constant from RUUVI_BOARDS
 * parameter state: New state of given led
 * 
 * return: Status code from the stack. RUUVI_DRIVER_SUCCESS if no errors occured. 
 **/
ruuvi_driver_status_t task_led_write(uint8_t led, task_led_state_t state);

/**
 * Task demonstrator, cycles LEDs ON and OFF in sequence. 
 *
 * return: Status code from the stack. RUUVI_DRIVER_SUCCESS if no errors occured. 
 */
ruuvi_driver_status_t task_led_cycle(void);

#endif