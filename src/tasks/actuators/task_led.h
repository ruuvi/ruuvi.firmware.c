#ifndef  TASK_LED_H
#define  TASK_LED_H

/**
  * @defgroup actuator_tasks  Interacting with outside world
  */
/*@{*/
/**
 * @defgroup led_tasks LED tasks
 * @brief LED functions
 *
 */
/*@}*/
/**
 * @addtogroup led_tasks
 */
/*@{*/
/**
 * @file task_led.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-11-18
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * LED control.
 *
 * Typical usage:
 *
 * @code{.c}
 *  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
 *  err_code = task_gpio_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_led_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_led_write (RUUVI_BOARD_LED_GREEN, RUUVI_BOARD_LEDS_ACTIVE_STATE);
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_led_activity_led_set (RUUVI_BOARD_LED_GREEN);
 *  ruuvi_interface_yield_indication_set (task_led_activity_indicate);
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  _do_stuff_();
 *  err_code = task_led_uninit();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 * @endcode
 */

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
 * @retval RUUVI_DRIVER_SUCCESS if value was written
 * @retval RUUVI_ERROR_INVALID_PARAM  if GPIO pin is not led.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if GPIO task is not initialized.
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

#ifdef CEEDLING
bool is_led (const uint16_t led);
#endif

/*@}*/
#endif
