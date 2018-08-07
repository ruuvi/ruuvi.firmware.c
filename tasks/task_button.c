#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include "task_button.h"
#include <stddef.h>

ruuvi_driver_status_t task_button_init(ruuvi_interface_gpio_slope_t slope, task_button_fp_t action)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_platform_pin_interrupt_init();
  err_code |= ruuvi_platform_gpio_interrupt_enable(RUUVI_BOARD_BUTTON_1, slope, RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP, action);
  return err_code;
}