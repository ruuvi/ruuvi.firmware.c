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
#include "app_comms.h"
#include "app_heartbeat.h"
#include "app_led.h"
#include "app_power.h"
#include "app_sensor.h"
#include "main.h"
#include "run_integration_tests.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_power.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_button.h"
#include "ruuvi_task_gpio.h"
#include "ruuvi_task_led.h"

#if (!RUUVI_RUN_TESTS)
#ifndef CEEDLING
static
#endif
void on_wdt (void)
{
    // Store cause of reset to flash - TODO
}
#endif

/**
 * @brief setup MCU peripherals and board peripherals.
 *
 */
void setup (void)
{
    rd_status_t err_code = RD_SUCCESS;
#   if (!RUUVI_RUN_TESTS)
    err_code |= ri_watchdog_init (APP_WDT_INTERVAL_MS, &on_wdt);
    err_code |= ri_log_init (APP_LOG_LEVEL); // Logging to terminal.
    err_code |= ri_yield_init();
#   endif
    err_code |= ri_timer_init();
    err_code |= ri_scheduler_init();
    err_code |= rt_gpio_init();
    err_code |= app_button_init();
    err_code |= app_dc_dc_init();
    err_code |= app_led_init();
    err_code |= app_sensor_init();
    err_code |= app_comms_init();
    err_code |= app_heartbeat_init(); // Broadcast data.
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/**
 * @brief Actual main, redirected for Ceedling
 */
#ifdef  CEEDLING
int app_main (void)
#else
int main (void)
#endif
{
#   if RUUVI_RUN_TESTS
    integration_tests_run();
#   endif
    setup();

    do
    {
        // Run scheduled tasks
        ri_scheduler_execute();
        // Sleep - woken up on event
        ri_yield();
        // Prevent loop being optimized away
        __asm__ ("");
    } while (LOOP_FOREVER);

    // Intentionally non-reachable code unless unit testing.
    return -1;
}

/** @} */
