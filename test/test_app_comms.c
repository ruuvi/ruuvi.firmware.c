#include "unity.h"

#include "app_config.h"
#include "app_comms.h"
#include "ruuvi_boards.h"
#include "ruuvi_endpoints.h"
#include "mock_app_heartbeat.h"
#include "mock_app_led.h"
#include "mock_app_sensor.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_interface_communication_ble_advertising.h"
#include "mock_ruuvi_interface_communication_ble_gatt.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_rtc.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_timer.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_ruuvi_task_advertisement.h"
#include "mock_ruuvi_task_communication.h"
#include "mock_ruuvi_task_gatt.h"
#include "mock_ruuvi_task_nfc.h"
#include <string.h>

extern mode_changes_t m_mode_ops;
extern ri_timer_id_t m_comm_timer;
extern bool m_config_enabled_on_curr_conn;
extern bool m_config_enabled_on_next_conn;
static uint32_t m_expect_sends = 0;
static uint32_t m_dummy_timeouts = 0;

void setUp (void)
{
    rd_error_check_Ignore();
    m_config_enabled_on_curr_conn = false;
    m_config_enabled_on_next_conn = false;
    m_mode_ops.switch_to_normal = false;
    m_expect_sends = 0;
    m_dummy_timeouts = 0;
}

void tearDown (void)
{
}



static rd_status_t dummy_comm (ri_comm_message_t * const msg)
{
    rd_status_t err_code = RD_SUCCESS;

    if (0 < m_dummy_timeouts)
    {
        m_dummy_timeouts--;
        err_code = RD_ERROR_NO_MEM;
    }

    m_expect_sends++;
    return err_code;
}

static void RD_ERROR_CHECK_EXPECT (rd_status_t err_code, rd_status_t fatal)
{
    rd_error_check_Expect (err_code, fatal, "", 0);
    rd_error_check_IgnoreArg_p_file ();
    rd_error_check_IgnoreArg_line ();
}

static void test_dis_init (ri_comm_dis_init_t * const p_dis, const bool secure)
{
    memset (p_dis, 0, sizeof (ri_comm_dis_init_t));
    rt_com_get_mac_str_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_com_get_mac_str_ReturnArrayThruPtr_mac_str ("AA:BB:CC:DD:EE:FF", 18);
    snprintf (p_dis->deviceaddr, sizeof (p_dis->deviceaddr), "AA:BB:CC:DD:EE:FF");

    if (!secure)
    {
        rt_com_get_id_str_ExpectAnyArgsAndReturn (RD_SUCCESS);
        rt_com_get_id_str_ReturnArrayThruPtr_id_str ("00:11:22:33:44:55:66:77", 24);
        snprintf (p_dis->deviceid, sizeof (p_dis->deviceid), "00:11:22:33:44:55:66:77");
    }

    snprintf (p_dis->fw_version, sizeof (p_dis->fw_version), "Ruuvi FW v0.0.1+default");
    snprintf (p_dis->hw_version, sizeof (p_dis->hw_version), "Check PCB");
    snprintf (p_dis->manufacturer, sizeof (p_dis->manufacturer), RB_MANUFACTURER_STRING);
    snprintf (p_dis->model, sizeof (p_dis->model), RB_MODEL_STRING);
}

/**
 * @brief Initialize communication methods supported by board
 *
 * After calling this function, NFC is ready to provide device information,
 * advertisement data can be sent and advertisements are connectable for enabling GATT
 * connection.
 *
 * Use ruuvi task functions, such as rt_gatt_send_asynchronous to send data out.
 *
 * @retval RD_SUCCESS on success.
 * @return error code from stack on error.
 */
static rt_adv_init_t adv_settings =
{
    .channels = {
        .channel_37 = 1,
        .channel_38 = 1,
        .channel_39 = 1
    },
    .adv_interval_ms = 100U,
    .adv_pwr_dbm = RB_TX_POWER_MAX,
    .manufacturer_id = RB_BLE_MANUFACTURER_ID
};

static void adv_init_Expect (void)
{
#if APP_ADV_ENABLED
    rt_adv_init_ExpectAndReturn (&adv_settings, RD_SUCCESS);
    ri_adv_type_set_ExpectAndReturn (NONCONNECTABLE_NONSCANNABLE, RD_SUCCESS);
    ri_timer_stop_ExpectAndReturn (m_comm_timer, RD_SUCCESS);
    ri_timer_start_ExpectAndReturn (m_comm_timer, APP_FAST_ADV_TIME_MS, &m_mode_ops,
                                    RD_SUCCESS);
#endif
}

static void gatt_init_Expect (ri_comm_dis_init_t * p_dis, const bool secure)
{
#if APP_GATT_ENABLED
    static uint64_t address = 0xAABBCCDDEEFF01A0;
    ri_radio_address_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_radio_address_get_ReturnThruPtr_address (&address);
    rt_gatt_init_ExpectAndReturn ("Ruuvi 01A0", RD_SUCCESS);

    if (!secure)
    {
        rt_gatt_dfu_init_ExpectAndReturn (RD_SUCCESS);
    }

    rt_gatt_dis_init_ExpectWithArrayAndReturn (p_dis, 1, RD_SUCCESS);
    rt_gatt_nus_init_ExpectAndReturn (RD_SUCCESS);
    rt_gatt_set_on_connected_isr_Expect (&on_gatt_connected_isr);
    rt_gatt_set_on_disconn_isr_Expect (&on_gatt_disconnected_isr);
    rt_gatt_set_on_received_isr_Expect (&on_gatt_data_isr);
    rt_gatt_adv_enable_ExpectAndReturn (RD_SUCCESS);
#endif
}

static void nfc_init_Expect (ri_comm_dis_init_t * p_dis)
{
#if APP_NFC_ENABLED
    rt_nfc_init_ExpectWithArrayAndReturn (p_dis, 1, RD_SUCCESS);
    rt_nfc_set_on_connected_isr_Expect (&on_nfc_connected_isr);
    rt_nfc_set_on_disconn_isr_Expect (&on_nfc_disconnected_isr);
#endif
}

static void app_comms_ble_adv_init_Expect (void)
{
    adv_init_Expect();
}

static void app_comms_ble_uninit_Expect (void)
{
    rt_adv_uninit_ExpectAndReturn (RD_SUCCESS);
    rt_gatt_uninit_ExpectAndReturn (RD_SUCCESS);
}

static void app_comms_ble_init_Expect (const bool secure, ri_comm_dis_init_t * p_dis)
{
    test_dis_init (p_dis, secure);
    adv_init_Expect();
    gatt_init_Expect (p_dis, secure);
    ri_radio_activity_callback_set_Expect (&app_sensor_vdd_measure_isr);
}

void test_app_comms_init_ok (void)
{
    ri_comm_dis_init_t ble_dis;
    ri_comm_dis_init_t nfc_dis;
    memset (&ble_dis, 0, sizeof (ri_comm_dis_init_t));
    memset (&nfc_dis, 0, sizeof (ri_comm_dis_init_t));
    // Allow switchover to extended / 2 MBPS comms.
    ri_radio_init_ExpectAndReturn (APP_MODULATION, RD_SUCCESS);
    ri_timer_create_ExpectAndReturn (&m_comm_timer, RI_TIMER_MODE_SINGLE_SHOT,
                                     &comm_mode_change_isr, RD_SUCCESS);
    test_dis_init (&nfc_dis, false);
    nfc_init_Expect (&nfc_dis);
    adv_init_Expect();
    test_dis_init (&ble_dis, true);
    gatt_init_Expect (&ble_dis, true);
    ri_radio_activity_callback_set_Expect (&app_sensor_vdd_measure_isr);
    rd_status_t err_code = app_comms_init (true);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

static void app_comms_configure_next_enable_Expect (void)
{
    static ri_comm_dis_init_t ble_dis;
    memset (&ble_dis, 0, sizeof (ble_dis));
    app_comms_ble_uninit_Expect();
    app_comms_ble_init_Expect (false, &ble_dis);
    app_led_configuration_signal_Expect (true);
    ri_timer_stop_ExpectAndReturn (m_comm_timer, RD_SUCCESS);
    ri_timer_start_ExpectAndReturn (m_comm_timer, APP_CONFIG_ENABLED_TIME_MS, &m_mode_ops,
                                    RD_SUCCESS);
}

static void connection_cleanup_Expect (void)
{
    static ri_comm_dis_init_t ble_dis;
    memset (&ble_dis, 0, sizeof (ble_dis));
    app_comms_ble_uninit_Expect();
    app_comms_ble_init_Expect (true, &ble_dis);
    app_led_configuration_signal_Expect (false);
}

void test_app_comms_configure_next_enable_ok (void)
{
    app_comms_configure_next_enable_Expect();
    app_comms_configure_next_enable();
    TEST_ASSERT (1 == m_mode_ops.disable_config);
    TEST_ASSERT (true == m_config_enabled_on_next_conn);
}

void test_app_comms_init_timer_fail (void)
{
    ri_radio_init_ExpectAndReturn (APP_MODULATION, RD_SUCCESS);
    ri_timer_create_ExpectAndReturn (&m_comm_timer, RI_TIMER_MODE_SINGLE_SHOT,
                                     &comm_mode_change_isr, RD_ERROR_RESOURCES);
    rd_status_t err_code = app_comms_init (true);
    TEST_ASSERT (RD_ERROR_RESOURCES == err_code);
}

void test_handle_gatt_connected (void)
{
    rt_gatt_adv_disable_ExpectAndReturn (RD_SUCCESS);
    rt_adv_uninit_ExpectAndReturn (RD_SUCCESS);
    app_comms_ble_adv_init_Expect();
    handle_gatt_connected (NULL, 0);
    TEST_ASSERT (!m_config_enabled_on_next_conn);
}

void test_on_gatt_connected_isr (void)
{
    m_mode_ops.disable_config = 1;
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &handle_gatt_connected, RD_SUCCESS);
    on_gatt_connected_isr (NULL, 0);
    TEST_ASSERT (!m_mode_ops.disable_config);
}

void test_handle_gatt_disconnected (void)
{
    connection_cleanup_Expect();
    handle_gatt_disconnected (NULL, 0);
    TEST_ASSERT (!m_config_enabled_on_curr_conn);
    TEST_ASSERT (!m_config_enabled_on_next_conn);
    TEST_ASSERT (!m_mode_ops.disable_config);
}

void test_on_gatt_disconnected_isr (void)
{
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &handle_gatt_disconnected, RD_SUCCESS);
    on_gatt_disconnected_isr (NULL, 0);
}

void test_on_gatt_received_isr (void)
{
    uint8_t data[RI_SCHEDULER_SIZE];
    ri_scheduler_event_put_ExpectAndReturn (data, sizeof (data), &handle_gatt_data,
                                            RD_SUCCESS);
    RD_ERROR_CHECK_EXPECT (RD_SUCCESS, RD_SUCCESS);
    on_gatt_data_isr (data, sizeof (data));
}

void test_on_gatt_received_toobig (void)
{
    uint8_t toobig[RI_SCHEDULER_SIZE + 1];
    ri_scheduler_event_put_ExpectAndReturn (toobig, sizeof (toobig), &handle_gatt_data,
                                            RD_ERROR_INVALID_LENGTH);
    RD_ERROR_CHECK_EXPECT (RD_ERROR_INVALID_LENGTH, RD_SUCCESS);
    on_gatt_data_isr (toobig, sizeof (toobig));
}

void test_handle_gatt_sensor_op_acc (void)
{
    uint8_t mock_data[RE_STANDARD_MESSAGE_LENGTH] = {0};
    mock_data[RE_STANDARD_DESTINATION_INDEX] = RE_STANDARD_DESTINATION_ACCELERATION;
    app_heartbeat_stop_ExpectAndReturn (RD_SUCCESS);
    ri_gatt_params_request_ExpectAndReturn (RI_GATT_TURBO, (30 * 1000), RD_SUCCESS);
    app_sensor_handle_ExpectAndReturn (&rt_gatt_send_asynchronous, mock_data,
                                       sizeof (mock_data), RD_SUCCESS);
    ri_gatt_params_request_ExpectAndReturn (RI_GATT_LOW_POWER, 0, RD_SUCCESS);
    app_heartbeat_start_ExpectAndReturn (RD_SUCCESS);
    RD_ERROR_CHECK_EXPECT (RD_SUCCESS, ~RD_ERROR_FATAL);
    handle_gatt_data (mock_data, sizeof (mock_data));
}

void test_handle_gatt_null_data (void)
{
    RD_ERROR_CHECK_EXPECT (RD_ERROR_NULL, ~RD_ERROR_FATAL);
    handle_gatt_data (NULL, 0);
}

void test_handle_gatt_short_data (void)
{
    uint8_t mock_data[RE_STANDARD_HEADER_LENGTH] = {0};
    mock_data[RE_STANDARD_DESTINATION_INDEX] = RE_STANDARD_DESTINATION_ACCELERATION;
    RD_ERROR_CHECK_EXPECT (RD_ERROR_INVALID_PARAM, ~RD_ERROR_FATAL);
    handle_gatt_data (mock_data, sizeof (mock_data));
}

void test_bleadv_repeat_count_set_get (void)
{
    const uint8_t count = APP_NUM_REPEATS;
    app_comms_bleadv_send_count_set (count);
    uint8_t check = app_comms_bleadv_send_count_get();
    TEST_ASSERT (count == check);
}

void test_comm_mode_change_isr_switch_to_normal (void)
{
    mode_changes_t mode = {0};
    mode.switch_to_normal = 1;
    app_comms_bleadv_send_count_set (APP_NUM_REPEATS);
    ri_adv_tx_interval_set_ExpectAndReturn (APP_BLE_INTERVAL_MS, RD_SUCCESS);
    comm_mode_change_isr (&mode);
    uint8_t adv_repeat = app_comms_bleadv_send_count_get();
    TEST_ASSERT (0 == mode.switch_to_normal);
    TEST_ASSERT (APP_NUM_REPEATS == adv_repeat);
}

void test_comm_mode_change_isr_disable_config (void)
{
    mode_changes_t mode = {0};
    mode.disable_config = 1;
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &handle_config_disable, RD_SUCCESS);
    comm_mode_change_isr (&mode);
    TEST_ASSERT (0 == mode.disable_config);
}

void test_handle_config_disable_connected (void)
{
    rt_gatt_nus_is_connected_ExpectAndReturn (true);
    handle_config_disable (NULL, 0);
}

void test_handle_config_disable_not_connected (void)
{
    static ri_comm_dis_init_t ble_dis;
    memset (&ble_dis, 0, sizeof (ble_dis));
    rt_gatt_nus_is_connected_ExpectAndReturn (false);
    app_comms_ble_uninit_Expect();
    app_comms_ble_init_Expect (true, &ble_dis);
    app_led_configuration_signal_Expect (false);
    handle_config_disable (NULL, 0);
}

void test_handle_nfc_connected (void)
{
    app_comms_ble_uninit_Expect();
    handle_nfc_connected (NULL, 0);
    TEST_ASSERT (m_config_enabled_on_curr_conn);
}

void test_handle_nfc_disconnected (void)
{
    connection_cleanup_Expect();
    app_comms_configure_next_enable_Expect();
    handle_nfc_disconnected (NULL, 0);
    TEST_ASSERT (m_mode_ops.disable_config);
}

void test_app_comm_configurable_gatt_after_nfc (void)
{
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &handle_nfc_connected, RD_SUCCESS);
    on_nfc_connected_isr (NULL, 0);
    test_handle_nfc_connected ();
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &handle_nfc_disconnected, RD_SUCCESS);
    on_nfc_disconnected_isr (NULL, 0);
    test_handle_nfc_disconnected ();
    TEST_ASSERT (m_mode_ops.switch_to_normal);
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &handle_gatt_connected, RD_SUCCESS);
    on_gatt_connected_isr (NULL, 0);
    test_handle_gatt_connected ();
    TEST_ASSERT (m_config_enabled_on_curr_conn);
    TEST_ASSERT (!m_mode_ops.disable_config);
}

void test_app_comms_blocking_send_ok (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_rtc_millis_ExpectAndReturn (1000);
    ri_rtc_millis_ExpectAndReturn (2000);
    // reply_fp would actually return ERROR_NULL, but we can mock around it in test.
    err_code = app_comms_blocking_send (&dummy_comm,
                                        NULL);
    TEST_ASSERT (RD_SUCCESS == err_code);
    TEST_ASSERT (1 == m_expect_sends);
}

void test_app_comms_blocking_send_no_mem_once (void)
{
    rd_status_t err_code = RD_SUCCESS;
    m_dummy_timeouts = 1;
    ri_rtc_millis_ExpectAndReturn (1000);
    ri_rtc_millis_ExpectAndReturn (1000);
    ri_yield_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (4000);
    // reply_fp would actually return ERROR_NULL, but we can mock around it in test.
    err_code = app_comms_blocking_send (&dummy_comm,
                                        NULL);
    TEST_ASSERT (RD_SUCCESS == err_code);
    TEST_ASSERT (2 == m_expect_sends);
}

void test_app_comms_blocking_send_timeout (void)
{
    rd_status_t err_code = RD_SUCCESS;
    m_dummy_timeouts = 10;
    ri_rtc_millis_ExpectAndReturn (1000);
    ri_rtc_millis_ExpectAndReturn (1000);
    ri_yield_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (1500);
    ri_yield_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (2000);
    ri_yield_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (2500);
    ri_yield_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (3000);
    ri_yield_ExpectAndReturn (RD_SUCCESS);
    ri_rtc_millis_ExpectAndReturn (5500);
    // reply_fp would actually return ERROR_NULL, but we can mock around it in test.
    err_code = app_comms_blocking_send (&dummy_comm,
                                        NULL);
    TEST_ASSERT ( (RD_ERROR_TIMEOUT | RD_ERROR_NO_MEM) == err_code);
    TEST_ASSERT (5 == m_expect_sends);
}