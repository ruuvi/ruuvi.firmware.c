#include "app_config.h"
#include "app_comms.h"
#include "app_heartbeat.h"
#include "app_led.h"
#include "app_sensor.h"
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
#ifndef CEEDLING
static
#endif
bool m_config_enabled_on_curr_conn; //!< This connection has config enabled.
#ifndef CEEDLING
static
#endif
bool m_config_enabled_on_next_conn; //!< Next connection has config enabled.
#endif

#ifndef CEEDLING
typedef struct
{
    unsigned int switch_to_normal : 1; //!< Stop initial fast advertising.
    unsigned int disable_config : 1;   //!< Disable configuration mode.
} mode_changes_t;
#endif

static uint8_t m_bleadv_repeat_count; //!< Number of times to repeat advertisement.

#ifndef CEEDLING
static
#endif
ri_timer_id_t m_comm_timer;    //!< Timer for communication mode changes.

#ifndef CEEDLING
static
#endif
mode_changes_t m_mode_ops;     //!< Pending mode changes.

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

static void handle_comms (const ri_comm_xfer_fp_t reply_fp, void * p_data,
                          size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;

    if (NULL == p_data)
    {
        err_code |= RD_ERROR_NULL;
    }
    else if (data_len < RE_STANDARD_MESSAGE_LENGTH)
    {
        err_code |= RD_ERROR_INVALID_PARAM;
    }
    else
    {
        // Stop heartbeat processing.
        err_code |= app_heartbeat_stop();
        // Switch GATT to faster params.
        err_code |= ri_gatt_params_request (RI_GATT_TURBO, CONN_PARAM_UPDATE_DELAY_MS);
        // Parse message type.
        const uint8_t * const raw_message = (uint8_t *) p_data;
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

    if (enable)
    {
        app_led_configuration_signal (true);
    }
    else
    {
        app_led_configuration_signal (false);
    }

    return err_code;
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

#ifndef CEEDLING
static
#endif
void handle_gatt_data (void * p_data, uint16_t data_len)
{
    handle_comms (&rt_gatt_send_asynchronous, p_data, data_len);
}

/**
 * @brief Disable advertising for GATT connection and setup current connection.
 */
#ifndef CEEDLING
static
#endif
void handle_gatt_connected (void * p_data, uint16_t data_len)
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
#ifndef CEEDLING
static
#endif
void on_gatt_connected_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len,
                                        &handle_gatt_connected);
    // Configuration will be disabled on disconnection, no need to trigger timer action.
    m_mode_ops.disable_config = 0;
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

#ifndef CEEDLING
static
#endif
void handle_gatt_disconnected (void * p_data, uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    config_cleanup_on_disconnect();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/** @brief Callback when GATT is disconnected" */
#ifndef CEEDLING
static
#endif
void on_gatt_disconnected_isr (void * p_data, size_t data_len)
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
#ifndef CEEDLING
static
#endif
void on_gatt_data_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len, &handle_gatt_data);
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
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

#ifndef CEEDLING
static
#endif
void handle_nfc_connected (void * p_data, uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    // Kick BLE connection to not allow it to configure tag
    app_comms_ble_uninit();
    m_config_enabled_on_next_conn = true;
    config_setup_on_this_conn();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

#ifndef CEEDLING
static
#endif
void on_nfc_connected_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len, &handle_nfc_connected);
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

#ifndef CEEDLING
static
#endif
void handle_nfc_disconnected (void * p_data, uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    config_cleanup_on_disconnect();
    err_code |= app_comms_configure_next_enable();
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

/** @brief Callback when NFC is disconnected" */
#ifndef CEEDLING
static
#endif
void on_nfc_disconnected_isr (void * p_data, size_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len,
                                        &handle_nfc_disconnected);
    RD_ERROR_CHECK (err_code, RD_SUCCESS);
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

#ifndef CEEDLING
static
#endif
void handle_config_disable (void * p_data, uint16_t data_len)
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
#endif
    return err_code;
}

#ifndef CEEDLING
static
#endif
void comm_mode_change_isr (void * const p_context)
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
