#include "boards.h"
#include "app_error.h"
#include <string.h>

#include "application_config.h"

#include "ruuvi_sensor.h"
#include "ruuvi_error.h"
#include "communication.h"
#include "gpio.h"
#include "tasks.h"
#include "yield.h"
#include "ble4_stack.h"
#include "ble4_gatt.h"
// #include "ruuvi_endpoints.h"
#include "bme280.h"

#define PLATFORM_LOG_MODULE_NAME main
#if MAIN_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       MAIN_LOG_LEVEL
#define PLATFORM_LOG_INFO_COLOR  MAIN_INFO_COLOR
#else // ANT_BPWR_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       0
#endif // ANT_BPWR_LOG_ENABLED
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();

static uint8_t name[] = {'R', 'u', 'u', 'v', 'i'};

int main(void)
{
  //Init LOG
  APP_ERROR_CHECK(PLATFORM_LOG_INIT(NULL));
  PLATFORM_LOG_DEFAULT_BACKENDS_INIT();
  PLATFORM_LOG_INFO("Logging started");
  platform_yield_init();

  uint8_t errors = 0;
  uint8_t error_code[20] = {'E', 'r', 'r', ':', ' '};
  uint8_t err_index = 5;


  //Init peripherals
  ruuvi_status_t err_code = task_init_peripherals();
  task_blink_leds(1000);
  if (RUUVI_SUCCESS != err_code)
  {
    error_code[err_index++] = 'P'; // Peripheral
    errors++;
  }
  PLATFORM_LOG_INFO("Peripheral init status: 0x%X", err_code);

  // err_code = task_init_accelerometer();
  // err_code |= task_setup_accelerometer();
  // if (RUUVI_SUCCESS != err_code)
  // {
  //   error_code[err_index++] = 'A'; // Accelerometer
  //   errors++;
  // }
  // PLATFORM_LOG_INFO("Accelerometer init status: 0x%X", err_code);

  err_code = task_init_environmental();
  err_code |= task_setup_environmental();
  if (RUUVI_SUCCESS != err_code)
  {
    error_code[err_index++] = 'E'; // Environmental
    errors++;
  }
  PLATFORM_LOG_INFO("Environmental init status: 0x%X", err_code);

  err_code  = task_bluetooth_init();
  err_code |= task_bluetooth_advertise(name, sizeof(name) - 1);
  if (RUUVI_SUCCESS != err_code)
  {
    error_code[err_index++] = 'B'; // Bluetooth
    errors++;
  }
  PLATFORM_LOG_INFO("Bluetooth init status: 0x%X", err_code);

  // ruuvi_acceleration_data_t  acceleration  = { 0 };
  ruuvi_environmental_data_t environmental = { 0 };

  uint8_t status_led = LED_GREEN;
  if (errors)
  {
    PLATFORM_LOG_ERROR("Errors detected: %s", error_code);
    err_code  = ble4_set_name(error_code, err_index, false);
    err_code |= task_bluetooth_advertise(name, sizeof(name) - 1);
    PLATFORM_LOG_INFO("Name update status: 0x%X", err_code);
    status_led = LED_RED;
  }

  platform_gpio_toggle(status_led);
  platform_delay_ms(3000);
  platform_gpio_toggle(status_led);
  PLATFORM_LOG_INFO("Entering main loop");

  // err_code = task_get_acceleration(&acceleration);
  // PLATFORM_LOG_INFO("Acceleration data status %x", err_code);
  // PLATFORM_LOG_INFO("X:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(acceleration.x_mg));
  // PLATFORM_LOG_INFO("Y:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(acceleration.y_mg));
  // PLATFORM_LOG_INFO("Z:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(acceleration.z_mg));

  err_code = task_get_environmental(&environmental);
  PLATFORM_LOG_INFO("Environmental data status: %X", err_code);
  PLATFORM_LOG_INFO("T:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.temperature));
  PLATFORM_LOG_INFO("P:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.pressure));
  PLATFORM_LOG_INFO("H:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.humidity));
  uint8_t data[] = {'H', 'e', 'l', 'l', 'o'};
  task_bluetooth_send_asynchronous(data, sizeof(data));

  err_code = task_generate_keys();
  PLATFORM_LOG_INFO("Keygen data status: %X", err_code);
  task_generate_dummy_hash();
  task_sign_hash();
  task_verify_hash();

  ruuvi_communication_message_t ble_in_msg;
  uint8_t ble_in_data[24];
  ble_in_msg.payload = ble_in_data;
  ble_in_msg.payload_length = sizeof(ble_in_data);
  while (1)
  {
    task_bluetooth_process();
    PLATFORM_LOG_INFO("Processing");
    while (RUUVI_SUCCESS == ble4_nus_message_get(&ble_in_msg))
    {
      //TODO: Task to identify and reply to message
      PLATFORM_LOG_HEXDUMP_INFO(ble_in_msg.payload, ble_in_msg.payload_length);
      ble_in_msg.payload_length = sizeof(ble_in_data);
      task_bluetooth_send_asynchronous(data, sizeof(data));
    }

    platform_yield();
  }
}