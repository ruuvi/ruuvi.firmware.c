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
#include "task_acceleration.h"
#include "task_button.h"
#include "task_environmental.h"
#include "task_led.h"
#include "task_spi.h"

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

  // Initialize SPI
  status |= task_spi_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Initialize environmental- nRF52 will return ERROR NOT SUPPORTED if
  // DSP was configured, log warning
  status = task_environmental_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

  status |= task_acceleration_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_ERROR_NOT_FOUND);

  // Initialize button with on_button task
  status = task_button_init(RUUVI_INTERFACE_GPIO_SLOPE_HITOLO, task_button_on_press);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  status = task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_OFF);

  // Turn GREEN LED on if BME280 was found
  if(RUUVI_DRIVER_SUCCESS == status)
  {
    status |= task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_ON);
  }
  ruuvi_platform_delay_ms(1000);
  status |= task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_OFF);

  // Reset status flag
  status = RUUVI_DRIVER_SUCCESS;

  while (1)
  {
    status |= ruuvi_platform_yield();
  }
}