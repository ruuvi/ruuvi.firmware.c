#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_nfc.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_watchdog.h"
#include "task_nfc.h"
#include <stdio.h>
#include <string.h>

static ruuvi_interface_communication_t channel;

ruuvi_driver_status_t task_nfc_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  int written = 0;
  uint8_t fw_prefix[] = {'F', 'W', ':', ' '};
  uint8_t version_string[APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE] = { 0 };
  memcpy(version_string, fw_prefix, sizeof(fw_prefix));
  written = snprintf((char*)(version_string + sizeof(fw_prefix)),
                     APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE - sizeof(fw_prefix),
                     "%s", APPLICATION_FW_VERSION);
  if(!(written > 0 && APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE > written)) { err_code |= RUUVI_DRIVER_ERROR_DATA_SIZE; }
  err_code |= ruuvi_interface_communication_nfc_fw_version_set(version_string, strlen((char*)version_string));

  uint64_t mac = 0;
  err_code |=  ruuvi_interface_communication_radio_address_get(&mac);
  uint8_t mac_buffer[6] = {0};
  mac_buffer[0] = (mac >> 40) & 0xFF;
  mac_buffer[1] = (mac >> 32) & 0xFF;
  mac_buffer[2] = (mac >> 24) & 0xFF;
  mac_buffer[3] = (mac >> 16) & 0xFF;
  mac_buffer[4] = (mac >> 8) & 0xFF;
  mac_buffer[5] = (mac >> 0) & 0xFF;
  //8 hex bytes
  uint8_t name[APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE] = { 0 };
  written = snprintf((char*)name,
                     APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE,
                     "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
                     mac_buffer[0], mac_buffer[1], mac_buffer[2], mac_buffer[3], mac_buffer[4], mac_buffer[5]);
  if(!(written > 0 && APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE > written)) { err_code |= RUUVI_DRIVER_ERROR_DATA_SIZE; }
  err_code |= ruuvi_interface_communication_nfc_address_set(name, strlen((char*)name));

  uint64_t id = 0;
  err_code |= ruuvi_interface_communication_id_get(&id);
  uint8_t prefix[] = {'I', 'D', ':', ' '};
  uint8_t id_string[APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE] = { 0 };
  memcpy(id_string, prefix, sizeof(prefix));
  uint32_t id0 = (id>>32) & 0xFFFFFFFF;
  uint32_t id1 = id & 0xFFFFFFFF;
  written = snprintf((char*)(id_string + sizeof(prefix)),
                     APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE - sizeof(prefix),
                     "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                     (unsigned int)(id0>>24)&0xFF, (unsigned int)(id0>>16)&0xFF,
                     (unsigned int)(id0>>8)&0xFF, (unsigned int)id0&0xFF,
                     (unsigned int)(id1>>24)&0xFF, (unsigned int)(id1>>16)&0xFF,
                     (unsigned int)(id1>>8)&0xFF, (unsigned int)id1&0xFF);
  if(!(written > 0 && APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE > written)) { err_code |= RUUVI_DRIVER_ERROR_DATA_SIZE; }
  err_code |= ruuvi_interface_communication_nfc_id_set(id_string, strlen((char*)id_string));
  err_code |= ruuvi_interface_communication_nfc_init(&channel);
  err_code |= ruuvi_interface_communication_nfc_data_set(); // Call this to setup data to buffers
  channel.on_evt = task_nfc_on_nfc;
  return err_code;
}

ruuvi_driver_status_t task_nfc_send(ruuvi_interface_communication_message_t* message)
{
  return channel.send(message);
}

void task_acceleration_scheduler_task(void *p_event_data, uint16_t event_size)
{
  // Message + null + <\r>\<n>
  char str[APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE + 3] = { 0 };
  ruuvi_interface_communication_message_t message = {0};
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  do{
     message.data_length = sizeof(message.data);
     memset(&(message.data), 0, sizeof(message.data));
     err_code = channel.read(&message);
     snprintf(str, sizeof(str), "%s\r\n", (char *)message.data);
     ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, str);
  }while(RUUVI_DRIVER_SUCCESS == err_code || RUUVI_DRIVER_STATUS_MORE_AVAILABLE == err_code);
}

ruuvi_driver_status_t task_nfc_on_nfc(ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  switch(evt)
  {
    case RUUVI_INTERFACE_COMMUNICATION_CONNECTED:
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "NFC connected \r\n");
      break;

    case RUUVI_INTERFACE_COMMUNICATION_DISCONNECTED:
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "NFC disconnected \r\n");
      break;

    case RUUVI_INTERFACE_COMMUNICATION_SENT:
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "NFC data sent\r\n");
      ruuvi_interface_watchdog_feed();
      break;

    case RUUVI_INTERFACE_COMMUNICATION_RECEIVED:
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "NFC data received\r\n");
      ruuvi_platform_scheduler_event_put(NULL, 0, task_acceleration_scheduler_task);
      break;

    default:
      break;

  }
  return err_code;
}