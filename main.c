#include "boards.h"
#include "app_error.h"
#include <string.h>

#include "application_config.h"

#include "ruuvi_sensor.h"
#include "ruuvi_error.h"
#include "environmental.h"
#include "bme280_interface.h"
#include "spi.h"
#include "yield.h"
// #include "ruuvi_endpoints.h"
#include "bme280.h"

//TODO use platform log
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
  ruuvi_status_t err_code = spi_init();
  NRF_LOG_INFO("SPI init status: %X", err_code);

  ruuvi_sensor_t environmental_sensor;

  err_code = bme280_interface_init();
  NRF_LOG_INFO("BME init status: %X", err_code);
  if(RUUVI_SUCCESS == err_code)
  {
    environmental_sensor.init           = bme280_interface_init;
    environmental_sensor.uninit         = bme280_interface_uninit;
    environmental_sensor.samplerate_set = bme280_interface_samplerate_set;
    environmental_sensor.samplerate_get = bme280_interface_samplerate_get;
    environmental_sensor.resolution_set = bme280_interface_resolution_set;
    environmental_sensor.resolution_get = bme280_interface_resolution_get;
    environmental_sensor.scale_set      = bme280_interface_scale_set;
    environmental_sensor.scale_get      = bme280_interface_scale_get;
    environmental_sensor.dsp_set        = bme280_interface_dsp_set;
    environmental_sensor.dsp_get        = bme280_interface_dsp_get;
    environmental_sensor.mode_set       = bme280_interface_mode_set;
    environmental_sensor.mode_get       = bme280_interface_mode_get;
    environmental_sensor.interrupt_set  = bme280_interface_interrupt_set;
    environmental_sensor.interrupt_get  = bme280_interface_interrupt_get;
    environmental_sensor.data_get       = bme280_interface_data_get;
  }


  ruuvi_sensor_samplerate_t samplerate = 1;
  err_code = environmental_sensor.samplerate_set(&samplerate);
  NRF_LOG_INFO("BME samplerate status: %X", err_code);

  ruuvi_sensor_dsp_function_t dsp = RUUVI_SENSOR_DSP_IIR;
  uint8_t dsp_parameter = 16;
  err_code = environmental_sensor.dsp_set(&dsp, &dsp_parameter);
  NRF_LOG_INFO("BME DSP status: %X", err_code);

  ruuvi_sensor_mode_t bme280_mode = RUUVI_SENSOR_MODE_CONTINOUS;
  err_code = environmental_sensor.mode_set(&bme280_mode);
  NRF_LOG_INFO("BME mode status: %X", err_code);

  ruuvi_environmental_data_t environmental;

  //init_acceleration();

  while (1)
  {
    NRF_LOG_FLUSH();
    platform_yield();
    platform_delay_ms(1000);
    
    err_code = environmental_sensor.data_get(&environmental);
    NRF_LOG_INFO("BME data status: %X", err_code);
    NRF_LOG_INFO("T:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.temperature));
    NRF_LOG_INFO("P:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.pressure)); 
    NRF_LOG_INFO("H:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.humidity));
  }
}