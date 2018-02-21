#include "boards.h"
#include "app_error.h"
#include <string.h>

#include "application_config.h"

#include "ruuvi_sensor.h"
#include "ruuvi_error.h"
#include "environmental.h"
#include "bme280_interface.h"
#include "spi.h"
// #include "ruuvi_endpoints.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

int main(void)
{
//    bsp_board_leds_init();

  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  NRF_LOG_INFO("SPI example.");

  // Register callbacks to drivers
  //init_endpoints();

  // Register environmental driver if available
  // init_environmental();
  // environmental_init();
  // environmental_selftest();
  // ruuvi_sensor_t environmental;
  ruuvi_status_t err_code = spi_init();
  NRF_LOG_INFO("SPI init status: %X", err_code);
  err_code = bme280_interface_init();
  NRF_LOG_INFO("BME init status: %X", err_code);


  //init_acceleration();

  while (1)
  {

    NRF_LOG_FLUSH();
  }
}