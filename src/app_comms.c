#include "app_config.h"
#include "app_comms.h"
#include "app_heartbeat.h"
#include "app_led.h"
#include "app_sensor.h"
#include "ruuvi_boards.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_task_advertisement.h"
#include "ruuvi_task_communication.h"
#include "ruuvi_task_gatt.h"
#include "ruuvi_task_nfc.h"
#include <stdio.h>

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

static bool m_config_enabled_on_current_conn; //!< This connection has config enabled.
static bool m_config_enabled_on_next_conn;    //!< Next connection has config enabled.

/**
 * @brief Allow configuration commands on connection.
 *
 * @param[in] enable True to enable configuration on connection, false to disable.
 */
static rd_status_t enable_config_on_this_conn (const bool enable)
{
    m_config_enabled_on_current_conn = enable;
    return RD_SUCCESS;
}

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
    err_code |= ri_timer_start (m_comm_timer, APP_FAST_ADV_TIME_MS, &m_mode_ops);
    return err_code;
}

static rd_status_t prepare_mode_change (const mode_changes_t * p_change)
{
    rd_status_t err_code = RD_SUCCESS;

    if (p_change->switch_to_normal)
    {
        err_code |= timed_switch_to_normal_mode();
    }

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
        app_comms_bleadv_send_count_set (1);
        p_change->switch_to_normal = 0;
    }

    if (p_change->disable_config)
    {
        (void) enable_config_on_this_conn (false);
        p_change->disable_config = 0;
    }
}

/**
 * @brief Calculate how many times advertisements must be repeated
 *        to send at the initial fast rate.
 */
static uint8_t initial_adv_send_count (void)
{
    uint8_t num_sends = (APP_HEARTBEAT_INTERVAL_MS / 100U);

    if (0 == num_sends)
    {
        num_sends = 1;
    }

    if (APP_COMM_ADV_REPEAT_FOREVER == num_sends)
    {
        num_sends = APP_COMM_ADV_REPEAT_FOREVER - 1;
    }

    return num_sends;
}

#if APP_GATT_ENABLED

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
        // Parse message type
        const uint8_t * const raw_message = (uint8_t *) p_data;
        re_type_t type = raw_message[RE_STANDARD_DESTINATION_INDEX];

        // Route message to proper handler
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

        // Resume heartbeat processing.
        err_code |= app_heartbeat_start();
    }

    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
}

#ifndef CEEDLING
static
#endif
void handle_gatt (void * p_data, uint16_t data_len)
{
    handle_comms (&rt_gatt_send_asynchronous, p_data, data_len);
}

/** @brief Callback when GATT is connected" */
#ifndef CEEDLING
static
#endif
void on_gatt_connected_isr (void * p_data, size_t data_len)
{
    // Stop advertising for GATT
    rt_gatt_disable();

    if (m_config_enabled_on_next_conn)
    {
        m_config_enabled_on_next_conn = false;
        (void) enable_config_on_this_conn (true);
    }
}

/** @brief Callback when GATT is disconnected" */
#ifndef CEEDLING
static
#endif
void on_gatt_disconnected_isr (void * p_data, size_t data_len)
{
    // Start advertising for GATT
    rt_gatt_enable();
    m_config_enabled_on_next_conn = false;
    (void) enable_config_on_this_conn (false);
    app_led_activity_set (RB_LED_ACTIVITY);
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
    err_code |= ri_scheduler_event_put (p_data, (uint16_t) data_len, &handle_gatt);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
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
/**
 * @brief Callback when NFC is connected.
 *
 * Enables configuration on NFC connection and on next connection.
 */
#ifndef CEEDLING
static
#endif
void on_nfc_connected_isr (void * p_data, size_t data_len)
{
    m_config_enabled_on_next_conn = true;
    (void) enable_config_on_this_conn (true);
    app_comms_configure_next_enable();
}

/** @brief Callback when NFC is disconnected" */
#ifndef CEEDLING
static
#endif
void on_nfc_disconnected_isr (void * p_data, size_t data_len)
{
    (void) enable_config_on_this_conn (false);
}

#endif


static rd_status_t dis_init (ri_comm_dis_init_t * const p_dis)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_com_get_mac_str (p_dis->deviceaddr, sizeof (p_dis->deviceaddr));
    err_code |= rt_com_get_id_str (p_dis->deviceid, sizeof (p_dis->deviceid));
    snprintf (p_dis->fw_version, sizeof (p_dis->fw_version), APP_FW_NAME);
    snprintf (p_dis->hw_version, sizeof (p_dis->hw_version), "Check PCB");
    snprintf (p_dis->manufacturer, sizeof (p_dis->manufacturer), RB_MANUFACTURER_STRING);
    snprintf (p_dis->model, sizeof (p_dis->model), RB_MODEL_STRING);
    return err_code;
}

static rd_status_t adv_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
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
    err_code |= ri_adv_type_set (NONCONNECTABLE_NONSCANNABLE);
    app_comms_bleadv_send_count_set (initial_adv_send_count());
    m_mode_ops.switch_to_normal = 1;
    prepare_mode_change (&m_mode_ops);
    return err_code;
}

rd_status_t app_comms_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_comm_dis_init_t dis = {0};
    err_code |= ri_radio_init (APP_MODULATION);
    err_code |= ri_timer_create (&m_comm_timer, RI_TIMER_MODE_SINGLE_SHOT,
                                 &comm_mode_change_isr);

    if (RD_SUCCESS == err_code)
    {
        err_code |= dis_init (&dis);
#if APP_NFC_ENABLED
        err_code |= rt_nfc_init (&dis);
        rt_nfc_set_on_connected_isr (&on_nfc_connected_isr);
        rt_nfc_set_on_disconn_isr (&on_nfc_disconnected_isr);
#endif
#if APP_ADV_ENABLED
        err_code |= adv_init();
#endif
#if APP_GATT_ENABLED
        char name[SCAN_RSP_NAME_MAX_LEN + 1] = {0};
        ble_name_string_create (name, sizeof (name));
        err_code |= rt_gatt_init (name);
        err_code |= rt_gatt_dis_init (&dis);
        err_code |= rt_gatt_nus_init();
        rt_gatt_set_on_connected_isr (&on_gatt_connected_isr);
        rt_gatt_set_on_disconn_isr (&on_gatt_disconnected_isr);
        rt_gatt_set_on_received_isr (&on_gatt_data_isr);
        err_code |= rt_gatt_enable();
#endif
    }

    return err_code;
}

rd_status_t app_comms_configure_next_enable (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_gatt_dfu_init();
    m_mode_ops.disable_config = 1;
    err_code |= ri_timer_stop (m_comm_timer);
    err_code |= ri_timer_start (m_comm_timer, APP_CONFIG_ENABLED_TIME_MS, &m_mode_ops);
    m_config_enabled_on_next_conn = true;
    err_code |= app_led_activity_set (RB_LED_CONFIG_ENABLED);
    return err_code;
}

/** @} */
