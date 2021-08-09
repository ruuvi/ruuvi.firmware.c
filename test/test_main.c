#include "unity.h"

#include "app_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_enabled_modules.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_library.h"

#include "main.h"
#include "semver.h"

#include "mock_app_button.h"
#include "mock_app_comms.h"
#include "mock_app_heartbeat.h"
#include "mock_app_led.h"
#include "mock_app_log.h"
#include "mock_app_power.h"
#include "mock_app_sensor.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_task_flash.h"
#include "mock_ruuvi_interface_gpio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_power.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_timer.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_ruuvi_interface_watchdog.h"
#include "mock_ruuvi_task_gpio.h"

semver_t current = {};
semver_t compare = {};

void setUp (void)
{
    ri_log_init_IgnoreAndReturn (RD_SUCCESS);
    ri_log_Ignore();
    rd_error_check_Ignore();
}

void tearDown (void)
{
    // Free allocated memory when we're done
    semver_free (&current);
    semver_free (&compare);
}

void test_app_on_error_fatal (void)
{
    char file[] = "main.h";
    ri_power_reset_Expect();
    app_on_error (RD_ERROR_INVALID_STATE,
                  true,
                  file,
                  7);
}

void test_app_on_error_nonfatal (void)
{
    char file[] = "main.h";
    app_on_error (RD_ERROR_INVALID_STATE,
                  false,
                  file,
                  7);
}

void test_main_ok (void)
{
    // <setup>
    float motion_threshold = APP_MOTION_THRESHOLD;
    ri_watchdog_init_ExpectAndReturn (APP_WDT_INTERVAL_MS, &on_wdt, RD_SUCCESS);
    ri_yield_init_ExpectAndReturn (RD_SUCCESS);
    ri_timer_init_ExpectAndReturn (RD_SUCCESS);
    ri_scheduler_init_ExpectAndReturn (RD_SUCCESS);
    rt_gpio_init_ExpectAndReturn (RD_SUCCESS);
    ri_yield_low_power_enable_ExpectAndReturn (true, RD_SUCCESS);
    rt_flash_init_ExpectAndReturn (RD_SUCCESS);
    app_led_init_ExpectAndReturn (RD_SUCCESS);
    app_led_error_signal_Expect (true);
    app_button_init_ExpectAndReturn (RD_SUCCESS);
    app_dc_dc_init_ExpectAndReturn (RD_SUCCESS);
    app_sensor_init_ExpectAndReturn (RD_SUCCESS);
    app_log_init_ExpectAndReturn (RD_SUCCESS);
    app_sensor_acc_thr_set_ExpectWithArrayAndReturn (&motion_threshold, 1, RD_SUCCESS);
    app_comms_init_ExpectAndReturn (true, RD_SUCCESS);
    app_sensor_vdd_sample_ExpectAndReturn (RD_SUCCESS);
    app_heartbeat_init_ExpectAndReturn (RD_SUCCESS);
    app_heartbeat_start_ExpectAndReturn (RD_SUCCESS);
    app_led_error_signal_Expect (false);
    app_led_activity_signal_Expect (true);
    ri_delay_ms_ExpectAndReturn (APP_SELFTEST_OK_DELAY_MS, RD_SUCCESS);
    app_led_activity_signal_Expect (false);
    rd_error_cb_set_Expect (&app_on_error);
    // </setup>
    ri_scheduler_execute_ExpectAndReturn (RD_SUCCESS);
    ri_yield_ExpectAndReturn (RD_SUCCESS);
    app_main();
}

void test_main_error (void)
{
    // <setup>
    float motion_threshold = APP_MOTION_THRESHOLD;
    ri_watchdog_init_ExpectAndReturn (APP_WDT_INTERVAL_MS, &on_wdt, RD_SUCCESS);
    ri_yield_init_ExpectAndReturn (RD_SUCCESS);
    ri_timer_init_ExpectAndReturn (RD_SUCCESS);
    ri_scheduler_init_ExpectAndReturn (RD_SUCCESS);
    rt_gpio_init_ExpectAndReturn (RD_SUCCESS);
    ri_yield_low_power_enable_ExpectAndReturn (true, RD_SUCCESS);
    rt_flash_init_ExpectAndReturn (RD_SUCCESS);
    app_led_init_ExpectAndReturn (RD_SUCCESS);
    app_led_error_signal_Expect (true);
    app_button_init_ExpectAndReturn (RD_SUCCESS);
    app_dc_dc_init_ExpectAndReturn (RD_SUCCESS);
    app_sensor_init_ExpectAndReturn (RD_SUCCESS);
    app_log_init_ExpectAndReturn (RD_SUCCESS);
    app_sensor_acc_thr_set_ExpectWithArrayAndReturn (&motion_threshold, 1, RD_SUCCESS);
    app_comms_init_ExpectAndReturn (true, RD_SUCCESS);
    app_sensor_vdd_sample_ExpectAndReturn (RD_SUCCESS);
    app_heartbeat_init_ExpectAndReturn (RD_ERROR_INTERNAL);
    app_heartbeat_start_ExpectAndReturn (RD_ERROR_INVALID_STATE);
    rd_error_cb_set_Expect (&app_on_error);
    // </setup>
    ri_scheduler_execute_ExpectAndReturn (RD_SUCCESS);
    ri_yield_ExpectAndReturn (RD_SUCCESS);
    app_main();
}

void test_semver_boards (void)
{
    char operator[] = "^";
    semver_parse (RUUVI_BOARDS_SEMVER, &current);
    semver_parse (RUUVI_BOARDS_REQ, &compare);
    TEST_ASSERT (semver_satisfies (current, compare, operator));
}

void test_semver_drivers (void)
{
    char operator[] = "^";
    semver_parse (RUUVI_DRIVERS_SEMVER, &current);
    semver_parse (RUUVI_DRIVERS_REQ, &compare);
    TEST_ASSERT (semver_satisfies (current, compare, operator));
}

void test_semver_endpoints (void)
{
    char operator[] = "^";
    semver_parse (RUUVI_ENDPOINTS_SEMVER, &current);
    semver_parse (RUUVI_ENDPOINTS_REQ, &compare);
    TEST_ASSERT (semver_satisfies (current, compare, operator));
}


void test_semver_libraries (void)
{
    char operator[] = "^";
    semver_parse (RUUVI_LIBRARIES_SEMVER, &current);
    semver_parse (RUUVI_LIBRARIES_REQ, &compare);
    TEST_ASSERT (semver_satisfies (current, compare, operator));
}
