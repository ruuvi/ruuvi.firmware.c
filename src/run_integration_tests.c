#include "app_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_enabled_modules.h"
#include "run_integration_tests.h"
#include "ruuvi_interface_gpio_test.h"
#include "ruuvi_interface_gpio_interrupt_test.h"
#include "ruuvi_interface_flash_test.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_power_test.h"
#include "ruuvi_interface_scheduler_test.h"
#include "ruuvi_interface_watchdog.h"
/**
 * @addtogroup integration_test
 */
/*@{*/
/**
 * @file run_integration_test.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-02-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

void on_integration_test_wdt (void)
{
    // Store cause of reset to flash
}

static inline void LOG (const char * const msg)
{
    ri_log (RI_LOG_LEVEL_INFO, msg);
}

/** @brief Print test open JSON to console */
void integration_test_start (void)
{
    ri_watchdog_init (APP_WDT_INTERVAL_MS, &on_integration_test_wdt);
    ri_log_init (APP_LOG_LEVEL);
    LOG ("{\r\n\"firmware\":\"" APP_FW_NAME "\",\r\n");
    LOG ("\"board\":\"" RB_MODEL_STRING "\",\r\n");
    LOG ("\"ruuvi.boards.c\":\"" RUUVI_BOARDS_SEMVER "\",\r\n");
    LOG ("\"ruuvi.drivers.c\":\"" RUUVI_DRIVERS_SEMVER "\",\r\n");
}

/** @brief Print test close JSON to console */
void integration_test_stop (void)
{
    LOG ("}");
}

static void integration_test_power (void)
{
    ri_power_regulators_t regs = {0};
#   if (RB_DCDC_INTERNAL_INSTALLED)
    regs.DCDC_INTERNAL = 1;
#endif
#   if (RB_DCDC_HV_INSTALLED)
    regs.DCDC_HV = 1;
#endif
    ri_power_run_integration_test (&LOG, regs);
}

/** @brief Run integration tests*/
void integration_tests_run (void)
{
    integration_test_start();
    ri_flash_run_integration_test (&LOG);
#if defined(RB_GPIO_TEST_INPUT) && defined(RB_GPIO_TEST_OUTPUT)
    ri_gpio_run_integration_test (&LOG, RB_GPIO_TEST_INPUT, RB_GPIO_TEST_OUTPUT);
    ri_gpio_interrupt_run_integration_test (&LOG, RB_GPIO_TEST_INPUT, RB_GPIO_TEST_OUTPUT);
#endif
    integration_test_power();
    ri_scheduler_run_integration_test(&LOG);
    integration_test_stop();
}

/*@}*/