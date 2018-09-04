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

#if RUUVI_RUN_TESTS

static size_t total  = 0;
static size_t passed = 0;

ruuvi_driver_status_t test_sensor_init(const ruuvi_driver_sensor_init_fp init, const ruuvi_driver_bus_t bus, const uint8_t handle)
{
  // - Sensor must return RUUVI_DRIVER_SUCCESS on first init.
  ruuvi_driver_sensor_t DUT;
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  bool failed = false;
  total++;
  err_code = init(&DUT, bus, handle);
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    passed = false;
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
    passed = false;
  }

  // - Init and Uninit must return RUUVI_DRIVER_ERROR_NULL if pointer to sensor struct is NULL
  err_code = init(NULL, bus, handle);
  err_code |= DUT.uninit(NULL, bus, handle);
  if(RUUVI_DRIVER_ERROR_NULL != err_code)
  {
    RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    passed = false;
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