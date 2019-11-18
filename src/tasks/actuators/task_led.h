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

/**
 * @brief LED initialization function.
 * - Returns error if leds were already initialized.
 * - Initializes GPIO if GPIO wasn't initialized.
 * - returns error code if GPIO cannot be initialized
 * - Configures GPIOs as high-drive output and sets LEDs as inactive.
 * - Sets activity led to uninitialized
 *
 * @retval RUUVI_DRIVER_SUCCESS if no errors occured.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if leds were already initialized.
 * @retval error code from stack on other error.
 **/
ruuvi_driver_status_t task_led_init (void);

/**
 * @brief LED uninitialization function.
 * - Returns error if leds were already initialized.
 * - Configures GPIOs as high-z.
 * - Sets activity led to uninitialized
 *
 * @retval RUUVI_DRIVER_SUCCESS if no errors occured.
 * @retval error code from stack on other error.
 **/
ruuvi_driver_status_t task_led_uninit (void);

/**
 * @brief LED write function. Set given LED ON or OFF.
 *
 * @param[in] led  LED to change, use constant from RUUVI_BOARDS
 * @param[in] state  true to turn led on, false to turn led off.
 *
 * @return Status code from the stack. @c RUUVI_DRIVER_SUCCESS if no errors occured.
 **/
ruuvi_driver_status_t task_led_write (const uint16_t led, const bool state);

/**
 * @brief Task demonstrator, cycles LEDs ON and OFF in sequence.
 *
 * @return Status code from the stack. @c RUUVI_DRIVER_SUCCESS if no errors occured.
 */
ruuvi_driver_status_t task_led_cycle (void);

/**
 * These functions indicate activity and sleep by turning led on while active
 * and turning led off while in sleep. Configure ruuvi_interface_yield() to call these.
 */
void task_led_activity_indicate (const bool state);

/**
 * Set LED which is used to indicate activity.
 */
void task_led_activity_led_set (const uint16_t led);

/**
 * get LED which is used to indicate activity.
 */
uint16_t task_led_activity_led_get (void);

#endif