#include "application_config.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_scheduler.h"
#include "task_scheduler.h"

ruuvi_driver_status_t task_scheduler_init(void)
{
  return ruuvi_interface_scheduler_init(APPLICATION_TASK_DATA_MAX_SIZE, APPLICATION_TASK_QUEUE_MAX_LENGTH);
}