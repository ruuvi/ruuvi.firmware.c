#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "test_acceleration.h"
#include "test_adc.h"
#include "test_environmental.h"
#include "test_sensor.h"

#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#if RUUVI_RUN_TESTS

// Number of times to run test on statistics-dependent tests, such as sampling noise.
#define MAX_RETRIES 50
static size_t m_total  = 0;
static size_t m_passed = 0;

ruuvi_driver_status_t test_sensor_init(const ruuvi_driver_sensor_init_fp init, const ruuvi_driver_bus_t bus, const uint8_t handle)
{
  // - Sensor must return RUUVI_DRIVER_SUCCESS on first init.
  ruuvi_driver_sensor_t DUT;
  memset(&DUT, 0, sizeof(DUT));
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  bool test_ok = true;
  bool failed = false;

  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

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
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
    failed = true;
  }
  test_sensor_register(test_ok);

  // - Sensor must return RUUVI_DRIVER_ERROR_INVALID_STATE when initializing sensor which is already init
  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_ERROR_INVALID_STATE != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
    failed = true;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor must return RUUVI_DRIVER_SUCCESS on first uninit
  err_code = DUT.uninit(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
    failed = true;
  }
  test_sensor_register(test_ok);
  test_ok = true;

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
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
    failed = true;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor initialization must be successful after uninit.
  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
    failed = true;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  //  * - Sensor mode_get must return RUUVI_DRIVER_SENSOR_CFG_SLEEP after init.
  uint8_t mode;
  DUT.mode_get(&mode);
  if(RUUVI_DRIVER_SUCCESS != err_code || RUUVI_DRIVER_SENSOR_CFG_SLEEP != mode)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
    failed = true;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Init and Uninit must return RUUVI_DRIVER_ERROR_NULL if pointer to sensor struct is NULL
  err_code = init(NULL, bus, handle);
  err_code |= DUT.uninit(NULL, bus, handle);
  if(RUUVI_DRIVER_ERROR_NULL != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
    failed = true;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // Uninitialise sensor after test
  DUT.uninit(&DUT, bus, handle);

  if(failed)
  {
    return RUUVI_DRIVER_ERROR_SELFTEST;
  }

  return RUUVI_DRIVER_SUCCESS;
}

static ruuvi_driver_status_t test_sensor_setup_set_get(const ruuvi_driver_sensor_t* DUT, const ruuvi_driver_sensor_setup_fp set, const ruuvi_driver_sensor_setup_fp get)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  uint8_t config = 0;
  uint8_t original = 0;
  bool failed = false;

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
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
  }

  //  Sensor must return RUUVI_DRIVER_ERROR_INVALID_STATE if sensor is not in SLEEP mode while being configured.
  uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS;
  config = RUUVI_DRIVER_SENSOR_CFG_DEFAULT;
  DUT->mode_set(&mode);
  err_code = set(&config);
  if(RUUVI_DRIVER_ERROR_INVALID_STATE != err_code)
  {
      failed = true;
      RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
  }
  mode = RUUVI_DRIVER_SENSOR_CFG_SLEEP;
  DUT->mode_set(&mode);


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
    if(RUUVI_DRIVER_ERROR_NOT_SUPPORTED == err_code)
    {
      break;
    }

    // Return error on any other error code
    if(RUUVI_DRIVER_SUCCESS != err_code)
    {
      return RUUVI_DRIVER_ERROR_SELFTEST;
    }
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

  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t test_sensor_setup(const ruuvi_driver_sensor_init_fp init, const ruuvi_driver_bus_t bus, const uint8_t handle)
{
  // - Sensor must return RUUVI_DRIVER_SUCCESS on first init.
  ruuvi_driver_sensor_t DUT;
  memset(&DUT, 0, sizeof(DUT));

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

  bool failed = false;
  bool test_ok = true;

  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    // Init is test elsewhere, do not register result.
  }

  // Test scale
  err_code = test_sensor_setup_set_get(&DUT, DUT.scale_set, DUT.scale_get);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // Test samplerate
  err_code = test_sensor_setup_set_get(&DUT, DUT.samplerate_set, DUT.samplerate_get);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // Test resolution
  err_code = test_sensor_setup_set_get(&DUT, DUT.resolution_set, DUT.resolution_get);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // Uninitialise sensor after test
  DUT.uninit(&DUT, bus, handle);

  if(failed)
  {
    return RUUVI_DRIVER_ERROR_SELFTEST;
  }
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t test_sensor_modes(ruuvi_driver_sensor_init_fp init, ruuvi_driver_bus_t bus, uint8_t handle)
{
  // - Sensor must return RUUVI_DRIVER_SUCCESS on first init.
  ruuvi_driver_sensor_t DUT;
  memset(&DUT, 0, sizeof(DUT));

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  bool failed = false;
  bool test_ok = true;

  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    // Init is test elsewhere, do not register result.
  }

  // - Sensor must be in SLEEP mode after init
  uint8_t mode;
  err_code = DUT.mode_get(&mode);
  if(RUUVI_DRIVER_SUCCESS != err_code || RUUVI_DRIVER_SENSOR_CFG_SLEEP != mode)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor must return all values as INVALID if sensor is read before first sample
  ruuvi_driver_sensor_data_t invalid_data = {0};
  invalid_data.timestamp = RUUVI_DRIVER_SENSOR_INVALID_TIMSTAMP;
  invalid_data.value0    = RUUVI_DRIVER_SENSOR_INVALID_VALUE;
  invalid_data.value1    = RUUVI_DRIVER_SENSOR_INVALID_VALUE;
  invalid_data.value2    = RUUVI_DRIVER_SENSOR_INVALID_VALUE;
  ruuvi_driver_sensor_data_t new_data = {0};
  err_code = DUT.data_get(&new_data);
  if(RUUVI_DRIVER_SUCCESS != err_code ||  memcmp (&invalid_data, &new_data, sizeof(ruuvi_driver_sensor_data_t)))
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor must be in SLEEP mode after mode has been set to SINGLE
  mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
  err_code = DUT.mode_set(&mode);
  if(RUUVI_DRIVER_SUCCESS != err_code || RUUVI_DRIVER_SENSOR_CFG_SLEEP != mode)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor must have new data after setting mode to SINGLE returns ()
  ruuvi_driver_sensor_data_t old_data = {0};
  memcpy(&old_data, &new_data, sizeof(ruuvi_driver_sensor_data_t));
  mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
  ruuvi_platform_delay_ms(2);
  err_code = DUT.mode_set(&mode);
  err_code = DUT.data_get(&new_data);
  if(RUUVI_DRIVER_SUCCESS != err_code || (0 == memcmp (&old_data, &new_data, sizeof(ruuvi_driver_sensor_data_t))))
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor must same values, including timestamp, on successive calls to DATA_GET after SINGLE sample
  memcpy(&old_data, &new_data, sizeof(ruuvi_driver_sensor_data_t));
  ruuvi_platform_delay_ms(10);
  err_code = DUT.data_get(&new_data);
  if(RUUVI_DRIVER_SUCCESS != err_code || 0 != memcmp (&old_data, &new_data, sizeof(ruuvi_driver_sensor_data_t)))
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor must stay in CONTINUOUS mode after being set to continuous.
  mode = RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS;
  err_code = DUT.mode_set(&mode);
  err_code |= DUT.mode_get(&mode);
  if(RUUVI_DRIVER_SUCCESS != err_code || RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS != mode)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor must return RUUVI_DRIVER_ERROR_INVALID_STATE if set to SINGLE while in continuous mode and remain in continuous mode
  mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
  err_code = DUT.mode_set(&mode);
  if(RUUVI_DRIVER_ERROR_INVALID_STATE != err_code || RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS != mode)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  err_code = DUT.mode_set(NULL);
  if(RUUVI_DRIVER_ERROR_NULL != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  err_code = DUT.mode_get(NULL);
  if(RUUVI_DRIVER_ERROR_NULL != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // Sensor must return updated data in CONTINUOUS mode, at least timestamp has to be updated after two ms wait.
  mode = RUUVI_DRIVER_SENSOR_CFG_SLEEP;
  err_code = DUT.mode_set(&mode);
  uint8_t samplerate = RUUVI_DRIVER_SENSOR_CFG_MAX;
  err_code = DUT.samplerate_set(&samplerate);
  mode = RUUVI_DRIVER_SENSOR_CFG_CONTINUOUS;
  err_code = DUT.mode_set(&mode);
  uint32_t interval = (1000 / (samplerate+1));
  ruuvi_platform_delay_ms(2*interval);
  err_code |= DUT.data_get(&old_data);
  int retries = 0;
  for(; retries < MAX_RETRIES; retries++)
  {
    ruuvi_platform_delay_ms(2*interval);
    err_code |= DUT.data_get(&new_data);
    if(old_data.timestamp == new_data.timestamp || RUUVI_DRIVER_SUCCESS != err_code)
    {
      if(RUUVI_DRIVER_STATUS_MORE_AVAILABLE == err_code)
      {
        do
        {
          err_code |= DUT.data_get(&new_data);
        }
        while(RUUVI_DRIVER_STATUS_MORE_AVAILABLE == err_code);
        continue;
      }
      else
      {
        RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_INTERNAL, ~RUUVI_DRIVER_ERROR_FATAL);
        failed = true;
        test_ok = false;
        break;
      }
    }

    if(old_data.value0 != new_data.value0
     ||old_data.value1 != new_data.value2
     ||old_data.value2 != new_data.value2)
    {
      break;
    }
  }

  if(MAX_RETRIES == retries)
  {
    failed = true;
    test_ok = false;
  }
  test_sensor_register(test_ok);
  test_ok = true;

  // - Sensor must return RUUVI_DRIVER_ERROR_NULL if null mode is passed as a parameter

  // Uninitialise sensor after test
  DUT.uninit(&DUT, bus, handle);

  if(failed)
  {
    return RUUVI_DRIVER_ERROR_SELFTEST;
  }
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t test_sensor_status(size_t* tests_total, size_t* tests_passed)
{
  *tests_total =  m_total;
  *tests_passed = m_passed;
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t test_sensor_register(bool passed)
{
  m_total++;
  if(passed) { m_passed++; }
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