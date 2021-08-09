#ifndef APP_POWER_H
#define APP_POWER_H
#include "ruuvi_driver_error.h"

/**
 * @addtogroup app
 */
/** @{ */
/**
 * @defgroup app_power Power configuration
 * @brief Enable regulators, reset and enter bootloader functions.
 *
 */
/*@}*/
/**
 * @addtogroup app_power
 */
/*@{*/
/**
 * @file app_power.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-02-13
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

/**
 * @brief Initialize regulatore.
 *
 * After initialization the DC/DC regulators on boards are automatically used when
 * needed.
 *
 * @retval RD_SUCCESS if regulators were initialized.
 */
rd_status_t app_dc_dc_init (void);

/** @} */

#endif // APP_POWER_H
