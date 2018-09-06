#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "test_acceleration.h"
#include "test_adc.h"
#include "test_environmental.h"
#include "test_sensor.h"


#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#if RUUVI_RUN_TESTS

static size_t total  = 0;
static size_t passed = 0;

ruuvi_driver_status_t test_sensor_init(const ruuvi_driver_sensor_init_fp init, const ruuvi_driver_bus_t bus, const uint8_t handle)
{
  // - Sensor must return RUUVI_DRIVER_SUCCESS on first init.
  ruuvi_driver_sensor_t DUT;
  memset(&DUT, 0, sizeof(DUT));
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  bool failed = false;
  total++;
  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // - None of the sensor function pointers may be NULL after init
  if(DUT.init              == NULL ||
     DUT.uninit            == NULL ||
     DUT.configuration_get == NULL ||
     DUT.configuration_set == NULL ||
     DUT.data_get          == NULL ||
     DUT.dsp_get           == NULL ||
     DUT.dsp_set           == NULL ||
     DUT.mode_get          == NULL ||
     DUT.mode_set          == NULL ||
     DUT.resolution_get    == NULL ||
     DUT.resolution_set    == NULL ||
     DUT.samplerate_get    == NULL ||
     DUT.samplerate_set    == NULL ||
     DUT.scale_get         == NULL ||
     DUT.scale_set         == NULL)
  {
    failed = true;
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
  }

  // - Sensor must return RUUVI_DRIVER_ERROR_INVALID_STATE when initializing sensor which is already init
  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_ERROR_INVALID_STATE != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // - Sensor must return RUUVI_DRIVER_SUCCESS on first uninit
  err_code = DUT.uninit(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // - All of sensor function pointers must be NULL after uninit
  if(DUT.init              != NULL ||
     DUT.uninit            != NULL ||
     DUT.configuration_get != NULL ||
     DUT.configuration_set != NULL ||
     DUT.data_get          != NULL ||
     DUT.dsp_get           != NULL ||
     DUT.dsp_set           != NULL ||
     DUT.mode_get          != NULL ||
     DUT.mode_set          != NULL ||
     DUT.resolution_get    != NULL ||
     DUT.resolution_set    != NULL ||
     DUT.samplerate_get    != NULL ||
     DUT.samplerate_set    != NULL ||
     DUT.scale_get         != NULL ||
     DUT.scale_set         != NULL)
  {
    failed = true;
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
  }
  // - Sensor initialization must be successful after uninit.
  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // - Init and Uninit must return RUUVI_DRIVER_ERROR_NULL if pointer to sensor struct is NULL
  err_code = init(NULL, bus, handle);
  err_code |= DUT.uninit(NULL, bus, handle);
  if(RUUVI_DRIVER_ERROR_NULL != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // Uninitialise sensor after test
  DUT.uninit(&DUT, bus, handle);

  if(failed)
  {
    return RUUVI_DRIVER_ERROR_SELFTEST;
  }
  passed++;
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t test_sensor_status(size_t* tests_total, size_t* tests_passed)
{
  *tests_total =  total;
  *tests_passed = passed;
  return RUUVI_DRIVER_SUCCESS;
}

static ruuvi_driver_status_t test_sensor_setup_set_get(ruuvi_driver_sensor_setup_fp set, ruuvi_driver_sensor_setup_fp get)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  uint8_t config = 0;
  uint8_t original = 0;
  bool failed = false;
  total++;
  // Test constant values
  uint8_t cfg_constants[] = { RUUVI_DRIVER_SENSOR_CFG_DEFAULT, RUUVI_DRIVER_SENSOR_CFG_MAX, RUUVI_DRIVER_SENSOR_CFG_MIN, RUUVI_DRIVER_SENSOR_CFG_NO_CHANGE };

  for(size_t ii = 0; ii < sizeof(cfg_constants); ii++)
  {
    config  = cfg_constants[ii];
    err_code = set(&config);
    original = config;
    err_code |= get(&config);
    if(config != original ||
       RUUVI_DRIVER_SUCCESS != err_code)
    {
      failed = true;
      RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    }
  }

  // Test values 1 ... 200
  for(uint8_t ii = 1; ii < 200; ii++)
  {
    config  = ii;
    original = config;
    err_code = set(&config);

    // Set value must be at least requested value
    if(RUUVI_DRIVER_SUCCESS == err_code &&
       original > config)
    {
      failed = true;
      RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    }

    // Get must be as what was returned in set
    original = config;
    err_code |= get(&config);
    if(config != original &&
       RUUVI_DRIVER_SUCCESS == err_code)
    {
      failed = true;
      RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
      break;
    }
    // Break on not supported
    if(RUUVI_DRIVER_ERROR_NOT_SUPPORTED == err_code) { break;}
    // Return error on any other error code
    if(RUUVI_DRIVER_SUCCESS != err_code) { return RUUVI_DRIVER_ERROR_SELFTEST; }
  }

  // Check NULL check
  err_code = set(NULL);
  if(RUUVI_DRIVER_ERROR_NULL != err_code)
  {
      failed = true;
      RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
  }
  err_code = get(NULL);
  if(RUUVI_DRIVER_ERROR_NULL != err_code)
  {
      failed = true;
      RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
  }

  if(failed)
  {
    return RUUVI_DRIVER_ERROR_SELFTEST;
  }

  passed++;
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t test_sensor_setup(const ruuvi_driver_sensor_init_fp init, const ruuvi_driver_bus_t bus, const uint8_t handle)
{
  // - Sensor must return RUUVI_DRIVER_SUCCESS on first init.
  ruuvi_driver_sensor_t DUT;
  memset(&DUT, 0, sizeof(DUT));
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  bool failed = false;
  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // Test scale
  err_code = test_sensor_setup_set_get(DUT.scale_set, DUT.scale_get);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // Test samplerate
  err_code = test_sensor_setup_set_get(DUT.samplerate_set, DUT.samplerate_get);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // Test resolution
  err_code = test_sensor_setup_set_get(DUT.resolution_set, DUT.resolution_get);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  // Uninitialise sensor after test
  DUT.uninit(&DUT, bus, handle);

  if(failed)
  {
    return RUUVI_DRIVER_ERROR_SELFTEST;
  }
  passed++;
  return RUUVI_DRIVER_SUCCESS;
}

void test_sensor_run(void)
{
  // Give a few milliseconds between tests to flush the logs
  test_adc_run();
  ruuvi_platform_delay_ms(20);
  test_environmental_run();
  ruuvi_platform_delay_ms(20);
  test_acceleration_run();
  ruuvi_platform_delay_ms(20);
}
#else
// Dummy implementation
ruuvi_driver_status_t test_sensor_status(size_t* total, size_t* passed)
{
  *total = 0;
  *passed = 0;
  return RUUVI_DRIVER_SUCCESS;
}
void test_sensor_run(void)
{}
#endif