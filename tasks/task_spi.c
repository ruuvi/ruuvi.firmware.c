/**
 * Ruuvi Firmware 3.x SPI tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_spi.h"
#include "task_spi.h"

ruuvi_driver_status_t task_spi_init(void)
{
  ruuvi_interface_spi_init_config_t config;
  uint8_t ss_pins[] = RUUVI_BOARD_SPI_SS_LIST;
  config.mosi = RUUVI_BOARD_SPI_MOSI_PIN;
  config.miso = RUUVI_BOARD_SPI_MISO_PIN;
  config.sclk = RUUVI_BOARD_SPI_SCLK_PIN;
  config.ss_pins = ss_pins;
  config.ss_pins_number = sizeof(ss_pins);
  // Assume mode 0 always.
  config.mode = RUUVI_INTERFACE_SPI_MODE_0;
  switch(RUUVI_BOARD_SPI_FREQ)
  {
    case RUUVI_BOARD_SPI_FREQUENCY_1M:
      config.frequency = RUUVI_INTERFACE_SPI_FREQUENCY_1M;
      break;

    case RUUVI_BOARD_SPI_FREQUENCY_2M:
      config.frequency = RUUVI_INTERFACE_SPI_FREQUENCY_2M;
      break;

    case RUUVI_BOARD_SPI_FREQUENCY_4M:
      config.frequency = RUUVI_INTERFACE_SPI_FREQUENCY_4M;
      break;

    case RUUVI_BOARD_SPI_FREQUENCY_8M:
      config.frequency = RUUVI_INTERFACE_SPI_FREQUENCY_8M;
      break;

    default:
      config.frequency = RUUVI_INTERFACE_SPI_FREQUENCY_1M;
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_WARNING, "Unknown SPI frequency, defaulting to 1M\r\n");
  }
  return ruuvi_platform_spi_init(&config);
}