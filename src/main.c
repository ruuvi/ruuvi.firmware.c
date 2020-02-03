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
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"

#ifndef CEEDLING
static
#endif
void on_wdt(void)
{
  // Store cause of reset to flash
}

/** @brief setup MCU peripherals and board peripherals. 
 *
 * Runs integration tests on test builds. 
 *
 */
void setup(void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_watchdog_init(APP_WDT_INTERVAL_MS, &on_wdt);
    err_code |= ri_yield_init();
    RD_ERROR_CHECK(err_code, RD_SUCCESS);
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
        // Prevent loop being optimized away
        __asm__("");
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
