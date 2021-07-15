#ifndef MAIN_H
#define MAIN_H

#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_log.h"
#include <stdlib.h>
#include <stdio.h>

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
#define RUUVI_BOARDS_REQ "3.7.1"
#define RUUVI_DRIVERS_REQ "3.9.0"
#define RUUVI_ENDPOINTS_REQ "3.0.0"
#define RUUVI_LIBRARIES_REQ "3.0.0"

#define APP_SELFTEST_OK_DELAY_MS (1000U) //!< time to show "ok" led.

#ifdef CEEDLING
void on_wdt (void);
void app_on_error (const rd_status_t error,
                   const bool fatal,
                   const char * file,
                   const int line);
void setup (void);
int app_main (void);
#define LOOP_FOREVER 0 //!< Ceedling run exits
#else
#define LOOP_FOREVER 1 //!< Normal run does not exit.
#endif

/*@}*/
#endif // MAIN_H
