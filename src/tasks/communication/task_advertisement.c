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
#include "task_sensor.h"

static ruuvi_interface_timer_id_t advertisement_timer;
static ruuvi_interface_communication_t channel;
static uint64_t m_advertisement_startup;

// Populate advertisement buffer with initial data
ruuvi_driver_status_t advertisement_data_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_communication_message_t msg = {0};
  err_code |= task_sensor_encode_to_5((uint8_t*)&msg.data);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  msg.data_length = sizeof(msg.data);
  err_code |= ruuvi_interface_communication_ble4_advertising_data_set(msg.data,
              msg.data_length);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  return err_code;
}

//handler for scheduled advertisement event
void task_advertisement_scheduler_task(void* p_event_data, uint16_t event_size)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

  // Slow down advertisement after startup
  if(task_rtc_millis() > (APPLICATION_ADVERTISING_STARTUP_INTERVAL_MS +
                          m_advertisement_startup))
  {
    err_code |= ruuvi_interface_communication_ble4_advertising_tx_interval_set(
                  APPLICATION_ADVERTISING_INTERVAL_MS);
  }

  if(ruuvi_interface_communication_ble4_advertising_ongoing())
  {
    err_code |= ruuvi_interface_communication_ble4_advertising_stop();
    err_code |= ruuvi_interface_communication_ble4_advertising_start();
  }

  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    err_code |=  ruuvi_interface_timer_start(advertisement_timer,
                 APPLICATION_ADVERTISING_STARTUP_PERIOD_MS);
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
  ruuvi_driver_status_t data_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_communication_ble4_advertising_init(&channel);
  int8_t target_power = APPLICATION_ADVERTISING_POWER_DBM;
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_power_set(&target_power);
  err_code |= ruuvi_interface_communication_ble4_advertising_manufacturer_id_set(
                RUUVI_BOARD_BLE_MANUFACTURER_ID);
  err_code |= ruuvi_interface_timer_create(&advertisement_timer,
              RUUVI_INTERFACE_TIMER_MODE_SINGLE_SHOT, task_advertisement_timer_cb);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  // Allow errors on sensors not foud in case we're running on a basic model
  data_code = advertisement_data_init();
  RUUVI_DRIVER_ERROR_CHECK(data_code, ~RUUVI_DRIVER_ERROR_FATAL);
  return err_code;
}

ruuvi_driver_status_t task_advertisement_uninit(void)
{
  return ruuvi_interface_communication_ble4_advertising_uninit(&channel);
}

ruuvi_driver_status_t task_advertisement_start(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  m_advertisement_startup = task_rtc_millis();
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_interval_set(
                APPLICATION_ADVERTISING_STARTUP_INTERVAL_MS);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |= ruuvi_interface_communication_ble4_advertising_start();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |=  ruuvi_interface_timer_start(advertisement_timer,
               APPLICATION_ADVERTISING_STARTUP_PERIOD_MS);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |=  task_communication_heartbeat_configure(
                 APPLICATION_ADVERTISEMENT_UPDATE_INTERVAL_MS,
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

ruuvi_driver_status_t task_advertisement_send_data(
  ruuvi_interface_communication_message_t* const msg)
{
  if(NULL == msg) { return RUUVI_DRIVER_ERROR_NULL; }

  if(NULL == channel.send) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  return channel.send(msg);
}