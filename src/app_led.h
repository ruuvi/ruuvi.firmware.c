#ifndef APP_LED_H
#define APP_LED_H

/**
 * @addtogroup app
 */
/** @{ */
/**
 * @defgroup app_led Led control.
 *
 */
/** @} */
/**
 * @addtogroup app_led
 */
/** @{ */
/**
 * @file app_led.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-09-10
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"

/**
 * @brief Initialize leds.
 *
 * After calling this function all LED GPIOs are high-drive outputs and in inactive state.
 * Led active states and pins are loaded from board definition.
 *
 * @retval RD_SUCCESS on successful init.
 * @retval RD_ERROR_INVALID_STATE if already initialized.
 */
rd_status_t app_led_init (void);

/**
 * @brief Activate led at given GPIO pin.
 *
 * After calling this function given led is active.
 *
 * @param[in] led GPIO of led to activate.
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_PARAM if there is no led at given pin.
 * @retval RD_ERROR_INVALID_STATE if leds are not initialized.
 */
rd_status_t app_led_activate (const ri_gpio_id_t led);

/**
 * @brief Deactivate led at given GPIO pin.
 *
 * After calling this function
 *
 * @param[in] led GPIO of led to deactivate.
 * @return RD_SUCCESS
 */
rd_status_t app_led_deactivate (const ri_gpio_id_t led);

/**
 * @brief Set activity led.
 *
 * Call this to select a led which is turned on when @ref app_led_activity_indicate is
 * called. Calling this function deactivates the led.
 *
 * @param[in] led Activity led.
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_PARAM if there's not led at given pin.
 */
rd_status_t app_led_activity_set (const ri_gpio_id_t led);

/**
 * @brief Activate led at given GPIO pin.
 *
 * After calling this function given led is active.
 *
 * @param[in] active True to activate activity led, false to deactivate.
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_PARAM if there is no led at consfigured given pin.
 */
rd_status_t app_led_activity_indicate (const bool active);

/**
 * @brief Pause activity indications.
 *
 * Call this to pause  and resume activity indications, for example
 * when button press indication should stay on for duration of button press.
 *
 * @param[in] pause True to pause indications, false to stop.
 */
void app_led_activity_pause (const bool pause);

/** @} */

#endif // APP_LED_H
