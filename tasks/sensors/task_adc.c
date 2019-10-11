#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_adc_mcu.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_timer.h"
#include "task_adc.h"
#include "task_rtc.h"
#include "task_sensor.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

static ruuvi_driver_sensor_t battery_sensor = {0};
static ruuvi_driver_sensor_t ntc_sensor = {0};
static ruuvi_driver_sensor_t photo_sensor = {0};
static volatile uint64_t t_battery_sample = 0;
static volatile float after_tx = 0;

// Callback for radio event. Configured to radio on TASK ADC INIT on droop mode and radio mode,
// see application_config.h
static void task_adc_trigger_on_radio(const
                                      ruuvi_interface_communication_radio_activity_evt_t evt)
{
  // If event is before radio activity and ADC refresh interval has been met.
  if(RUUVI_INTERFACE_COMMUNICATION_RADIO_BEFORE == evt &&
      (APPLICATION_ADC_SAMPLE_INTERVAL_MS < (task_rtc_millis() - t_battery_sample) ||
       0 == t_battery_sample))
  {
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  uint8_t handle = RUUVI_INTERFACE_ADC_AINVDD;
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ADC_SAMPLERATE;
  config.resolution    = APPLICATION_ADC_RESOLUTION;
  config.scale         = APPLICATION_ADC_SCALE;
  config.dsp_function  = APPLICATION_ADC_DSPFUNC;
  config.dsp_parameter = APPLICATION_ADC_DSPPARAM;
  config.mode          = APPLICATION_ADC_MODE;
  err_code |= ruuvi_interface_adc_mcu_init(&battery_sensor, bus, handle);
  err_code |= task_sensor_configure(&battery_sensor, &config, "V");

  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  }

  // If event is after radio activity and ADC is initialized take the sample
  if(RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER == evt &&
     ruuvi_driver_sensor_is_init(&battery_sensor))
  {
      // TEST: LED ON to see the sample time relative to supply voltage, oscilloscope triggered by this signal
      // task_led_write(RUUVI_BOARD_LED_GREEN, RUUVI_BOARD_LEDS_ACTIVE_STATE);
      t_battery_sample = task_rtc_millis();
      uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
      battery_sensor.mode_set(&mode);
      // Read radio-synched ADC sample
      ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
      ruuvi_interface_adc_data_t active;
      status |= battery_sensor.data_get(&active);
      RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
      after_tx = active.adc_v;
      // task_led_write(RUUVI_BOARD_LED_GREEN, !RUUVI_BOARD_LEDS_ACTIVE_STATE);
      battery_sensor.uninit(&battery_sensor, RUUVI_DRIVER_BUS_NONE, RUUVI_INTERFACE_ADC_AINVDD);
      RUUVI_DRIVER_ERROR_CHECK(status, ~RUUVI_DRIVER_ERROR_FATAL);
  }
}

ruuvi_driver_status_t task_adc_init(void)
{
  ruuvi_interface_communication_radio_activity_callback_set(task_adc_trigger_on_radio);
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_adc_battery_get(ruuvi_interface_adc_data_t* const data)
{
  data->adc_v = after_tx;
  data->timestamp_ms = t_battery_sample;
  return RUUVI_DRIVER_SUCCESS;
}


