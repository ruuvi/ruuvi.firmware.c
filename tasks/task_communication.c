#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_yield.h"

#include "task_acceleration.h"
#include "task_adc.h"
#include "task_environmental.h"
#include "task_rtc.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

static ruuvi_driver_status_t task_communication_target_api_get(task_communication_api_t* api, uint8_t target)
{
  if(NULL == api) { return RUUVI_DRIVER_ERROR_NULL; }
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  
  switch(target)
  {
    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ACCELERATION:
      task_acceleration_api_get(&api);
      break;

    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ACCELERATION_X:
      task_acceleration_api_x_get(&api);
      break;

    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ACCELERATION_Y:
      task_acceleration_api_y_get(&api);
      break;

    case RUUVI_ENDPOINT_STANDARD_DESTINATION_ACCELERATION_Z:
      task_acceleration_api_z_get(&api);
      break;

    case RUUVI_ENDPOINT_STANDARD_DESTINATION_RTC:
      task_rtc_api_get(&api);
      break;
/*
    case RUUVI_ENDPOINT_STANDARD_ADC:
      task_adc_api_get(&api);
      break;

    case RUUVI_ENDPOINT_STANDARD_ENVIRONMENTAL:
      task_environmental_api_get(&api);
      break;

    */
    
    default:
      err_code = RUUVI_DRIVER_ERROR_INVALID_PARAM;
  }
  return err_code;
}


ruuvi_driver_status_t task_communication_on_data(const ruuvi_interface_communication_message_t* const incoming, ruuvi_interface_communication_message_t* const reply)
{
  // return error if data is not understood.
  if(NULL == incoming ||
     RUUVI_ENDPOINT_STANDARD_MESSAGE_LENGTH != incoming->data_length ||
     NULL == reply ||
     RUUVI_ENDPOINT_STANDARD_MESSAGE_LENGTH != reply->data_length)
  {
    return RUUVI_DRIVER_ERROR_INVALID_PARAM;   
  }

  // Get target API
  task_communication_api_t api;
  ruuvi_driver_sensor_configuration_t config;
  float offset;
  uint8_t payload[RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH];
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_sensor_data_t data;
  err_code |= task_communication_target_api_get(&api, incoming->data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX]);
  // Unless something was done with the data, assume error
  reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
  switch(incoming->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX])
  {
    case RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_WRITE:
      if(NULL == api.sensor) 
      { 
        reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
        break; 
      }
      mempcpy(&config, &(incoming->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
      api.sensor->configuration_set(api.sensor, &config);
      // Intentional fallthrough to configuration read

    case RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_READ:
      if(NULL == api.sensor) 
      { 
        reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
        break; 
      }
      api.sensor->configuration_get(api.sensor, &config);
      memcpy(&(reply->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), &config, RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
      // Write state of sensor back to application
      reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_WRITE;
      break;

    case RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_WRITE:
      if(NULL == api.offset_set) 
      { 
        reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
        break; 
      }
      memcpy(payload, &(incoming->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
      api.offset_set(payload);
      // Intentional fallthrough to offset read

    case RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_READ:
      if(NULL == api.offset_get) 
      { 
        reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
        break; 
      }
      api.offset_get(payload);
      memcpy(&(reply->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), payload, RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);

      // Write state of sensor back to application
      reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_WRITE;
      break;

    case RUUVI_ENDPOINT_STANDARD_VALUE_READ:
      if(NULL == api.data_get)   
      { 
        reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
        break; 
      }
      api.data_get(payload);
      memcpy(&(reply->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), payload, RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
      reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_VALUE_WRITE;
      break;

    /*
    case RUUVI_ENDPOINT_STANDARD_LOG_CONFIGURATION_WRITE:
      
      break;

    case RUUVI_ENDPOINT_STANDARD_LOG_CONFIGURATION_READ:

      break;
    */

    default:
      reply->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
      err_code |= RUUVI_DRIVER_ERROR_NOT_SUPPORTED;
    }
  }


  reply->data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX] = incoming->data[RUUVI_ENDPOINT_STANDARD_SOURCE_INDEX];
  reply->data[RUUVI_ENDPOINT_STANDARD_SOURCE_INDEX] = incoming->data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX];
  return err_code;
}

void task_communication_offsets_apply(ruuvi_driver_sensor_data_t* const data, const ruuvi_driver_sensor_data_t* const offsets)
{
  if(isfinite(data->value0) && isfinite(offsets->value0)) { data->value0 += offsets->value0; }
  if(isfinite(data->value1) && isfinite(offsets->value1)) { data->value1 += offsets->value1; }
  if(isfinite(data->value2) && isfinite(offsets->value2)) { data->value2 += offsets->value2; }
}

ruuvi_driver_status_t task_communication_offsets_i32f32_to_float(const uint8_t* const offset, float* const converted)
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
  *converted =  integer + (float)fraction/INT32_MAX;
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_communication_offsets_float_to_i32f32(const float* const offset, uint8_t* const converted)
{
  if(NULL == offset || NULL == converted) { return RUUVI_DRIVER_ERROR_NULL; }
  int32_t integer = (int32_t)  *offset;
  int32_t fraction = (int32_t) ((*offset-floor(*offset)) * INT32_MAX);
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