#include "task_led.h"
#include "task_environmental.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include <stddef.h>

static uint16_t m_activity_led;

ruuvi_driver_status_t task_led_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

  if(!ruuvi_interface_gpio_is_init())
  {
    err_code |= ruuvi_interface_gpio_init();
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
  ruuvi_interface_gpio_id_t leds[] = RUUVI_BOARD_LEDS_LIST;
#pragma GCC diagnostic pop

  for(size_t ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
  {
    ruuvi_interface_gpio_configure(leds[ii], RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE);
    ruuvi_interface_gpio_write(leds[ii], !RUUVI_BOARD_LEDS_ACTIVE_STATE);
  }

  m_activity_led = RUUVI_INTERFACE_GPIO_ID_UNUSED;
  return err_code;
}

ruuvi_driver_status_t task_led_write(const uint16_t led, const task_led_state_t state)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_gpio_id_t pin = {.pin = led };
  err_code |= ruuvi_interface_gpio_write(pin, state);
  return err_code;
}

void task_led_activity_indicate(const bool active)
{
  const task_led_state_t state = (active) ? RUUVI_BOARD_LEDS_ACTIVE_STATE :
                                 !RUUVI_BOARD_LEDS_ACTIVE_STATE;
  task_led_write(m_activity_led, state);
}

void task_led_activity_led_set(uint16_t led)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
  ruuvi_interface_gpio_id_t const leds[] = RUUVI_BOARD_LEDS_LIST;
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