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
#include "app_led.h"
#include "app_melody.h"
#include "app_power.h"
#include "main.h"
#include "run_integration_tests.h"
#include "ruuvi_interface_power.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_advertisement.h"
#include "ruuvi_task_button.h"
#include "ruuvi_task_gpio.h"
#include "ruuvi_task_led.h"
#include "ruuvi_task_nfc.h"
#include <string.h>
#include <stdio.h>

#ifndef CEEDLING
static
#endif
void on_wdt (void)
{
    // Store cause of reset to flash
}

static rd_status_t setup_nfc(void)
{
    rd_status_t err_code = RD_SUCCESS;
    uint64_t id = 0;
    uint64_t mac = 0;
    size_t written = 0;
    // dis is copied to users, no need to retain it after setup.
    ri_communication_dis_init_t dis_data =
    {
        .fw_version = APP_FW_NAME,
        .model = RB_MODEL_STRING,
        .hw_version = 0,
        .manufacturer = RB_MANUFACTURER_STRING
    };
    err_code |=  ri_radio_address_get(&mac);
    uint8_t mac_buffer[6] = {0};
    mac_buffer[0] = (mac >> 40) & 0xFF;
    mac_buffer[1] = (mac >> 32) & 0xFF;
    mac_buffer[2] = (mac >> 24) & 0xFF;
    mac_buffer[3] = (mac >> 16) & 0xFF;
    mac_buffer[4] = (mac >> 8) & 0xFF;
    mac_buffer[5] = (mac >> 0) & 0xFF;
    written = snprintf ( dis_data.deviceaddr,
                         RI_COMMUNICATION_DIS_STRLEN,
                         "%02X:%02X:%02X:%02X:%02X:%02X",
                         mac_buffer[0], mac_buffer[1], mac_buffer[2], mac_buffer[3], mac_buffer[4], mac_buffer[5]);

    if (! (written > 0 && RI_COMMUNICATION_DIS_STRLEN > written))
    {
        err_code |= RD_ERROR_DATA_SIZE;
    }
    err_code |= ri_communication_id_get(&id);
    uint32_t id0 = (id >> 32) & 0xFFFFFFFF;
    uint32_t id1 = id & 0xFFFFFFFF;
    written = snprintf ( dis_data.deviceid,
                         RI_COMMUNICATION_DIS_STRLEN,
                         "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                         (unsigned int) (id0 >> 24) & 0xFF, (unsigned int) (id0 >> 16) & 0xFF,
                         (unsigned int) (id0 >> 8) & 0xFF, (unsigned int) id0 & 0xFF,
                         (unsigned int) (id1 >> 24) & 0xFF, (unsigned int) (id1 >> 16) & 0xFF,
                         (unsigned int) (id1 >> 8) & 0xFF, (unsigned int) id1 & 0xFF);

    if (! (written > 0 && RI_COMMUNICATION_DIS_STRLEN > written))
    {
        err_code |= RD_ERROR_DATA_SIZE;
    }
    err_code |= rt_nfc_init(&dis_data);
    return  err_code;
}

static rd_status_t setup_adv(void)
{
    rd_status_t err_code = RD_SUCCESS;
    rt_adv_init_t init;
    init.adv_interval_ms = 100;
    init.adv_pwr_dbm = 4;
    init.manufacturer_id = 0x0499;
    err_code |= rt_adv_init(&init);
    err_code |= rt_adv_start();
    ri_communication_message_t msg;
    snprintf(msg.data, sizeof(msg.data), "Ave mundi");
    msg.data_length = sizeof("Ave mundi");
    err_code |= rt_adv_send_data(&msg);
    return  err_code;
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
    err_code |= app_dc_dc_init();
    err_code |= app_led_init();
    err_code |= app_melody_init();
    app_melody_play();
    err_code |= ri_timer_init();
    err_code |= ri_scheduler_init(RI_SCHEDULER_SIZE, RI_SCHEDULER_LENGTH);
    // Low-power yield requires timer. After this call ms-delays use timer + sleep.
    err_code |= ri_yield_low_power_enable(true);
    setup_nfc();
    setup_adv();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/**
 * @brief Actual main, redirected for Ceedling
 */
int app_main (void)
{
    do
    {
        // Execute scheduled tasks.
        ri_scheduler_execute();
        ri_watchdog_feed();
        // Sleep - woken up on event.
        ri_yield();
        
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
