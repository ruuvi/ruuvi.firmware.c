/**
 * Ruuvi Firmware 3.x GATT tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_atomic.h"
#include "ruuvi_interface_communication_ble4_advertising.h"
#include "ruuvi_interface_communication_ble4_gatt.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_library.h"
#include "ruuvi_library_ringbuffer.h"
#include "task_acceleration.h"
#include "task_advertisement.h"
#include "task_gatt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if APPLICATION_COMMUNICATION_GATT_ENABLED

#ifndef TASK_GATT_LOG_LEVEL
  #define TASK_GATT_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

#define LOG(msg) ruuvi_interface_log(TASK_GATT_LOG_LEVEL, msg)
#define LOGD(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_DEBUG, msg)
#define LOGE(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_ERROR, msg)
#define LOGHEX(msg, len) ruuvi_interface_log_hex(TASK_GATT_LOG_LEVEL, msg, len)
#define LOGDHEX(msg, len) ruuvi_interface_log_hex(RUUVI_INTERFACE_LOG_DEBUG, msg, len)


static uint8_t
buffer[1024];                   //!< Raw buffer for GATT data TX. Must be power of two.
static ruuvi_interface_atomic_t buffer_wlock;
static ruuvi_interface_atomic_t buffer_rlock;
/** @brief Buffer structure for outgoing data */
static ruuvi_library_ringbuffer_t ringbuf = {.head = 0,
                                             .tail = 0,
                                             .block_size = 32,
                                             .storage_size = sizeof(buffer),
                                             .index_mask = (sizeof(buffer) / 32) - 1,
                                             .storage = buffer,
                                             .lock = ruuvi_interface_atomic_flag,
                                             .writelock = &buffer_wlock,
                                             .readlock  = &buffer_rlock
                                            };
static ruuvi_interface_atomic_t
m_tx_scheduler_lock; //!< Flag for scheduled handling of TX done event.
static ruuvi_interface_communication_t channel;   //!< API for sending data.

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
    index += snprintf(dis.deviceid + index, sizeof(dis.deviceid) - index, "%02X",
                      mac_buffer[ii]);

    if(ii < 5) { index += snprintf(dis.deviceid + index, sizeof(dis.deviceid) - index, ":"); }
  }

  memcpy(dis.fw_version, APPLICATION_FW_VERSION, sizeof(APPLICATION_FW_VERSION));
  memcpy(dis.model, RUUVI_BOARD_MODEL_STRING, sizeof(RUUVI_BOARD_MODEL_STRING));
  memcpy(dis.manufacturer, RUUVI_BOARD_MANUFACTURER_STRING,
         sizeof(RUUVI_BOARD_MANUFACTURER_STRING));
  err_code |= ruuvi_interface_communication_ble4_gatt_init();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |= ruuvi_interface_communication_ble4_gatt_nus_init(&channel);
  channel.on_evt = task_gatt_on_nus;
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |= ruuvi_interface_communication_ble4_gatt_dfu_init();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  err_code |= ruuvi_interface_communication_ble4_gatt_dis_init(&dis);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  // Scan response has 31 payload bytes. 18 of those bytes are reserved for 2-byte header + 128-bit UUID.
  // This leaves 13 bytes for name + 2-byte header. Since NULL isn't transmitted we cap string at 12 bytes.
  char name[12];
  snprintf(name, sizeof(name), "%s %02X%02X", RUUVI_BOARD_BLE_NAME_STRING, mac_buffer[4],
           mac_buffer[5]);
  // Send name + NUS UUID. Note that this doesn't update sofdevice buffer, you have to call
  // advertising functions to encode data and start the advertisements.
  err_code |= ruuvi_interface_communication_ble4_advertising_scan_response_setup(name,
              true);
  err_code |= ruuvi_interface_communication_ble4_advertising_type_set(
                CONNECTABLE_SCANNABLE);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  return err_code;
}

/**
 * @brief Process event of data sent inside scheduler.
 *
 * This function handles non-realtime cleanup after data has been sent, such as
 * removing ringbuffer elements which are in softdevice queue.
 *
 * @param[in] p_context Pointer to a pointer to a ringbuffer into which reply data should be sent
 * @param[in] data_len size of the pointer.
 */
static void task_gatt_communication_sent_scheduler(void* p_context, uint16_t data_len)
{
  ruuvi_interface_atomic_flag(&m_tx_scheduler_lock, false);
}

/**
 * @brief Queue new element into softdevice buffer if available.
 *
 * This function handles realtime feeding of softdevice from TX buffer.
 * It is meant to run in high-priority interrupt context. This function should only be called
 * on RUUVI_INTERFACE_COMMUNICATION_SENT event to queue next TX into softdevice.
 */
static void task_gatt_queue_process_interrupt()
{
  ruuvi_library_status_t status = RUUVI_LIBRARY_SUCCESS;
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_communication_message_t* p_msg;
  status = ruuvi_library_ringbuffer_dequeue(&ringbuf, &p_msg);
  LOGD("I");

  if(RUUVI_LIBRARY_SUCCESS == status)
  {
    LOGD("RB>;");
    LOGDHEX(p_msg->data, p_msg->data_length);
    LOGD("\r\n");
    err_code = channel.send(p_msg);
  }
  else
  {
    LOGD("E");
  }

  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
}


static void task_gatt_communication_received_scheduler(void* p_context, uint16_t data_len)
{
  if(data_len > RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH)
  {
    LOG("Received too long message, truncating\r\n");
    data_len = RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH;
  }

  ruuvi_interface_communication_message_t message;
  memcpy(message.data, p_context, data_len);
  message.data_length = data_len;
  LOG("<<<;");
  LOGHEX(message.data, data_len);
  LOG("\r\n");
  task_communication_on_data(&message, task_gatt_send_asynchronous);
}

/**
 * @brief handle NUS connection ended event.
 *
 * This function gets called in scheduler after central has deregistered to Nordic
 * UART Service notifications
 */
static void task_gatt_communication_disconnected_scheduler(void* p_context,
    uint16_t data_len)
{
  LOG("Disconnected\r\n");
  task_advertisement_start();
}

/**
 * @brief handle NUS connection established event.
 *
 * This function gets called in scheduler after central has registered to Nordic
 * UART Service notifications
 */
static void task_gatt_communication_connected_scheduler(void* p_context,
    uint16_t data_len)
{
  LOG("Connected\r\n");
  task_communication_heartbeat_configure(APPLICATION_GATT_HEARTBEAT_INTERVAL_MS,
                                         18,
                                         task_gatt_send_asynchronous);
}

ruuvi_driver_status_t task_gatt_on_nus(ruuvi_interface_communication_evt_t evt,
                                       void* p_data, size_t data_len)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

  switch(evt)
  {
    // Note: This gets called only after the NUS notifications have been registered.
    case RUUVI_INTERFACE_COMMUNICATION_CONNECTED:
      err_code |= ruuvi_interface_scheduler_event_put(NULL, 0,
                  task_gatt_communication_connected_scheduler);
      break;

    case RUUVI_INTERFACE_COMMUNICATION_DISCONNECTED:
      err_code |= ruuvi_interface_scheduler_event_put(NULL, 0,
                  task_gatt_communication_disconnected_scheduler);
      break;

    case RUUVI_INTERFACE_COMMUNICATION_SENT:
      task_gatt_queue_process_interrupt();

      // Schedule only one data sent event to avoid filling scheduler queue.
      if(ruuvi_interface_atomic_flag(&m_tx_scheduler_lock, true))
      {
        err_code |= ruuvi_interface_scheduler_event_put(NULL, 0,
                    task_gatt_communication_sent_scheduler);
      }

      break;

    case RUUVI_INTERFACE_COMMUNICATION_RECEIVED:
      err_code |= ruuvi_interface_scheduler_event_put(p_data, data_len,
                  task_gatt_communication_received_scheduler);

      if(RUUVI_DRIVER_SUCCESS != err_code)
      {
        LOGE("No memory in op queue remaining, incoming data discarded\r\n");
      }

      break;

    default:
      break;
  }

  ruuvi_interface_watchdog_feed();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
  return err_code;
}

/**
 * @brief Function for sending data out via Nordic UART Service
 *
 * This function must not be called from interrupt context, as it may add data  to circular
 * buffer which gets consumed in interrupt context. The function will return immediately,
 * with not guarantee or acknowledgement that the message will be received.
 * In general if this function returns successfully the message will be sent and
 * delivery verified by link layer, but if connection is lost before data is sent
 * the data is lost.
 *
 * @param[in] msg Message to be sent out.
 * @return RUUVI_DRIVER_SUCCESS if message was queued to softdevice or application ringbuffer
 * @return RUUVI_DRIVER_ERROR_NULL if message is NULL
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE of GATT is not initialized
 * @return RUUVI_DRIVER_ERROR_NO_MEM if message cannot be queued due to buffers being full.
 */
ruuvi_driver_status_t task_gatt_send_asynchronous(ruuvi_interface_communication_message_t*
    const msg)
{
  // State, input check
  if(NULL == msg)          { return RUUVI_DRIVER_ERROR_NULL; }

  if(NULL == channel.send) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  // Try to put data to SD
  err_code |= channel.send(msg);

  // If success, return. Else put data to ringbuffer
  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    LOG(">>>;");
    LOGHEX(msg->data, msg->data_length);
    LOG("\r\n");
    return err_code;
  }
  // If the error code is something else than buffer full, return error.
  else if(err_code != RUUVI_DRIVER_ERROR_RESOURCES)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    return err_code;
  }

  // Try to put data to ringbuffer
  err_code = ruuvi_library_ringbuffer_queue(&ringbuf, msg,
             sizeof(ruuvi_interface_communication_message_t));

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    LOGD(">>>;");
    LOGD("\r\n");
  }

  return err_code;
}
#endif