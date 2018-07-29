/**
 * Ruuvi Firmware 3.x code. Reads the sensors onboard RuuviTag and broadcasts the sensor data in a manufacturer specific format.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#include "application_config.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_boards.h"

#include <stdio.h>

int main(void)
{
  // Init logging
  ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
  status |= ruuvi_platform_log_init(APPLICATION_LOG_LEVEL);
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Logging started\r\n");
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Init yield
  status |= ruuvi_platform_yield_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Init GPIO
  status |= ruuvi_platform_gpio_init();
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Peripherals initialized\r\n");
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Turn off sensors∫
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SS_ACCELERATION_PIN,  RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SS_ACCELERATION_PIN,  RUUVI_INTERFACE_GPIO_HIGH);
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN, RUUVI_INTERFACE_GPIO_HIGH);

  // Put SPI lines into HIGH state to avoid power leaks
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SCK_PIN,  RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SCK_PIN,  RUUVI_INTERFACE_GPIO_HIGH);
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_MOSI_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_MOSI_PIN, RUUVI_INTERFACE_GPIO_HIGH);

  // LEDs high / inactive
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_LED_RED,   RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_LED_RED,   RUUVI_INTERFACE_GPIO_HIGH);
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_LED_GREEN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_LED_GREEN, RUUVI_INTERFACE_GPIO_HIGH);

  // Button, and SPI MISO lines pulled up
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_BUTTON_1,     RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP);
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_MISO_PIN, RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  char message[APPLICATION_LOG_BUFFER_SIZE];
  size_t index = 0;
  index += snprintf (message, APPLICATION_LOG_BUFFER_SIZE, "%s:%d GPIO ready, status ", __FILE__, __LINE__);
  index += ruuvi_platform_error_to_string(status, message + index, sizeof(message) - index);
  index += snprintf( message + index, sizeof(message) - index,", entering main loop\r\n");
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, message);

  while (1)
  {
    ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Going to sleep\r\n");
    ruuvi_platform_yield();
    ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Waking up\r\n");
  }
}