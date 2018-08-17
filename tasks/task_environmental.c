#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_environmental_mcu.h"
#include "ruuvi_interface_log.h"
#include "task_environmental.h"
#include "task_led.h"

#include <stddef.h>
#include <stdio.h>

static ruuvi_driver_sensor_t environmental_sensor = {0};
static ruuvi_driver_status_t task_environmental_configure(void)
{
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ENVIRONMENTAL_SAMPLERATE;
  config.resolution    = APPLICATION_ENVIRONMENTAL_RESOLUTION;
  config.scale         = APPLICATION_ENVIRONMENTAL_SCALE;
  config.dsp_function  = APPLICATION_ENVIRONMENTAL_DSPFUNC;
  config.dsp_parameter = APPLICATION_ENVIRONMENTAL_DSPPARAM;
  config.mode          = APPLICATION_ENVIRONMENTAL_MODE;
  if(NULL == environmental_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return environmental_sensor.configuration_set(&environmental_sensor, &config);
}

ruuvi_driver_status_t task_environmental_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  uint8_t handle = 0;

  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_PRESENT
    err_code = RUUVI_DRIVER_SUCCESS;
    // Only SPI supported for now
    bus = RUUVI_DRIVER_BUS_SPI;
    handle = RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN;
    err_code |= ruuvi_interface_bme280_init(&environmental_sensor, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

    if(RUUVI_DRIVER_SUCCESS == err_code)
    {
      err_code |= task_environmental_configure();
      return err_code;
    }
  #endif

  #if RUUVI_BOARD_ENVIRONMENTAL_MCU_PRESENT
    err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= ruuvi_interface_environmental_mcu_init(&environmental_sensor, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
    if(RUUVI_DRIVER_SUCCESS == err_code)
    {
      err_code |= task_environmental_configure();
      return err_code;
    }
  #endif
  // Return error if usable environmental sensor was not found.
  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_environmental_data_log(const ruuvi_interface_log_severity_t level)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_environmental_data_t data;
  if(NULL == environmental_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  err_code |= environmental_sensor.data_get(&data);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Time: %llu\r\n", data.timestamp_ms);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Temperature: %.2f\r\n", data.temperature_c);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Pressure: %.2f\r\n" ,data.pressure_pa);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Humidity: %.2f\r\n", data.humidity_rh);
  ruuvi_platform_log(level, message);
  return err_code;
}

ruuvi_driver_status_t task_environmental_on_button(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_ON);
  err_code |= task_environmental_data_log(RUUVI_INTERFACE_LOG_INFO);
  err_code |= task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_OFF);
  return err_code;
}