#include "app_led.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_led.h"

static const uint16_t        m_led_pins[]   = RB_LEDS_LIST;
static const ri_gpio_state_t m_led_active[] = RB_LEDS_ACTIVE_STATE;

rd_status_t app_led_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_led_init (m_led_pins, m_led_active, RB_LEDS_NUMBER);

    if (RD_SUCCESS == err_code)
    {
        for (size_t ii = 0; ii < RB_LEDS_NUMBER; ii++)
        {
            err_code |= rt_led_write (m_led_pins[ii], RT_LED_ON);
            err_code |= ri_delay_ms (APP_LED_INIT_DELAY_MS);
            err_code |= rt_led_write (m_led_pins[ii], RT_LED_OFF);
        }

        err_code |= rt_led_activity_led_set (RB_LED_ACTIVITY);

        if (RD_SUCCESS == err_code)
        {
            ri_yield_indication_set (&rt_led_activity_indicate);
        }
    }
    return  err_code;
}
