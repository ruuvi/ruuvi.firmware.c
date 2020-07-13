#include "app_config.h"
#include "app_comms.h"
#include "ruuvi_boards.h"
#include "ruuvi_interface_communication_radio.h"
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
 * @date 2020-04-29
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Typical usage:
 * @code{.c}
 * TODO
 * @endcode
 */

#if APP_GATT_ENABLED
/** @brief Callback when GATT is connected" */
#ifndef CEEDLING
static
#endif
void on_gatt_connected_isr (void * p_data, size_t data_len)
{
    // Stop advertising for GATT
    rt_gatt_disable();
}

/** @brief Callback when GATT is disconnected" */
#ifndef CEEDLING
static
#endif
void on_gatt_disconnected_isr (void * p_data, size_t data_len)
{
    // Start advertising for GATT
    rt_gatt_enable();
}
// TODO: On data received

static rd_status_t ble_name_string_create (char * const name_str, const size_t name_len)
{
    uint64_t radio_addr = 0;
    rd_status_t err_code = ri_radio_address_get (&radio_addr);
    radio_addr &= 0xFFFF;
    snprintf (name_str, name_len, "%s %04X", RB_BLE_NAME_STRING, (uint16_t) radio_addr);
    return err_code;
}

#endif //!< if GATT ENABLED


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
    return err_code;
}

rd_status_t app_comms_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_comm_dis_init_t dis = {0};
    // Allow switchover to extended / 2 MBPS comms.
    err_code |= ri_radio_init (APP_MODULATION);

    if (RD_SUCCESS == err_code)
    {
        err_code |= dis_init (&dis);
#if APP_NFC_ENABLED
        err_code |= rt_nfc_init (&dis);
#endif
#if APP_ADV_ENABLED
        err_code |= adv_init();
#endif
#if APP_GATT_ENABLED
        char name[SCAN_RSP_NAME_MAX_LEN + 1] = {0};
        ble_name_string_create (name, sizeof (name));
        err_code |= rt_gatt_init (name);
        err_code |= rt_gatt_dfu_init();
        err_code |= rt_gatt_dis_init (&dis);
        err_code |= rt_gatt_nus_init();
        rt_gatt_set_on_connected_isr (&on_gatt_connected_isr);
        rt_gatt_set_on_disconn_isr (&on_gatt_disconnected_isr);
        err_code |= rt_gatt_enable();
#endif
    }

    return err_code;
}

/** @} */
