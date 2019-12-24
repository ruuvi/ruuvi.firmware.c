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
 * @date 2019-12-17
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "application_config.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_boards.h"
#include "task_acceleration.h"
#include "task_adc.h"
#include "task_advertisement.h"
#include "task_button.h"
#include "task_environmental.h"
#include "task_flash.h"
#include "task_gatt.h"
#include "task_gpio.h"
#include "task_led.h"
#include "task_i2c.h"
#include "task_nfc.h"
#include "task_power.h"
#include "task_rtc.h"
#include "task_scheduler.h"
#include "task_sensor.h"
#include "task_spi.h"
#include "task_timer.h"
#include "test_sensor.h"
#include "test_acceleration.h"
#include "test_adc.h"
#include "test_environmental.h"
#include "test_library.h"

#include <stdio.h>
#include <string.h>

/**
 * Delay to let user see actions on board, such as led sequence.
 * Must be at least 1000 ms to avoid RTC hangup in test, shorter is generally better.
 */
#define BOOT_DELAY_MS (1000U)
#define LOG_BUF_SIZE  (128U)
#define GATT_HEARTBEAT_SIZE (18U) //!< 20 would be max, 18 cuts the data to while fields.

#ifndef MAIN_LOG_LEVEL
#define MAIN_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

static inline void LOG (const char * const msg)
{
    ruuvi_interface_log (MAIN_LOG_LEVEL, msg);
}

#if 0
static inline void LOGD (const char * const msg)
{
    ruuvi_interface_log (RUUVI_INTERFACE_LOG_DEBUG, msg);
}

static inline void LOGHEX (const uint8_t * const msg, const size_t len)
{
    ruuvi_interface_log_hex (MAIN_LOG_LEVEL, msg, len);
}
#endif

/** Run tests which rely only on MCU.
 *  These tests require relevant peripherals being uninitialized
 *  before tests and leave the peripherals uninitialized.
 *  Production firmware should not run these tests.
 *
 *  @Note These are integration tests run on actual hardware, Ceedling
 *  should not run this function.
 */
static void run_mcu_tests (void)
{
#if RUUVI_RUN_TESTS
    LOG ("'mcu_tests':{\r\n");
    // This is good only for internal MCU tests, it does not apply
    // offset configured by user in application to timestamps.
    // Use task_rtc function to apply offset configured by user to sensor values.
    ruuvi_driver_sensor_timestamp_function_set (ruuvi_interface_rtc_millis);
    ruuvi_interface_rtc_init();
    test_adc_run();
    test_library_run();
    // Delay to avoid locking RTC
    ruuvi_interface_delay_ms (BOOT_DELAY_MS);
    ruuvi_interface_rtc_uninit();
    LOG ("}\r\n");
#endif
}

/*
 * @brief Initialize MCU peripherals.
 */
#ifndef CEEDLING
static
#endif
void init_mcu (void)
{
    // Init watchdog
    ruuvi_interface_watchdog_init (APPLICATION_WATCHDOG_INTERVAL_MS);
    // Init yield
    ruuvi_driver_status_t status = ruuvi_interface_yield_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
    // Init GPIO
    status = task_gpio_init();
    // Initialize LED gpio pins
    status |= task_led_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
    // Initialize SPI, I2C
    status = task_spi_init();
    status |= task_i2c_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
    // Initialize RTC, timer and scheduler. Enable low-power sleep.
    // Dummy implementation of RTC requires timer.
    status = task_timer_init();
    status |= task_rtc_init();
    status |= task_scheduler_init();
    status |= ruuvi_interface_yield_low_power_enable (true);
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
    // Initialize power
    status |= task_power_dcdc_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
    // Initialize ADC
    status |= task_adc_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
    // Initialize flash
    status = task_flash_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
}

/*
 * @brief Run series of selftests which verify the underlying drivers, libraries etc.
 *
 * @note  These are integration tests, Ceedling should not run these.
 */
static void run_sensor_tests (void)
{
#if RUUVI_RUN_TESTS
    // Tests will initialize and uninitialize the sensors,
    // run this before using them in application
    ruuvi_interface_log (RUUVI_INTERFACE_LOG_INFO,
                         "Running extended self-tests, this might take a while\r\n");
    ruuvi_interface_watchdog_feed();
    test_acceleration_run();
    ruuvi_interface_watchdog_feed();
    test_environmental_run();
    ruuvi_interface_watchdog_feed();
    // Print unit test status, activate tests by building in DEBUG configuration under SES
    size_t tests_run, tests_passed;
    test_sensor_status (&tests_run, &tests_passed);
    char message[LOG_BUF_SIZE] = {0};
    snprintf (message, sizeof (message), "Tests ran: %u, passed: %u\r\n", tests_run,
              tests_passed);
    ruuvi_interface_log (RUUVI_INTERFACE_LOG_INFO, message);
    // Init watchdog after tests. Normally init at the start of the program
#endif
}

static void init_sensors (void)
{
    ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
    // Initialize environmental- nRF52 will return ERROR NOT SUPPORTED on RuuviTag basic
    // if DSP was configured, log warning
    status = task_environmental_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);
    // Allow NOT FOUND in case we're running on basic model
    status = task_acceleration_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_ERROR_NOT_FOUND);
}

#if APPLICATION_COMMUNICATION_GATT_ENABLED
static void on_gatt_connected_isr (void * data, size_t data_len)
{
    LOG ("GATT Connected ISR\r\n");
    task_communication_heartbeat_configure (APPLICATION_GATT_HEARTBEAT_INTERVAL_MS,
                                            GATT_HEARTBEAT_SIZE,
                                            task_sensor_encode_to_5, task_gatt_send_asynchronous);
    task_advertisement_start();
}

static void on_gatt_disconnected_isr (void * data, size_t data_len)
{
    LOG ("GATT Disconnected ISR\r\n");
    task_communication_heartbeat_configure (APPLICATION_ADVERTISEMENT_UPDATE_INTERVAL_MS,
                                            RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH,
                                            task_sensor_encode_to_5, task_advertisement_send_data);
}

static void process_gatt_command (void * p_event_data,
                                  uint16_t event_size)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if ( (NULL != p_event_data)
            && (RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH >= event_size))
    {
        // Pause heartbeats for processing
        task_communication_heartbeat_configure (0,
                                                GATT_HEARTBEAT_SIZE,
                                                task_sensor_encode_to_5, task_gatt_send_asynchronous);
        ruuvi_interface_communication_message_t msg = {0};
        memcpy (msg.data, p_event_data, event_size);
        msg.data_length = event_size;
        err_code |= task_communication_on_data (&msg,
                                                task_gatt_send_asynchronous);
        // Restore heartbeats
        task_communication_heartbeat_configure (APPLICATION_GATT_HEARTBEAT_INTERVAL_MS,
                                                GATT_HEARTBEAT_SIZE,
                                                task_sensor_encode_to_5, task_gatt_send_asynchronous);
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_DATA_SIZE;
    }

    RUUVI_DRIVER_ERROR_CHECK (err_code, ~RUUVI_DRIVER_ERROR_FATAL);
}

static void on_gatt_received_isr (void * data, size_t data_len)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    LOG ("GATT RX ISR\r\n");
    err_code |= ruuvi_interface_scheduler_event_put (data, data_len,
                &process_gatt_command);
    RUUVI_DRIVER_ERROR_CHECK (err_code, ~RUUVI_DRIVER_ERROR_FATAL);
}

static void on_gatt_sent_isr (void * data, size_t data_len)
{
    LOG ("GATT TX ISR\r\n");
}

static ruuvi_driver_status_t get_mac (uint8_t * const mac_buffer)
{
    ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;

    if (NULL != mac_buffer)
    {
        uint64_t mac;
        status |= ruuvi_interface_communication_radio_address_get (&mac);
        mac_buffer[0U] = (mac >> 40U) & 0xFFU;
        mac_buffer[1U] = (mac >> 32U) & 0xFFU;
        mac_buffer[2U] = (mac >> 24U) & 0xFFU;
        mac_buffer[3U] = (mac >> 16U) & 0xFFU;
        mac_buffer[4U] = (mac >> 8U) & 0xFFU;
        mac_buffer[5U] = (mac >> 0U) & 0xFFU;
    }
    else
    {
        status = RUUVI_DRIVER_ERROR_NULL;
    }

    return status;
}

static void init_dfu (void)
{
    ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
    status = task_gatt_dfu_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
}

/**
 * @brief Initialize Device Information Service
 *
 * DIS lets user read basic device information over BLE in a standard format.
 */
static void init_dis (void)
{
    ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_gatt_dis_init_t dis;
    memset (&dis, 0, sizeof (dis));
    uint8_t mac_buffer[6] = {0};
    get_mac (mac_buffer);
    size_t index = 0U;

    for (size_t ii = 0U; ii < 6U; ii ++)
    {
        index += snprintf (dis.deviceid + index, sizeof (dis.deviceid) - index, "%02X",
                           mac_buffer[ii]);

        if (ii < 5U)
        {
            index += snprintf (dis.deviceid + index, sizeof (dis.deviceid) - index, ":");
        }
    }

    memcpy (dis.fw_version, APPLICATION_FW_VERSION, sizeof (APPLICATION_FW_VERSION));
    memcpy (dis.model, RUUVI_BOARD_MODEL_STRING, sizeof (RUUVI_BOARD_MODEL_STRING));
    memcpy (dis.manufacturer, RUUVI_BOARD_MANUFACTURER_STRING,
            sizeof (RUUVI_BOARD_MANUFACTURER_STRING));
    status |= task_gatt_dis_init (&dis);
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
}

static void init_nus (void)
{
    ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
    status = task_gatt_nus_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
}
#endif

static void init_comms (void)
{
    ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
#if APPLICATION_BUTTON_ENABLED
    // Initialize button with on_button task
    status = task_button_init (RUUVI_INTERFACE_GPIO_SLOPE_HITOLO, task_button_on_press);
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
#endif
#if APPLICATION_COMMUNICATION_ADVERTISING_ENABLED
    // Initialize BLE - and start advertising.
    status = task_advertisement_init();
    status |= task_advertisement_start();
    status |= task_communication_heartbeat_configure (
                  APPLICATION_ADVERTISEMENT_UPDATE_INTERVAL_MS,
                  RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH,
                  task_sensor_encode_to_5, task_advertisement_send_data);
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
#endif
#if APPLICATION_COMMUNICATION_GATT_ENABLED
    status = task_gatt_init (RUUVI_BOARD_BLE_NAME_STRING);
    init_dis();
    init_nus();
    init_dfu();
    task_gatt_set_on_connected_isr (&on_gatt_connected_isr);
    task_gatt_set_on_disconn_isr (&on_gatt_disconnected_isr);
    task_gatt_set_on_received_isr (&on_gatt_received_isr);
    task_gatt_set_on_sent_isr (&on_gatt_sent_isr);
    status |= task_gatt_enable();
    status |= task_advertisement_stop();  // Reinitialize with scan response
    status |= task_advertisement_start();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
#endif
#if APPLICATION_COMMUNICATION_NFC_ENABLED
    // Initialize nfc. Note that NFC requires initialized radio to get
    // radio address.
    status |= task_nfc_init();
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
#endif
}

// Init logging
static void init_logging (void)
{
    ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
    status |= ruuvi_interface_log_init (APPLICATION_LOG_LEVEL);
    RUUVI_DRIVER_ERROR_CHECK (status, RUUVI_DRIVER_SUCCESS);
    char version[LOG_BUF_SIZE];
    ruuvi_interface_log (RUUVI_INTERFACE_LOG_INFO, "Program start\r\n");
    snprintf (version, sizeof (version), "Version: %s\r\n", APPLICATION_FW_VERSION);
    ruuvi_interface_log (RUUVI_INTERFACE_LOG_INFO, version);
}

/** @brief actual main, redirected fot Ceedling */
int app_main (void)
{
    init_logging();   // Initializes logging to user console
    run_mcu_tests();  // Runs tests which do not rely on MCU peripherals being initialized
    init_mcu();       // Initialize MCU peripherals, except for communication with users.
    // Delay one second to make sure timestamps are > 1 s after initialization
    ruuvi_interface_delay_ms (BOOT_DELAY_MS);
    task_led_write (RUUVI_BOARD_LED_ACTIVITY,
                    true); // Turn activity led on
    run_sensor_tests(); // Run tests which rely on MCU peripherals, e.g. sensor drivers
    init_sensors();     // Initializes sensors with application-defined default mode.
    // Initialize communication with outside world - BLE, NFC, Buttons
    /* IMPORTANT! After this point pausing the program flow asserts,
     * since softdevice asserts on missed timer. This includes debugger.
     */
    // run comms tests - TODO @ojousima
    init_comms();
    // Turn activity led off. Turn status_ok led on if no errors occured
    task_led_write (RUUVI_BOARD_LED_ACTIVITY, false);
    task_led_activity_led_set (RUUVI_BOARD_LED_ACTIVITY);

    if (RUUVI_DRIVER_SUCCESS == ruuvi_driver_errors_clear())
    {
        task_led_write (RUUVI_BOARD_LED_STATUS_OK, true);
        task_led_activity_led_set (RUUVI_BOARD_LED_STATUS_OK);
        ruuvi_interface_delay_ms (BOOT_DELAY_MS);
    }

    // Turn LEDs off
    task_led_write (RUUVI_BOARD_LED_STATUS_OK, false);
    // Configure activity indication
    ruuvi_interface_yield_indication_set (task_led_activity_indicate);

    while (1)
    {
        // Execute scheduled tasks
        ruuvi_interface_scheduler_execute();
        // Sleep - woken up on event
        ruuvi_interface_yield();
    }

    return -1;
}

#ifndef CEEDLING
int main (void)
{
    return app_main();
}
#endif

/*@}*/
