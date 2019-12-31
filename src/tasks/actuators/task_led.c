/**
  * @addtogroup led_tasks
  */
/*@{*/
/**
 * @file task_led.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-11-19
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "task_led.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include <stddef.h>

static uint16_t m_activity_led;
static bool     m_initialized;
// TODO @ojousima - refactor the static variable out
static ruuvi_interface_gpio_id_t leds[RUUVI_BOARD_LEDS_NUMBER];

#ifndef CEEDLING
static
#endif
bool is_led (const uint16_t led)
{
    bool led_valid = false;

    for (size_t ii = 0U; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        if (led == leds[ii].pin)
        {
            led_valid = true;
            break;
        }
    }

    return led_valid;
}

static inline ruuvi_interface_gpio_state_t led_to_pin_state (bool active)
{
    ruuvi_interface_gpio_state_t state;

    if (true == RUUVI_BOARD_LEDS_ACTIVE_STATE)
    {
        state = active ? RUUVI_INTERFACE_GPIO_HIGH : RUUVI_INTERFACE_GPIO_LOW;
    }
    else
    {
        state = active ? RUUVI_INTERFACE_GPIO_LOW : RUUVI_INTERFACE_GPIO_HIGH;
    }

    return state;
}

ruuvi_driver_status_t task_led_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (m_initialized)
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        if (!ruuvi_interface_gpio_is_init())
        {
            err_code |= ruuvi_interface_gpio_init();
        }
    }

    if (RUUVI_DRIVER_SUCCESS == err_code)
    {
        // TODO @ojousima: refactor this array out.
        uint16_t pins[] = RUUVI_BOARD_LEDS_LIST;

        for (size_t ii = 0u; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
        {
            ruuvi_interface_gpio_id_t led;
            led.pin = pins[ii];
            leds[ii] = led;
            err_code |= ruuvi_interface_gpio_configure (leds[ii],
                        RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE);
            err_code |= ruuvi_interface_gpio_write (leds[ii],
                                                    !RUUVI_BOARD_LEDS_ACTIVE_STATE);
        }

        m_activity_led = RUUVI_INTERFACE_GPIO_ID_UNUSED;
        m_initialized = true;
    }

    return err_code;
}

ruuvi_driver_status_t task_led_uninit (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    for (size_t ii = 0U; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        err_code |= ruuvi_interface_gpio_configure (leds[ii], RUUVI_INTERFACE_GPIO_MODE_HIGH_Z);
    }

    m_activity_led = RUUVI_INTERFACE_GPIO_ID_UNUSED;
    m_initialized = false;
    return err_code;
}

ruuvi_driver_status_t task_led_write (const uint16_t led, const bool active)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!is_led (led))
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_PARAM;
    }
    else if (!m_initialized)
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        ruuvi_interface_gpio_id_t pin = {.pin = led };
        const ruuvi_interface_gpio_state_t state = led_to_pin_state (active);
        err_code |= ruuvi_interface_gpio_write (pin, state);
    }

    return err_code;
}

void task_led_activity_indicate (const bool active)
{
    // Error code cannot be returned, ignore
    (void) task_led_write (m_activity_led, active);
}

ruuvi_driver_status_t task_led_activity_led_set (uint16_t led)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (RUUVI_INTERFACE_GPIO_ID_UNUSED == led)
    {
        m_activity_led = RUUVI_INTERFACE_GPIO_ID_UNUSED;
    }
    else if (!is_led (led))
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_PARAM;
    }
    else
    {
        m_activity_led = led;
    }

    return err_code;
}

uint16_t task_led_activity_led_get (void)
{
    return m_activity_led;
}
/*@}*/
