/**
 * Ruuvi Firmware 3.x I2C tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_i2c.h"
#include "task_i2c.h"

ruuvi_driver_status_t task_i2c_init(void)
{
  ruuvi_interface_i2c_init_config_t config;
  ruuvi_interface_gpio_id_t scl = { .pin = RUUVI_BOARD_I2C_SCL_PIN };
  ruuvi_interface_gpio_id_t sda = { .pin = RUUVI_BOARD_I2C_SDA_PIN };
  config.sda = sda;
  config.scl = scl;

  switch(RUUVI_BOARD_I2C_FREQ)
  {
    case RUUVI_BOARD_I2C_FREQUENCY_100k:
      config.frequency = RUUVI_INTERFACE_I2C_FREQUENCY_100k;
      break;

    case RUUVI_BOARD_I2C_FREQUENCY_250k:
      config.frequency = RUUVI_INTERFACE_I2C_FREQUENCY_250k;
      break;

    case RUUVI_BOARD_I2C_FREQUENCY_400k:
      config.frequency = RUUVI_INTERFACE_I2C_FREQUENCY_400k;
      break;

    default:
      config.frequency = RUUVI_INTERFACE_I2C_FREQUENCY_100k;
      ruuvi_interface_log(RUUVI_INTERFACE_LOG_WARNING,
                          "Unknown I2C frequency, defaulting to 100k\r\n");
  }

  return ruuvi_interface_i2c_init(&config);
}