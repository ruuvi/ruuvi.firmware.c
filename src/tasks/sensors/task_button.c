/**
 * @addtogroup button_tasks
 */
/*@{*/
/**
 * @file task_button.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-12-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 *
 */

#include "ruuvi_driver_error.h"
#include "ruuvi_boards.h"
#include "application_config.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_log.h"
#include "task_button.h"

#include <stddef.h>

#if APPLICATION_BUTTON_ENABLED
static task_button_fp_t button_callback = NULL;

// Wrapper to gpio interrupt function
#ifndef CEEDLING
static
#endif
void on_button_isr (ruuvi_interface_gpio_evt_t event)
{
    if (NULL != button_callback)
    {
        button_callback (event);
    }

    ruuvi_interface_log (RUUVI_INTERFACE_LOG_INFO, "Button\r\n");
}

ruuvi_driver_status_t task_button_init (task_button_fp_t action)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    button_callback = action;
    ruuvi_interface_gpio_id_t pin = {.pin = RUUVI_BOARD_BUTTON_1};
    ruuvi_interface_gpio_slope_t slope = RUUVI_INTERFACE_GPIO_SLOPE_TOGGLE;
    ruuvi_interface_gpio_mode_t  mode  = RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLDOWN;

    if (RUUVI_INTERFACE_GPIO_LOW == RUUVI_BOARD_BUTTONS_ACTIVE_STATE)
    {
        mode  = RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP;
    }

    err_code |= ruuvi_interface_gpio_interrupt_enable (pin, slope,
                mode, &on_button_isr);
    return err_code;
}

#else // APPLICATION_BUTTON_ENABLED
ruuvi_driver_status_t task_button_init (task_button_fp_t action)
{
    return RUUVI_DRIVER_SUCCESS;
}
#endif
/*@}*/