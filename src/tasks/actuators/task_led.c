#include "task_led.h"
#include "task_environmental.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include <stddef.h>

static uint16_t m_activity_led;
static bool     m_initialized;

ruuvi_driver_status_t task_led_init(void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    if(m_initialized) {
        return RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    if(!ruuvi_interface_gpio_is_init())
    {
        err_code |= ruuvi_interface_gpio_init();
        if(RUUVI_DRIVER_SUCCESS != err_code) {
            return err_code;
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
    ruuvi_interface_gpio_id_t leds[RUUVI_BOARD_LEDS_NUMBER] = RUUVI_BOARD_LEDS_LIST;
#pragma GCC diagnostic pop

    for(size_t ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        err_code |= ruuvi_interface_gpio_configure(leds[ii], RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE);
        err_code |= ruuvi_interface_gpio_write(leds[ii], !RUUVI_BOARD_LEDS_ACTIVE_STATE);
    }

    m_activity_led = RUUVI_INTERFACE_GPIO_ID_UNUSED;
    m_initialized = true;
    return err_code;
}

ruuvi_driver_status_t task_led_uninit(void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
    ruuvi_interface_gpio_id_t leds[RUUVI_BOARD_LEDS_NUMBER] = RUUVI_BOARD_LEDS_LIST;
#pragma GCC diagnostic pop
    for(size_t ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        err_code |= ruuvi_interface_gpio_configure(leds[ii], RUUVI_INTERFACE_GPIO_MODE_HIGH_Z);
    }

    m_activity_led = RUUVI_INTERFACE_GPIO_ID_UNUSED;
    m_initialized = false;
    return err_code;
}

ruuvi_driver_status_t task_led_write(const uint16_t led, const bool active)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_gpio_id_t pin = {.pin = led };
    const ruuvi_interface_gpio_state_t state = active ? RUUVI_INTERFACE_GPIO_HIGH :
            RUUVI_INTERFACE_GPIO_LOW;
    err_code |= ruuvi_interface_gpio_write(pin, state);
    return err_code;
}

void task_led_activity_indicate(const bool active)
{
    task_led_write(m_activity_led, active);
}

void task_led_activity_led_set(uint16_t led)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
    ruuvi_interface_gpio_id_t const leds[RUUVI_BOARD_LEDS_NUMBER] = RUUVI_BOARD_LEDS_LIST;
#pragma GCC diagnostic pop
    m_activity_led = RUUVI_INTERFACE_GPIO_ID_UNUSED;

    for(size_t ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        if(leds[ii].pin == led)
        {
            m_activity_led = led;
            break;
        }
    }
}

uint16_t task_led_activity_led_get(void)
{
    return m_activity_led;
}