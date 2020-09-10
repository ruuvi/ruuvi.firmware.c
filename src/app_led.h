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

/** @} */

#endif // APP_LED_H
