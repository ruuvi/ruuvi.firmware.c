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
#include "ruuvi_interface_watchdog.h"
#include "task_gatt.h"
#include "task_acceleration.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static ruuvi_interface_communication_t channel;

ruuvi_driver_status_t task_gatt_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_communication_ble4_gatt_dis_init_t dis = {0};
  uint64_t id;
  err_code |= ruuvi_interface_communication_id_get(&id);
  size_t index = 0;
  for (size_t ii = 0; ii < 8; ii ++)
  {
    index += snprintf(dis.deviceid + index, sizeof(dis.deviceid)-index, "%02X", (uint8_t)(id >> ((7 - ii) * 8)) & 0xFF);
    if(ii < 7) { index += snprintf(dis.deviceid + index, sizeof(dis.deviceid)-index, ":"); }
  }
  memcpy(dis.fw_version, APPLICATION_FW_VERSION, sizeof(APPLICATION_FW_VERSION));
  memcpy(dis.model, RUUVI_BOARD_MODEL_STRING, sizeof(RUUVI_BOARD_MODEL_STRING));
  memcpy(dis.manufacturer, RUUVI_BOARD_MANUFACTURER_STRING, sizeof(RUUVI_BOARD_MANUFACTURER_STRING));

  err_code |= ruuvi_interface_communication_radio_init(RUUVI_INTERFACE_COMMUNICATION_RADIO_GATT);
  // Init fails if something else - such as advertising - as reserved radio.
  if(RUUVI_DRIVER_SUCCESS != err_code) { return err_code; }
  err_code |= ruuvi_interface_communication_ble4_gatt_init();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

  err_code |= ruuvi_interface_communication_ble4_gatt_nus_init(&channel);
  channel.on_evt = task_gatt_on_gatt;
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

  err_code |= ruuvi_interface_communication_ble4_gatt_dfu_init();
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

  err_code |= ruuvi_interface_communication_ble4_gatt_dis_init(&dis);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

  err_code |= ruuvi_interface_communication_ble4_gatt_advertise_connectablity(true, "Ruuvi", true, true);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

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

ruuvi_driver_status_t task_gatt_on_gatt(ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  // 20 bytes GATT message + NULL + <\r><\n>
  char str[23] = { 0 };
  switch(evt)
  {
    case RUUVI_INTERFACE_COMMUNICATION_CONNECTED:
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Connected \r\n");
      err_code |= task_acceleration_fifo_use(true);
      ruuvi_interface_communication_message_t msg = {0};
      memcpy(msg.data, "Hello! Here's data!", 20);
      msg.data_length = 20;
      channel.send(&msg);
      break;

    // TODO: Handle case where connection was made but NUS was not registered
    case RUUVI_INTERFACE_COMMUNICATION_DISCONNECTED:
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Disconnected \r\n");
      err_code |= task_acceleration_fifo_use(false);
      ruuvi_interface_communication_ble4_gatt_advertise_connectablity(true, "Ruuvi", true, true);
      break;

    case RUUVI_INTERFACE_COMMUNICATION_SENT:
      ruuvi_interface_watchdog_feed();
      break;

    case RUUVI_INTERFACE_COMMUNICATION_RECEIVED:
      // Space for trailing <\r><\n><NULL>
      snprintf(str, sizeof(str), "%s\r\n", (char *)p_data);
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, str);
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

  return channel.send(msg);
}