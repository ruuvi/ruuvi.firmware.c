#include "application_config.h"
#include "data_structures/ruuvi_library_ringbuffer.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_atomic.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_shtcx.h"
#include "ruuvi_interface_environmental_mcu.h"
#include "ruuvi_interface_log.h"
#include "task_environmental.h"
#include "task_pressure.h"
#include "task_led.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

static ruuvi_driver_sensor_t environmental_sensor = {0}; // Sensor API
static uint8_t buffer[1024];                      //!< Raw buffer for environmental logs
static ruuvi_interface_atomic_t buffer_wlock;
static ruuvi_interface_atomic_t buffer_rlock;
/** @brief Buffer structure for outgoing data */
static ruuvi_library_ringbuffer_t ringbuf = {.head = 0,
                                             .tail = 0,
                                             .block_size = 32,
                                             .storage_size = sizeof(buffer),
                                             .index_mask = (sizeof(buffer) / 32) - 1,
                                             .storage = buffer,
                                             .lock = ruuvi_interface_atomic_flag,
                                             .writelock = &buffer_wlock,
                                             .readlock  = &buffer_rlock};

ruuvi_driver_status_t task_environmental_configure(ruuvi_driver_sensor_configuration_t*
    config)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO,
                      "\r\nAttempting to configure environmental with:\r\n");
  // Use empty in place of unit
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "");
  err_code |= environmental_sensor.configuration_set(&environmental_sensor, config);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, "Actual configuration:\r\n");
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "");
  return err_code;
}

ruuvi_driver_status_t task_environmental_init(void)
{
  // Assume "Not found", gets set to "Success" if a usable sensor is present
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ENVIRONMENTAL_SAMPLERATE;
  config.resolution    = APPLICATION_ENVIRONMENTAL_RESOLUTION;
  config.scale         = APPLICATION_ENVIRONMENTAL_SCALE;
  config.dsp_function  = APPLICATION_ENVIRONMENTAL_DSPFUNC;
  config.dsp_parameter = APPLICATION_ENVIRONMENTAL_DSPPARAM;
  config.mode          = APPLICATION_ENVIRONMENTAL_MODE;
  uint8_t handle = 0;

  #if (RUUVI_BOARD_ENVIRONMENTAL_SHTCX_PRESENT && RUUVI_INTERFACE_ENVIRONMENTAL_SHTCX_ENABLED)
  err_code = RUUVI_DRIVER_SUCCESS;
  bus = RUUVI_DRIVER_BUS_I2C;
  handle = RUUVI_BOARD_SHTCX_I2C_ADDRESS;
  err_code |= ruuvi_interface_shtcx_init(&environmental_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |= task_environmental_configure(&config);
    // Try to configure pressure sensor for the application. 
    task_pressure_init();
    return err_code;
  }
  #endif

  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_PRESENT
  err_code = RUUVI_DRIVER_SUCCESS;
  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_SPI_USE
  bus = RUUVI_DRIVER_BUS_SPI;
  handle = RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN;
  #endif
  #if RUUVI_BOARD_ENVIRONMENTAL_BME280_I2C_USE
  bus = RUUVI_DRIVER_BUS_I2C;
  handle = RUUVI_BOARD_BME280_I2C_ADDRESS;
  #endif
  err_code |= ruuvi_interface_bme280_init(&environmental_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |= task_environmental_configure(&config);
    return err_code;
  }
  #endif

  #if RUUVI_BOARD_ENVIRONMENTAL_MCU_PRESENT
  err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_environmental_mcu_init(&environmental_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |= task_environmental_configure(&config);
    return err_code;
  }

  #endif
  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_environmental_sample(void)
{
  if(NULL == environmental_sensor.mode_set) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
  return environmental_sensor.mode_set(&mode);
}

ruuvi_driver_status_t task_environmental_data_log(const ruuvi_interface_log_severity_t
    level)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_environmental_data_t data;

  if(NULL == environmental_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  // If the mode is single, take a new sample.
  if(APPLICATION_ENVIRONMENTAL_MODE == RUUVI_DRIVER_SENSOR_CFG_SINGLE)
  {
    err_code |= task_environmental_sample();
  }

  err_code |= environmental_sensor.data_get(&data);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Time: %lu\r\n",
           (uint32_t)(data.timestamp_ms & 0xFFFFFFFF));
  ruuvi_interface_log(level, message);
  snprintf(message, sizeof(message), "Temperature: %.2f\r\n", data.temperature_c);
  ruuvi_interface_log(level, message);
  snprintf(message, sizeof(message), "Pressure: %.2f\r\n", data.pressure_pa);
  ruuvi_interface_log(level, message);
  snprintf(message, sizeof(message), "Humidity: %.2f\r\n", data.humidity_rh);
  ruuvi_interface_log(level, message);
  return err_code;
}

ruuvi_driver_status_t task_environmental_data_get(ruuvi_interface_environmental_data_t*
    const data)
{
  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }

  if(NULL == environmental_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code = environmental_sensor.data_get(data);
  if(RUUVI_DRIVER_FLOAT_INVALID == data->pressure_pa && task_pressure_is_init())
  {
    ruuvi_interface_environmental_data_t pressure_data;
    err_code |= task_pressure_data_get(&pressure_data);
    data->pressure_pa = pressure_data.pressure_pa;
  }

  return err_code;
}

ruuvi_driver_status_t task_environmental_on_button(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= task_environmental_data_log(RUUVI_INTERFACE_LOG_INFO);
  return err_code;
}