#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_log.h"
#include "task_pressure.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

static ruuvi_driver_sensor_t pressure_sensor = {0};

ruuvi_driver_status_t task_pressure_configure(ruuvi_driver_sensor_configuration_t*
    config)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO,
                      "\r\nAttempting to configure pressure with:\r\n");
  // Use empty in place of unit
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "");
  err_code |= pressure_sensor.configuration_set(&pressure_sensor, config);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, "Actual configuration:\r\n");
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "");
  return err_code;
}

ruuvi_driver_status_t task_pressure_init(void)
{
  // Assume "Not found", gets set to "Success" if a usable sensor is present
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;

  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_PRESENT
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ENVIRONMENTAL_SAMPLERATE;
  config.resolution    = APPLICATION_ENVIRONMENTAL_RESOLUTION;
  config.scale         = APPLICATION_ENVIRONMENTAL_SCALE;
  config.dsp_function  = APPLICATION_ENVIRONMENTAL_DSPFUNC;
  config.dsp_parameter = APPLICATION_ENVIRONMENTAL_DSPPARAM;
  config.mode          = APPLICATION_ENVIRONMENTAL_MODE;
  uint8_t handle = 0;
  err_code = RUUVI_DRIVER_SUCCESS;
  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_SPI_USE
  bus = RUUVI_DRIVER_BUS_SPI;
  handle = RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN;
  #endif
  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_I2C_USE
  bus = RUUVI_DRIVER_BUS_I2C;
  handle = RUUVI_BOARD_BME280_I2C_ADDRESS;
  #endif
  err_code |= ruuvi_interface_bme280_init(&pressure_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |= task_pressure_configure(&config);
    return err_code;
  }
  #endif

  return err_code;
}

ruuvi_driver_status_t task_pressure_data_get(ruuvi_interface_environmental_data_t*
    const data)
{
  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }

  if(NULL == pressure_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code = pressure_sensor.data_get(data);

  return err_code;
}


bool task_pressure_is_init()
{
  return NULL != pressure_sensor.data_get;
}