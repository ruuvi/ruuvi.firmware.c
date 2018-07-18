#include "application_config.h"
#include "application_bluetooth_config.h"
#include "boards.h"
#include "ble4_stack.h"
#include "ble4_gatt.h"
#include "ble4_advertisement.h"
#include "ruuvi_error.h"
#include "ruuvi_endpoints.h"
#include "communication.h"
#include <stdbool.h>
#include <string.h>

#include "ble_nus.h"

// Radio callback
#include "task_adc.h"

#define PLATFORM_LOG_MODULE_NAME application_bluetooth
#if MAIN_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       MAIN_LOG_LEVEL
#define PLATFORM_LOG_INFO_COLOR  MAIN_INFO_COLOR
#else // ANT_BPWR_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       0
#endif // ANT_BPWR_LOG_ENABLED
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();

static ruuvi_communication_channel_t m_adv_channel;

//TODO: implement GATT as communication channel

ruuvi_status_t task_bluetooth_init(void)
{
  ruuvi_status_t err_code = RUUVI_SUCCESS;
  err_code |= ble4_stack_init();
  // Do not include trailing NULL
  err_code |= ble4_set_name((uint8_t*)DEVICE_NAME, sizeof(DEVICE_NAME) - 1, true);

  err_code |= ble4_advertisement_init(&m_adv_channel);
  err_code |= ble4_advertisement_set_interval(100);
  err_code |= ble4_advertisement_set_power(4);
  err_code |= ble4_advertisement_set_type(CONNECTABLE_SCANNABLE);
  err_code |= ble4_advertisement_set_manufacturer_id(0x0499);

    // Called after message has been sent
  m_adv_channel.set_on_tx(task_adc_sample_asynchronous_battery);

  // Order of init seems to be important for the UUID advertised in scan response.
  err_code |= ble4_gatt_init();

  err_code |= ble4_nus_init();
  err_code |= ble4_dfu_init();
  err_code |= ble4_dis_init();
  return err_code;
}

ruuvi_status_t task_bluetooth_advertise(uint8_t* data, size_t data_length)
{
  if (NULL == data) { return RUUVI_ERROR_NULL; }
  if (RUUVI_COMMUNICATION_MESSAGE_MAX_PAYLOAD_LENGTH < data_length) { return RUUVI_ERROR_INVALID_LENGTH; }

  ruuvi_status_t err_code = RUUVI_SUCCESS;
  ruuvi_communication_message_t msg;
  uint8_t payload_buf[RUUVI_COMMUNICATION_MESSAGE_MAX_PAYLOAD_LENGTH];
  msg.payload = payload_buf;
  msg.repeat = false;
  memcpy(msg.payload, data, data_length);
  msg.payload_length = data_length;
  err_code |= m_adv_channel.message_put(&msg);
  err_code |= m_adv_channel.process_asynchronous();
  return err_code;
}

ruuvi_status_t task_bluetooth_send_asynchronous(uint8_t* data, size_t data_length)
{
  if (NULL == data) { return RUUVI_ERROR_NULL; }
  if (BLE_NUS_MAX_DATA_LEN < data_length) { return RUUVI_ERROR_INVALID_LENGTH; }

  ruuvi_communication_message_t msg;
  msg.payload = data;
  msg.payload_length = data_length;
  msg.repeat = false;

  PLATFORM_LOG_DEBUG("Send message");

  return ble4_nus_message_put(&msg);
}

ruuvi_status_t task_bluetooth_process(void)
{
  ruuvi_communication_message_t ble_in_msg;
  uint8_t ble_in_data[24];
  ble_in_msg.payload = ble_in_data;
  ble_in_msg.payload_length = sizeof(ble_in_data);
  ruuvi_standard_message_t message;

    // While there is incoming data
    while (RUUVI_SUCCESS == ble4_nus_message_get(&ble_in_msg))
    {
      PLATFORM_LOG_INFO("Got data");
      PLATFORM_LOG_HEXDUMP_INFO(ble_in_msg.payload, ble_in_msg.payload_length);
      // Ensure we got valid length message
      if (sizeof(message) != ble_in_msg.payload_length)
      {
        PLATFORM_LOG_ERROR("Invalid message");
      }
      else
      {
        memcpy(&message, ble_in_msg.payload, sizeof(message));

        //Route incoming data to program
        route_message(&message);
        PLATFORM_LOG_DEBUG("Processed data, replying");

        // Queue reply back to central
        task_bluetooth_send_asynchronous((void*)&message, sizeof(message));
        PLATFORM_LOG_DEBUG("Reply sent");
      }

      //reset message length
      ble_in_msg.payload_length = sizeof(ble_in_data);
    }
  PLATFORM_LOG_DEBUG("Process queue message");
  return ble4_nus_process_asynchronous();
}