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
#include "ruuvi_interface_gpio.h"

typedef enum
{
  TASK_LED_ON  =  RUUVI_BOARD_LEDS_ACTIVE_STATE,
  TASK_LED_OFF = !RUUVI_BOARD_LEDS_ACTIVE_STATE
} task_led_state_t;

/**
 * LED initialization function. Configures GPIOs as high-drive output and sets LEDs as inactive.
 *
 * return: Status code from the stack. @c RUUVI_DRIVER_SUCCESS if no errors occured.
 **/
ruuvi_driver_status_t task_led_init(void);

/**
 * @brief LED write function. Set given LED ON or OFF.
 *
 * @param[in] led  LED to change, use constant from RUUVI_BOARDS
 * @param[in] state  New state of given led
 *
 * @return Status code from the stack. @c RUUVI_DRIVER_SUCCESS if no errors occured.
 **/
ruuvi_driver_status_t task_led_write(const uint16_t led, const task_led_state_t state);

/**
 * @brief Task demonstrator, cycles LEDs ON and OFF in sequence.
 *
 * @return Status code from the stack. @c RUUVI_DRIVER_SUCCESS if no errors occured.
 */
ruuvi_driver_status_t task_led_cycle(void);

/**
 * These functions indicata activity and sleep by turning led on while active
 * and turning led off while in sleep. Configure ruuvi_interface_yield() to call these.
 */
void task_led_activity_indicate(const bool state);

/**
 * Set LED which is used to indicate activity.
 */
void task_led_activity_led_set(uint16_t led);

#endif