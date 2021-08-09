/**
 * @defgroup main Program main
 *
 */
/** @{ */
/**
 * @file main.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-07-13
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */
#include "app_config.h"
#include "app_button.h"
#include "app_comms.h"
#include "app_heartbeat.h"
#include "app_led.h"
#include "app_log.h"
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
#include "ruuvi_task_flash.h"
#include "ruuvi_task_gpio.h"
#include "ruuvi_task_led.h"
#include "ruuvi_task_adc.h"

#if (!RUUVI_RUN_TESTS)
#ifndef CEEDLING
static
#endif
void on_wdt (void)
{
    // Store cause of reset to flash - TODO
}
#endif

#ifndef CEEDLING
static
#endif
void app_on_error (const rd_status_t error,
                   const bool fatal,
                   const char * file,
                   const int line)
{
    // TODO: store error source to flash.
    if (fatal)
    {
        ri_power_reset();
    }
}

/**
 * @brief setup MCU peripherals and board peripherals.
 *
 */
void setup (void)
{
    rd_status_t err_code = RD_SUCCESS;
    float motion_threshold = APP_MOTION_THRESHOLD;
#   if (!RUUVI_RUN_TESTS)
    err_code |= ri_watchdog_init (APP_WDT_INTERVAL_MS, &on_wdt);
    err_code |= ri_log_init (APP_LOG_LEVEL); // Logging to terminal.
#   endif
    err_code |= ri_yield_init();
    err_code |= ri_timer_init();
    err_code |= ri_scheduler_init();
    err_code |= rt_gpio_init();
    err_code |= ri_yield_low_power_enable (true);
    err_code |= rt_flash_init();
    err_code |= app_led_init();
    app_led_error_signal (true);
    err_code |= app_button_init();
    err_code |= app_dc_dc_init();
    err_code |= app_sensor_init();
    err_code |= app_log_init();
    // Allow fail on boards which do not have accelerometer.
    (void) app_sensor_acc_thr_set (&motion_threshold);
    err_code |= app_comms_init (APP_LOCKED_AT_BOOT);
    err_code |= app_sensor_vdd_sample();
    err_code |= app_heartbeat_init();
    err_code |= app_heartbeat_start();

    if (RD_SUCCESS == err_code)
    {
        app_led_error_signal (false);
        app_led_activity_signal (true);
        err_code |= ri_delay_ms (APP_SELFTEST_OK_DELAY_MS);
        app_led_activity_signal (false);
    }

    rd_error_cb_set (&app_on_error);
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

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
        ri_scheduler_execute();
        // Sleep - woken up on event
        // Do not indicate activity here to conserve power.
        // Sensor reads take ~20ms, having led on for that time is expensive.
        ri_yield();
        // Prevent loop being optimized away
        __asm__ ("");
    } while (LOOP_FOREVER);

    // Intentionally non-reachable code unless unit testing.
    return -1;
}

/** @} */
