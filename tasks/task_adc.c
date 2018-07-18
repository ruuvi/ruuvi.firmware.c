#include "task_adc.h"
#include "ruuvi_error.h"
#include "ruuvi_sensor.h"
#include "adc.h"
#include "task_bluetooth.h"
#include <stdbool.h>
#include <stdio.h>

#include "application_config.h"
#include "boards.h"
#include <stdbool.h>

#define PLATFORM_LOG_MODULE_NAME application_adc
#if MAIN_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       MAIN_LOG_LEVEL
#define PLATFORM_LOG_INFO_COLOR  MAIN_INFO_COLOR
#else
#define PLATFORM_LOG_LEVEL       0
#endif
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();

static ruuvi_sensor_t m_adc;
static bool m_adc_is_init;

ruuvi_status_t task_adc_init(void)
{
  ruuvi_status_t err_code = adc_init(&m_adc);
  if(RUUVI_SUCCESS == err_code) { m_adc_is_init = true; }
  return err_code;
}


// Start sampling given channel
ruuvi_status_t task_adc_sample_asynchronous(adc_channel_t channel)
{
  if(!m_adc_is_init) { return RUUVI_ERROR_INVALID_STATE; }
  // Note: channel is ignored for now, only battery can be sampled. 
  // TODO: sanity check channel
  return adc_sample_asynchronous(channel);
}

// read latest data from given channel. Return millivolts, or RUUVI_FLOAT_INVALID if ADC cannot be sampled
float task_adc_get_data(adc_channel_t channel)
{
  if(!m_adc_is_init) { return RUUVI_ERROR_INVALID_STATE; }
   // Note: channel is ignored for now, only battery can be sampled. 
   // TODO: sanity check channel
   return adc_get_data(channel);
}

static char data[20];
ruuvi_status_t task_adc_sample_asynchronous_battery(void)
{
  // TODO, channel
  float previous_voltage = task_adc_get_data(0);
  uint16_t mv = (uint16_t) previous_voltage;
  // sprintf(data, "Battery: %d mV", (uint8_t)previous_voltage);
  data[0] = (mv / 1000 << 4) + ((mv%1000) / 100);
  data[1] = ((mv%100) / 10 << 4) + ((mv%10));


  task_bluetooth_advertise((void*)data, strlen(data));
  return task_adc_sample_asynchronous(0);
}
