#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_interface_lis2dh12.h"
#include "ruuvi_interface_log.h"
#include "task_acceleration.h"
#include "task_led.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

static ruuvi_driver_sensor_t acceleration_sensor = {0};

static ruuvi_driver_status_t task_acceleration_configure(void)
{
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ACCELEROMETER_SAMPLERATE;
  config.resolution    = APPLICATION_ACCELEROMETER_RESOLUTION;
  config.scale         = APPLICATION_ACCELEROMETER_SCALE;
  config.dsp_function  = APPLICATION_ACCELEROMETER_DSPFUNC;
  config.dsp_parameter = APPLICATION_ACCELEROMETER_DSPPARAM;
  config.mode          = APPLICATION_ACCELEROMETER_MODE;
  if(NULL == acceleration_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return acceleration_sensor.configuration_set(&acceleration_sensor, &config);
}

ruuvi_driver_status_t task_acceleration_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  uint8_t handle = 0;

  #if RUUVI_BOARD_ACCELEROMETER_LIS2DH12_PRESENT
    err_code = RUUVI_DRIVER_SUCCESS;
    // Only SPI supported for now
    bus = RUUVI_DRIVER_BUS_SPI;
    handle = RUUVI_BOARD_SPI_SS_ACCELEROMETER_PIN;
    err_code |= ruuvi_interface_lis2dh12_init(&acceleration_sensor, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

    if(RUUVI_DRIVER_SUCCESS == err_code)
    {
      err_code |= task_acceleration_configure();
      return err_code;
    }
  #endif

  // Return error if usable acceleration sensor was not found.
  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_acceleration_data_log(const ruuvi_interface_log_severity_t level)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_acceleration_data_t data;
  if(NULL == acceleration_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  err_code |= acceleration_sensor.data_get(&data);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Time: %lu\r\n", (uint32_t)(data.timestamp_ms&0xFFFFFFFF));
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "X: %.3f\r\n", data.x_g);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Y: %.3f\r\n" ,data.y_g);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Z: %.3f\r\n", data.z_g);
  ruuvi_platform_log(level, message);
  return err_code;
}

ruuvi_driver_status_t task_acceleration_on_button(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= task_acceleration_data_log(RUUVI_INTERFACE_LOG_INFO);
  return err_code;
}