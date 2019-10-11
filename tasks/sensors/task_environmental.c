#include "application_config.h"
#include "data_structures/ruuvi_library_ringbuffer.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_atomic.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_shtcx.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_environmental_mcu.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "task_communication.h"
#include "task_environmental.h"
#include "task_pressure.h"
#include "task_rtc.h"
#include "task_led.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifndef TASK_ENVIRONMENTAL_LOG_LEVEL
#define TASK_ENVIRONMENTAL_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

#define LOG(msg) ruuvi_interface_log(TASK_ENVIRONMENTAL_LOG_LEVEL, msg)
#define LOGHEX(msg, len) ruuvi_interface_log_hex(TASK_ENVIRONMENTAL_LOG_LEVEL, msg, len)

static ruuvi_driver_sensor_t environmental_sensor = {0}; // Sensor API
static ruuvi_interface_timer_id_t environmental_timer;

static uint8_t buffer[APPLICATION_ENVIRONMENTAL_RAMBUFFER_SIZE]; //!< Raw buffer for environmental logs
static ruuvi_interface_atomic_t buffer_wlock;
static ruuvi_interface_atomic_t buffer_rlock;
/** @brief Buffer structure for outgoing data */
static ruuvi_library_ringbuffer_t ringbuf = {.head = 0,
                                             .tail = 0,
                                             .block_size = sizeof(environmental_log_t),
                                             .storage_size = sizeof(buffer),
                                             .index_mask = (sizeof(buffer) / sizeof(environmental_log_t)) - 1,
                                             .storage = buffer,
                                             .lock = ruuvi_interface_atomic_flag,
                                             .writelock = &buffer_wlock,
                                             .readlock  = &buffer_rlock};

static task_communication_api_t environmental_api = {
  .sensor      = &environmental_sensor,
  .offset_set  = NULL,
  .offset_get  = NULL,
  .data_target = NULL,
  .log_cfg     = NULL,
  .log_read    = task_environmental_log_read
};

ruuvi_driver_status_t task_environmental_api_get(task_communication_api_t** api)
{
 if(api == NULL) { return RUUVI_DRIVER_ERROR_NULL; }
 *api = &environmental_api;
 LOG("Returned API \r\n");
 return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_environmental_configure(ruuvi_driver_sensor_configuration_t*
    config)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  LOG("\r\nAttempting to configure environmental with:\r\n");
  // Use empty in place of unit
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "");
  err_code |= environmental_sensor.configuration_set(&environmental_sensor, config);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  LOG("Actual configuration:\r\n");
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "");
  return err_code;
}

static void task_environmental_scheduler_task(void* p_event_data, uint16_t event_size)
{
  task_environmental_log();
}

// Timer callback, schedule event here or execute it right away if it's timing-critical
static void task_environmental_timer_cb(void* p_context)
{
  ruuvi_interface_scheduler_event_put(NULL, 0, task_environmental_scheduler_task);
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
  ruuvi_interface_timer_create(&environmental_timer, RUUVI_INTERFACE_TIMER_MODE_REPEATED, task_environmental_timer_cb);
  ruuvi_interface_timer_start(environmental_timer, APPLICATION_ENVIRONMENTAL_LOG_INTERVAL_MS);

  #if (RUUVI_BOARD_ENVIRONMENTAL_SHTCX_PRESENT && RUUVI_INTERFACE_ENVIRONMENTAL_SHTCX_ENABLED)
  err_code = RUUVI_DRIVER_SUCCESS;
  bus = RUUVI_DRIVER_BUS_I2C;
  handle = RUUVI_BOARD_SHTCX_I2C_ADDRESS;
  err_code |= ruuvi_interface_shtcx_init(&environmental_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |= task_environmental_configure(&config);
    // Try to configure pressure sensor for the application. 
    task_pressure_init();
    return err_code;
  }
  #endif

  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_PRESENT
  err_code = RUUVI_DRIVER_SUCCESS;
  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_SPI_USE
  bus = RUUVI_DRIVER_BUS_SPI;
  handle = RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN;
  #endif
  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_I2C_USE
  bus = RUUVI_DRIVER_BUS_I2C;
  handle = RUUVI_BOARD_BME280_I2C_ADDRESS;
  #endif
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

ruuvi_driver_status_t task_environmental_data_log(const ruuvi_interface_log_severity_t
    level)
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
  snprintf(message, sizeof(message), "Time: %lu\r\n",
           (uint32_t)(data.timestamp_ms & 0xFFFFFFFF));
  ruuvi_interface_log(level, message);
  snprintf(message, sizeof(message), "Temperature: %.2f\r\n", data.temperature_c);
  ruuvi_interface_log(level, message);
  snprintf(message, sizeof(message), "Pressure: %.2f\r\n", data.pressure_pa);
  ruuvi_interface_log(level, message);
  snprintf(message, sizeof(message), "Humidity: %.2f\r\n", data.humidity_rh);
  ruuvi_interface_log(level, message);
  return err_code;
}

ruuvi_driver_status_t task_environmental_data_get(ruuvi_interface_environmental_data_t*
    const data)
{
  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }

  if(NULL == environmental_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code = environmental_sensor.data_get(data);
  if(RUUVI_DRIVER_FLOAT_INVALID == data->pressure_pa && task_pressure_is_init())
  {
    ruuvi_interface_environmental_data_t pressure_data;
    err_code |= task_pressure_data_get(&pressure_data);
    data->pressure_pa = pressure_data.pressure_pa;
  }

  return err_code;
}

ruuvi_driver_status_t task_environmental_on_button(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= task_environmental_data_log(RUUVI_INTERFACE_LOG_INFO);
  return err_code;
}

ruuvi_driver_status_t task_environmental_log(void)
{
  ruuvi_interface_environmental_data_t data;
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= task_environmental_data_get(&data);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  environmental_log_t log = { .timestamp_s   = data.timestamp_ms/1000,
                              .temperature_c = data.temperature_c,
                              .humidity_rh   = data.humidity_rh,
                              .pressure_pa   = data.pressure_pa};
  ruuvi_library_status_t status = ruuvi_library_ringbuffer_queue(&ringbuf, &log, sizeof(log));
  // Drop old sample if buffer is full
  if(RUUVI_LIBRARY_ERROR_NO_MEM == status)
  {
    LOG("Discarded data... ");
    environmental_log_t drop;
    status = ruuvi_library_ringbuffer_dequeue(&ringbuf, &drop);
    status |= ruuvi_library_ringbuffer_queue(&ringbuf, &log, sizeof(log));
  }
  RUUVI_DRIVER_ERROR_CHECK(status, ~RUUVI_DRIVER_ERROR_FATAL);
  if(RUUVI_DRIVER_SUCCESS == (status | err_code))
  {
    LOG("Stored data\r\n");
  }
  return status | err_code;
}

ruuvi_driver_status_t task_environmental_log_read(const ruuvi_interface_communication_xfer_fp_t reply_fp,
                                                  const ruuvi_interface_communication_message_t* const query)
{
  LOG("Preparing to send logs\r\n");
  ruuvi_interface_communication_message_t msg = { 0 };
  uint64_t systime = task_rtc_millis()/1000;
  uint32_t now =   (query->data[3] << 24) +
                   (query->data[4] << 16) +
                   (query->data[5] << 8) +
                   (query->data[6] << 0);
  uint32_t start = (query->data[7] << 24) +
                   (query->data[8] << 16) +
                   (query->data[9] << 8) +
                   (query->data[10] << 0);
  uint32_t offset = (now > systime)? now-systime : 0;

  environmental_log_t* p_log;
  msg.data_length = RUUVI_ENDPOINT_STANDARD_MESSAGE_LENGTH;
  msg.data[0] = query->data[1];
  msg.data[1] = query->data[0];
  msg.data[2] = RUUVI_ENDPOINT_STANDARD_LOG_VALUE_WRITE;
  ruuvi_library_status_t status = RUUVI_LIBRARY_SUCCESS;
  // As long as we have more elements
  size_t index = 0;
  do
  {
    status = ruuvi_library_ringbuffer_peek(&ringbuf, &p_log, index++);
    // Send logged element
    if(RUUVI_LIBRARY_SUCCESS == status)
    {
      // Calculate real time of event
      uint32_t timestamp = offset + p_log->timestamp_s;
      // Check if the event is in range to send. Continue if not
      if(0 == p_log->timestamp_s) 
      { 
        LOG("WARNING: Empty element\r\n");
        continue;
      } 
      if(start > timestamp) { continue; }
      msg.data[3] = timestamp >> 24;
      msg.data[4] = timestamp >> 16;
      msg.data[5] = timestamp >> 8;
      msg.data[6] = timestamp >> 0;
      uint8_t destination = query->data[0];
      // send temp, humi, pressure
      if(RUUVI_ENDPOINT_STANDARD_DESTINATION_ENVIRONMENTAL == destination ||
         RUUVI_ENDPOINT_STANDARD_DESTINATION_TEMPERATURE   == destination)
      {
        int32_t temperature_cc = p_log->temperature_c*100;
        msg.data[1] = RUUVI_ENDPOINT_STANDARD_DESTINATION_TEMPERATURE;
        msg.data[7] = temperature_cc >> 24;
        msg.data[8] = temperature_cc >> 16;
        msg.data[9] = temperature_cc >> 8;
        msg.data[10] = temperature_cc >> 0;
    
        // Repeat sending here
        while(RUUVI_LIBRARY_ERROR_NO_MEM == reply_fp(&msg))
        {
          // Sleep
          ruuvi_interface_yield();
        }
      }

      if(RUUVI_ENDPOINT_STANDARD_DESTINATION_ENVIRONMENTAL == destination ||
         RUUVI_ENDPOINT_STANDARD_DESTINATION_HUMIDITY      == destination)
      {
        uint32_t humidity_crh = p_log->humidity_rh*100;
        msg.data[1] = RUUVI_ENDPOINT_STANDARD_DESTINATION_HUMIDITY;
        msg.data[7]  = humidity_crh >> 24;
        msg.data[8]  = humidity_crh >> 16;
        msg.data[9]  = humidity_crh >> 8;
        msg.data[10] = humidity_crh >> 0;
    
        // Repeat sending here
        while(RUUVI_LIBRARY_ERROR_NO_MEM == reply_fp(&msg))
        {
          // Sleep
          ruuvi_interface_yield();
        }
      }

      if(RUUVI_ENDPOINT_STANDARD_DESTINATION_ENVIRONMENTAL == destination ||
         RUUVI_ENDPOINT_STANDARD_DESTINATION_PRESSURE      == destination)
      {
        if(isnan( p_log->pressure_pa)) { continue; }
        uint32_t pressure_pa = p_log->pressure_pa;
        msg.data[1] = RUUVI_ENDPOINT_STANDARD_DESTINATION_PRESSURE;
        msg.data[7]  = pressure_pa >> 24;
        msg.data[8]  = pressure_pa >> 16;
        msg.data[9]  = pressure_pa >> 8;
        msg.data[10] = pressure_pa >> 0;
    
        // Repeat sending here
        while(RUUVI_LIBRARY_ERROR_NO_MEM == reply_fp(&msg))
        {
          // Sleep
          ruuvi_interface_yield();
        }
      }
    }
  }while(RUUVI_LIBRARY_SUCCESS == status);
  LOG("Logs sent\r\n");
  return RUUVI_DRIVER_SUCCESS;
}
