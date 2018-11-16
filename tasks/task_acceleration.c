#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_lis2dh12.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_yield.h"
#include "task_acceleration.h"
#include "task_gatt.h"
#include "task_led.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

RUUVI_PLATFORM_TIMER_ID_DEF(acceleration_timer);
static ruuvi_driver_sensor_t acceleration_sensor = {0};
static uint8_t m_nbr_movements;


//handler for scheduled accelerometer event
static void task_acceleration_scheduler_task(void *p_event_data, uint16_t event_size)
{
  // No action necessary
}

// Timer callback, schedule accelerometer event here.
static void task_acceleration_timer_cb(void* p_context)
{
  ruuvi_platform_scheduler_event_put(NULL, 0, task_acceleration_scheduler_task);
}

static void task_acceleration_fifo_full_task(void *p_event_data, uint16_t event_size)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_acceleration_data_t data[32];
  size_t data_len = sizeof(data);
  err_code |= ruuvi_interface_lis2dh12_fifo_read(&data_len, data);
  char msg[APPLICATION_LOG_BUFFER_SIZE] = { 0 };
  snprintf(msg, sizeof(msg), "%lu: Read %u data points\r\n", (uint32_t)ruuvi_platform_rtc_millis(), data_len);
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, msg);
  for(int ii = 0; ii < data_len; ii++)
  {
    memset(msg, 0, sizeof(msg));
    /*snprintf(msg, sizeof(msg), "T: %lu; X: %.3f; Y: %.3f; Z: %.3f;\r\n", (uint32_t)(data[ii].timestamp_ms&0xFFFFFFFF), data[ii].x_g, data[ii].y_g, data[ii].z_g);
    ruuvi_platform_log(RUUVI_INTERFACE_LOG_DEBUG, msg);
    ruuvi_platform_delay_ms(1);*/
    snprintf(msg, 21,"%+4.2f;%+4.2f;%+4.2f", data[ii].x_g, data[ii].y_g, data[ii].z_g );
    ruuvi_interface_communication_message_t gatt_msg = { 0 };
    memcpy(gatt_msg.data, msg, 20);
    gatt_msg.data_length = 20;
    // Loop here until data is sent
    // TODO: Handle case where NUS disconnection happens here
    while(RUUVI_DRIVER_SUCCESS != task_gatt_send(&gatt_msg))
    {
      ruuvi_platform_yield();
    };
  }

  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);
}

static void on_fifo (ruuvi_interface_gpio_evt_t event)
{
  ruuvi_platform_scheduler_event_put(NULL, 0, task_acceleration_fifo_full_task);
}


static void on_movement (ruuvi_interface_gpio_evt_t event)
{
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_DEBUG, "Activity\r\n");
  m_nbr_movements++;
}


static ruuvi_driver_status_t task_acceleration_configure(void)
{
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ACCELEROMETER_SAMPLERATE;
  config.resolution    = APPLICATION_ACCELEROMETER_RESOLUTION;
  config.scale         = APPLICATION_ACCELEROMETER_SCALE;
  config.dsp_function  = APPLICATION_ACCELEROMETER_DSPFUNC;
  config.dsp_parameter = APPLICATION_ACCELEROMETER_DSPPARAM;
  config.mode          = APPLICATION_ACCELEROMETER_MODE;
  if(NULL == acceleration_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return acceleration_sensor.configuration_set(&acceleration_sensor, &config);
}

ruuvi_driver_status_t task_acceleration_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  uint8_t handle = 0;
  m_nbr_movements = 0;

  // Initialize timer for accelerometer task. Note: the timer is not started.
 err_code |= ruuvi_platform_timer_create(&acceleration_timer, RUUVI_INTERFACE_TIMER_MODE_REPEATED, task_acceleration_timer_cb);

  #if RUUVI_BOARD_ACCELEROMETER_LIS2DH12_PRESENT
    err_code = RUUVI_DRIVER_SUCCESS;
    // Only SPI supported for now
    bus = RUUVI_DRIVER_BUS_SPI;
    handle = RUUVI_BOARD_SPI_SS_ACCELEROMETER_PIN;
    err_code |= ruuvi_interface_lis2dh12_init(&acceleration_sensor, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

    if(RUUVI_DRIVER_SUCCESS == err_code)
    {
      err_code |= task_acceleration_configure();

      float ths = APPLICATION_ACCELEROMETER_ACTIVITY_THRESHOLD;
      err_code |= ruuvi_interface_lis2dh12_activity_interrupt_use(true, &ths);

      // Let pins settle
      ruuvi_platform_delay_ms(10);
      // Setup FIFO and activity interrupts
      err_code |= ruuvi_platform_gpio_interrupt_enable(RUUVI_BOARD_INT_ACC1_PIN, RUUVI_INTERFACE_GPIO_SLOPE_LOTOHI, RUUVI_INTERFACE_GPIO_MODE_INPUT_NOPULL, on_fifo);
      err_code |= ruuvi_platform_gpio_interrupt_enable(RUUVI_BOARD_INT_ACC2_PIN, RUUVI_INTERFACE_GPIO_SLOPE_LOTOHI, RUUVI_INTERFACE_GPIO_MODE_INPUT_NOPULL, on_movement);
      char msg[APPLICATION_LOG_BUFFER_SIZE] = { 0 };
      snprintf(msg, sizeof(msg), "Configured interrupt threshold at %.3f mg\r\n", ths);
      ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, msg);

      return err_code;
    }
  #endif

  // Return error if usable acceleration sensor was not found.
  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_acceleration_data_log(const ruuvi_interface_log_severity_t level)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_acceleration_data_t data;
  if(NULL == acceleration_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  err_code |= acceleration_sensor.data_get(&data);
  char message[128] = {0};
  snprintf(message, sizeof(message), "Time: %lu\r\n", (uint32_t)(data.timestamp_ms&0xFFFFFFFF));
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "X: %.3f\r\n", data.x_g);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Y: %.3f\r\n" ,data.y_g);
  ruuvi_platform_log(level, message);
  snprintf(message, sizeof(message), "Z: %.3f\r\n", data.z_g);
  ruuvi_platform_log(level, message);
  return err_code;
}

ruuvi_driver_status_t task_acceleration_data_get(ruuvi_interface_acceleration_data_t* const data)
{
  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }
  if(NULL == acceleration_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }
  return acceleration_sensor.data_get(data);
}

ruuvi_driver_status_t task_acceleration_on_button(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= task_acceleration_data_log(RUUVI_INTERFACE_LOG_INFO);
  return err_code;
}

ruuvi_driver_status_t task_acceleration_movement_count_get(uint8_t * const count)
{
  *count = m_nbr_movements;
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_acceleration_fifo_use(const bool enable)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  if(true == enable)
  {
    err_code |= ruuvi_interface_lis2dh12_fifo_use(true);
    err_code |= ruuvi_interface_lis2dh12_fifo_interrupt_use(true);
  }
  if(false == enable)
  {
    err_code |= ruuvi_interface_lis2dh12_fifo_use(false);
    err_code |= ruuvi_interface_lis2dh12_fifo_interrupt_use(false);
  }
  return err_code;
}