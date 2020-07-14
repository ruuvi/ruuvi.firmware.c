#include "unity.h"

#include "app_config.h"
#include "app_comms.h"
#include "ruuvi_boards.h"
#include "mock_ruuvi_interface_communication_ble_advertising.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_task_advertisement.h"
#include "mock_ruuvi_task_communication.h"
#include "mock_ruuvi_task_gatt.h"
#include "mock_ruuvi_task_nfc.h"

void setUp (void)
{
}

void tearDown (void)
{
}

static void test_dis_init (ri_comm_dis_init_t * const p_dis)
{
    rt_com_get_mac_str_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_com_get_mac_str_ReturnArrayThruPtr_mac_str ("AA:BB:CC:DD:EE:FF", 18);
    rt_com_get_id_str_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_com_get_id_str_ReturnArrayThruPtr_id_str ("00:11:22:33:44:55:66:77", 24);
    snprintf (p_dis->deviceaddr, sizeof (p_dis->deviceaddr), "AA:BB:CC:DD:EE:FF");
    snprintf (p_dis->deviceid, sizeof (p_dis->deviceid), "00:11:22:33:44:55:66:77");
    snprintf (p_dis->fw_version, sizeof (p_dis->fw_version), APP_FW_NAME);
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

void test_app_comms_init_ok (void)

{
    ri_comm_dis_init_t dis = {0};
    // Allow switchover to extended / 2 MBPS comms.
    ri_radio_init_ExpectAndReturn (APP_MODULATION, RD_SUCCESS);
    test_dis_init (&dis);
#if APP_NFC_ENABLED
    rt_nfc_init_ExpectWithArrayAndReturn (&dis, 1, RD_SUCCESS);
#endif
#if APP_ADV_ENABLED
    rt_adv_init_ExpectAndReturn (&adv_settings, RD_SUCCESS);
    ri_adv_type_set_ExpectAndReturn (NONCONNECTABLE_NONSCANNABLE, RD_SUCCESS);
#endif
#if APP_GATT_ENABLED
    uint64_t address = 0xAABBCCDDEEFF01A0;
    ri_radio_address_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_radio_address_get_ReturnThruPtr_address (&address);
    rt_gatt_init_ExpectAndReturn ("Ruuvi 01A0", RD_SUCCESS);
    rt_gatt_dfu_init_ExpectAndReturn (RD_SUCCESS);
    rt_gatt_dis_init_ExpectWithArrayAndReturn (&dis, 1, RD_SUCCESS);
    rt_gatt_nus_init_ExpectAndReturn (RD_SUCCESS);
    rt_gatt_set_on_connected_isr_Expect (&on_gatt_connected_isr);
    rt_gatt_set_on_disconn_isr_Expect (&on_gatt_disconnected_isr);
    rt_gatt_enable_ExpectAndReturn (RD_SUCCESS);
#endif
    rd_status_t err_code = app_comms_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}


void test_on_gatt_connected (void)
{
    rt_gatt_disable_ExpectAndReturn (RD_SUCCESS);
    on_gatt_connected_isr (NULL, 0);
}

void test_on_gatt_disconnected (void)
{
    rt_gatt_enable_ExpectAndReturn (RD_SUCCESS);
    on_gatt_disconnected_isr (NULL, 0);
}