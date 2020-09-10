#include "app_led.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_task_led.h"

/**
 * @addtogroup app_led
 */
/** @{ */
/**
 * @file app_led.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-09-10
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

static const uint16_t        m_led_pins[]   = RB_LEDS_LIST;
static const ri_gpio_state_t m_led_active[] = RB_LEDS_ACTIVE_STATE;

rd_status_t app_led_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_led_init (m_led_pins, m_led_active, RB_LEDS_NUMBER);
    return  err_code;
}

rd_status_t app_led_activate (const ri_gpio_id_t led)
{
    return rt_led_write (led, true);
}

rd_status_t app_led_deactivate (const ri_gpio_id_t led)
{
    return rt_led_write (led, false);
}

/** @} */
