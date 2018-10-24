#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_interface_lis2dh12.h"
#include "ruuvi_interface_log.h"
#include "test_sensor.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

// Use own file for each test run to track the source of any errors.
static ruuvi_driver_status_t test_run(ruuvi_driver_sensor_init_fp init, ruuvi_driver_bus_t bus, uint8_t handle)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  #ifdef RUUVI_RUN_TESTS
    err_code = test_sensor_init(init, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_SELFTEST);

    err_code = test_sensor_setup(init, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_SELFTEST);

    err_code = test_sensor_modes(init, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_SELFTEST);
  #endif
  return err_code;
}

ruuvi_driver_status_t test_acceleration_run(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  uint8_t handle = 0;

  #if RUUVI_BOARD_ACCELEROMETER_LIS2DH12_PRESENT
    // Only SPI supported for now
    bus = RUUVI_DRIVER_BUS_SPI;
    handle = RUUVI_BOARD_SPI_SS_ACCELEROMETER_PIN;
    err_code |= test_run(ruuvi_interface_lis2dh12_init, bus, handle);
  #endif

  return err_code;
}