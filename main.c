#include "boards.h"
#include "app_error.h"
#include <string.h>

#include "application_config.h"

#include "ruuvi_sensor.h"
#include "ruuvi_error.h"
#include "environmental.h"
#include "acceleration.h"
#include "communication.h"
#include "bme280_interface.h"
#include "lis2dh12_interface.h"
#include "nfc_tag.h"
#include "spi.h"
#include "yield.h"
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

static uint8_t text[] = {'d', 'a', 't', 'a'};
static size_t text_len = sizeof(text);
static uint8_t url[] = {'r', 'u', 'u', 'v', 'i', '.', 'c', 'o', 'm'};
static size_t url_len = sizeof(url);
// static uint8_t app[] = {'c', 'o', 'm', '.', 'r', 'u', 'u', 'v', 'i', '.', 's', 't', 'a', 't', 'i', 'o', 'n'};
// static size_t app_len = sizeof(app);

int main(void)
{

  APP_ERROR_CHECK(PLATFORM_LOG_INIT(NULL));
  PLATFORM_LOG_DEFAULT_BACKENDS_INIT();

  //Init LED

  //Init NFC

  ruuvi_status_t err_code = spi_init();
  PLATFORM_LOG_INFO("SPI init status: %X", err_code);

  ruuvi_sensor_t acceleration_sensor;
  err_code = lis2dh12_interface_init();
  PLATFORM_LOG_INFO("LIS init status: %X", err_code);

  if (RUUVI_SUCCESS == err_code)
  {
    //TODO: refactor pointer setup into driver
    acceleration_sensor.init           = lis2dh12_interface_init;
    acceleration_sensor.uninit         = lis2dh12_interface_uninit;
    acceleration_sensor.samplerate_set = lis2dh12_interface_samplerate_set;
    acceleration_sensor.samplerate_get = lis2dh12_interface_samplerate_get;
    acceleration_sensor.resolution_set = lis2dh12_interface_resolution_set;
    acceleration_sensor.resolution_get = lis2dh12_interface_resolution_get;
    acceleration_sensor.scale_set      = lis2dh12_interface_scale_set;
    acceleration_sensor.scale_get      = lis2dh12_interface_scale_get;
    acceleration_sensor.dsp_set        = lis2dh12_interface_dsp_set;
    acceleration_sensor.dsp_get        = lis2dh12_interface_dsp_get;
    acceleration_sensor.mode_set       = lis2dh12_interface_mode_set;
    acceleration_sensor.mode_get       = lis2dh12_interface_mode_get;
    acceleration_sensor.interrupt_set  = lis2dh12_interface_interrupt_set;
    acceleration_sensor.interrupt_get  = lis2dh12_interface_interrupt_get;
    acceleration_sensor.data_get       = lis2dh12_interface_data_get;
  }

  ruuvi_sensor_samplerate_t accelerometer_samplerate = 1;
  err_code = acceleration_sensor.samplerate_set(&accelerometer_samplerate);
  PLATFORM_LOG_INFO("LIS samplerate status: %X", err_code);

  ruuvi_sensor_scale_t accelerometer_scale = RUUVI_SENSOR_SCALE_MIN;
  err_code = acceleration_sensor.scale_set(&accelerometer_scale);
  PLATFORM_LOG_INFO("LIS scale status: %X", err_code);

  ruuvi_sensor_resolution_t accelerometer_resolution = 10;
  err_code = acceleration_sensor.resolution_set(&accelerometer_resolution);
  PLATFORM_LOG_INFO("LIS resolution status: %X", err_code);

  ruuvi_sensor_mode_t accelerometer_mode = RUUVI_SENSOR_MODE_CONTINOUS;
  err_code = acceleration_sensor.mode_set(&accelerometer_mode);
  PLATFORM_LOG_INFO("LIS mode status: %X", err_code);

  ruuvi_sensor_t environmental_sensor;

  err_code = bme280_interface_init();
  PLATFORM_LOG_INFO("BME init status: %X", err_code);

  if (RUUVI_SUCCESS == err_code)
  {
    //TODO: refactor pointer setup into driver
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
  PLATFORM_LOG_INFO("BME samplerate status: %X", err_code);

  ruuvi_sensor_dsp_function_t dsp = RUUVI_SENSOR_DSP_IIR;
  uint8_t dsp_parameter = 16;
  err_code = environmental_sensor.dsp_set(&dsp, &dsp_parameter);
  PLATFORM_LOG_INFO("BME DSP status: %X", err_code);

  ruuvi_sensor_mode_t bme280_mode = RUUVI_SENSOR_MODE_CONTINOUS;
  err_code = environmental_sensor.mode_set(&bme280_mode);
  PLATFORM_LOG_INFO("BME mode status: %X", err_code);

  // ruuvi_status_t nfc_app_record_set(const uint8_t scheme, const chat* uri, size_t length);

  // Functions for implementing communication api
  ruuvi_communication_channel_t nfc;
  nfc.init = nfc_init;
  nfc.uninit = nfc_uninit;
  nfc.message_get = nfc_message_get;
  nfc.process_asynchronous = nfc_process_asynchronous;

  // Setup constant records
  err_code = nfc_text_record_set(text, text_len);
  err_code |= nfc_uri_record_set(url, url_len);
  //err_code = nfc_app_record_set(app, app_len);

  //init
  err_code = nfc.init();
  PLATFORM_LOG_INFO("NFC init status: %d", err_code);
  err_code = nfc.process_asynchronous();

  PLATFORM_LOG_INFO("NFC data set status: %d", err_code);

  ruuvi_environmental_data_t environmental;
  ruuvi_acceleration_data_t  acceleration;
  ruuvi_communication_message_t rx;
  uint8_t payload[20];
  rx.payload = payload;
  rx.payload_length = sizeof(payload);
  platform_delay_ms(1000);

  while (1)
  {
    PLATFORM_LOG_FLUSH();
    nfc.process_asynchronous();
    while (RUUVI_SUCCESS == nfc.message_get(&rx))
    {
      PLATFORM_LOG_INFO("Got message");
      PLATFORM_LOG_HEXDUMP_INFO(rx.payload, rx.payload_length);
      // Payload length gets overwritten, let the driver know full length of buffer
      rx.payload_length = sizeof(payload);
    }
    err_code = environmental_sensor.data_get(&environmental);
    // NRF_LOG_INFO("BME data status: %X", err_code);
    // NRF_LOG_INFO("T:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.temperature));
    // NRF_LOG_INFO("P:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.pressure));
    // NRF_LOG_INFO("H:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(environmental.humidity));

    err_code = acceleration_sensor.data_get(&acceleration);
    // NRF_LOG_INFO("LIS data status %x", err_code);
    // NRF_LOG_INFO("X:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(acceleration.x_mg));
    // NRF_LOG_INFO("Y:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(acceleration.y_mg));
    // NRF_LOG_INFO("Z:" NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(acceleration.z_mg));

    platform_yield();
  }
}