#include "ruuvi_error.h"
#include "ruuvi_sensor.h"
#include "application_config.h"
#include "boards.h"
#include <stdbool.h>

#include "acceleration.h"
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
static bool m_sensor_is_init = false;

ruuvi_status_t task_accelerometer_init(void)
{
  m_sensor_is_init = true;
  ruuvi_status_t err_code = RUUVI_SUCCESS;

  #if LIS2DH12_ACCELERATION
  PLATFORM_LOG_INFO("Trying to init lis2dh12");
  err_code = lis2dh12_interface_init(&acceleration_sensor);
  if(RUUVI_SUCCESS == err_code) { return err_code; }
  PLATFORM_LOG_INFO("Trying to init lis2dh12 failed");
  #endif

  PLATFORM_LOG_INFO("No accelerometer was found");
  m_sensor_is_init = false;
  return RUUVI_ERROR_NOT_FOUND;
}

// Configure accelerometer with given configuration.
// Important: The configuration will have implemented configuration error codes as output.
ruuvi_status_t task_accelerometer_setup(const ruuvi_sensor_configuration_t* configuration)
{
  if(!m_sensor_is_init) { return RUUVI_ERROR_INVALID_STATE; }
  ruuvi_status_t err_code = acceleration_sensor.samplerate_set(configuration->samplerate);
  PLATFORM_LOG_DEBUG("Accelerometer samplerate status: %X", err_code);

  err_code = acceleration_sensor.scale_set(configuration->scale);
  PLATFORM_LOG_DEBUG("Accelerometer scale status: %X", err_code);

  err_code = acceleration_sensor.resolution_set(configuration->resolution);
  PLATFORM_LOG_DEBUG("Accelerometer resolution status: %X", err_code);

  err_code = acceleration_sensor.mode_set(configuration->mode);
  PLATFORM_LOG_DEBUG("Accelerometer mode status: %X", err_code);
  return err_code;
}


ruuvi_status_t task_accelerometer_get (ruuvi_acceleration_data_t* data)
{
  data->x_mg = RUUVI_FLOAT_INVALID;
  data->y_mg = RUUVI_FLOAT_INVALID;
  data->z_mg = RUUVI_FLOAT_INVALID;
  if(NULL == data || NULL == acceleration_sensor.data_get) { return RUUVI_ERROR_NULL; }
  if(!m_sensor_is_init) { return RUUVI_ERROR_INVALID_STATE; }
  PLATFORM_LOG_DEBUG("Getting acceleration");
  return acceleration_sensor.data_get(data);
}