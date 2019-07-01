/**
 * Ruuvi Firmware 3.x GATT tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication_ble4_gatt.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_watchdog.h"
#include "task_gatt.h"
#include "task_acceleration.h"
#include "ringbuffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Buffer for GATT data
static uint8_t buffer[4096];
static ringbuffer_t tx_buffer;
static size_t buffer_index = 0;

static ruuvi_interface_communication_t channel;


// Push out queued gatt messages
static void task_gatt_queue_process(void* p_event_data, uint16_t event_size)
{
  if(NULL == channel.send) { return; }

  if(ringbuffer_empty(&tx_buffer))
  {
    ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, "Buffer processed\r\n");
    return;
  }

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  static uint64_t last = 0;
  static uint8_t processed = 0;
  char log[128];
  snprintf(log, 128, "Processed %d elements in %lu ms\r\n", processed,
           (uint32_t)(ruuvi_interface_rtc_millis() - last));
  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, log);
  processed = 0;
  ruuvi_interface_communication_message_t* p_msg;
  // Queue as many transmissions as possible
  last = ruuvi_interface_rtc_millis();

  do
  {
    if(buffer_index > ringbuffer_get_count(&tx_buffer)) { break; }

    // Bluetooth driver takes address of data. Therefore data must be stored
    // until it is sent - do not discard here.
    p_msg = ringbuffer_peek_at(&tx_buffer, buffer_index);
    err_code = channel.send(p_msg);

    if(RUUVI_DRIVER_SUCCESS == err_code)
    {
      buffer_index++;
      processed++;
    }
  } while(RUUVI_DRIVER_SUCCESS == err_code);
}

ruuvi_driver_status_t task_gatt_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_communication_ble4_gatt_dis_init_t dis = {0};
  uint64_t mac;
  err_code |= ruuvi_interface_communication_radio_address_get(&mac);
  uint8_t mac_buffer[6] = {0};
  mac_buffer[0] = (mac >> 40) & 0xFF;
  mac_buffer[1] = (mac >> 32) & 0xFF;
  mac_buffer[2] = (mac >> 24) & 0xFF;
  mac_buffer[3] = (mac >> 16) & 0xFF;
  mac_buffer[4] = (mac >> 8) & 0xFF;
  mac_buffer[5] = (mac >> 0) & 0xFF;
  size_t index = 0;

  for(size_t ii = 0; ii < 6; ii ++)
  {
    index += snprintf(dis.deviceid + index, sizeof(dis.deviceid) - index, "%02X", mac_buffer[ii]);

    if(ii < 5) { index += snprintf(dis.deviceid + index, sizeof(dis.deviceid) - index, ":"); }
  }

  memcpy(dis.fw_version, APPLICATION_FW_VERSION, sizeof(APPLICATION_FW_VERSION));
  memcpy(dis.model, RUUVI_BOARD_MODEL_STRING, sizeof(RUUVI_BOARD_MODEL_STRING));
  memcpy(dis.manufacturer, RUUVI_BOARD_MANUFACTURER_STRING,
         sizeof(RUUVI_BOARD_MANUFACTURER_STRING));
  err_code |= ruuvi_interface_communication_ble4_gatt_init();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |= ruuvi_interface_communication_ble4_gatt_nus_init(&channel);
  channel.on_evt = task_gatt_on_gatt;
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |= ruuvi_interface_communication_ble4_gatt_dfu_init();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |= ruuvi_interface_communication_ble4_gatt_dis_init(&dis);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  size_t max_messages = sizeof(buffer) / sizeof(ruuvi_interface_communication_message_t);
  ringbuffer_init(&tx_buffer, max_messages, sizeof(ruuvi_interface_communication_message_t),
                  &buffer);
  return err_code;
}


//ruuvi_driver_status_t task_gatt_on_accelerometer(ruuvi_interface_communication_evt_t evt)
//{
//  /*if(evt == FIFO_FULL)
//  {
//    // FIFO READ
//
//    // IF CONNECTED
//      // FIFO SEND
//  }*/
//  return RUUVI_DRIVER_SUCCESS;
//}
/*
ruuvi_driver_status_t task_gatt_on_advertisement(ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len)
{
  return RUUVI_DRIVER_SUCCESS;
}
*/
//ruuvi_driver_status_t task_gatt_on_button(ruuvi_interface_communication_evt_t evt)
//{
//  // BECOME_CONNECTABLE
//  return RUUVI_DRIVER_SUCCESS;
//}

ruuvi_driver_status_t task_gatt_on_gatt(ruuvi_interface_communication_evt_t evt,
                                        void* p_data, size_t data_len)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_communication_message_t msg = {0};
  ruuvi_interface_communication_message_t reply = {0};
  

  switch(evt)
  {
    case RUUVI_INTERFACE_COMMUNICATION_CONNECTED:
      ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, "Connected \r\n");
      break;

    // TODO: Handle case where connection was made but NUS was not registered
    case RUUVI_INTERFACE_COMMUNICATION_DISCONNECTED:
      ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, "Disconnected \r\n");
      break;

    case RUUVI_INTERFACE_COMMUNICATION_SENT:
      // Message has been sent. Decrease index, process queue.
      buffer_index--;
      ringbuffer_popqueue(&tx_buffer, &msg);

      if(0 == buffer_index)
      {
        ruuvi_interface_scheduler_event_put(NULL, 0,  task_gatt_queue_process);
      }

      break;

    case RUUVI_INTERFACE_COMMUNICATION_RECEIVED:
      if(RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH < data_len)
      {
        ruuvi_interface_log(RUUVI_INTERFACE_LOG_WARNING, "Too long message received, discarding\r\n");
        break;
      }
      msg.data_length = data_len;
      reply.data_length = RUUVI_ENDPOINT_STANDARD_MESSAGE_LENGTH;
      memcpy(msg.data, p_data, msg.data_length);
      task_communication_on_data(&msg, &reply);
      task_gatt_send(&reply);
      break;

    default:
      break;
  }

  return err_code;
}

/*
ruuvi_driver_status_t task_gatt_on_nfc(ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len)
{
  // BECOME_CONNECTABLE
  return RUUVI_DRIVER_SUCCESS;
}
*/

ruuvi_driver_status_t task_gatt_send(ruuvi_interface_communication_message_t* const msg)
{
  if(NULL == msg)          { return RUUVI_DRIVER_ERROR_NULL; }

  if(NULL == channel.send) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  // Add to queue if there's room
  if(ringbuffer_full(&tx_buffer))
  {
    ruuvi_interface_log(RUUVI_INTERFACE_LOG_WARNING, "FIFO out of space\r\n");
    return RUUVI_DRIVER_ERROR_NO_MEM;
  }

  ringbuffer_push(&tx_buffer, msg);
  // schedule queue to be transmitted if tx is not already ongoing
  ruuvi_interface_scheduler_event_put(NULL, 0,  task_gatt_queue_process);
  return RUUVI_DRIVER_SUCCESS;
}