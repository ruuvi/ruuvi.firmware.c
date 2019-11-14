#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_nrf5_sdk15_error.h"
#include "ruuvi_interface_flash.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_power.h"
#include "ruuvi_interface_yield.h"
#include "task_flash.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifndef TASK_FLASH_LOG_LEVEL
  #define TASK_FLASH_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

#define LOG(msg) ruuvi_interface_log(TASK_FLASH_LOG_LEVEL, msg)
#define LOGD(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_DEBUG, msg)
#define LOGW(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_WARNING, msg)
#define LOGHEX(msg, len) ruuvi_interface_log_hex(TASK_FLASH_LOG_LEVEL, msg, len)

#if APPLICATION_FLASH_ENABLED
typedef struct
{
  ruuvi_driver_status_t error;
  char filename[32];
  int line;
} error_cause_t;

static void on_error(const ruuvi_driver_status_t err,
                     const bool fatal,
                     const char* file,
                     const int line)
{
  if(!fatal) { return; }

  error_cause_t error = {.error = err, .line = line };
  ruuvi_driver_status_t err_code;
  uint32_t timeout = 0;
  strncpy(error.filename, file, sizeof(error.filename));
  // Store reason of fatal error
  err_code = task_flash_store(APPLICATION_FLASH_ERROR_FILE,
                              APPLICATION_FLASH_ERROR_RECORD,
                              &error, sizeof(error));

  // Wait for flash store op to complete
  while(RUUVI_DRIVER_SUCCESS == err_code &&
        timeout < 1000 &&
        ruuvi_interface_flash_is_busy())
  {
    timeout++;
    // Use microsecond wait to busyloop instead of millisecond wait to low-power sleep
    // as low-power sleep may hang on interrupt context.
    ruuvi_interface_delay_us(1000);
  }

  // Try to enter bootloader, if that fails reset.
  ruuvi_interface_power_enter_bootloader();
  ruuvi_interface_power_reset();
}

static void print_error_cause(void)
{
  error_cause_t error;
  uint32_t timeout = 0;
  ruuvi_driver_status_t err_code;
  err_code = task_flash_load(APPLICATION_FLASH_ERROR_FILE,
                             APPLICATION_FLASH_ERROR_RECORD,
                             &error, sizeof(error));

  if(RUUVI_DRIVER_SUCCESS != err_code) { return; }

  // Wait for flash store op to complete
  while(timeout < 1000 &&
        ruuvi_interface_flash_is_busy())
  {
    timeout++;
    ruuvi_interface_delay_ms(10);
  }

  char error_str[128];
  size_t index = 0;
  index += snprintf(error_str, sizeof(error_str), "Previous fatal error: %s:%d: ",
                    error.filename, error.line);
  index += ruuvi_interface_error_to_string(error.error, error_str + index,
           sizeof(error_str) - index);
  snprintf(error_str + index,  sizeof(error_str) - index, "\r\n");
  LOG(error_str);
}

ruuvi_driver_status_t task_flash_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_flash_init();

  // Error on flash? purge, reboot
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    ruuvi_interface_flash_purge();
    ruuvi_interface_power_reset();
  }

  // Print previous fatal error
  print_error_cause();
  // Setup error logger
  ruuvi_driver_error_cb_set(on_error);
  return err_code;
}

ruuvi_driver_status_t task_flash_store(const uint16_t page_id, const uint16_t record_id,
                                       const void* const message, const size_t message_length)
{
  ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
  status = ruuvi_interface_flash_record_set(page_id, record_id, message_length, message);

  if(RUUVI_DRIVER_ERROR_NO_MEM == status)
  {
    ruuvi_interface_flash_gc_run();

    while(ruuvi_interface_flash_is_busy())
    {
      ruuvi_interface_yield();
    }

    status = ruuvi_interface_flash_record_set(page_id, record_id, message_length, message);
  }

  return status;
}

ruuvi_driver_status_t task_flash_load(const uint16_t page_id, const uint16_t record_id,
                                      void* const message, const size_t message_length)
{
  return ruuvi_interface_flash_record_get(page_id, record_id, message_length, message);
}

ruuvi_driver_status_t task_flash_free(const uint16_t file_id, const uint16_t record_id)
{
  return ruuvi_interface_flash_record_delete(file_id, record_id);
}

ruuvi_driver_status_t task_flash_gc_run(void)
{
  while(ruuvi_interface_flash_is_busy())
  {
    ruuvi_interface_yield();
  }

  return ruuvi_interface_flash_gc_run();
}

bool task_flash_busy(void)
{
  return ruuvi_interface_flash_is_busy();
}



#else
static void on_error(const ruuvi_driver_status_t err,
                     const bool fatal,
                     const char* file,
                     const int line)
{
  // Try to enter bootloader, if that fails reset.
  ruuvi_interface_power_enter_bootloader();
  ruuvi_interface_power_reset();
}

ruuvi_driver_status_t task_flash_init(void)
{
  // Setup error reset
  ruuvi_driver_error_cb_set(on_error);
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_flash_store(const uint16_t file_id, const uint16_t record_id,
                                       const void* const message, const size_t message_length)
{
  return RUUVI_DRIVER_ERROR_NOT_SUPPORTED;
}

ruuvi_driver_status_t task_flash_load(const uint16_t page_id, const uint16_t record_id,
                                      void* const message, const size_t message_length)
{
  return RUUVI_DRIVER_ERROR_NOT_SUPPORTED;
}

bool task_flash_busy(void)
{
  return false;
}
#endif
