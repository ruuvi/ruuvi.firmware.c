/**
 * @addtogroup communication_tasks
 */
/*@{*/
/**
 * @file task_gatt.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-10-12
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 *
 *
 */
#ifndef  TASK_GATT_H
#define  TASK_GATT_H

#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_ble4_gatt.h"

#ifdef CEEDLING
// Assist function for unit tests.
void task_gatt_mock_state_reset();

// Expose callback to Ceedling
ruuvi_driver_status_t task_gatt_on_nus_isr (ruuvi_interface_communication_evt_t evt,
        void * p_data, size_t data_len);

#endif

/* @brief Callback handler for GATT communication events */
typedef void (*task_gatt_cb_t) (void * p_data, size_t data_len);

/**
 * @brief Send given message via NUS
 *
 * This function queues a message to be sent and returns immediately.
 * There is no guarantee on when the data is actually sent, and
 * there is no acknowledgement or callback after the data has been sent.
 *
 * @retval RUUVI_DRIVER_SUCCESS if data was placed in send buffer
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if NUS is not connected
 * @retval RUUVI_DRIVER_ERROR_NO_MEM if tx buffer is full
 * @retval error code from stack on other error
 *
 */
ruuvi_driver_status_t task_gatt_send_asynchronous (ruuvi_interface_communication_message_t
        *
        const msg);

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
ruuvi_driver_status_t task_gatt_dfu_init (void);

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
ruuvi_driver_status_t task_gatt_nus_init();

/**
 * @brief Initialize GATT. Must be called as a first function in task_gatt.
 *
 * After calling this function underlying software stack is ready to setup GATT services.
 *
 * @param[in] name Full name of device to be advertised in scan responses. Maximum 11 chars + trailing NULL. Must not be NULL, 0-length string is valid.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_NULL if name is NULL (use 0-length string instead)
 * @retval RUUVI_DRIVER_ERROR_INVALID_LENGTH if name is longer than @ref SCAN_RSP_NAME_MAX_LEN
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if GATT is already initialized or advertisements are not initialized.
 *
 */
ruuvi_driver_status_t task_gatt_init (const char * const name);

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
ruuvi_driver_status_t task_gatt_enable ();

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
ruuvi_driver_status_t task_gatt_disable ();

/**
 * @brief check if GATT task is initialized
 *
 * @return true if GATT is initialized, false otherwise.
 */
bool task_gatt_is_init();

/**
 * @brief check if NUS is connected, i.e. central has registered to TX notifications.
 *
 * @return true if NUS is connected is initialized, false otherwise.
 */
bool task_gatt_nus_is_connected();

/** @brief Setup connection event handler.
 *
 *  The event handler has signature of @code void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size) @endcode
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on connection in interrupt context.
 */
void task_gatt_set_on_connected_isr (const task_gatt_cb_t cb);

/** @brief Setup disconnection event handler.
 *
 *  The event handler has signature of @code void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size) @endcode
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on disconnection in interrupt context.
 */
void task_gatt_set_on_disconn_isr (const task_gatt_cb_t cb);

/** @brief Setup data received event handler.
 *
 *  The event handler has signature of @code void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size) @endcode
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on data received in interrupt context.
 */
void task_gatt_set_on_received_isr (const task_gatt_cb_t cb);

/** @brief Setup data sent event handler.
 *
 *  The event handler has signature of @code void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size) @endcode
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on data sent in interrupt context.
 */
void task_gatt_set_on_sent_isr (const task_gatt_cb_t cb);

#endif
/*@}*/