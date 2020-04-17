#ifndef APP_BUTTON_H
#define APP_BUTTON_H

#include "app_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_task_button.h"
#include "ruuvi_task_gpio.h"

/**
 * @addtogroup app
 */
/*@{*/
/**
 * @defgroup app_button Button, HALL etc support.
 * @brief Actions when button, hall etc are activated.
 *
 */
/*@}*/
/**
 * @addtogroup app_button
 */
/*@{*/
/**
 * @file app_button.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-02-12
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

/**
 * @brief Initialize buttons.
 *
 * After initialization buttons are powered if needed and
 * the callback defined at app_button.c is called on activation.
 *
 * @retval RD_SUCCESS if buttons were initialized
 * @retval RD_ERROR_INVALID_STATE if RI_GPIO or RI_GPIO_INTERRUPT are not initialized.
 */
rd_status_t app_button_init (void);

#ifdef CEEDLING
void on_button_press (const ri_gpio_evt_t evt);
ri_gpio_slope_t get_activation (ri_gpio_evt_t evt);
#endif
/*@}*/

#endif // APP_BUTTON_H
