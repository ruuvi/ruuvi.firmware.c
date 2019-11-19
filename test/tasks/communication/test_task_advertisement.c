/**
 * @file test_task_advertisement.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-11-18
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "unity.h"

#include "application_config.h"
#include "task_advertisement.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"

#include "mock_ruuvi_interface_communication_ble4_advertising.h"
#include "mock_ruuvi_interface_log.h"

static ruuvi_interface_communication_t m_adv_channel =
{
    .send = ruuvi_interface_communication_ble4_advertising_send,
    .read = ruuvi_interface_communication_ble4_advertising_receive,
    .init = ruuvi_interface_communication_ble4_advertising_init,
    .uninit = ruuvi_interface_communication_ble4_advertising_uninit
};

static ruuvi_interface_communication_t m_uninit_channel =
{
    .send = NULL,
    .read = NULL,
    .init = NULL,
    .uninit = NULL
};


void setUp (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_init_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_init_ReturnArrayThruPtr_channel (
        &m_adv_channel, 1);
    int8_t power = APPLICATION_ADVERTISING_POWER_DBM;
    ruuvi_interface_communication_ble4_advertising_tx_interval_set_ExpectAndReturn (
        APPLICATION_ADVERTISING_INTERVAL_MS, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_tx_power_set_ExpectWithArrayAndReturn (
        &power, sizeof (power), RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        NONCONNECTABLE_NONSCANNABLE, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_manufacturer_id_set_ExpectAndReturn (
        RUUVI_BOARD_BLE_MANUFACTURER_ID, RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_init();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (task_advertisement_is_init());
}

void tearDown (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_uninit_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_uninit_ReturnArrayThruPtr_channel (
        &m_uninit_channel, 1);
    err_code = task_advertisement_uninit();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (!task_advertisement_is_init());
}

/**
 * @brief Initializes data advertising.
 *
 * The function setups advertisement interval, advertisement power, advertisement type,
 * manufacturer ID for manufacturer specific data according to constants in
 * application_config.h and ruuvi_boards.h
 * After calling this function advertisement data can be queued into advertisement buffer.
 * You should queue at least one message into buffer before starting advertising.
 *
 * @retval @c RUUVI_DRIVER_SUCCESS on success.
 * @retval @c RUUVI_DRIVER_ERROR_INVALID_STATE if advertising is already initialized.
 * @®etval @c RUUVI_DRIVER_ERROR_INVALID_PARAM if configuration constant is invalid.
 */
void test_task_advertisement_init_ok (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_init_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_init_ReturnArrayThruPtr_channel (
        &m_adv_channel, 1);
    int8_t power = APPLICATION_ADVERTISING_POWER_DBM;
    ruuvi_interface_communication_ble4_advertising_tx_interval_set_ExpectAndReturn (
        APPLICATION_ADVERTISING_INTERVAL_MS, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_tx_power_set_ExpectWithArrayAndReturn (
        &power, sizeof (power), RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        NONCONNECTABLE_NONSCANNABLE, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_manufacturer_id_set_ExpectAndReturn (
        RUUVI_BOARD_BLE_MANUFACTURER_ID, RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_init();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (task_advertisement_is_init());
}

void test_task_advertisement_init_invalid_interval (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_init_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_init_ReturnArrayThruPtr_channel (
        &m_adv_channel, 1);
    ruuvi_interface_communication_ble4_advertising_tx_interval_set_ExpectAndReturn (
        APPLICATION_ADVERTISING_INTERVAL_MS, RUUVI_DRIVER_ERROR_INVALID_PARAM);
    int8_t power = APPLICATION_ADVERTISING_POWER_DBM;
    ruuvi_interface_communication_ble4_advertising_tx_power_set_ExpectWithArrayAndReturn (
        &power, sizeof (power), RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        NONCONNECTABLE_NONSCANNABLE, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_manufacturer_id_set_ExpectAndReturn (
        RUUVI_BOARD_BLE_MANUFACTURER_ID, RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_PARAM == err_code);
    TEST_ASSERT (!task_advertisement_is_init());
}

void test_task_advertisement_init_invalid_power (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_init_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_init_ReturnArrayThruPtr_channel (
        &m_adv_channel, 1);
    int8_t power = APPLICATION_ADVERTISING_POWER_DBM;
    ruuvi_interface_communication_ble4_advertising_tx_interval_set_ExpectAndReturn (
        APPLICATION_ADVERTISING_INTERVAL_MS, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_tx_power_set_ExpectWithArrayAndReturn (
        &power, sizeof (power), RUUVI_DRIVER_ERROR_INVALID_PARAM);
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        NONCONNECTABLE_NONSCANNABLE, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_manufacturer_id_set_ExpectAndReturn (
        RUUVI_BOARD_BLE_MANUFACTURER_ID, RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_PARAM == err_code);
    TEST_ASSERT (!task_advertisement_is_init());
}

void test_task_advertisement_init_invalid_type (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_init_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_init_ReturnArrayThruPtr_channel (
        &m_adv_channel, 1);
    int8_t power = APPLICATION_ADVERTISING_POWER_DBM;
    ruuvi_interface_communication_ble4_advertising_tx_interval_set_ExpectAndReturn (
        APPLICATION_ADVERTISING_INTERVAL_MS, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_tx_power_set_ExpectWithArrayAndReturn (
        &power, sizeof (power), RUUVI_DRIVER_SUCCESS);
    RUUVI_INTERFACE_COMMUNICATION_BLE4_ADVERTISING_TYPE type = NONCONNECTABLE_NONSCANNABLE;
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (type,
            RUUVI_DRIVER_ERROR_INVALID_PARAM);
    ruuvi_interface_communication_ble4_advertising_manufacturer_id_set_ExpectAndReturn (
        RUUVI_BOARD_BLE_MANUFACTURER_ID, RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_PARAM == err_code);
    TEST_ASSERT (!task_advertisement_is_init());
}

void test_task_advertisement_init_twice (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_advertisement_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
    TEST_ASSERT (task_advertisement_is_init());
}

/**
 * @brief Uninitializes data advertising.
 *
 * Can be called even if advertising was not initialized.
 * Does not uninitialize timers even if they were initialized for advertisement module.
 * Clears previous advertisement data if there was any.
 *
 * @retval @c RUUVI_DRIVER_SUCCESS on success
 * @retval error code from stack on error
 */
void test_task_advertisement_uninit (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_uninit_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_uninit_ReturnArrayThruPtr_channel (
        &m_uninit_channel, 1);
    err_code = task_advertisement_uninit();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (!task_advertisement_is_init());
}

/**
 * @brief Starts advertising.
 *
 * Before this function is called, you must initialize advertising and should
 * set some data into advertisement buffer. Otherwise empty advertisement packets are sent.
 * It might be desirable to send empty advertisement payloads as GATT connection
 * advertisements piggyback on data advertisements.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if advertising is not initialized.
 * returns error code from stack on error
 *
 */
void test_task_advertisement_start_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_start_ExpectAndReturn (
        RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_start();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_advertisement_start_not_init (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_advertisement_start();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Stops advertising.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval error code from stack on error
 */
void test_task_advertisement_stop_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_stop_ExpectAndReturn (
        RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_stop();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

/** @brief Send given message as a BLE advertisement.
 *
 *  This function configures the primary advertisement packet with the flags and manufacturer specific data.
 *  Payload of the msg will be sent as the manufacturer specific data payload.
 *  Manufacturer ID is defined by RUUVI_BOARD_BLE_MANUFACTURER_ID in ruuvi_boards.h.
 *
 *  If the device is connectable, call @ref task_advertisement_connectability to setup the
 *  scan response and flags to advertise connectability.
 *
 *  @param[in] msg message to be sent as manufacturer specific data payload
 *  @retval    RUUVI_DRIVER_ERROR_NULL if msg is NULL
 *  @retval    RUUVI_DRIVER_ERROR_INVALID_STATE if advertising isn't initialized or started.
 *  @retval    RUUVI_DRIVER_ERROR_DATA_SIZE if payload size is larger than 24 bytes
 *  @retval    error code from stack on other error.
 */

void test_task_advertisement_send_data_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t message;
    message.data_length = 10;
    ruuvi_interface_communication_ble4_advertising_send_ExpectWithArrayAndReturn (&message,
            sizeof (message), RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_send_data (&message);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_advertisement_send_data_null (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_advertisement_send_data (NULL);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_NULL == err_code);
}

void test_task_advertisement_send_data_not_init (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t message;
    message.data_length = 10;
    err_code = task_advertisement_send_data (&message);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_advertisement_send_data_excess_size_25 (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t message;
    message.data_length = 25;
    err_code = task_advertisement_send_data (&message);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_DATA_SIZE == err_code);
}

/** @brief Start advertising BLE GATT connection
 *
 *  This function configures the primary advertisement to be SCANNABLE_CONNECTABLE and
 *  sets up a scan response which has given device name (max 10 characters + NULL)
 *  and UUID of Nordic UART Service.
 *
 *  Be sure to configure the GATT
 *
 *  @param[in] enable true to enable connectability, false to disable.
 *  @param[in] name NULL-terminated string representing device name, max 10 Chars + NULL.
 *  @retval    RUUVI_DRIVER_SUCCESS if operation was finished as expected.
 *  @retval    RUUVI_DRIVER_ERROR_NULL if name is NULL and trying to enable the scan response
 *  @retval    RUUVI_DRIVER_ERROR_INVALID_STATE if advertising isn't initialized or started.
 *  @retval    RUUVI_DRIVER_ERROR_INVALID_LENGTH if name size exceeds 10 bytes + NULL
 *  @retval    error code from stack on other error.
 */
void test_task_advertisement_connectability_set_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    const char name[] = "Ceedling";
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        CONNECTABLE_SCANNABLE, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_scan_response_setup_ExpectAndReturn (name,
            true, RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_connectability_set (true, "Ceedling");
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_advertisement_connectability_set_not_init (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_advertisement_connectability_set (true, "Ceedling");
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_advertisement_connectability_set_error_long_name (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_advertisement_connectability_set (true, "Ceedling spawnling");
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_LENGTH == err_code);
}

void test_task_advertisement_connectability_set_error_null (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_advertisement_connectability_set (true, NULL);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_NULL == err_code);
}

void test_task_advertisement_connectability_set_off (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        NONCONNECTABLE_NONSCANNABLE, RUUVI_DRIVER_SUCCESS);
    err_code = task_advertisement_connectability_set (false, NULL);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}
