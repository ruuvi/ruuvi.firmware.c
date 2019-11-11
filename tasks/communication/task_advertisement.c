/**
 * Ruuvi Firmware 3.x advertisement tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_endpoint_3.h"
#include "ruuvi_endpoint_5.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_communication_ble4_advertising.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_environmental.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_watchdog.h"
#include "task_adc.h"
#include "task_advertisement.h"
#include "task_acceleration.h"
#include "task_communication.h"
#include "task_environmental.h"
#include "task_rtc.h"

static ruuvi_interface_timer_id_t advertisement_timer;
static ruuvi_interface_communication_t channel;

// Populate advertisement buffer with initial data
static ruuvi_driver_status_t advertisement_data_init(void)
{

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_endpoint_5_data_t data = { 0 };
  ruuvi_driver_sensor_data_t sensor = {0};
  float sensor_values[7];
  sensor.data = sensor_values;
  sensor.fields.datas.acceleration_x_g = 1;
  sensor.fields.datas.acceleration_y_g = 1;
  sensor.fields.datas.acceleration_z_g = 1;
  sensor.fields.datas.temperature_c = 1;
  sensor.fields.datas.humidity_rh = 1;
  sensor.fields.datas.pressure_pa = 1;
  sensor.fields.datas.voltage_v = 1;
  // Get data from sensors
  err_code |= task_acceleration_data_get(&sensor);
  err_code |= task_environmental_data_get(&sensor);
  err_code |= task_adc_battery_get(&sensor);
  data.accelerationx_g = ruuvi_driver_sensor_data_parse(&sensor, (ruuvi_driver_sensor_data_fields_t){.datas.acceleration_x_g=1});
  data.accelerationy_g = ruuvi_driver_sensor_data_parse(&sensor, (ruuvi_driver_sensor_data_fields_t){.datas.acceleration_y_g=1});
  data.accelerationz_g = ruuvi_driver_sensor_data_parse(&sensor, (ruuvi_driver_sensor_data_fields_t){.datas.acceleration_z_g=1});
  data.humidity_rh = ruuvi_driver_sensor_data_parse(&sensor, (ruuvi_driver_sensor_data_fields_t){.datas.humidity_rh=1});
  data.temperature_c = ruuvi_driver_sensor_data_parse(&sensor, (ruuvi_driver_sensor_data_fields_t){.datas.temperature_c=1});
  data.pressure_pa = ruuvi_driver_sensor_data_parse(&sensor, (ruuvi_driver_sensor_data_fields_t){.datas.pressure_pa=1});
  data.battery_v = ruuvi_driver_sensor_data_parse(&sensor, (ruuvi_driver_sensor_data_fields_t){.datas.voltage_v=1});
  data.tx_power = APPLICATION_ADVERTISING_POWER_DBM;
  data.tx_power          = APPLICATION_ADVERTISING_POWER_DBM;
  data.measurement_count = 0;
  data.movement_count    = 0;;
  ruuvi_interface_communication_radio_address_get(&(data.address));
  ruuvi_interface_communication_message_t message;
  message.data_length = RUUVI_ENDPOINT_5_DATA_LENGTH;
  ruuvi_endpoint_5_encode(message.data, &data);
  return   ruuvi_interface_communication_ble4_advertising_data_set(message.data, message.data_length);
}

//handler for scheduled advertisement event
void task_advertisement_scheduler_task(void* p_event_data, uint16_t event_size)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  // Slow down advertisement after startup
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_interval_set(
              APPLICATION_ADVERTISING_INTERVAL_MS);
  if(ruuvi_interface_communication_ble4_advertising_ongoing())
  {
    err_code |= ruuvi_interface_communication_ble4_advertising_stop();
    err_code |= ruuvi_interface_communication_ble4_advertising_start();
  }

  if(RUUVI_DRIVER_SUCCESS != err_code)
  { 
    err_code |=  ruuvi_interface_timer_start(advertisement_timer, APPLICATION_ADVERTISING_STARTUP_PERIOD_MS);
  }

  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
}

// Timer callback, schedule advertisement event here.
static void task_advertisement_timer_cb(void* p_context)
{
  ruuvi_interface_scheduler_event_put(NULL, 0, task_advertisement_scheduler_task);
}

ruuvi_driver_status_t task_advertisement_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_communication_ble4_advertising_init(&channel);
  int8_t target_power = APPLICATION_ADVERTISING_POWER_DBM;
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_power_set(&target_power);
  err_code |= ruuvi_interface_communication_ble4_advertising_manufacturer_id_set(
                RUUVI_BOARD_BLE_MANUFACTURER_ID);
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_interval_set(
                APPLICATION_ADVERTISING_STARTUP_INTERVAL_MS);
  err_code |= ruuvi_interface_timer_create(&advertisement_timer,
              RUUVI_INTERFACE_TIMER_MODE_SINGLE_SHOT, task_advertisement_timer_cb);
  err_code |= advertisement_data_init();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  return err_code;
}

ruuvi_driver_status_t task_advertisement_uninit(void)
{
  return ruuvi_interface_communication_ble4_advertising_uninit(&channel);
}

ruuvi_driver_status_t task_advertisement_start(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_communication_ble4_advertising_start();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |=  ruuvi_interface_timer_start(advertisement_timer, APPLICATION_ADVERTISING_STARTUP_PERIOD_MS);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |=  task_communication_heartbeat_configure(APPLICATION_ADVERTISEMENT_UPDATE_INTERVAL_MS, 
                                                      RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH, 
                                                      channel.send);
  return err_code;
}

ruuvi_driver_status_t task_advertisement_stop(void)
{
  // Ignore error code from stopping, as the advertisement will
  // always be stopped but INVALID_STATE is returned if
  // advertisement wasn't ongoing.
  ruuvi_interface_communication_ble4_advertising_stop();
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_advertisement_send_data(ruuvi_interface_communication_message_t* const msg)
{
  if(NULL == msg) { return RUUVI_DRIVER_ERROR_NULL; }
  if(NULL == channel.send) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return channel.send(msg);
}