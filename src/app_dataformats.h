#ifndef APP_DATAFORMATS_H
#define APP_DATAFORMATS_H

/**
 * @file app_dataformats.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2021-08-19
 * @copyright Ruuvi Innovations Ltd, License BSD-3-Clause.
 *
 * Helper to encode data into various formats
 *
 */

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_3.h"
#include "ruuvi_endpoint_5.h"
//#include "ruuvi_endpoint_8.h"
//#include "ruuvi_endpoint_fa.h"

typedef struct {
  unsigned int df_3  : 1; //!< Dataformat 3 enabled
  unsigned int df_5  : 1; //!< Dataformat 5 enabled
  unsigned int df_8  : 1; //!< Dataformat 8 enabled
  unsigned int df_fa : 1; //!< Dataformat fa enabled
}app_dataformats_t;

#endif // APP_DATAFORMATS_H
