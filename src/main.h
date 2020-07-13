#ifndef MAIN_H
#define MAIN_H

#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_log.h"
#include <stdlib.h>

/**
 * @addtogroup main
 */
/*@{*/
/**
 * @file main.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-12-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * This file exists only to provide Ceedling with handles to functions to test.
 *
 */

// Submodule requirements
#define RUUVI_BOARDS_REQ "0.7.0"
#define RUUVI_DRIVERS_REQ "0.2.5"
#define RUUVI_ENDPOINTS_REQ "0.2.0"
#define RUUVI_LIBRARIES_REQ "0.2.0"

#ifdef CEEDLING
void on_wdt (void);
void setup (void);
int app_main (void);
#define LOOP_FOREVER 0 //!< Ceedling run exits
#else
#define LOOP_FOREVER 1 //!< Normal run does not exit.
#endif

/*@}*/
#endif // MAIN_H
