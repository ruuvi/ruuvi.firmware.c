#ifndef  TASK_ADVERTISEMENT_H
#define  TASK_ADVERTISEMENT_H

/**
 * @defgroup communication_tasks Sending and receiving data.
 */
/*@{*/
/**
 * @defgroup advertisement_tasks Advertisement tasks
 * @brief Bluetooth Low Energy advertising
 *
 */
/*@}*/
/**
 * @addtogroup advertisement_tasks
 */
/*@{*/
/**
 * @file task_advertisement.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-11-19
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Advertise data and GATT connection if available.
 *
 *
 * Typical usage:
 *
 * @code{.c}
 *  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
 *  err_code = task_advertisement_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_advertisement_send_data();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_advertisement_start();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 * @endcode
 */

#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication_ble4_advertising.h"

#define SCAN_RSP_NAME_MAX_LEN 11 //!< Longer name gets truncated when advertised with UUID.
/**
 * @brief Initializes data advertising.
 *
 * The function setups advertisement interval, advertisement power, advertisement type,
 * manufacturer ID for manufacturer specific data according to constants in
 * application_config.h and ruuvi_boards.h.
 *
 * It also configures a callback to be executed after advertisement for internal use.
 * After calling this function advertisement data can be queued into advertisement buffer.
 * You should queue at least one message into buffer before starting advertising.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if advertising is already initialized.
 * @®etval RUUVI_DRIVER_ERROR_INVALID_PARAM if configuration constant is invalid.
 */
ruuvi_driver_status_t task_advertisement_init (void);

/**
 * @brief Uninitializes data advertising.
 *
 * Can be called even if advertising was not initialized.
 * Clears previous advertisement data if there was any.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval error code from stack on error
 */
ruuvi_driver_status_t task_advertisement_uninit (void);

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
ruuvi_driver_status_t task_advertisement_start (void);

/**
 * @brief Stops advertising.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval error code from stack on error
 */
ruuvi_driver_status_t task_advertisement_stop (void);

/** @brief Send given message as a BLE advertisement.
 *
 *  This function configures the primary advertisement packet with the flags and manufacturer specific data.
 *  Payload of the msg will be sent as the manufacturer specific data payload.
 *  Manufacturer ID is defined by RUUVI_BOARD_BLE_MANUFACTURER_ID in ruuvi_boards.h.
 *
 *  If the device is connectable, call @code task_advertisement_connectability @endcode to setup the
 *  scan response and flags to advertise connectability.
 *
 *  @param[in] msg message to be sent as manufacturer specific data payload
 *  @retval    RUUVI_DRIVER_ERROR_NULL if msg is NULL
 *  @retval    RUUVI_DRIVER_ERROR_INVALID_STATE if advertising isn't initialized or started.
 *  @retval    RUUVI_DRIVER_ERROR_DATA_SIZE if payload size is larger than 24 bytes
 *  @retval    error code from stack on other error.
 */
ruuvi_driver_status_t task_advertisement_send_data (
    ruuvi_interface_communication_message_t * const msg);

/** @brief Start advertising BLE GATT connection
 *
 *  This function configures the primary advertisement to be SCANNABLE_CONNECTABLE and
 *  sets up a scan response which has given device name (max 10 characters + NULL)
 *  and UUID of Nordic UART Service.
 *
 *  Be sure to configure the GATT before calling this function, as behaviour is undefined
 *  if someone tries to connect to tag while GATT is not configured.
 *
 *  @param[in] enable true to enable connectability, false to disable.
 *  @param[in] device_name NULL-terminated string representing device name, max 10 Chars + NULL.
 *  @retval    RUUVI_DRIVER_SUCCESS if operation was finished as expected.
 *  @retval    RUUVI_DRIVER_ERROR_NULL if name is NULL and trying to enable the scan response
 *  @retval    RUUVI_DRIVER_ERROR_INVALID_STATE if advertising isn't initialized or started.
 *  @retval    RUUVI_DRIVER_ERROR_INVALID_LENGTH if name size exceeds 10 bytes + NULL
 *  @retval    error code from stack on other error.
 */
ruuvi_driver_status_t task_advertisement_connectability_set (const bool enable,
        const char * const device_name);

/** @brief check if advertisement is initialized
 *  @return true if advertisement is initialized, false otherwise.
 */
bool task_advertisement_is_init (void);
/*@}*/

#endif