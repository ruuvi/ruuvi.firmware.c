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

#include "app_button.h"
#include "main.h"
#include "run_integration_tests.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_button.h"
#include "ruuvi_task_gpio.h"

#ifndef CEEDLING
static
#endif
void on_wdt (void)
{
    // Store cause of reset to flash
}

/**
 * @brief setup MCU peripherals and board peripherals.
 *
 */
void setup (void)
{
    rd_status_t err_code = RD_SUCCESS;
#   if (!RUUVI_RUN_TESTS)
    err_code |= ri_watchdog_init (APP_WDT_INTERVAL_MS, &on_wdt);
    err_code |= ri_log_init (APP_LOG_LEVEL);
    err_code |= ri_yield_init();
#   endif
    err_code |= rt_gpio_init();
    err_code |= app_button_init();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/**
 * @brief Actual main, redirected for Ceedling
 */
int app_main (void)
{
    do
    {
        // Sleep - woken up on event
        ri_yield();
        // Prevent loop being optimized away
        __asm__ ("");
    } while (LOOP_FOREVER);

    // Intentionally non-reachable code.
    return -1;
}

#ifndef CEEDLING
int main (void)
{
#   if RUUVI_RUN_TESTS
    integration_tests_run();
#   endif
    setup();
    // Will never return.
    return app_main();
}
#endif

/*@}*/
