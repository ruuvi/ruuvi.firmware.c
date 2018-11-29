#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_environmental_mcu.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "task_environmental.h"
#include "task_led.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

RUUVI_PLATFORM_TIMER_ID_DEF(environmental_timer);
static ruuvi_driver_sensor_t environmental_sensor = {0};

//handler for scheduled accelerometer event
static void task_environmental_scheduler_task(void *p_event_data, uint16_t event_size)
{
  // No action necessary
}

// Timer callback, schedule accelerometer event here.
static void task_environmental_timer_cb(void* p_context)
{
  ruuvi_platform_scheduler_event_put(NULL, 0, task_environmental_scheduler_task);
}

ruuvi_driver_status_t task_environmental_configure(ruuvi_driver_sensor_configuration_t* config)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "\r\nAttempting to configure environmental with:\r\n");
  // Use empty in place of unit
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "");
  err_code |= environmental_sensor.configuration_set(&environmental_sensor, config);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Actual configuration:\r\n");
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "");
  return err_code;
}

ruuvi_driver_status_t task_environmental_init(void)
{
  // Assume "Not found", gets set to "Success" if a usable sensor is present
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ENVIRONMENTAL_SAMPLERATE;
  config.resolution    = APPLICATION_ENVIRONMENTAL_RESOLUTION;
  config.scale         = APPLICATION_ENVIRONMENTAL_SCALE;
  config.dsp_function  = APPLICATION_ENVIRONMENTAL_DSPFUNC;
  config.dsp_parameter = APPLICATION_ENVIRONMENTAL_DSPPARAM;
  config.mode          = APPLICATION_ENVIRONMENTAL_MODE;
  uint8_t handle = 0;

  // Initialize timer for environmental task. Note: the timer is not started.
  err_code |= ruuvi_platform_timer_create(&environmental_timer, RUUVI_INTERFACE_TIMER_MODE_REPEATED, task_environmental_timer_cb);

  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_PRESENT
    err_code = RUUVI_DRIVER_SUCCESS;
    // Only SPI supported for now
    bus = RUUVI_DRIVER_BUS_SPI;
    handle = RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN;
    err_code |= ruuvi_interface_bme280_init(&environmental_sensor, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

    if(RUUVI_DRIVER_SUCCESS == err_code)
    {
      err_code |= task_environmental_configure(&config);
      return err_code;
    }
  #endif

  #if RUUVI_BOARD_ENVIRONMENTAL_MCU_PRESENT
    err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= ruuvi_interface_environmental_mcu_init(&environmental_sensor, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
    if(RUUVI_DRIVER_SUCCESS == err_code)
    {
      err_code |= task_environmental_configure(&config);
      return err_code;
    }
  #endif

  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_environmental_sample(void)
{
  if(NULL == environmental_sensor.mode_set) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
  return environmental_sensor.mode_set(&mode);
}

ruuvi_driver_status_t task_environmental_data_log(const ruuvi_interface_log_severity_t level)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_environmental_data_t data;
  if(NULL == environmental_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  // If the mode is single, take a new sample.
  if(APPLICATION_ENVIRONMENTAL_MODE == RUUVI_DRIVER_SENSOR_CFG_SINGLE)
  {
    err_code |= task_environmental_sample();
  }

  err_code |= environmental_sensor.data_get(&data);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Time: %lu\r\n", (uint32_t)(data.timestamp_ms&0xFFFFFFFF));
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Temperature: %.2f\r\n", data.temperature_c);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Pressure: %.2f\r\n" ,data.pressure_pa);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Humidity: %.2f\r\n", data.humidity_rh);
  ruuvi_platform_log(level, message);
  return err_code;
}

ruuvi_driver_status_t task_environmental_data_get(ruuvi_interface_environmental_data_t* const data)
{
  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }
  if(NULL == environmental_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return environmental_sensor.data_get(data);
}

ruuvi_driver_status_t task_environmental_on_button(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= task_environmental_data_log(RUUVI_INTERFACE_LOG_INFO);
  return err_code;
}