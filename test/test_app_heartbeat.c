#include "unity.h"

#include "app_config.h"
#include "app_heartbeat.h"
#include "mock_app_comms.h"
#include "mock_app_dataformats.h"
#include "mock_app_led.h"
#include "mock_app_log.h"
#include "mock_app_sensor.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_driver_sensor.h"
#include "mock_ruuvi_endpoint_5.h"
#include "mock_ruuvi_interface_communication_ble_advertising.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_rtc.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_timer.h"
#include "mock_ruuvi_interface_watchdog.h"
#include "mock_ruuvi_task_adc.h"
#include "mock_ruuvi_task_advertisement.h"
#include "mock_ruuvi_task_gatt.h"
#include "mock_ruuvi_task_nfc.h"

#include "application_mode_default.h" //!< Ceedling doesn't follow includes by default.

static unsigned int mock_tid = 0xAA; //!< Mock timer ID to be returned, size system int.
static void * p_mock_tid = &mock_tid; //!< Pointer to mock ID.
extern uint16_t m_measurement_count;
static uint64_t next_rtc_sim = 1;

void setUp (void)
{
    ri_log_init_IgnoreAndReturn (RD_SUCCESS);
    ri_log_Ignore();
    rd_error_check_Ignore();
}

void tearDown (void)
{
}

void resetTest (void); //!< Clears test memory.

static void app_encode_expect (void)
{
    app_dataformat_next_ExpectAnyArgsAndReturn (DF_5);
    app_dataformat_encode_ExpectAnyArgsAndReturn (RD_SUCCESS);
}

static void heartbeat_all_ok_Expect (void)
{
    static rd_sensor_data_fields_t fields = {0}; //!< Gets ignored in test.
    app_sensor_available_data_ExpectAndReturn (fields);
    rd_sensor_data_fieldcount_ExpectAnyArgsAndReturn (7);
    app_sensor_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    app_led_activity_signal_Expect (true);
    app_encode_expect ();
    app_comms_bleadv_send_count_get_ExpectAndReturn (1);
    rt_adv_send_data_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_gatt_send_asynchronous_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_nfc_send_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_watchdog_feed_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (next_rtc_sim);
    app_led_activity_signal_Expect (false);
    app_log_process_ExpectAnyArgsAndReturn (RD_SUCCESS);
}

/**
 * @brief Initializes timers for reading and sending heartbeat transmissions.
 *
 * The heartbeat interval should be at most as logging rate to make sure
 * that application will log fresh data.
 *
 * @retval RD_SUCCESS on success
 * @retval RD_ERROR_INVALID_STATE if timers or scheduler is not initialized.
 * @retval RD_ERROR_RESOURCES if a timer cannot be allocated.
 */
void test_app_heartbeat_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_id_t * p_heart_timer = get_heart_timer();
    ri_timer_is_init_ExpectAndReturn (true);
    ri_scheduler_is_init_ExpectAndReturn (true);
    ri_timer_create_ExpectAndReturn (p_heart_timer, RI_TIMER_MODE_REPEATED,
                                     &schedule_heartbeat_isr, RD_SUCCESS);
    ri_timer_create_ReturnArrayThruPtr_p_timer_id (&p_mock_tid, 1);
    ri_timer_start_ExpectAndReturn (&mock_tid, APP_HEARTBEAT_INTERVAL_MS, NULL, RD_SUCCESS);
    err_code = app_heartbeat_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_heartbeat_init_notimer (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_is_init_ExpectAndReturn (false);
    err_code = app_heartbeat_init();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}

void test_app_heartbeat_init_noscheduler (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_is_init_ExpectAndReturn (true);
    ri_scheduler_is_init_ExpectAndReturn (false);
    err_code = app_heartbeat_init();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}

void test_app_heartbeat_init_timer_alloc_fail (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_id_t * p_heart_timer = get_heart_timer();
    ri_timer_is_init_ExpectAndReturn (true);
    ri_scheduler_is_init_ExpectAndReturn (true);
    ri_timer_create_ExpectAndReturn (p_heart_timer, RI_TIMER_MODE_REPEATED,
                                     &schedule_heartbeat_isr, RD_ERROR_RESOURCES);
    err_code = app_heartbeat_init();
    TEST_ASSERT (RD_ERROR_RESOURCES == err_code);
}


void test_schedule_heartbeat_isr (void)
{
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &heartbeat, RD_SUCCESS);
    schedule_heartbeat_isr (NULL);
}

void test_app_heartbeat_stop (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_id_t * p_heart_timer = get_heart_timer();
    ri_timer_stop_ExpectAndReturn (*p_heart_timer, RD_SUCCESS);
    err_code |= app_heartbeat_stop();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_heartbeat_start (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_id_t * p_heart_timer = get_heart_timer();
    heartbeat_all_ok_Expect();
    ri_timer_start_ExpectAndReturn (*p_heart_timer, APP_HEARTBEAT_INTERVAL_MS, NULL,
                                    RD_SUCCESS);
    err_code |= app_heartbeat_start();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_heartbeat_start_notinit (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_id_t * p_heart_timer = get_heart_timer();
    *p_heart_timer = NULL;
    err_code |= app_heartbeat_start();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}

void test_app_heartbeat_stop_notinit (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_id_t * p_heart_timer = get_heart_timer();
    *p_heart_timer = NULL;
    err_code |= app_heartbeat_stop();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}

void test_heartbeat_all_ok (void)
{
    heartbeat_all_ok_Expect();
    heartbeat (NULL, 0);
}

void test_heartbeat_adv_ok (void)
{
    static rd_sensor_data_fields_t fields = {0}; //!< Gets ignored in test.
    app_sensor_available_data_ExpectAndReturn (fields);
    rd_sensor_data_fieldcount_ExpectAnyArgsAndReturn (7);
    app_sensor_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    app_led_activity_signal_Expect (true);
    app_encode_expect ();
    app_comms_bleadv_send_count_get_ExpectAndReturn (1);
    rt_adv_send_data_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_gatt_send_asynchronous_ExpectAnyArgsAndReturn (RD_ERROR_INVALID_STATE);
    rt_nfc_send_ExpectAnyArgsAndReturn (RD_ERROR_NOT_ENABLED);
    ri_watchdog_feed_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (1);
    app_led_activity_signal_Expect (false);
    app_log_process_ExpectAnyArgsAndReturn (RD_SUCCESS);
    heartbeat (NULL, 0);
}

void test_heartbeat_adv_disabled (void)
{
    static rd_sensor_data_fields_t fields = {0}; //!< Gets ignored in test.
    app_sensor_available_data_ExpectAndReturn (fields);
    rd_sensor_data_fieldcount_ExpectAnyArgsAndReturn (7);
    app_sensor_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    app_led_activity_signal_Expect (true);
    app_encode_expect ();
    app_comms_bleadv_send_count_get_ExpectAndReturn (0);
    rt_adv_stop_ExpectAndReturn (RD_SUCCESS);
    rt_gatt_send_asynchronous_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_nfc_send_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_watchdog_feed_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (1);
    app_led_activity_signal_Expect (false);
    app_log_process_ExpectAnyArgsAndReturn (RD_SUCCESS);
    heartbeat (NULL, 0);
}

void test_heartbeat_none_ok (void)
{
    static rd_sensor_data_fields_t fields = {0}; //!< Gets ignored in test.
    app_sensor_available_data_ExpectAndReturn (fields);
    rd_sensor_data_fieldcount_ExpectAnyArgsAndReturn (7);
    app_sensor_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    app_led_activity_signal_Expect (true);
    app_encode_expect ();
    app_comms_bleadv_send_count_get_ExpectAndReturn (1);
    rt_adv_send_data_ExpectAnyArgsAndReturn (RD_ERROR_INVALID_STATE);
    rt_gatt_send_asynchronous_ExpectAnyArgsAndReturn (RD_ERROR_NOT_ENABLED);
    rt_nfc_send_ExpectAnyArgsAndReturn (RD_ERROR_NOT_ENABLED);
    app_led_activity_signal_Expect (false);
    app_log_process_ExpectAnyArgsAndReturn (RD_SUCCESS);
    heartbeat (NULL, 0);
}

void test_app_heartbeat_overdue_no (void)
{
    next_rtc_sim = 1;
    test_heartbeat_all_ok();
    ri_rtc_millis_ExpectAndReturn (APP_HEARTBEAT_OVERDUE_INTERVAL_MS - 1);
    TEST_ASSERT (!app_heartbeat_overdue());
}

void test_app_heartbeat_overdue_yes (void)
{
    next_rtc_sim = 1;
    test_heartbeat_all_ok();
    ri_rtc_millis_ExpectAndReturn (APP_HEARTBEAT_OVERDUE_INTERVAL_MS);
    TEST_ASSERT (!app_heartbeat_overdue());
}