#include "ruuvi_driver_error.h"
#include "ruuvi_boards.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_yield.h"
#include "task_button.h"
#include <stddef.h>

static task_button_fp_t button_callback = NULL;
static ruuvi_interface_gpio_interrupt_fp_t interrupt_table[RUUVI_BOARD_GPIO_NUMBER + 1 ] = {0};

// Wrapper to gpio interrupt function
static void on_button(ruuvi_interface_gpio_evt_t event)
{
  if(NULL != button_callback) { button_callback(); }
}

ruuvi_driver_status_t task_button_init(ruuvi_interface_gpio_slope_t slope, task_button_fp_t action)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  button_callback = action;
  err_code |= ruuvi_platform_gpio_interrupt_init(interrupt_table, sizeof(interrupt_table));
  err_code |= ruuvi_platform_gpio_interrupt_enable(RUUVI_BOARD_BUTTON_1, slope, RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP, on_button);
  return err_code;
}