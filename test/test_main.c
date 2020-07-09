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
#include "mock_app_power.h"
#include "mock_app_sensor.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_interface_gpio.h"
#include "mock_ruuvi_interface_log.h"
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

void test_main (void)
{
    // <setup>
    ri_watchdog_init_ExpectAndReturn (APP_WDT_INTERVAL_MS, &on_wdt, RD_SUCCESS);
    ri_yield_init_ExpectAndReturn (RD_SUCCESS);
    ri_timer_init_ExpectAndReturn (RD_SUCCESS);
    ri_scheduler_init_ExpectAndReturn (RD_SUCCESS);
    rt_gpio_init_ExpectAndReturn (RD_SUCCESS);
    app_button_init_ExpectAndReturn (RD_SUCCESS);
    app_dc_dc_init_ExpectAndReturn (RD_SUCCESS);
    app_led_init_ExpectAndReturn (RD_SUCCESS);
    app_sensor_init_ExpectAndReturn (RD_SUCCESS);
    app_comms_init_ExpectAndReturn (RD_SUCCESS);
    app_heartbeat_init_ExpectAndReturn (RD_SUCCESS);
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
