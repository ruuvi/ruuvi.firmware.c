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
#include "task_led.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

RUUVI_PLATFORM_TIMER_ID_DEF(adc_timer);
static ruuvi_driver_sensor_t adc_sensor = {0};
static volatile uint64_t t_sample = 0;
static volatile float droop = 0;
static volatile float after_tx = 0;

/* Use these functions for using ADC at regular, timed intervals
 * Remember to start the timer at init
 */
//handler for scheduled accelerometer event
static void task_adc_scheduler_task(void *p_event_data, uint16_t event_size)
{
  ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;

  // Take new sample
  status |= task_adc_sample();
  // Log warning if adc sampling failed.
  RUUVI_DRIVER_ERROR_CHECK(status, ~RUUVI_DRIVER_ERROR_FATAL);
}

// Timer callback, schedule event here or execute it right away if it's timing-critical
static void task_adc_timer_cb(void* p_context)
{
  ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_adc_data_t rest;

  // Simple mode
  if(APPLICATION_BATTERY_VOLTAGE_SIMPLE)
  {
    ruuvi_platform_scheduler_event_put(NULL, 0, task_adc_scheduler_task);
  }

  // Droop mode
  if(APPLICATION_BATTERY_VOLTAGE_DROOP)
  {
  // Take new ADC sample
  status |= task_adc_sample();

  // Read new sample
  status |= adc_sensor.data_get(&rest);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);

  // Subtract active sample from rest sample
  droop = rest.adc_v - after_tx;
  if(0.0 > droop) { droop = 0.0; }
  }
}


// Callback for radio event. Configured to radio on TASK ADC INIT on droop mode and radio mode,
// see application_config.h
static void task_adc_trigger_on_radio(const ruuvi_interface_communication_radio_activity_evt_t evt)
{

  // If event is after radio activity
  if(RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER == evt)
  {
    // Sample
    if((APPLICATION_ADC_SAMPLE_INTERVAL_MS < ruuvi_platform_rtc_millis() - t_sample || 0 == t_sample))
    {
      // TEST: LED ON to see the sample time relative to supply voltage, oscilloscope triggered by this signal
      // task_led_write(RUUVI_BOARD_LED_GREEN, RUUVI_BOARD_LEDS_ACTIVE_STATE);

      t_sample = ruuvi_platform_rtc_millis();
      task_adc_sample();

      // Read radio-synched ADC sample
      ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
      ruuvi_interface_adc_data_t active;
      status |= adc_sensor.data_get(&active);
      RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
      after_tx = active.adc_v;

      // task_led_write(RUUVI_BOARD_LED_GREEN, !RUUVI_BOARD_LEDS_ACTIVE_STATE);

      // Sample again for droop mode
      if(APPLICATION_BATTERY_VOLTAGE_DROOP)
      {
        ruuvi_platform_timer_start(adc_timer, APPLICATION_BATTERY_DROOP_DELAY_MS);
      }
    }
  }
}


static ruuvi_driver_status_t task_adc_configure(void)
{
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ADC_SAMPLERATE;
  config.resolution    = APPLICATION_ADC_RESOLUTION;
  config.scale         = APPLICATION_ADC_SCALE;
  config.dsp_function  = APPLICATION_ADC_DSPFUNC;
  config.dsp_parameter = APPLICATION_ADC_DSPPARAM;
  config.mode          = APPLICATION_ADC_MODE;
  if(NULL == adc_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return adc_sensor.configuration_set(&adc_sensor, &config);
}

ruuvi_driver_status_t task_adc_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  uint8_t handle = RUUVI_INTERFACE_ADC_AINVDD;

  // Initialize timer for adc task.
  // Note: the timer is not started.
  // Note: use REPEATED mode for free-running sampling.
  ruuvi_interface_timer_mode_t mode = APPLICATION_BATTERY_VOLTAGE_SIMPLE ? RUUVI_INTERFACE_TIMER_MODE_REPEATED : RUUVI_INTERFACE_TIMER_MODE_SINGLE_SHOT;
  err_code |= ruuvi_platform_timer_create(&adc_timer, mode, task_adc_timer_cb);

  err_code |= ruuvi_interface_adc_mcu_init(&adc_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |= task_adc_configure();

    // Start the ADC timer here if you use timer-based battery measurement
    if(true == APPLICATION_BATTERY_VOLTAGE_SIMPLE)
    {
      err_code |= ruuvi_platform_timer_start(adc_timer, APPLICATION_ADC_SAMPLE_INTERVAL_MS);
    }

    // Configure the radio callback here to synchronize radio to ADC, unless we're in simple mode
    if(false == APPLICATION_BATTERY_VOLTAGE_SIMPLE)
    {
      ruuvi_interface_communication_radio_activity_callback_set(task_adc_trigger_on_radio);
    }
    return err_code;
  }

  // Return error if ADC could not be configured
  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_adc_data_log(const ruuvi_interface_log_severity_t level)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_adc_data_t data;
  if(NULL == adc_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  // If the mode is single, take a new sample.
  if(APPLICATION_ADC_MODE == RUUVI_DRIVER_SENSOR_CFG_SINGLE)
  {
    err_code |= task_adc_sample();
  }

  err_code |= adc_sensor.data_get(&data);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Time: %lu\r\n", (uint32_t)(data.timestamp_ms&0xFFFFFFFF));
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Battery: %.3f\r\n", data.adc_v);
  ruuvi_platform_log(level, message);
  return err_code;
}

ruuvi_driver_status_t task_adc_sample(void)
{
  if(NULL == adc_sensor.mode_set) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
  return adc_sensor.mode_set(&mode);
}

// Implement your button action here
ruuvi_driver_status_t task_adc_on_button(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  return err_code;
}

ruuvi_driver_status_t task_adc_data_get(ruuvi_interface_adc_data_t* const data)
{
  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }
  if(NULL == adc_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return adc_sensor.data_get(data);
}

ruuvi_driver_status_t task_adc_battery_get(ruuvi_interface_adc_data_t* const data)
{
  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }
  if(NULL == adc_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  data->timestamp_ms = t_sample;
  if(APPLICATION_BATTERY_VOLTAGE_SIMPLE) { err_code |= task_adc_data_get(data); }
  if(APPLICATION_BATTERY_VOLTAGE_RADIO)  { data->adc_v = after_tx; }
  if(APPLICATION_BATTERY_VOLTAGE_DROOP)  { data->adc_v = droop; }
  return err_code;
}

