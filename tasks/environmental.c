#include "application_config.h"
#include "boards.h"
#include <stdbool.h>

#include "bme280_interface.h"
#include "environmental.h"

#define PLATFORM_LOG_MODULE_NAME application_environmental
#if MAIN_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       MAIN_LOG_LEVEL
#define PLATFORM_LOG_INFO_COLOR  MAIN_INFO_COLOR
#else
#define PLATFORM_LOG_LEVEL       0
#endif
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();

static ruuvi_sensor_t environmental_sensor;
static bool sensor_is_init = false;

ruuvi_status_t task_init_environmental(void)
{
  ruuvi_status_t err_code = bme280_interface_init(&environmental_sensor);
  PLATFORM_LOG_INFO("BME init status: %X", err_code);
  if(RUUVI_SUCCESS == err_code) { sensor_is_init = true; }
  return err_code;
}

ruuvi_status_t task_setup_environmental(void)
{
  if(!sensor_is_init) { return RUUVI_ERROR_INVALID_STATE; }
  ruuvi_sensor_samplerate_t samplerate = 1;
  ruuvi_status_t err_code = environmental_sensor.samplerate_set(&samplerate);
  PLATFORM_LOG_DEBUG("BME samplerate status: %X", err_code);

  ruuvi_sensor_dsp_function_t dsp = RUUVI_SENSOR_DSP_IIR;
  uint8_t dsp_parameter = 16;
  err_code |= environmental_sensor.dsp_set(&dsp, &dsp_parameter);
  PLATFORM_LOG_DEBUG("BME DSP status: %X", err_code);

  ruuvi_sensor_mode_t bme280_mode = RUUVI_SENSOR_MODE_CONTINOUS;
  err_code |= environmental_sensor.mode_set(&bme280_mode);
  PLATFORM_LOG_DEBUG("BME mode status: %X", err_code);
  return err_code;
}

ruuvi_status_t task_get_environmental (ruuvi_environmental_data_t* data)
{
  data->temperature = RUUVI_FLOAT_INVALID; // C
  data->humidity = RUUVI_FLOAT_INVALID;    // RH-%
  data->pressure = RUUVI_FLOAT_INVALID;    // Pa
  if(NULL == data || NULL == environmental_sensor.data_get) { return RUUVI_ERROR_NULL; }
  if(!sensor_is_init) { return RUUVI_ERROR_INVALID_STATE; }
  return environmental_sensor.data_get(data);
}