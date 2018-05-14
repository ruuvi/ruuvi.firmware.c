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
#else
#define PLATFORM_LOG_LEVEL       0
#endif
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();

static uint8_t name[] = {'R', 'u', 'u', 'v', 'i'};
static uint8_t errors = 0;
static uint8_t error_code[20] = {'E', 'r', 'r', ':', ' '};
static uint8_t err_index = 5;

// Read default configuration from application_config.h and initialize accelerometer with it.
// If there is error, add error information to global error counters.
// Call peripheral init before calling this to have SPI bus and timers operational.
static ruuvi_status_t app_accelerometer_init(void)
{
  err_code = task_accelerometer_init();
  ruuvi_sensor_configuration_t accelerometer_configuration;
  accelerometer_configuration->resolution = 10;
  err_code |= task_accelerometer_setup();
  if (RUUVI_SUCCESS != err_code)
  {
    error_code[err_index++] = 'A'; // Accelerometer
    errors++;
  }
  PLATFORM_LOG_INFO("Accelerometer init status: 0x%X", err_code);
}

// Initialize bluetooth with default configuration from application_bluetooth_configuration.h
//
static ruuvi_status_t app_bluetooth_init(void)
{
  err_code  = task_bluetooth_init();
  err_code |= task_bluetooth_advertise(name, sizeof(name) - 1);
  if (RUUVI_SUCCESS != err_code)
  {
    error_code[err_index++] = 'B'; // Bluetooth
    errors++;
  }
  PLATFORM_LOG_INFO("Bluetooth init status: 0x%X", err_code);
}


// Init environmental sensor. Reads default configuration from application_config.h
static ruuvi_status_t app_environmental_init()
{
  err_code = task_environmental_init();
  err_code |= task_environmental_setup();
  if (RUUVI_SUCCESS != err_code)
  {
    error_code[err_index++] = 'E'; // Environmental
    errors++;
  }
  PLATFORM_LOG_INFO("Environmental init status: 0x%X", err_code);
}

// Init peripherals, such as leds, timers, SPI, WDT and NFC. Default configuration is in application_config.h if applicable.
// Some board-specific configuration, such as SPI speed and pinout is in board configuration.
// If there is error, add error information to global error counters.
static ruuvi_status_t app_peripheral_init()
{
  task_leds_init();
  task_spi_init();
  task_twi_init();
  task_uart_init();
  task_timers_init();
  task_rtc_init();
  task_crypto_init();
  task_nfc_init();
  task_adc_init();

  if (RUUVI_SUCCESS != err_code)
  {
    error_code[err_index++] = 'P'; // Peripheral
    errors++;
  }
  PLATFORM_LOG_INFO("Peripheral init status: 0x%X", err_code);
}

// Init events
static ruuvi_status_t app_events_init()
{

}

// Init periodic task
static ruuvi_status_t app_periodic_task_init()
{

}

int main(void)
{
  // Init log first
  APP_ERROR_CHECK(PLATFORM_LOG_INIT(NULL));
  PLATFORM_LOG_DEFAULT_BACKENDS_INIT();
  PLATFORM_LOG_INFO("Logging started");
  
  // Init yield then, as other init tasks might require delays
  platform_yield_init();

  // Init peripherals, sensors will require them
  app_peripheral_init();
  
  // Init bluetooth
  app_bluetooth_init();

  // Init sensors
  app_accelerometer_init();
  app_environmental_init();
  
  // Run self-test
  app_selftest();

  // init event handling
  app_events_init();

  app_periodic_task_init();

  // Execute schedule and go back to sleep
  while (1)
  {
    PLATFORM_LOG_DEBUG("Processing");
    platform_scheduler_execute();
    PLATFORM_LOG_DEBUG("Yielding");
    platform_yield();
    PLATFORM_LOG_DEBUG("Resuming");
  }
}