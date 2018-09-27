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
#include "task_adc.h"
#include "task_advertisement.h"
#include "task_button.h"
#include "task_environmental.h"
#include "task_led.h"
#include "task_nfc.h"
#include "task_power.h"
#include "task_rtc.h"
#include "task_spi.h"
#include "test_sensor.h"

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

  // Initialize LED gpio pins, turn RED led on.
  status |= task_led_init();
  status |= task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_ON);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Initialize SPI
  status |= task_spi_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Initialize RTC
  status |= task_rtc_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Initialize power
  status |= task_power_dcdc_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Initialize nfc
  status |= task_nfc_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  #if RUUVI_RUN_TESTS
  // Tests will initialize and uninitialize the sensors, run this before using them in application
  test_sensor_run();

  // Print unit test status, activate tests by building in DEBUG configuration under SES
  size_t tests_run, tests_passed;
  test_sensor_status(&tests_run, &tests_passed);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Tests ran: %u, passed: %u\r\n", tests_run, tests_passed);
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, message);
  #endif

  // Initialize BLE
  status |= task_advertisement_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Initialize ADC
  status |= task_adc_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Initialize environmental- nRF52 will return ERROR NOT SUPPORTED if
  // DSP was configured, log warning
  status |= task_environmental_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

  // Allow NOT FOUND in case we're running on basic model, NOT_SUPPORTED in case we have previous error from BME280
  status |= task_acceleration_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_ERROR_NOT_FOUND | RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

  // Initialize button with on_button task, do not reset on errors from model Basic
  status |= task_button_init(RUUVI_INTERFACE_GPIO_SLOPE_HITOLO, task_button_on_press);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_ERROR_NOT_FOUND | RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

  // Turn RED led off. Turn GREEN LED on if no errors occured
  status |= task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_OFF);
  if(RUUVI_DRIVER_SUCCESS == status)
  {
    status |= task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_ON);
    ruuvi_platform_delay_ms(1000);
  }
  // Reset any previous errors, turn LEDs off, continue unless fatal error occurs
  status |= task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_OFF);
  RUUVI_DRIVER_ERROR_CHECK(status, ~RUUVI_DRIVER_ERROR_FATAL);

  while (1)
  {
    status = ruuvi_platform_yield();
    // Reset only on fatal error
    RUUVI_DRIVER_ERROR_CHECK(status, ~RUUVI_DRIVER_ERROR_FATAL);
  }
}