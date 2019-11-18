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
 * @retval @c RUUVI_DRIVER_SUCCESS if value was written
 * @retval @c RUUVI_ERROR_INVALID_PARAM  if GPIO pin is not led.
 * @retval @c RUUVI_DRIVER_ERROR_INVALID_STATE if GPIO task is not initialized.
 **/
ruuvi_driver_status_t task_led_write (const uint16_t led, const bool state);

/**
 * @brief Function to indicate activity in program.
 * Led is turned on while program is active
 * and off while in sleep.
 * Call ruuvi_interface_yield_indication_set to setup this function to be called
 * when entering / leaving sleep for example.
 *
 * @param[in] state True to indicate activity, false to indicate sleep.
 */
void task_led_activity_indicate (const bool state);

/**
 * @brief Set LED which is used to indicate activity.
 *
 * This function can be called before GPIO or LEDs are initialized.
 * Call with RUUVI_INTERFACE_GPIO_ID_UNUSED to disable activity indication.
 *
 * @param[in] led LED to indicate activity with.
 *
 * @retval RUUVI_DRIVER_SUCCESS if valid led was set.
 * @retval RUUVI_DRIVER_ERROR_INVALID_PARAM if there is no pin in LED.
 */
ruuvi_driver_status_t task_led_activity_led_set (const uint16_t led);

/**
 * @brief Get LED which is used to indicate activity.
 *
 * @return Led which is activity indicator, RUUVI_INTERFACE_GPIO_ID_UNUSED if none.
 */
uint16_t task_led_activity_led_get (void);

#endif