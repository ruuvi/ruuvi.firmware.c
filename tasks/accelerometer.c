#include "tasks.h"
#include "ruuvi_error.h"
#include "ruuvi_sensor.h"
#include "application_config.h"
#include "boards.h"
#include <stdbool.h>

#include "lis2dh12_interface.h"
#include "lis2dw12_interface.h"

#define PLATFORM_LOG_MODULE_NAME application_accelerometer
#if MAIN_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       MAIN_LOG_LEVEL
#define PLATFORM_LOG_INFO_COLOR  MAIN_INFO_COLOR
#else // ANT_BPWR_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       0
#endif // ANT_BPWR_LOG_ENABLED
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();

static ruuvi_sensor_t acceleration_sensor;
static bool sensor_is_init = false;

ruuvi_status_t task_init_accelerometer(void)
{
  sensor_is_init = true;
  ruuvi_status_t err_code = RUUVI_SUCCESS;

  #if LIS2DH12_ACCELERATION
  PLATFORM_LOG_INFO("Trying to init lis2dh12");
  err_code = lis2dh12_interface_init(&acceleration_sensor);
  if(RUUVI_SUCCESS == err_code) { return err_code; }
  PLATFORM_LOG_INFO("Trying to init lis2dh12 failed");
  #endif

  PLATFORM_LOG_INFO("No accelerometer was found");
  sensor_is_init = false;
  return RUUVI_ERROR_NOT_FOUND;
}

ruuvi_status_t task_setup_accelerometer(void)
{
  if(!sensor_is_init) { return RUUVI_ERROR_INVALID_STATE; }
  ruuvi_sensor_samplerate_t accelerometer_samplerate = 1;
  ruuvi_status_t err_code = acceleration_sensor.samplerate_set(&accelerometer_samplerate);
  PLATFORM_LOG_DEBUG("Accelerometer samplerate status: %X", err_code);

  ruuvi_sensor_scale_t accelerometer_scale = RUUVI_SENSOR_SCALE_MIN;
  err_code = acceleration_sensor.scale_set(&accelerometer_scale);
  PLATFORM_LOG_DEBUG("Accelerometer scale status: %X", err_code);

  ruuvi_sensor_resolution_t accelerometer_resolution = 10;
  err_code = acceleration_sensor.resolution_set(&accelerometer_resolution);
  PLATFORM_LOG_DEBUG("Accelerometer resolution status: %X", err_code);

  ruuvi_sensor_mode_t accelerometer_mode = RUUVI_SENSOR_MODE_CONTINOUS;
  err_code = acceleration_sensor.mode_set(&accelerometer_mode);
  PLATFORM_LOG_DEBUG("Accelerometer mode status: %X", err_code);
  return err_code;
}


ruuvi_status_t task_get_acceleration (ruuvi_acceleration_data_t* data)
{
  data->x_mg = RUUVI_FLOAT_INVALID;
  data->y_mg = RUUVI_FLOAT_INVALID;
  data->z_mg = RUUVI_FLOAT_INVALID;
  if(NULL == data || NULL == acceleration_sensor.data_get) { return RUUVI_ERROR_NULL; }
  if(!sensor_is_init) { return RUUVI_ERROR_INVALID_STATE; }
  PLATFORM_LOG_DEBUG("Getting acceleration");
  return acceleration_sensor.data_get(data);
}