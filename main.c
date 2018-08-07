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
#include "task_button.h"
#include "task_led.h"

#include <stdio.h>

int main(void)
{
  // Init logging
  ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
  status |= ruuvi_platform_log_init(APPLICATION_LOG_LEVEL);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Init yield
  status |= ruuvi_platform_yield_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Init GPIO
  status |= ruuvi_platform_gpio_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // LEDs high / inactive
  status |= task_led_init();
  status |= task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_ON);
  
  // Initialize button with led_cycle task
  status |= task_button_init(RUUVI_INTERFACE_GPIO_SLOPE_HITOLO, task_led_cycle);

  // Turn off sensors
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SS_ACCELERATION_PIN,  RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SS_ACCELERATION_PIN,  RUUVI_INTERFACE_GPIO_HIGH);
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN, RUUVI_INTERFACE_GPIO_HIGH);

  // Put SPI lines into HIGH state to avoid power leaks
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SCK_PIN,  RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SCK_PIN,  RUUVI_INTERFACE_GPIO_HIGH);
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_MOSI_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  status |= ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_MOSI_PIN, RUUVI_INTERFACE_GPIO_HIGH);

  // SPI MISO line is pulled up
  status |= ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_MISO_PIN, RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  status |= task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_OFF);

  if(RUUVI_DRIVER_SUCCESS == status)
  {
    status |= task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_ON);
  }
  ruuvi_platform_delay_ms(1000);
  status |= task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_OFF);
 
  while (1)
  {
    ruuvi_platform_yield();
  }
}