#include "task_gpio.h"
#include "ruuvi_boards.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_gpio_interrupt.h"

static ruuvi_interface_gpio_interrupt_fp_t interrupt_table[RUUVI_BOARD_GPIO_NUMBER + 1 ] = {0};

ruuvi_driver_status_t task_gpio_init()
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_gpio_init();
  err_code |= ruuvi_interface_gpio_interrupt_init(interrupt_table, sizeof(interrupt_table));
  return err_code;
}