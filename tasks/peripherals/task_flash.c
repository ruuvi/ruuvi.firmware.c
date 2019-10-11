#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_nrf5_sdk15_error.h"
#include "ruuvi_interface_flash.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "task_flash.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

#if APPLICATION_FLASH_ENABLED

ruuvi_driver_status_t task_flash_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_flash_init();
  // Error on flash? purge, reboot
  if(RUUVI_DRIVER_SUCCESS != err_code)
  {
    ruuvi_interface_flash_purge();
    // Log fatal error which triggers bootloader
    RUUVI_DRIVER_ERROR_CHECK(RUUVI_DRIVER_ERROR_FATAL, RUUVI_DRIVER_SUCCESS);
  }
  return err_code;
}

ruuvi_driver_status_t task_flash_store(const uint16_t file_id, const uint16_t record_id,
                                       const void* const message, const size_t message_length)
{
  ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
  status = ruuvi_interface_flash_record_set(file_id, file_id, message_length, message);
  if(RUUVI_DRIVER_ERROR_NO_MEM == status)
  {
    ruuvi_interface_flash_gc_run();
    while(ruuvi_interface_flash_is_busy())
    {
      ruuvi_interface_yield();
    }
    status = ruuvi_interface_flash_record_set(file_id, file_id, message_length, message);
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
ruuvi_driver_status_t task_flash_init(void)
{
  return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_flash_demo()
{
  return RUUVI_DRIVER_SUCCESS;
}
#endif
