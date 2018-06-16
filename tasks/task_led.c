#include "task_led.h"
#include "boards.h"
#include "gpio.h"

// TODO: Generalise using boards.h led list
ruuvi_status_t task_led_init(void)
{
  ruuvi_status_t err_code = RUUVI_SUCCESS;
  err_code |= platform_gpio_init();
  err_code |= platform_gpio_configure(LED_RED, RUUVI_GPIO_MODE_OUTPUT_STANDARD);
  err_code |= platform_gpio_configure(LED_GREEN, RUUVI_GPIO_MODE_OUTPUT_STANDARD);
  err_code |= platform_gpio_set(LED_RED);
  err_code |= platform_gpio_set(LED_GREEN);

  return err_code;
}
