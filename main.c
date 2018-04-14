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
  uint8_t hello[] = {'H', 'e', 'l', 'l', 'o'};
  task_bluetooth_send_asynchronous(hello, sizeof(hello));

  err_code = task_generate_keys();
  // PLATFORM_LOG_INFO("Keygen data status: %X", err_code);
  // task_generate_dummy_hash();
  // char* data = "{\"type\":\"CryptoMessage\",\"signer\":\"sender\",\"pubKey\":\"publicKeyOfSenderToIOTA\",\"signature\":\"signatureOfSenderToIOTA\",\"cryptoFunction\":\"SHA256\",\"message\":{\"type\":\"CryptoMessage\",\"tsSigned\":\"Timestampaddedbysender\",\"signer\":\"notary\",\"pubKey\":\"pubKeyOfBeacon\",\"signature\":\"signatureOfBeacon\",\"cryptoFunction\":\"SHA256\",\"message\":{\"type\":\"proofOfVisit\",\"version\":\"0.0.1\",\"timeLimit\":30,\"tsSigned\":\"ISOsignerTimeStamp\",\"tsSentChallenge\":\"2017-10-18T17:29:00+02:00\",\"tsReceivedChallenge\":\"2017-10-18T17:29:18+02:00\",\"challenge\":{\"type\":\"CryptoMessage\",\"signer\":\"requestor\",\"pubKey\":\"publicKeyOfRequestor\",\"signature\":\"signatureOfSmartphone\",\"cryptoFunction\":\"SHA256\",\"message\":{\"type\":\"RandomString\",\"string\":\"SomeRandomString\",\"tsSigned\":\"ISOsignerTimeStamp\"}}}}}";
  // task_sha256_message((uint8_t*)data, strlen(data));
  // task_sign_hash();
  // task_verify_hash();

  ruuvi_communication_message_t ble_in_msg;
  uint8_t ble_in_data[24];
  ble_in_msg.payload = ble_in_data;
  ble_in_msg.payload_length = sizeof(ble_in_data);
  
  while (1)
  {
    PLATFORM_LOG_DEBUG("Processing");
    while (RUUVI_SUCCESS == ble4_nus_message_get(&ble_in_msg))
    {
      PLATFORM_LOG_DEBUG("Got data");
      // PLATFORM_LOG_HEXDUMP_INFO(ble_in_msg.payload, ble_in_msg.payload_length);
      // Data length from BLE stack is passed as length of message,
      // so setting invalid length to message field does not lead to overflow
      task_pop_process_incoming_data(ble_in_msg.payload, ble_in_msg.payload_length);
      ble_in_msg.payload_length = sizeof(ble_in_data);
    }
    task_bluetooth_process();
    PLATFORM_LOG_DEBUG("Yielding");
    platform_yield();
    PLATFORM_LOG_DEBUG("Resuming");
  }
}