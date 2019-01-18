#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_power.h"
#include "task_power.h"

ruuvi_driver_status_t task_power_dcdc_init(void)
{
  ruuvi_interface_power_regulators_t regulators = RUUVI_INTERFACE_POWER_REGULATORS_DISABLED;

  #ifdef RUUVI_BOARD_DCDC_INTERNAL_INSTALLED
    if(RUUVI_BOARD_DCDC_INTERNAL_INSTALLED) regulators |= RUUVI_INTERFACE_POWER_REGULATORS_DCDC_INTERNAL;
  #endif
  return ruuvi_interface_power_regulators_enable(regulators);
}