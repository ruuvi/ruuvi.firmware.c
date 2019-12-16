#include "unity.h"

#include "application_config.h"

#include "task_gatt.h"


#include "ruuvi_driver_error.h"

#include "mock_task_advertisement.h"
#include "mock_ruuvi_interface_atomic.h"
#include "mock_ruuvi_interface_communication_ble4_advertising.h"
#include "mock_ruuvi_interface_communication_ble4_gatt.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_library_ringbuffer.h"

static uint32_t send_count = 0;
static uint32_t read_count = 0;
static bool m_con_cb;
static bool m_discon_cb;
static bool m_tx_cb;
static bool m_rx_cb;
static const char m_name[] = "Ceedling";

#define SEND_COUNT_MAX (10U)

ruuvi_driver_status_t mock_send (ruuvi_interface_communication_message_t * const p_msg)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    static bool extra_error = false;

    if (send_count < SEND_COUNT_MAX)
    {
        send_count++;
    }
    else
    {
        if (!extra_error)
        {
            err_code |= RUUVI_DRIVER_ERROR_RESOURCES;
            extra_error = true;
        }
        else
        {
            err_code |= RUUVI_DRIVER_ERROR_INTERNAL;
        }
    }

    return err_code;
}

ruuvi_driver_status_t mock_read (ruuvi_interface_communication_message_t * const p_msg)
{
    read_count++;
    return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t mock_uninit (ruuvi_interface_communication_t * const p_channel)
{
    memset (p_channel, 0, sizeof (ruuvi_interface_communication_t));
    return RUUVI_DRIVER_SUCCESS;
}

void on_con_isr (void * p_data, size_t data_len)
{
    m_con_cb = true;
}

void on_discon_isr (void * p_data, size_t data_len)
{
    m_discon_cb = true;
}

void on_rx_isr (void * p_data, size_t data_len)
{
    m_rx_cb = true;
}

void on_tx_isr (void * p_data, size_t data_len)
{
    m_tx_cb = true;
}

ruuvi_driver_status_t mock_init (ruuvi_interface_communication_t * const p_channel)
{
    p_channel->send   = mock_send;
    p_channel->read   = mock_read;
    p_channel->uninit = mock_uninit;
    p_channel->init   = mock_init;
    p_channel->on_evt = task_gatt_on_nus_isr;
}

static ruuvi_interface_communication_t m_mock_gatt;
#if 0
=
{
    .send   = mock_send,
    .read   = mock_read,
    .init   = mock_init,
    .uninit = mock_uninit,
    .on_evt = mock_on_evt
};
#endif

void setUp (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    task_advertisement_is_init_ExpectAndReturn (true);
    ruuvi_interface_communication_ble4_gatt_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    err_code |= task_gatt_init (m_name);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (task_gatt_is_init());
}

void tearDown (void)
{
    memset (&m_mock_gatt, 0, sizeof (ruuvi_interface_communication_t));
    send_count = 0;
    read_count = 0;
    m_con_cb = false;
    m_discon_cb = false;
    m_tx_cb = false;
    m_rx_cb = false;
    task_gatt_mock_state_reset();
    TEST_ASSERT (!task_gatt_is_init());
}





/**
 * @brief Initialize Device Firmware Update service
 *
 * GATT must be initialized before calling this function, and once initialized the DFU
 * service cannot be uninitialized.
 *
 * Call will return successfully even if the device doesn't have useable bootloader, however
 * program will reboot if user tries to enter bootloader in that case.
 *
 * To use the DFU service advertisement module must send connectable (and preferably scannable) advertisements.
 *
 * @retval RUUVI_DRIVER_SUCCESS GATT was initialized successfully
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE DFU was already initialized or GATT is not initialized
 */
void test_task_gatt_dfu_init_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_gatt_dfu_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    err_code = task_gatt_dfu_init();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_gatt_dfu_init_no_gatt (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    tearDown();
    err_code = task_gatt_dfu_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_gatt_dfu_init_twice (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    test_task_gatt_dfu_init_ok();
    err_code = task_gatt_dfu_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Initialize Device Information Update service
 *
 * GATT must be initialized before calling this function, and once initialized the DIS
 * service cannot be uninitialized.
 *
 * DIS service lets user read basic information, such as firmware version and hardware model over GATT in a standard format.
 *
 * To use the DIS service advertisement module must send connectable (and preferably scannable) advertisements.
 *
 * @param[in] dis structure containing data to be copied into DIS, can be freed after call finishes.
 *
 * @retval RUUVI_DRIVER_SUCCESS GATT was initialized successfully
 * @retval RUUVI_DRIVER_ERROR_NULL if given NULL as the information.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE DIS was already initialized or GATT is not initialized
 */
void test_task_gatt_dis_init_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_gatt_dis_init_t dis = {0};
    ruuvi_interface_communication_ble4_gatt_dis_init_ExpectAndReturn (&dis,
            RUUVI_DRIVER_SUCCESS);
    err_code = task_gatt_dis_init (&dis);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_gatt_dis_init_twice (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_gatt_dis_init_t dis = {0};
    test_task_gatt_dis_init_ok();
    err_code = task_gatt_dis_init (&dis);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_gatt_dis_init_null (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_gatt_dis_init (NULL);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_NULL == err_code);
}

void test_task_gatt_dis_init_no_gatt (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_gatt_dis_init_t dis = {0};
    err_code = task_gatt_dis_init (&dis);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Initialize Nordic UART Service
 *
 * GATT must be initialized before calling this function, and once initialized the NUS
 * service cannot be uninitialized.
 *
 * NUS service lets user do bidirectional communication with the application.
 *
 * To use the NUS service advertisement module must send connectable (and preferably scannable) advertisements.
 *
 *
 * @retval RUUVI_DRIVER_SUCCESS GATT was initialized successfully
 * @retval RUUVI_DRIVER_ERROR_NULL if given NULL as the information.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE DIS was already initialized or GATT is not initialized
 *
 * @note To actually use the data in application, user must setup at least data received callback with @ref task_gatt_set_on_received_isr
 */
void test_task_gatt_nus_init_ok()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    mock_init (&m_mock_gatt);
    ruuvi_interface_communication_ble4_gatt_nus_init_ExpectAnyArgsAndReturn (
        RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_gatt_nus_init_ReturnArrayThruPtr_channel (
        &m_mock_gatt, 1);
    err_code = task_gatt_nus_init ();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

/**
 * @brief Initialize GATT. Must be called as a first function in task_gatt.
 *
 * After calling this function underlying software stack is ready to setup GATT services.
 *
 * @param[in] Full name of device to be advertised in scan responses. Maximum 11 chars + trailing NULL. Must not be NULL, 0-length string is valid.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_NULL if name is NULL (use 0-length string instead)
* @retval RUUVI_DRIVER_ERROR_INVALID_LENGTH if name is longer than @ref SCAN_RSP_NAME_MAX_LEN
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if GATT is already initialized or advertisements are not initialized.
 *
 */
void test_task_gatt_init_ok (void)
{
    // no implementation needed, done ins setup/teardown
}

void test_task_gatt_init_twice (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    task_advertisement_is_init_ExpectAndReturn (true);
    err_code |= task_gatt_init (m_name);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_gatt_init_null (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= task_gatt_init (NULL);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_NULL == err_code);
}

void test_task_gatt_init_too_long_name (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    tearDown();
    // Scan response has space for extra null already, so add + 2
    char toolong[SCAN_RSP_NAME_MAX_LEN + 2] = {0};

    for (size_t ii = 0; ii < sizeof (toolong); ii++)
    {
        toolong[ii] = 'A';
    }

    task_advertisement_is_init_ExpectAndReturn (true);
    err_code |= task_gatt_init (toolong);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_LENGTH == err_code);
    TEST_ASSERT (!task_gatt_is_init());
}

void test_task_gatt_init_max_len_name (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    tearDown();
    // Scan response has space for extra null already, so add + 2
    char maxlen[SCAN_RSP_NAME_MAX_LEN + 1] = {0};

    for (size_t ii = 0; ii < sizeof (maxlen); ii++)
    {
        maxlen[ii] = 'A';
    }

    maxlen[sizeof (maxlen) - 1] = '\0';
    task_advertisement_is_init_ExpectAndReturn (true);
    ruuvi_interface_communication_ble4_gatt_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    err_code |= task_gatt_init (maxlen);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (task_gatt_is_init());
}

/**
 * @brief Start advertising GATT connection to devices.
 *
 * Calling this function is not enough to let users to connect, you must also update advertised data
 * to add the scan response to data being advertised. This makes sure that advertised data stays valid.
 * This function has no effect if called while already enabled.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if GATT is not initialized.
 */
void test_task_gatt_enable_ok_no_nus()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_scan_response_setup_ExpectAndReturn (
        m_name,
        false, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        CONNECTABLE_SCANNABLE, RUUVI_DRIVER_SUCCESS);
    err_code |= task_gatt_enable ();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_gatt_enable_ok_with_nus()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    test_task_gatt_nus_init_ok();
    ruuvi_interface_communication_ble4_advertising_scan_response_setup_ExpectAndReturn (
        m_name,
        true, RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        CONNECTABLE_SCANNABLE, RUUVI_DRIVER_SUCCESS);
    err_code |= task_gatt_enable ();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_gatt_enable_gatt_not_init()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    tearDown();
    err_code |= task_gatt_enable ();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Stop advertising GATT connection to devices.
 *
 * Calling this function is not enough to stop advertising connection, you must also update advertised data
 * to remove the scan response from data being advertised. This makes sure that advertised data stays valid.
 * This function has not effect if called while already disabled
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if GATT is not initialized.
 */
void test_task_gatt_disable_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_ble4_advertising_type_set_ExpectAndReturn (
        NONCONNECTABLE_NONSCANNABLE, RUUVI_DRIVER_SUCCESS);
    err_code |= task_gatt_disable();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_gatt_disable_not_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    tearDown();
    err_code |= task_gatt_disable();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Send given message via NUS
 *
 * This function queues a message to be sent and returns immediately.
 * There is no guarantee on when the data is actually sent, and
 * there is no acknowledgement or callback after the data has been sent.
 *
 * @return RUUVI_DRIVER_SUCCESS if data was placed in send buffer.
 * @retval RUUVI_DRIVER_ERROR_NULL if NULL was tried to send.
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if NUS is not connected.
 * @retval RUUVI_DRIVER_ERROR_NO_MEM if transmit buffer is full.
 * @return error code from stack on error
 *
 */
void test_task_gatt_send_asynchronous_ok()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t msg = { 0 };
    msg.data_length = 11;
    test_task_gatt_nus_init_ok();
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_CONNECTED,
                          NULL, 0);
    ruuvi_interface_log_Ignore();
    ruuvi_interface_log_hex_Ignore();
    ruuvi_interface_log_Ignore();
    err_code = task_gatt_send_asynchronous (&msg);
    TEST_ASSERT (1 == send_count);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_gatt_send_asynchronous_null()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t msg = { 0 };
    msg.data_length = 11;
    test_task_gatt_nus_init_ok();
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_CONNECTED,
                          NULL, 0);
    err_code = task_gatt_send_asynchronous (NULL);
    TEST_ASSERT (0 == send_count);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_NULL == err_code);
}

void test_task_gatt_send_asynchronous_no_nus()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t msg = { 0 };
    msg.data_length = 11;
    test_task_gatt_nus_init_ok();
    err_code = task_gatt_send_asynchronous (&msg);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
    TEST_ASSERT (0 == send_count);
}

void test_task_gatt_send_asynchronous_no_mem()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t msg = { 0 };
    msg.data_length = 11;
    test_task_gatt_nus_init_ok();
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_CONNECTED,
                          NULL, 0);

    for (uint32_t ii = 0; ii <= SEND_COUNT_MAX; ii++)
    {
        ruuvi_interface_log_Ignore();
        ruuvi_interface_log_hex_Ignore();
        ruuvi_interface_log_Ignore();
        err_code |= task_gatt_send_asynchronous (&msg);
    }

    TEST_ASSERT (SEND_COUNT_MAX == send_count);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_NO_MEM == err_code);
}

void test_task_gatt_send_asynchronous_unknown_error()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t msg = { 0 };
    msg.data_length = 11;
    test_task_gatt_nus_init_ok();
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_CONNECTED,
                          NULL, 0);

    for (uint32_t ii = 0; ii < SEND_COUNT_MAX; ii++)
    {
        ruuvi_interface_log_Ignore();
        ruuvi_interface_log_hex_Ignore();
        ruuvi_interface_log_Ignore();
        err_code = task_gatt_send_asynchronous (&msg);
    }

    ruuvi_interface_error_to_string_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    err_code = task_gatt_send_asynchronous (&msg);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INTERNAL == err_code);
}

void test_task_gatt_callbacks_ok()
{
    test_task_gatt_nus_init_ok();
    task_gatt_set_on_received_isr (on_rx_isr);
    task_gatt_set_on_sent_isr (on_tx_isr);
    task_gatt_set_on_connected_isr (on_con_isr);
    task_gatt_set_on_disconn_isr (on_discon_isr);
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_CONNECTED,
                          NULL, 0);
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_DISCONNECTED,
                          NULL, 0);
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_SENT,
                          NULL, 0);
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_RECEIVED,
                          NULL, 0);
    TEST_ASSERT (m_rx_cb);
    TEST_ASSERT (m_tx_cb);
    TEST_ASSERT (m_con_cb);
    TEST_ASSERT (m_discon_cb);
}
void test_task_gatt_callbacks_null()
{
    test_task_gatt_nus_init_ok();
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_CONNECTED,
                          NULL, 0);
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_DISCONNECTED,
                          NULL, 0);
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_SENT,
                          NULL, 0);
    task_gatt_on_nus_isr (RUUVI_INTERFACE_COMMUNICATION_RECEIVED,
                          NULL, 0);
    TEST_ASSERT_FALSE (m_rx_cb);
    TEST_ASSERT_FALSE (m_tx_cb);
    TEST_ASSERT_FALSE (m_con_cb);
    TEST_ASSERT_FALSE (m_discon_cb);
}