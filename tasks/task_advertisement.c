#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_endpoint_3.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_communication_ble4_advertising.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_environmental.h"
#include "task_adc.h"
#include "task_acceleration.h"
#include "task_environmental.h"

static ruuvi_interface_communication_t channel;

ruuvi_driver_status_t task_advertisement_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_communication_ble4_advertising_init(&channel);
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_interval_set(APPLICATION_ADVERTISING_INTERVAL);
  int8_t target_power = APPLICATION_ADVERTISING_POWER;
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_power_set(&target_power);
  err_code |= ruuvi_interface_communication_ble4_advertising_manufacturer_id_set(RUUVI_BOARD_BLE_MANUFACTURER_ID);
  return err_code;
}

ruuvi_driver_status_t task_advertisement_send_3(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_acceleration_data_t acclereration;
  ruuvi_interface_adc_data_t battery;
  ruuvi_interface_environmental_data_t environmental;

  // TODO: support accelerometer single sampling
  task_adc_sample();
  task_environmental_sample();
  err_code |= task_acceleration_data_get(&acclereration);
  err_code |= task_environmental_data_get(&environmental);
  err_code |= task_adc_data_get(&battery);

  ruuvi_endpoint_3_data_t data;
  data.accelerationx_g = acclereration.x_g;
  data.accelerationy_g = acclereration.y_g;
  data.accelerationz_g = acclereration.z_g;
  data.humidity_rh = environmental.humidity_rh;
  data.temperature_c = environmental.temperature_c;
  data.pressure_pa = environmental.pressure_pa;
  data.battery_v = battery.adc_v;
  ruuvi_interface_communication_message_t message;
  message.data_length = RUUVI_ENDPOINT_3_DATA_LENGTH;
  ruuvi_endpoint_3_encode(message.data, &data, RUUVI_DRIVER_FLOAT_INVALID);
  err_code |= channel.send(&message);

  return err_code;
}