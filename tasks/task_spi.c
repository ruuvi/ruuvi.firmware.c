#include "task_spi.h"
#include "ruuvi_error.h"
#include "spi.h"


ruuvi_status_t task_spi_init(void)
{
  return spi_init();
}
