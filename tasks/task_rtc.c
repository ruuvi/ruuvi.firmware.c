#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_log.h"
#include "task_rtc.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>


ruuvi_driver_status_t task_rtc_init(void)
{
  ruuvi_driver_sensor_timestamp_function_set(ruuvi_platform_rtc_millis);
  return ruuvi_platform_rtc_init();
}
