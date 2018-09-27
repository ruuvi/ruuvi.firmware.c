#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_nfc.h"
#include "ruuvi_interface_communication_radio.h"
#include "task_nfc.h"
#include <stdio.h>
#include <string.h>

static ruuvi_interface_communication_t channel;

ruuvi_driver_status_t task_nfc_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_communication_nfc_fw_version_set(APPLICATION_FW_VERSION, sizeof(APPLICATION_FW_VERSION));

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
  static char name[30] = { 0 };
  snprintf(name, 20, "MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac_buffer[0], mac_buffer[1], mac_buffer[2], mac_buffer[3], mac_buffer[4], mac_buffer[5]);
  err_code |= ruuvi_interface_communication_nfc_address_set(name, strlen(name));

  uint64_t id = 0;
  err_code |= ruuvi_interface_communication_id_get(&id);
  uint8_t prefix[] = {'I', 'D', ':', ' '};
  static char id_string[30] = { 0 };
  memcpy(id_string, prefix, sizeof(prefix));
  uint32_t id0 = (id>>32) & 0xFFFFFFFF;
  uint32_t id1 = id & 0xFFFFFFFF;
  snprintf(id_string + sizeof(prefix), 29, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                                          (id0>>24)&0xFF, (id0>>16)&0xFF, (id0>>8)&0xFF, id0&0xFF,
                                          (id1>>24)&0xFF, (id1>>16)&0xFF, (id1>>8)&0xFF, id1&0xFF);
  err_code |= ruuvi_interface_communication_nfc_id_set(id_string, strlen(id_string));
  err_code |= ruuvi_interface_communication_nfc_init(&channel);
  ruuvi_interface_communication_nfc_data_set(); // Call this to setup data to buffers
  return err_code;
}

/**
 * Sets given message to NFC RAM buffer. Clears previous message
 *
 * return RUUVI_DRIVER_SUCCESS on success
 * return error code from stack on error
 */
ruuvi_driver_status_t task_nfc_send(ruuvi_interface_communication_message_t* message)
{
  return channel.send(message);
}
