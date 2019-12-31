#include "unity.h"

#include "main.h"

#include "ruuvi_boards.h"
#include "application_config.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_communication_ble4_gatt.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_gpio_interrupt.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_rtc.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_watchdog.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_task_acceleration.h"
#include "mock_task_adc.h"
#include "mock_task_advertisement.h"
#include "mock_task_button.h"
#include "mock_task_communication.h"
#include "mock_task_environmental.h"
#include "mock_task_flash.h"
#include "mock_task_gatt.h"
#include "mock_task_gpio.h"
#include "mock_task_led.h"
#include "mock_task_i2c.h"
#include "mock_task_nfc.h"
#include "mock_task_power.h"
#include "mock_task_rtc.h"
#include "mock_task_scheduler.h"
#include "mock_task_sensor.h"
#include "mock_task_spi.h"
#include "mock_task_timer.h"
#include "mock_test_sensor.h"
#include "mock_test_acceleration.h"
#include "mock_test_adc.h"
#include "mock_test_environmental.h"
#include "mock_test_library.h"

ruuvi_interface_communication_ble4_gatt_dis_init_t m_dis;
uint64_t m_mac = 0xAABBCCDDEEFF;
uint8_t  m_name[] = RUUVI_BOARD_BLE_NAME_STRING " EEFF";

void setUp (void)
{
    ruuvi_driver_error_check_Ignore();
    ruuvi_interface_log_Ignore();
}

void tearDown (void)
{
    task_adc_vdd_sample_IgnoreAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_rtc_millis_IgnoreAndReturn (0);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}

/**
 *  @brief Synchronize ADC measurement to radio.
 *  This is common to all radio modules, i.e.
 *  the callback gets called for every radio action.
 *
 *  @param[in] evt Type of radio event
 */
void test_main_on_radio_prepare_update (void)
{
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS * 2U);
    task_adc_vdd_prepare_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_BEFORE);
}

void test_main_on_radio_no_need_to_update (void)
{
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS / 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_BEFORE);
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS / 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}

void test_main_on_radio_do_update (void)
{
    test_main_on_radio_prepare_update();
    task_adc_vdd_sample_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS * 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}

/** @brief Handle NUS connection event.
 *
 * Configures application to start sending the data via GATT instead of
 * BLE advertisements.
 *
 * @param data Unused, contains event data which is NULL.
 * @param data_len Unused, always 0.
 */
void test_main_on_gatt_connected_isr (void)
{
    task_advertisement_stop_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    on_gatt_connected_isr (NULL, 0);
}

/** @brief Handle NUS disconnection event.
 *
 * Configures application to start data in  BLE advertisements.
 *
 * @param data Unused, contains event data which is NULL.
 * @param data_len Unused, always 0.
 */
void test_main_on_gatt_disconnected_isr (void)
{
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    task_advertisement_start_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    on_gatt_disconnected_isr (NULL, 0);
}

/** @brief Handle incoming NUS data outside interrupt context.
 *
 * Pauses heartbeat transmissions and passes the data along with
 * reply function pointer to task communication.
 *
 * @param data Unused, contains event data which is NULL.
 * @param data_len Unused, always 0.
 */
void test_main_process_gatt_command_ok (void)
{
    ruuvi_interface_communication_message_t msg = {0};
    char data[] = "ABCDEFGHIJ";
    snprintf (& (msg.data), sizeof (data), "%s", data);
    msg.data_length = sizeof (data);
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    task_communication_on_data_ExpectAndReturn (&msg, &task_gatt_send_asynchronous,
            RUUVI_DRIVER_SUCCESS);
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    process_gatt_command (data, sizeof (data));
}

void test_main_process_gatt_command_null (void)
{
    process_gatt_command (NULL, 0);
}

void test_main_process_gatt_command_toolong (void)
{
    char data[] = "ABCDEFGHIJ";
    process_gatt_command (data, (RUUVI_INTERFACE_COMMUNICATION_MESSAGE_MAX_LENGTH + 1));
}

/** @brief Handle incoming NUS data inside interrupt context.
 *
 * Schedules the processing of data to be executed outside of interrupt context.
 *
 * @param data Incoming bytes.
 * @param data_len Length of incoming bytes.
 */
void test_main_on_gatt_received_isr (void)
{
    char data[] = "ABCDEFGHIJ";
    ruuvi_interface_scheduler_event_put_ExpectAndReturn (data,  sizeof (data),
            &process_gatt_command, RUUVI_DRIVER_SUCCESS);
    on_gatt_received_isr (data, sizeof (data));
}

/** @brief Handle outgoing NUS data event.
 *
 * This function gets called once data queued to NUS is actually sent.
 *
 * @param data Unused, contains event data which is NULL.
 * @param data_len Unused, always 0.
 */
void test_main_on_gatt_sent_isr (void)
{
    on_gatt_sent_isr (NULL, 0);
}



/**
 * @brief initialize 2-way communication with outside world.
 *
 * The communication includes any way user can input data, such as button presses,
 * GATT, BLE advertisements and NFC. It does not include inter-board communication
 * such as I2C and SPI.
 */

void t_init_dis (void)
{
    memset (&m_dis, 0, sizeof (ruuvi_interface_communication_ble4_gatt_dis_init_t));
    snprintf (m_dis.deviceid, 32, "%s", "AA:BB:CC:DD:EE:FF");
    snprintf (m_dis.fw_version, 32, "%s", APPLICATION_FW_VERSION);
    snprintf (m_dis.model, 32, "%s", RUUVI_BOARD_MODEL_STRING);
    snprintf (m_dis.manufacturer, 32, "%s", RUUVI_BOARD_MANUFACTURER_STRING);
    ruuvi_interface_communication_radio_address_get_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_radio_address_get_ReturnArrayThruPtr_address (&m_mac, 1);
    task_gatt_dis_init_ExpectWithArrayAndReturn (&m_dis, 1, RUUVI_DRIVER_SUCCESS);
}


void t_main_init_comms (void)
{
    task_button_init_ExpectAndReturn (&button_on_event_isr, RUUVI_DRIVER_SUCCESS);
    task_advertisement_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_advertisement_start_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_communication_heartbeat_configure_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_radio_activity_callback_set_Expect (on_radio);
    ruuvi_interface_communication_radio_address_get_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_radio_address_get_ReturnArrayThruPtr_address (&m_mac, 1);
    task_gatt_init_ExpectAndReturn (m_name, RUUVI_DRIVER_SUCCESS);
    t_init_dis();
    task_gatt_nus_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_gatt_dfu_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_gatt_set_on_connected_isr_Expect (&on_gatt_connected_isr);
    task_gatt_set_on_disconn_isr_Expect (&on_gatt_disconnected_isr);
    task_gatt_set_on_received_isr_Expect (&on_gatt_received_isr);
    task_gatt_set_on_sent_isr_Expect (&on_gatt_sent_isr);
    task_gatt_enable_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_advertisement_stop_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_advertisement_start_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_nfc_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
}

void test_main_init_comms (void)
{
    t_main_init_comms();
    init_comms();
}

void t_main_init_logging (void)
{
    ruuvi_interface_log_init_ExpectAndReturn (APPLICATION_LOG_LEVEL, RUUVI_DRIVER_SUCCESS);
}

void t_main_init_mcu (void)
{
    ruuvi_interface_watchdog_init_ExpectAndReturn (APPLICATION_WATCHDOG_INTERVAL_MS,
            RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_yield_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_gpio_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_led_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_spi_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_i2c_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_timer_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_rtc_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_scheduler_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_yield_low_power_enable_ExpectAndReturn (true, RUUVI_DRIVER_SUCCESS);
    task_power_dcdc_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_flash_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_adc_vdd_prepare_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_adc_vdd_sample_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
}

void t_main_init_sensors (void)
{
    task_environmental_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_acceleration_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
}

void test_app_main_ok (void)
{
    t_main_init_logging();
    // Ceedling skips MCU tests
    t_main_init_mcu();
    ruuvi_interface_delay_ms_ExpectAndReturn (BOOT_DELAY_MS, RUUVI_DRIVER_SUCCESS);
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_RED, true, RUUVI_DRIVER_SUCCESS);
    // Ceedling skips sensor integration tests
    t_main_init_sensors();
    t_main_init_comms();
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_RED, false, RUUVI_DRIVER_SUCCESS);
    ruuvi_driver_errors_clear_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_STATUS_OK, true, RUUVI_DRIVER_SUCCESS);
    task_led_activity_led_set_ExpectAndReturn (RUUVI_BOARD_LED_STATUS_OK,
            RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_delay_ms_ExpectAndReturn (BOOT_DELAY_MS, RUUVI_DRIVER_SUCCESS);
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_STATUS_OK, false, RUUVI_DRIVER_SUCCESS);
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_STATUS_ERROR, false,
                                    RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_yield_indication_set_Expect (&task_led_activity_indicate);
    ruuvi_interface_scheduler_execute_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_yield_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    app_main();
}

void test_app_main_error (void)
{
    t_main_init_logging();
    // Ceedling skips MCU tests
    t_main_init_mcu();
    ruuvi_interface_delay_ms_ExpectAndReturn (BOOT_DELAY_MS, RUUVI_DRIVER_SUCCESS);
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_RED, true, RUUVI_DRIVER_SUCCESS);
    // Ceedling skips sensor integration tests
    t_main_init_sensors();
    t_main_init_comms();
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_RED, false, RUUVI_DRIVER_SUCCESS);
    ruuvi_driver_errors_clear_ExpectAndReturn (RUUVI_DRIVER_ERROR_NOT_FOUND);
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_STATUS_ERROR, true, RUUVI_DRIVER_SUCCESS);
    task_led_activity_led_set_ExpectAndReturn (RUUVI_BOARD_LED_STATUS_ERROR,
            RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_delay_ms_ExpectAndReturn (BOOT_DELAY_MS, RUUVI_DRIVER_SUCCESS);
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_STATUS_OK, false, RUUVI_DRIVER_SUCCESS);
    task_led_write_ExpectAndReturn (RUUVI_BOARD_LED_STATUS_ERROR, false,
                                    RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_yield_indication_set_Expect (&task_led_activity_indicate);
    ruuvi_interface_scheduler_execute_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_yield_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    app_main();
}

void test_main_button_isr (void)
{
    //Does nothing
    ruuvi_interface_gpio_evt_t evt;
    evt.pin = (ruuvi_interface_gpio_id_t) {.pin = 13};
    evt.slope = RUUVI_INTERFACE_GPIO_SLOPE_TOGGLE;
    button_on_event_isr (evt);
}

// get_mac is tested by DIS and GATT init tests.
void test_get_mac_null (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = get_mac (NULL);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_NULL == err_code);
}