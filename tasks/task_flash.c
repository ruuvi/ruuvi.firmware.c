#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_flash.h"
#include "ruuvi_interface_log.h"
#include "task_flash.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

#define PAGE_ID 1
#define RECORD_ID 1

ruuvi_driver_status_t task_flash_init(void)
{
  return ruuvi_interface_flash_init();
}

ruuvi_driver_status_t task_flash_store(const uint32_t page_id, const uint32_t record_id, const char* const message, const size_t message_length)
{
  return ruuvi_interface_flash_record_set(page_id, record_id, message_length, message);
}

ruuvi_driver_status_t task_flash_load(const uint32_t page_id, const uint32_t record_id, char* const message, const size_t message_length)
{
  return ruuvi_interface_flash_record_get(page_id, record_id, message_length, message);
}

ruuvi_driver_status_t task_flash_demo()
{
  ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Loading boot count from flash\r\n");
  char msg[128] = {0};
  if(RUUVI_DRIVER_SUCCESS == ruuvi_interface_flash_record_get(PAGE_ID, RECORD_ID, sizeof(msg), msg))
  {
    snprintf(msg + 4, sizeof(msg) - 4, "Got it, there's been %d boots\r\n", msg[0]);
    ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, msg+4);
  }
  else 
  {
    ruuvi_platform_log(RUUVI_INTERFACE_LOG_INFO, "Boot count not found, maybe this is first boot?\r\n");  
  }
  msg[0]++;
  return ruuvi_interface_flash_record_set(PAGE_ID, RECORD_ID, sizeof(msg), msg);
}

