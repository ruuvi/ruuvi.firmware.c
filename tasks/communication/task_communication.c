#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_5.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"

#include "task_acceleration.h"
#include "task_adc.h"
#include "task_advertisement.h"
#include "task_environmental.h"
#include "task_rtc.h"
#include "task_sensor.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define RETURN_ON_NON_STD_MSG(incoming)  if(NULL == incoming || \
                                            RUUVI_ENDPOINT_STANDARD_MESSAGE_LENGTH != incoming->data_length) \
                                            return RUUVI_DRIVER_ERROR_INVALID_PARAM

static ruuvi_interface_timer_id_t
heartbeat_timer;               //!< Timer for heartbeat action
static size_t
m_heartbeat_data_max_len;                          //!< Maximum data length for heartbeat data
static ruuvi_interface_communication_xfer_fp_t
heartbeat_target; //!< Function to which send the hearbeat data

static ruuvi_driver_status_t task_communication_target_api_get(
  task_communication_api_t** api, uint8_t target)
{
  if(NULL == api) { return RUUVI_DRIVER_ERROR_NULL; }

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

  switch(target)
  {
    // All environmental values are controlled through the same API
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ENVIRONMENTAL:
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_TEMPERATURE:
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_HUMIDITY:
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_PRESSURE:
      task_environmental_api_get(api);
      break;

    /*
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ACCELERATION:
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ACCELERATION_X:
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ACCELERATION_Y:
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ACCELERATION_Z:
      task_acceleration_api_get(api);
      break;

    case RUUVI_ENDPOINT_STANDARD_DESTINATION_RTC:
      task_rtc_api_get(api);
      break;

    case RUUVI_ENDPOINT_STANDARD_ADC:
      task_adc_api_get(api);
      break;
    */

    default:
      err_code = RUUVI_DRIVER_ERROR_INVALID_PARAM;
  }

  return err_code;
}


ruuvi_driver_status_t task_communication_on_data(const
    ruuvi_interface_communication_message_t* const incoming,
    ruuvi_interface_communication_xfer_fp_t reply_fp)
{
  // return error if data is not understood.
  RETURN_ON_NON_STD_MSG(incoming);
  ruuvi_interface_communication_message_t reply = {0};
  // Get target API
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  task_communication_api_t* api;
  err_code |= task_communication_target_api_get(&api,
              incoming->data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX]);
  ruuvi_driver_sensor_configuration_t config;
  uint8_t payload[RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH];
  // Unless something was done with the data, assume error
  reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    switch(incoming->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX])
    {
      case RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_WRITE:
        if(NULL == api->sensor)
        {
          reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
          break;
        }

        memcpy(&config, &(incoming->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]),
               RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
        (*(api->sensor))->configuration_set(*(api->sensor), &config);

      // Intentional fallthrough to configuration read

      case RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_READ:
        if(NULL == api->sensor)
        {
          reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
          break;
        }

        (*(api->sensor))->configuration_get(*(api->sensor), &config);
        memcpy(&(reply.data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), &config,
               RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
        // Write state of sensor back to application
        reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] =
          RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_WRITE;
        break;

      case RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_WRITE:
        if(NULL == api->offset_set)
        {
          reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
          break;
        }

        memcpy(payload, &(incoming->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]),
               RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
        api->offset_set(payload);

      // Intentional fallthrough to offset read

      case RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_READ:
        if(NULL == api->offset_get)
        {
          reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
          break;
        }

        api->offset_get(payload);
        memcpy(&(reply.data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), payload,
               RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
        // Write state of sensor back to application
        reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] =
          RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_WRITE;
        break;

      case RUUVI_ENDPOINT_STANDARD_VALUE_READ:
        if(NULL == api->data_get)
        {
          reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
          break;
        }

        api->data_get(payload);
        memcpy(&(reply.data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), payload,
               RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
        reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_VALUE_WRITE;
        break;

      case RUUVI_ENDPOINT_STANDARD_LOG_VALUE_READ:
        if(NULL == api->log_read)
        {
          reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
          break;
        }

        // This call blocks until error occurs or all the requested data has been sent.
        api->log_read(reply_fp, incoming);
        // Send end of data element
        reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_LOG_VALUE_WRITE;
        memset(&(reply.data[3]), 0xFF, RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
        break;

      /*
      case RUUVI_ENDPOINT_STANDARD_LOG_CONFIGURATION_WRITE:

        break;

      case RUUVI_ENDPOINT_STANDARD_LOG_CONFIGURATION_READ:

        break;
      */

      default:
        reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
        err_code |= RUUVI_DRIVER_ERROR_NOT_SUPPORTED;
    }
  }

  reply.data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX] =
    incoming->data[RUUVI_ENDPOINT_STANDARD_SOURCE_INDEX];
  reply.data[RUUVI_ENDPOINT_STANDARD_SOURCE_INDEX] =
    incoming->data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX];
  reply.data_length = RUUVI_ENDPOINT_STANDARD_MESSAGE_LENGTH;

  while(RUUVI_DRIVER_ERROR_NO_MEM == reply_fp(&reply))
  {
    ruuvi_interface_yield();
  }

  return err_code;
}

ruuvi_driver_status_t task_communication_offsets_i32f32_to_float(
  const uint8_t* const offset, float* const converted)
{
  if(NULL == offset || NULL == converted) { return RUUVI_DRIVER_ERROR_NULL; }

  int32_t integer = 0;
  int32_t fraction = 0;
  // uint8 array is big-endian, our CPU is little-endian. Convert.
  integer |= offset[0] << 0;
  integer |= offset[1] << 8;
  integer |= offset[2] << 16;
  integer |= offset[3] << 24;
  fraction |= offset[4] << 0;
  fraction |= offset[5] << 8;
  fraction |= offset[6] << 16;
  fraction |= offset[7] << 24;
  *converted =  integer + (float)fraction / INT32_MAX;
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_communication_offsets_float_to_i32f32(
  const float* const offset, uint8_t* const converted)
{
  if(NULL == offset || NULL == converted) { return RUUVI_DRIVER_ERROR_NULL; }

  int32_t integer = (int32_t)  * offset;
  int32_t fraction = (int32_t)((*offset - floor(*offset)) * INT32_MAX);
  // uint8 array is big-endian, our CPU is little-endian. Convert.
  converted[0] = (integer >> 0) & 0xFF;
  converted[1] = (integer >> 8) & 0xFF;
  converted[2] = (integer >> 16) & 0xFF;
  converted[3] = (integer >> 12) & 0xFF;
  converted[4] = (fraction >> 0) & 0xFF;
  converted[5] = (fraction >> 5) & 0xFF;
  converted[6] = (fraction >> 6) & 0xFF;
  converted[7] = (fraction >> 7) & 0xFF;
  return RUUVI_DRIVER_SUCCESS;
}

static void select_next_backend()
{
  static uint8_t index = 0;
  static const char list[5][9] = { "BME280",
                                   "LIS2DH12",
                                   "SHTCX",
                                   "nRF5TMP",
                                   "TMP117"
                                 };
  static uint8_t ids[] = {0x80, 0x12, 0xC3, 0x52, 0x17};
  uint8_t id;
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;

  while(err_code == RUUVI_DRIVER_ERROR_NOT_FOUND)
  {
    err_code = task_environmental_backend_set(list[index]);
    id = ids[index++];
    index = index % sizeof(ids);
  }

  uint64_t address;
  uint64_t mask = 0xFFFFFFFFFFFFFF00;
  err_code |= ruuvi_interface_communication_radio_address_get(&address);
  address &= mask;
  address |= id;
  err_code |= task_advertisement_stop();
  err_code |= ruuvi_interface_communication_radio_address_set(address);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
}

static void heartbeat_send(void* p_event_data, uint16_t event_size)
{
  select_next_backend();
  ruuvi_interface_communication_message_t msg = {0};
  task_sensor_encode_to_5((uint8_t*)&msg.data);
  msg.data_length = m_heartbeat_data_max_len;
  task_advertisement_start();
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_INTERNAL;

  if(NULL != heartbeat_target)
  {
    err_code = heartbeat_target(&msg);
  }

  if(RUUVI_DRIVER_SUCCESS == err_code) { ruuvi_interface_watchdog_feed(); }

  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
}

static void heartbeat_schedule(void* p_context)
{
  ruuvi_interface_scheduler_event_put(NULL, 0, heartbeat_send);
}

ruuvi_driver_status_t task_communication_heartbeat_configure(const uint32_t interval_ms,
    const size_t max_len, const ruuvi_interface_communication_xfer_fp_t send)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

  if(NULL == heartbeat_timer)
  {
    err_code |= ruuvi_interface_timer_create(&heartbeat_timer,
                RUUVI_INTERFACE_TIMER_MODE_REPEATED, heartbeat_schedule);

    if(RUUVI_DRIVER_SUCCESS != err_code) { return err_code; }
  }

  ruuvi_interface_timer_stop(heartbeat_timer);

  if(NULL == send) { return RUUVI_DRIVER_ERROR_NULL; }

  m_heartbeat_data_max_len = max_len;
  heartbeat_target = send;

  if(0 != interval_ms)
  {
    err_code |= ruuvi_interface_timer_start(heartbeat_timer, interval_ms);
  }

  return err_code;
}