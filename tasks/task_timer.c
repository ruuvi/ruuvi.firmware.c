#include "ruuvi_driver_error.h"
#include "ruuvi_interface_timer.h"
#include "task_timer.h"

ruuvi_driver_status_t task_timer_init(void)
{
  return ruuvi_platform_timers_init();
}