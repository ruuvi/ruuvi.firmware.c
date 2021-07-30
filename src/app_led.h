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
 *
 * Deprecation notice: App led uses signals set/cleared by application since 3.30.3,
 * some functions will be deprecated in 4.0.
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
 * @deprecated Use signals instead.
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
 * @deprecated Use signals instead.
 *
 *
 * After calling this function given led is inactive.
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
 * @deprecated Use signals instead.
 *
 * @param[in] led Activity led.
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_PARAM if there's not led at given pin.
 */
rd_status_t app_led_activity_set (const ri_gpio_id_t led);

/**
 * @brief Activate led at configured activity LED pin.
 * @deprecated Use signals instead.
 *
 * After calling this function activity led is active.
 *
 * @param[in] active True to activate activity led, false to deactivate.
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_STATE if no activity led is configured
 */
rd_status_t app_led_activity_indicate (const bool active);

/**
 * @brief Pause activity indications.
 * @deprecated Use signals instead.
 *
 * Call this to pause  and resume activity indications, for example
 * when button press indication should stay on for duration of button press.
 *
 * @param[in] pause True to pause indications, false to stop.
 */
void app_led_activity_pause (const bool pause);

/**
 * @brief Set/Clear activity indication
 *
 * Call this function to set / clear activity state of the leds. app_led decides
 * action based on other signals
 *
 * @param[in] active True to activate signal, false to deactivate.
 */
void app_led_activity_signal (const bool active);

/**
 * @brief Set/Clear configuration indication
 *
 * Call this function to set / clear configurable state of the leds. app_led decides
 * action based on other signals
 *
* @param[in] active True to activate signal, false to deactivate.
 */
void app_led_configuration_signal (const bool active);

/**
 * @brief Set/Clear interaction indication
 *
 * Call this function to set / clear interaction state of the leds.
 * Typically this is called on button press and release.
 * App_led decides action based on other signals.
 *
 * @param[in] active True to activate signal, false to deactivate.
 */
void app_led_interaction_signal (const bool active);

/**
 * @brief Set/Clear error indication
 *
 * Call this function to set / clear error state of the leds.
 * App_led decides action based on other signals.
 *
 * @param[in] active True to activate signal, false to deactivate.
 */
void app_led_error_signal (const bool active);

/** @} */

#endif // APP_LED_H
