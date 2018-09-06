#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_adc_mcu.h"
#include "ruuvi_interface_log.h"
#include "task_adc.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

static ruuvi_driver_sensor_t adc_sensor = {0};

static ruuvi_driver_status_t task_adc_configure(void)
{
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ADC_SAMPLERATE;
  config.resolution    = APPLICATION_ADC_RESOLUTION;
  config.scale         = APPLICATION_ADC_SCALE;
  config.dsp_function  = APPLICATION_ADC_DSPFUNC;
  config.dsp_parameter = APPLICATION_ADC_DSPPARAM;
  config.mode          = APPLICATION_ADC_MODE;
  if(NULL == adc_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return adc_sensor.configuration_set(&adc_sensor, &config);
}

ruuvi_driver_status_t task_adc_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  uint8_t handle = RUUVI_INTERFACE_ADC_AINVDD;

  err_code |= ruuvi_interface_adc_mcu_init(&adc_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |= task_adc_configure();
    return err_code;
  }

  // Return error if ADC could not be configured
  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_adc_data_log(const ruuvi_interface_log_severity_t level)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_adc_data_t data;
  if(NULL == adc_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  // If the mode is single, take a new sample.
  if(APPLICATION_ADC_MODE == RUUVI_DRIVER_SENSOR_CFG_SINGLE)
  {
    err_code |= task_adc_sample();
  }

  err_code |= adc_sensor.data_get(&data);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Time: %u\r\n", data.timestamp_ms&0xFFFFFFFF);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Battery: %.3f\r\n", data.adc_v);
  ruuvi_platform_log(level, message);
  return err_code;
}

ruuvi_driver_status_t task_adc_sample(void)
{
  if(NULL == adc_sensor.mode_set) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
  return adc_sensor.mode_set(&mode);
}

ruuvi_driver_status_t task_adc_on_button(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= task_adc_sample();
  err_code |= task_adc_data_log(RUUVI_INTERFACE_LOG_INFO);
  return err_code;
}