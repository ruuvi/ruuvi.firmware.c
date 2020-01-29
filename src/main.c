/**
 * @defgroup main Program main
 *
 */
/*@}*/
/**
 * @addtogroup main
 */
/*@{*/
/**
 * @file main.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-12-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "app_config.h"
#include "main.h"
#include "ruuvi_interface_yield.h"

/** @brief setup MCU peripherals and board peripherals. 
 *
 * Runs integration tests on test builds. 
 *
 */
void setup(void)
{
}

/** @brief Actual main, redirected for Ceedling
 *
 * Initializes logging, MCU peripherals, sensors and communication.
 * If all steps are complete without warnings, activity led is set to status_ok, else
 * activity led is set to status_error.
 *
 */
int app_main (void)
{

    do
    {
        // Sleep - woken up on event
        ri_yield();
    } while (LOOP_FOREVER);

    // Intentionally non-reachable code.
    return -1;
}

#ifndef CEEDLING
int main (void)
{
    setup();
    // Will never return.
    return app_main();
}
#endif

/*@}*/
