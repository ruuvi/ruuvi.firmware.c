#include "application_config.h"
#include "application_bluetooth_config.h"
#include "boards.h"
#include "ble4_stack.h"
#include "ble4_gatt.h"
#include "ble4_advertisement.h"
#include "tasks.h"
#include "ruuvi_error.h"
#include "communication.h"
#include <stdbool.h>
#include <string.h>

#include "ble_nus.h"

#define PLATFORM_LOG_MODULE_NAME application_bluetooth
#if MAIN_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       MAIN_LOG_LEVEL
#define PLATFORM_LOG_INFO_COLOR  MAIN_INFO_COLOR
#else // ANT_BPWR_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       0
#endif // ANT_BPWR_LOG_ENABLED
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();

ruuvi_status_t task_bluetooth_init(void)
{
  ruuvi_status_t err_code = RUUVI_SUCCESS;
  err_code |= ble4_stack_init();
  // Do not include trailing NULL
  err_code |= ble4_set_name((uint8_t*)DEVICE_NAME, sizeof(DEVICE_NAME)-1, true);

  err_code |= ble4_advertisement_init();
  err_code |= ble4_advertisement_set_interval(100);
  err_code |= ble4_advertisement_set_power(4);
  err_code |= ble4_advertisement_set_type(CONNECTABLE_SCANNABLE);
  err_code |= ble4_advertisement_set_manufacturer_id(0x0499);

  // Order of init seems to be important for the UUID advertised in scan response.
  err_code |= ble4_gatt_init();
  err_code |= ble4_nus_init();
  err_code |= ble4_dfu_init();
  err_code |= ble4_dis_init();
  return err_code;
}

ruuvi_status_t task_bluetooth_advertise(uint8_t* data, size_t data_length)
{
  if(NULL == data) { return RUUVI_ERROR_NULL; }
  if(RUUVI_COMMUNICATION_MESSAGE_MAX_PALYLOAD_LENGTH < data_length) { return RUUVI_ERROR_INVALID_LENGTH; }

  ruuvi_status_t err_code = RUUVI_SUCCESS;
  ruuvi_communication_message_t msg;
  uint8_t payload_buf[RUUVI_COMMUNICATION_MESSAGE_MAX_PALYLOAD_LENGTH];
  msg.payload = payload_buf;
  msg.repeat = false;
  memcpy(msg.payload, data, data_length);
  msg.payload_length = data_length;
  err_code |= ble4_advertisement_message_put(&msg);
  err_code |= ble4_advertisement_process_asynchronous();
  return err_code;
}

ruuvi_status_t task_bluetooth_send_asynchronous(uint8_t* data, size_t data_length)
{
  if(NULL == data) { return RUUVI_ERROR_NULL; }
  if(BLE_NUS_MAX_DATA_LEN < data_length) { return RUUVI_ERROR_INVALID_LENGTH; }

  ruuvi_communication_message_t msg;
  msg.payload = data;
  msg.payload_length = data_length;
  msg.repeat = false;

  PLATFORM_LOG_INFO("Send message");

  return ble4_nus_message_put(&msg);
}

ruuvi_status_t task_bluetooth_process(void)
{
  PLATFORM_LOG_INFO("Process queue message");
  return ble4_nus_process_asynchronous();
}