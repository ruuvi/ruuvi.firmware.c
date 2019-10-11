#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"
#include "task_sensor.h"

#ifndef TASK_SENSOR_LOG_LEVEL
#define TASK_SENSOR_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

#define LOG(msg) ruuvi_interface_log(TASK_SENSOR_LOG_LEVEL, msg)
#define LOGHEX(msg, len) ruuvi_interface_log_hex(TASK_SENSOR_LOG_LEVEL, msg, len)

ruuvi_driver_status_t task_sensor_configure(ruuvi_driver_sensor_t* const sensor, 
                                            ruuvi_driver_sensor_configuration_t* const config, 
                                            const char* const unit)
{
  if(NULL == sensor || NULL == config) { return RUUVI_DRIVER_ERROR_NULL; }
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  LOG("\r\nAttempting to configure "); 
  LOG(sensor->name);
  LOG(" with:\r\n");
  ruuvi_interface_log_sensor_configuration(TASK_SENSOR_LOG_LEVEL, config, unit);
  err_code |= sensor->configuration_set(sensor, config);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  LOG("Actual configuration:\r\n");
  ruuvi_interface_log_sensor_configuration(TASK_SENSOR_LOG_LEVEL, config, unit);
  return err_code;
}