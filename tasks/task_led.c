#include "task_led.h"
#include "task_environmental.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include <stddef.h>

ruuvi_driver_status_t task_led_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_platform_gpio_init();
  uint8_t leds[] = RUUVI_BOARD_LEDS_LIST;
  for(size_t ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
  {
    ruuvi_platform_gpio_configure(leds[ii], RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE);
    ruuvi_platform_gpio_write(leds[ii], !RUUVI_BOARD_LEDS_ACTIVE_STATE);
  }

  return err_code;
}

ruuvi_driver_status_t task_led_write(uint8_t led, task_led_state_t state)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_platform_gpio_write(led, state);
  return err_code;
}

ruuvi_driver_status_t task_led_cycle(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  static uint8_t phase = 0;
  uint8_t leds[] = RUUVI_BOARD_LEDS_LIST;
  err_code |= ruuvi_platform_gpio_toggle(leds[phase++]);
  if(RUUVI_BOARD_LEDS_NUMBER <= phase) { phase = 0; }
  return err_code;
}