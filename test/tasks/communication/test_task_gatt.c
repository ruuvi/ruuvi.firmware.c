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

ruuvi_driver_status_t mock_send (ruuvi_interface_communication_message_t * const p_msg)
{
    send_count++;
    return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t mock_read (ruuvi_interface_communication_message_t * const p_msg)
{
    read_count++;
    return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t mock_uninit (ruuvi_interface_communication_t * const p_channel)
{
    memset (p_channel, 0, sizeof (ruuvi_interface_communication_t));
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
    const char name[] = "Ceedling";
    task_advertisement_is_init_ExpectAndReturn (true);
    ruuvi_interface_communication_ble4_gatt_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_communication_ble4_advertising_scan_response_setup_ExpectAndReturn (name,
            false, RUUVI_DRIVER_SUCCESS);
    err_code |= task_gatt_init (name);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (task_gatt_is_init());
}

void tearDown (void)
{
    memset (&m_mock_gatt, 0, sizeof (ruuvi_interface_communication_t));
    send_count = 0;
    read_count = 0;
    task_gatt_mock_state_reset();
    TEST_ASSERT (!task_gatt_is_init());
}



/**
 * @brief Send given message via NUS
 *
 * This function queues a message to be sent and returns immediately.
 * There is no guarantee on when the data is actually sent, and
 * there is no acknowledgement or callback after the data has been sent.
 *
 * @return RUUVI_DRIVER_SUCCESS if data was placed in send buffer
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if NUS is not connected
 * @return error code from stack on error
 *
 */
void test_task_gatt_send_asynchronous_ok()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_communication_message_t msg = { 0 };
    msg.data_length = 11;
    err_code = task_gatt_send_asynchronous (&msg);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
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
    err_code = task_gatt_dfu_init();
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
ruuvi_driver_status_t task_gatt_dis_init (const
        ruuvi_interface_communication_ble4_gatt_dis_init_t * const dis);

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
void task_gatt_nus_init_ok()
{
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
    err_code |= task_gatt_init ("Ceedling");
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_gatt_init_null (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= task_gatt_init (NULL);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_NULL == err_code);
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
void test_task_gatt_enable_ok()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= task_gatt_enable ();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
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
    err_code |= task_gatt_disable();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}
/** @brief Setup connection event handler
 *
 *  The event handler has signature of void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size)
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on connection in interrupt context.
 */
