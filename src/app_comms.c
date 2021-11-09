#include "app_config.h"
#include "app_comms.h"
#include "app_heartbeat.h"
#include "app_led.h"
#include "app_sensor.h"
#include "app_testing.h"
#include "ruuvi_boards.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_ble_advertising.h"
#include "ruuvi_interface_communication_ble_gatt.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_advertisement.h"
#include "ruuvi_task_communication.h"
#include "ruuvi_task_gatt.h"
#include "ruuvi_task_nfc.h"
#include <stdio.h>
#include <string.h>

/**
 * @addtogroup app_comms
 */
/** @{ */
/**
 * @file app_comms.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-09-10
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Typical usage:
 * @code{.c}
 * TODO
 * @endcode
 */

/** @brief Set to long enough to handle existing queue, then as short as possible. */
#define BLOCKING_COMM_TIMEOUT_MS (4000U)
#define CONN_PARAM_UPDATE_DELAY_MS (30U * 1000U) //!< Delay before switching to faster conn params in long ops.

#if APP_COMMS_BIDIR_ENABLED
TESTABLE_STATIC bool
m_config_enabled_on_curr_conn; //!< This connection has config enabled.
TESTABLE_STATIC bool
m_config_enabled_on_next_conn; //!< Next connection has config enabled.
#endif

#ifndef CEEDLING
typedef struct
{
    unsigned int switch_to_normal : 1; //!< Stop initial fast advertising.
    unsigned int disable_config : 1;   //!< Disable configuration mode.
} mode_changes_t;
#endif

static volatile bool m_tx_done; //!< Flag for data transfer done
static uint8_t m_bleadv_repeat_count; //!< Number of times to repeat advertisement.
TESTABLE_STATIC ri_timer_id_t m_comm_timer;    //!< Timer for communication mode changes.
TESTABLE_STATIC mode_changes_t m_mode_ops;     //!< Pending mode changes.

uint8_t app_comms_bleadv_send_count_get (void)
{
    return m_bleadv_repeat_count;
}

void app_comms_bleadv_send_count_set (const uint8_t count)
{
    m_bleadv_repeat_count = count;
}

/**
 * @brief Start a timer for switching over to normal mode.
 */
static rd_status_t timed_switch_to_normal_mode (void)
{
    rd_status_t err_code = RD_SUCCESS;
    m_mode_ops.switch_to_normal = 1;
    err_code |= ri_timer_stop (m_comm_timer);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    err_code |= ri_timer_start (m_comm_timer, APP_FAST_ADV_TIME_MS, &m_mode_ops);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    return err_code;
}

/**
 * @brief Start a timer for changing mode of communications.
 */
static rd_status_t prepare_mode_change (const mode_changes_t * p_change)
{
    rd_status_t err_code = RD_SUCCESS;

    if (p_change->switch_to_normal)
    {
        err_code |= timed_switch_to_normal_mode();
    }

    return err_code;
}

/**
 * @brief Calculate how many times advertisements must be repeated
 *        to send at the initial fast rate.
 */
static uint8_t initial_adv_send_count (void)
{
    uint16_t num_sends = (APP_HEARTBEAT_INTERVAL_MS / 100U);

    if (0 == num_sends) //-V547
    {
        num_sends = 1;
    }

    if (APP_COMM_ADV_REPEAT_FOREVER <= num_sends) //-V547
    {
        num_sends = APP_COMM_ADV_REPEAT_FOREVER - 1;
    }

    return num_sends;
}

#if APP_COMMS_BIDIR_ENABLED

// Returns true if command is allowed.
// If Configuration is not allowed, command type must be read.
static bool command_is_authorized (const uint8_t op)
{
    return (RE_STANDARD_OP_READ_BIT & op) || m_config_enabled_on_curr_conn;
}

static rd_status_t reply_unauthorized (const ri_comm_xfer_fp_t reply_fp,
                                       const uint8_t * const raw_message)
{
    ri_comm_message_t msg = {0};
    msg.data_length = RE_STANDARD_MESSAGE_LENGTH;
    msg.repeat_count = 1;
    msg.data[RE_STANDARD_DESTINATION_INDEX] = raw_message[RE_STANDARD_SOURCE_INDEX];
    msg.data[RE_STANDARD_SOURCE_INDEX] = raw_message[RE_STANDARD_DESTINATION_INDEX];
    msg.data[RE_STANDARD_OPERATION_INDEX] = RE_STANDARD_OP_UNAUTHORIZED;

    for (uint8_t ii = RE_STANDARD_PAYLOAD_START_INDEX;
            ii < RE_STANDARD_MESSAGE_LENGTH; ii++)
    {
        msg.data[ii] = 0xFFU;
    }

    return reply_fp (&msg);
}

static rd_status_t reply_authorized (const ri_comm_xfer_fp_t reply_fp,
                                     const uint8_t * const raw_message)
{
    ri_comm_message_t msg = {0};
    msg.data_length = RE_STANDARD_MESSAGE_LENGTH;
    msg.repeat_count = 1;
    msg.data[RE_STANDARD_DESTINATION_INDEX] = raw_message[RE_STANDARD_SOURCE_INDEX];
    msg.data[RE_STANDARD_SOURCE_INDEX] = raw_message[RE_STANDARD_DESTINATION_INDEX];
    msg.data[RE_STANDARD_OPERATION_INDEX] = RE_STANDARD_VALUE_WRITE;

    for (uint8_t ii = RE_STANDARD_PAYLOAD_START_INDEX;
            ii < RE_STANDARD_MESSAGE_LENGTH; ii++)
    {
        msg.data[ii] = raw_message[ii];
    }

    return reply_fp (&msg);
}

/**
 * @brief Allow configuration commands on next connection.
 *
 * Has side effect of disconnecting current GATT connecction due to need to re-init
 * GATT services.
 *
 * @param[in] enable True to enable configuration on connection, false to disable.
 */
static rd_status_t enable_config_on_next_conn (const bool enable)
{
    rd_status_t err_code = RD_SUCCESS;
    // Kicks out current connection.
    err_code |= app_comms_ble_uninit();
    err_code |= app_comms_ble_init (!enable);
    m_config_enabled_on_next_conn = enable;
    app_led_configuration_signal (enable);
    return err_code;
}

static rd_status_t wait_for_tx_done (const uint32_t timeout_ms)
{
    rd_status_t err_code = RD_SUCCESS;
    const uint64_t start = ri_rtc_millis();
    const uint64_t timeout = start + timeout_ms;

    while ( (!m_tx_done) && (timeout > ri_rtc_millis()))
    {
        ri_yield();
    }

    if (!m_tx_done)
    {
        err_code |= RD_ERROR_TIMEOUT;
    }

    m_tx_done = false;
    return err_code;
}

static rd_status_t password_check (const ri_comm_xfer_fp_t reply_fp,
                                   const uint8_t * const raw_message)
{
    rd_status_t err_code = RD_SUCCESS;
    uint64_t entered_password = 0U;
    bool auth_ok = false;
    // Use non-zero initial value so passwords won't match on error
    uint64_t current_password = 0xDEADBEEF12345678U;
    re_op_t op = (re_op_t) raw_message[RE_STANDARD_OPERATION_INDEX];

    if (RE_STANDARD_VALUE_READ == op)
    {
        err_code |= ri_comm_id_get (&current_password);

        // Convert data to U64
        for (uint8_t ii = RE_STANDARD_PAYLOAD_START_INDEX;
                ii < RE_STANDARD_MESSAGE_LENGTH; ii++)
        {
            uint8_t byte_offset = RE_STANDARD_MESSAGE_LENGTH - ii - 1U;
            uint8_t bit_offset = byte_offset * 8U;
            uint64_t password_part = raw_message[ii];
            password_part = password_part << bit_offset;
            entered_password += password_part;
        }
    }

    m_tx_done = false;

    if (entered_password == current_password)
    {
        err_code |= reply_authorized (reply_fp, raw_message);
        auth_ok = true;
    }
    else
    {
        err_code |= reply_unauthorized (reply_fp, raw_message);
        auth_ok = false;
    }

    err_code |= wait_for_tx_done (BLOCKING_COMM_TIMEOUT_MS);

    if (auth_ok)
    {
        app_comms_configure_next_enable ();
    }
    else
    {
        app_comms_configure_next_disable ();
    }

    return  err_code;
}

TESTABLE_STATIC void handle_comms (const ri_comm_xfer_fp_t reply_fp, void * p_data,
                                   size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    const uint8_t * const raw_message = (uint8_t *) p_data;

    if (NULL == p_data)
    {
        err_code |= RD_ERROR_NULL;
    }
    else if (data_len < RE_STANDARD_MESSAGE_LENGTH)
    {
        err_code |= RD_ERROR_INVALID_PARAM;
    }
    else if (!command_is_authorized (raw_message[RE_STANDARD_OPERATION_INDEX]))
    {
        err_code |= reply_unauthorized (reply_fp, raw_message);
    }
    else
    {
        // Stop heartbeat processing.
        err_code |= app_heartbeat_stop();
        // Switch GATT to faster params.
        err_code |= ri_gatt_params_request (RI_GATT_TURBO, CONN_PARAM_UPDATE_DELAY_MS);
        // Parse message type.
        re_type_t type = raw_message[RE_STANDARD_DESTINATION_INDEX];

        // Route message to proper handler.
        switch (type)
        {
            case RE_ACC_XYZ:
            case RE_ACC_X:
            case RE_ACC_Y:
            case RE_ACC_Z:
            case RE_GYR_XYZ:
            case RE_GYR_X:
            case RE_GYR_Y:
            case RE_GYR_Z:
            case RE_ENV_ALL:
            case RE_ENV_TEMP:
            case RE_ENV_HUMI:
            case RE_ENV_PRES:
                err_code |= app_sensor_handle (reply_fp, raw_message, data_len);
                break;

            case RE_SEC_PASS:
                err_code |= password_check (reply_fp, raw_message);
                break;

            default:
                break;
        }

        // Switch GATT to slower params.
        err_code |= ri_gatt_params_request (RI_GATT_LOW_POWER, 0);
        // Resume heartbeat processing.
        err_code |= app_heartbeat_start();
    }

    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
}

/**
 * @brief Configures this connection, call this in on_connected handler.
 */
static void config_setup_on_this_conn (void)
{
    m_config_enabled_on_curr_conn = m_config_enabled_on_next_conn;
    m_config_enabled_on_next_conn = false;
    m_mode_ops.disable_config = 0;
}


/**
 * @brief Cleans up configuration related to closing connection, call this
 * in on_disconnected handler
 */
static void config_cleanup_on_disconnect (void)
{
    rd_status_t err_code = RD_SUCCESS;
    m_config_enabled_on_curr_conn = false;
    err_code |= enable_config_on_next_conn (false);
    m_mode_ops.disable_config = 0; // No need to disable config again.
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

#if APP_GATT_ENABLED

TESTABLE_STATIC void handle_gatt_data (void * p_data, uint16_t data_len)
{
    handle_comms (&rt_gatt_send_asynchronous, p_data, data_len);
}

/**
 * @brief Disable advertising for GATT connection and setup current connection.
 */
TESTABLE_STATIC void handle_gatt_connected (void * p_data, uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    // Disables advertising for GATT, does not kick current connetion out.
    err_code |= rt_gatt_adv_disable ();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
    err_code |= app_comms_ble_adv_init ();
    config_setup_on_this_conn ();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/**
 * @brief Callback when GATT is connected
 */
TESTABLE_STATIC void on_gatt_connected_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len,
                                        &handle_gatt_connected);
    // Configuration will be disabled on disconnection, no need to trigger timer action.
    m_mode_ops.disable_config = 0;
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

TESTABLE_STATIC void handle_gatt_disconnected (void * p_data, uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    config_cleanup_on_disconnect();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/** @brief Callback when GATT is disconnected" */
TESTABLE_STATIC void on_gatt_disconnected_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len,
                                        &handle_gatt_disconnected);
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/**
 * @brief Callback when data is received via GATT
 *
 * Schedule handling incoming message and replying back via given function pointer.
 */
TESTABLE_STATIC void on_gatt_data_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len, &handle_gatt_data);
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/**
 * @brief Callback when data is sent via GATT
 *
 * Schedule handling incoming message and replying back via given function pointer.
 */
TESTABLE_STATIC void on_gatt_tx_done_isr (void * p_data, size_t data_len)
{
    m_tx_done = true;
}


static rd_status_t ble_name_string_create (char * const name_str, const size_t name_len)
{
    uint64_t radio_addr = 0;
    rd_status_t err_code = ri_radio_address_get (&radio_addr);
    radio_addr &= 0xFFFF;
    snprintf (name_str, name_len, "%s %04X", RB_BLE_NAME_STRING, (uint16_t) radio_addr);
    return err_code;
}

#endif //!< if GATT ENABLED

#if APP_NFC_ENABLED

TESTABLE_STATIC void handle_nfc_connected (void * p_data, uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    // Kick BLE connection to not allow it to configure tag
    app_comms_ble_uninit();
    m_config_enabled_on_next_conn = true;
    config_setup_on_this_conn();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

TESTABLE_STATIC void on_nfc_connected_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len, &handle_nfc_connected);
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

TESTABLE_STATIC void handle_nfc_disconnected (void * p_data, uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    config_cleanup_on_disconnect();
    err_code |= app_comms_configure_next_enable();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/** @brief Callback when NFC is disconnected" */
TESTABLE_STATIC void on_nfc_disconnected_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len,
                                        &handle_nfc_disconnected);
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/** @brief Callback when NFC has sent data" */
TESTABLE_STATIC void on_nfc_tx_done_isr (void * p_data, size_t data_len)
{
    m_tx_done = true;
}
#endif

rd_status_t app_comms_configure_next_enable (void)
{
    rd_status_t err_code = RD_SUCCESS;
    m_mode_ops.disable_config = 1;
    err_code |= enable_config_on_next_conn (true);
    err_code |= ri_timer_stop (m_comm_timer);
    err_code |= ri_timer_start (m_comm_timer, APP_CONFIG_ENABLED_TIME_MS, &m_mode_ops);
    return err_code;
}

rd_status_t app_comms_configure_next_disable (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= enable_config_on_next_conn (false);
    return err_code;
}

TESTABLE_STATIC void handle_config_disable (void * p_data, uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;

    // Do not kick out current connection, disconnect handler
    // will disable config.
    if (!rt_gatt_nus_is_connected())
    {
        err_code |= enable_config_on_next_conn (false);
    }

    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

#else
rd_status_t app_comms_configure_next_enable (void)
{
    return RD_ERROR_NOT_ENABLED;
}
#endif //!< APP_COMMS_BIDIR_ENABLED

/**
 * @brief Initialize Device information data.
 *
 * @param[in] secure If true, security sensitive data will be NULL/0-length.
 * @retval RD_SUCCESS necessary data was available and is printed on p_dis.
 * @return Error code from driver in case some data cannot be printed.
 */
static rd_status_t dis_init (ri_comm_dis_init_t * const p_dis, const bool secure)
{
    rd_status_t err_code = RD_SUCCESS;
    memset (p_dis, 0, sizeof (ri_comm_dis_init_t));
#if APP_COMMS_BIDIR_ENABLED
    size_t name_idx = 0;
    err_code |= rt_com_get_mac_str (p_dis->deviceaddr, sizeof (p_dis->deviceaddr));

    if (!secure)
    {
        err_code |= rt_com_get_id_str (p_dis->deviceid, sizeof (p_dis->deviceid));
    }

    name_idx =  snprintf (p_dis->fw_version, sizeof (p_dis->fw_version), APP_FW_NAME);
    name_idx += snprintf (p_dis->fw_version + name_idx,
                          sizeof (p_dis->fw_version) - name_idx,
                          APP_FW_VERSION);
    snprintf (p_dis->fw_version + name_idx,
              sizeof (p_dis->fw_version) - name_idx,
              APP_FW_VARIANT);
    snprintf (p_dis->hw_version, sizeof (p_dis->hw_version), "Check PCB");
    snprintf (p_dis->manufacturer, sizeof (p_dis->manufacturer), RB_MANUFACTURER_STRING);
    snprintf (p_dis->model, sizeof (p_dis->model), RB_MODEL_STRING);
#endif
    return err_code;
}

static rd_status_t nfc_init (ri_comm_dis_init_t * const p_dis)
{
    rd_status_t err_code = RD_SUCCESS;
#if APP_NFC_ENABLED
    err_code |= rt_nfc_init (p_dis);
    rt_nfc_set_on_connected_isr (&on_nfc_connected_isr);
    rt_nfc_set_on_disconn_isr (&on_nfc_disconnected_isr);
    rt_nfc_set_on_sent_isr (&on_nfc_tx_done_isr);
#endif
    return err_code;
}

TESTABLE_STATIC void comm_mode_change_isr (void * const p_context)
{
    mode_changes_t * const p_change = (mode_changes_t *) p_context;

    if (p_change->switch_to_normal)
    {
        app_comms_bleadv_send_count_set (APP_NUM_REPEATS);
        ri_adv_tx_interval_set (APP_BLE_INTERVAL_MS);
        p_change->switch_to_normal = 0;
    }

#if APP_COMMS_BIDIR_ENABLED

    if (p_change->disable_config)
    {
        ri_scheduler_event_put (NULL, 0, &handle_config_disable);
        p_change->disable_config = 0;
    }

#endif
}

static rd_status_t adv_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
#if APP_ADV_ENABLED
    ri_radio_channels_t channels = {0};
    channels.channel_37 = 1;
    channels.channel_38 = 1;
    channels.channel_39 = 1;
    rt_adv_init_t adv_settings = {0};
    adv_settings.adv_interval_ms = 100U;
    adv_settings.adv_pwr_dbm = RB_TX_POWER_MAX;
    adv_settings.channels = channels;
    adv_settings.manufacturer_id = RB_BLE_MANUFACTURER_ID;
    err_code |= rt_adv_init (&adv_settings);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    err_code |= ri_adv_type_set (NONCONNECTABLE_NONSCANNABLE);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    app_comms_bleadv_send_count_set (initial_adv_send_count());
    m_mode_ops.switch_to_normal = 1;
    err_code |= prepare_mode_change (&m_mode_ops);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
#endif
    return err_code;
}

static rd_status_t gatt_init (const ri_comm_dis_init_t * const p_dis, const bool secure)
{
    rd_status_t err_code = RD_SUCCESS;
#if APP_GATT_ENABLED
    char name[SCAN_RSP_NAME_MAX_LEN + 1] = {0};
    ble_name_string_create (name, sizeof (name));
    err_code |= rt_gatt_init (name);

    if (!secure)
    {
        err_code |= rt_gatt_dfu_init();
    }

    err_code |= rt_gatt_dis_init (p_dis);
    err_code |= rt_gatt_nus_init ();
    rt_gatt_set_on_connected_isr (&on_gatt_connected_isr);
    rt_gatt_set_on_disconn_isr (&on_gatt_disconnected_isr);
    rt_gatt_set_on_received_isr (&on_gatt_data_isr);
    rt_gatt_set_on_sent_isr (&on_gatt_tx_done_isr);
    err_code |= rt_gatt_adv_enable();
#endif
    return err_code;
}

rd_status_t app_comms_init (const bool secure)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_radio_init (APP_MODULATION);
    err_code |= ri_timer_create (&m_comm_timer, RI_TIMER_MODE_SINGLE_SHOT,
                                 &comm_mode_change_isr);

    if (RD_SUCCESS == err_code)
    {
        if (!secure)
        {
            err_code |= app_comms_configure_next_enable();
        }

        ri_comm_dis_init_t dis = {0};
        err_code |= dis_init (&dis, false);
        err_code |= nfc_init (&dis);
        err_code |= adv_init();
        err_code |= dis_init (&dis, secure);
        err_code |= gatt_init (&dis, secure);
        ri_radio_activity_callback_set (&app_sensor_vdd_measure_isr);
    }

    return err_code;
}

rd_status_t app_comms_ble_init (const bool secure)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_comm_dis_init_t dis = {0};
    err_code |= dis_init (&dis, secure);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    err_code |= adv_init();
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    err_code |= gatt_init (&dis, secure);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    ri_radio_activity_callback_set (&app_sensor_vdd_measure_isr);
    return err_code;
}

rd_status_t app_comms_ble_adv_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_adv_uninit();
    err_code |= adv_init();
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    return err_code;
}

rd_status_t app_comms_ble_uninit (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_adv_uninit();
    err_code |= rt_gatt_uninit();
    return err_code;
}

rd_status_t app_comms_blocking_send (const ri_comm_xfer_fp_t reply_fp,
                                     ri_comm_message_t * const msg)
{
    rd_status_t err_code = RD_SUCCESS;
    const uint64_t start = ri_rtc_millis();

    do
    {
        if (ri_rtc_millis() > (start + BLOCKING_COMM_TIMEOUT_MS))
        {
            err_code |= RD_ERROR_TIMEOUT;
        }
        else
        {
            err_code = RD_SUCCESS;
        }

        if (RD_SUCCESS == err_code)
        {
            err_code |= reply_fp (msg);
        }

        if (RD_ERROR_NO_MEM == err_code)
        {
            ri_yield();
        }
    } while ( (RD_SUCCESS != err_code) && ! (RD_ERROR_TIMEOUT & err_code));

    return err_code;
}
/** @} */
