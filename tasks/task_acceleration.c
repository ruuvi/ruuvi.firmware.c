#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_lis2dh12.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_yield.h"

#include "task_acceleration.h"
#include "task_advertisement.h"
#include "task_button.h"
#include "task_communication.h"
#include "task_gatt.h"
#include "task_led.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define TASK_ACCELERATION_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#define LOG(x) ruuvi_interface_log(TASK_ACCELERATION_LOG_LEVEL, x);
#define LOGD(x) ruuvi_interface_log(RUUVI_INTERFACE_LOG_DEBUG, x);

static ruuvi_driver_sensor_t acceleration_sensor = {0};
static ruuvi_interface_acceleration_data_t offsets = { 0 };
static uint8_t m_nbr_movements;
static task_communication_api_t acceleration_api = {
  .sensor = &acceleration_sensor,
  .offset_set = NULL,
  .offset_get = NULL,
  .data_target = NULL,
  .log_cfg     = task_acceleration_logging_configure
};

static task_communication_api_t acceleration_x_api = {
  .sensor = NULL,
  .offset_set = task_acceleration_offset_x_set,
  .offset_get = task_acceleration_offset_x_get,
  .data_target = NULL,
  .log_cfg     = NULL
};

static task_communication_api_t acceleration_y_api = {
  .sensor = NULL,
  .offset_set = task_acceleration_offset_y_set,
  .offset_get = task_acceleration_offset_y_get,
  .data_target = NULL,
  .log_cfg     = NULL
};

static task_communication_api_t acceleration_z_api = {
  .sensor = NULL,
  .offset_set = task_acceleration_offset_z_set,
  .offset_get = task_acceleration_offset_z_get,
  .data_target = NULL,
  .log_cfg     = NULL
};

ruuvi_driver_status_t task_acceleration_configure(ruuvi_driver_sensor_configuration_t*
    const config)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  //float ths = APPLICATION_ACCELEROMETER_ACTIVITY_THRESHOLD;
  LOG("\r\nAttempting to configure accelerometer with:\r\n");
  ruuvi_interface_log_sensor_configuration(TASK_ACCELERATION_LOG_LEVEL, config, "g");
  err_code |= acceleration_sensor.configuration_set(&acceleration_sensor, config);
  LOG("Actual configuration:\r\n");
  ruuvi_interface_log_sensor_configuration(TASK_ACCELERATION_LOG_LEVEL, config, "g");
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  //acceleration_sensor.level_interrupt_set(true, &ths);
  return err_code;
}

static void task_acceleration_movement_task(void* p_event_data, uint16_t event_size)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_interval_set(
                APPLICATION_ADVERTISING_INTERVAL);
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ACCELEROMETER_SAMPLERATE;
  config.resolution    = APPLICATION_ACCELEROMETER_RESOLUTION;
  config.scale         = APPLICATION_ACCELEROMETER_SCALE;
  config.dsp_function  = APPLICATION_ACCELEROMETER_DSPFUNC;
  config.dsp_parameter = APPLICATION_ACCELEROMETER_DSPPARAM;
  config.mode          = APPLICATION_ACCELEROMETER_MODE;
  err_code |= task_acceleration_configure(&config);
  float ths = APPLICATION_ACCELEROMETER_ACTIVITY_THRESHOLD;
  err_code |= ruuvi_interface_lis2dh12_activity_interrupt_use(false, &ths);
  err_code |= ruuvi_interface_lis2dh12_fifo_interrupt_use(true);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  LOGD("Activity\r\n");
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
}

void task_acceleration_offset_x_set(uint8_t* const data)
{
  if(NULL == data)
  {
    offsets.x_g = 0;
  }
  else
  {
    task_communication_offsets_i32f32_to_float(data, &offsets.x_g);
  }
}

void task_acceleration_offset_x_get(uint8_t* const data)
{
  if(NULL == data)
  {
    // nothing to do
  }
  else
  {
    task_communication_offsets_float_to_i32f32(&offsets.x_g, data);
  }
}

void task_acceleration_offset_y_set(uint8_t* const data)
{
  if(NULL == data)
  {
    offsets.y_g = 0;
  }
  else
  {
    task_communication_offsets_i32f32_to_float(data, &offsets.y_g);
  }
}

void task_acceleration_offset_y_get(uint8_t* const data)
{
  if(NULL == data)
  {
    // nothing to do
  }
  else
  {
    task_communication_offsets_float_to_i32f32(&offsets.y_g, data);
  }
}

void task_acceleration_offset_z_set(uint8_t* const data)
{
  if(NULL == data)
  {
    offsets.z_g = 0;
  }
  else
  {
    task_communication_offsets_i32f32_to_float(data, &offsets.z_g);
  }
}

void task_acceleration_offset_z_get(uint8_t* const data)
{
  if(NULL == data)
  {
    // nothing to do
  }
  else
  {
    task_communication_offsets_float_to_i32f32(&offsets.z_g, data);
  }
}

ruuvi_driver_status_t task_acceleration_on_activity(void* p_event_data, uint16_t event_size)
{
  m_nbr_movements++;
  ruuvi_driver_status_t err_code = ruuvi_interface_scheduler_event_put(NULL, 0,
                                   task_acceleration_movement_task);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  return err_code;
}

ruuvi_driver_status_t task_acceleration_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ACCELEROMETER_SAMPLERATE;
  config.resolution    = APPLICATION_ACCELEROMETER_RESOLUTION;
  config.scale         = APPLICATION_ACCELEROMETER_SCALE;
  config.dsp_function  = APPLICATION_ACCELEROMETER_DSPFUNC;
  config.dsp_parameter = APPLICATION_ACCELEROMETER_DSPPARAM;
  config.mode          = APPLICATION_ACCELEROMETER_MODE;
  uint8_t handle = 0;
  m_nbr_movements = 0;

  // XXX TODO: Refactor gpio init into it's own task
  if(!ruuvi_interface_gpio_is_init()) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  #if RUUVI_BOARD_ACCELEROMETER_LIS2DH12_PRESENT
  err_code = RUUVI_DRIVER_SUCCESS;
  // Only SPI supported for now
  bus = RUUVI_DRIVER_BUS_SPI;
  handle = RUUVI_BOARD_SPI_SS_ACCELEROMETER_PIN;
  err_code |= ruuvi_interface_lis2dh12_init(&acceleration_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |= task_acceleration_configure(&config);
    return err_code;
  }

  #endif
  // Return error if usable acceleration sensor was not found.
  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_acceleration_data_log(const ruuvi_interface_log_severity_t
    level)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_acceleration_data_t data;

  if(NULL == acceleration_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  err_code |= acceleration_sensor.data_get(&data);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Time: %lu\r\n",
           (uint32_t)(data.timestamp_ms & 0xFFFFFFFF));
  LOG(message);
  snprintf(message, sizeof(message), "X: %.3f g, Y: %.3f g, Z: %.3f g\r\n", data.x_g, data.y_g, data.z_g);
  LOG(message);
  return err_code;
}

ruuvi_driver_status_t task_acceleration_data_get(ruuvi_interface_acceleration_data_t*
    const data)
{
  data->timestamp_ms = RUUVI_DRIVER_UINT64_INVALID;
  data->x_g = RUUVI_DRIVER_FLOAT_INVALID;
  data->y_g = RUUVI_DRIVER_FLOAT_INVALID;
  data->z_g = RUUVI_DRIVER_FLOAT_INVALID;

  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }

  if(NULL == acceleration_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  ruuvi_driver_status_t err_code = acceleration_sensor.data_get(data);
  task_communication_offsets_apply((ruuvi_driver_sensor_data_t*)data, (ruuvi_driver_sensor_data_t*)&offsets);

  return err_code;
}

ruuvi_driver_status_t task_acceleration_on_button(void)
{
  // No implementation needed
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_acceleration_movement_count_get(uint8_t* const count)
{
  *count = m_nbr_movements;
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_acceleration_fifo_use(const bool enable)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

  if(true == enable)
  {
    err_code |= ruuvi_interface_lis2dh12_fifo_use(true);
    err_code |= ruuvi_interface_lis2dh12_fifo_interrupt_use(true);
  }

  if(false == enable)
  {
    err_code |= ruuvi_interface_lis2dh12_fifo_use(false);
    err_code |= ruuvi_interface_lis2dh12_fifo_interrupt_use(false);
  }

  return err_code;
}

ruuvi_driver_status_t task_acceleration_api_get(task_communication_api_t** api)
{
 if(api == NULL) { return RUUVI_DRIVER_ERROR_NULL; }
 *api = &acceleration_api;
 return RUUVI_DRIVER_SUCCESS;

}

ruuvi_driver_status_t task_acceleration_api_x_get(task_communication_api_t** api)
{
 if(api == NULL) { return RUUVI_DRIVER_ERROR_NULL; }
 *api = &acceleration_x_api;
 return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_acceleration_api_y_get(task_communication_api_t** api)
{
  if(api == NULL) { return RUUVI_DRIVER_ERROR_NULL; }
  *api = &acceleration_y_api; 
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_acceleration_api_z_get(task_communication_api_t** api)
{
 if(api == NULL) { return RUUVI_DRIVER_ERROR_NULL; }
  *api = &acceleration_z_api;
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_acceleration_logging_configure(const task_api_data_target_t const target, const uint8_t interval)
{
  return RUUVI_DRIVER_ERROR_NOT_IMPLEMENTED;
}