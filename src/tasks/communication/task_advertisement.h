#ifndef  TASK_ADVERTISEMENT_H
#define  TASK_ADVERTISEMENT_H

/**
 * @defgroup communication_tasks Sending and receiveing data.
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
 * Advertising data and GATT connection if available
 *
 * @warning In middle of refactoring, doesn't do what you'd expect and does what you don't expect.
 *
 * Typical usage:
 *
 * @code{.c}
 *  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
 *  err_code = task_advertisement_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = advertisement_data_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_advertisement_start();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 * @endcode
 */

#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication_ble4_advertising.h"

/**
 * @brief Initializes data advertising.
 *
 * Parameters such as advertisement interval and power are defined in application_config.h
 * Initializes timers if timers haven't been initialized.
 * After calling this function advertisement data can be queued
 *
 * @retval @c RUUVI_DRIVER_SUCCESS on success
 * @retval @c RUUVI_DRIVER_ERROR_INVALID_STATE
 */
ruuvi_driver_status_t task_advertisement_init (void);

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
ruuvi_driver_status_t task_advertisement_uninit (void);

/**
 * Populate advertisement buffer with initial data.
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 *
 */
ruuvi_driver_status_t advertisement_data_init (void);

/**
 * Starts advertising. Reads sensors for latest data, but does not initialize sensors themselves
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns RUUVI_DRIVER_ERROR_INVALID_STATE if advertising is not initialized.
 * returns error code from stack on error
 *
 */
ruuvi_driver_status_t task_advertisement_start (void);

/**
 * Stops advertising.
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error
 */
ruuvi_driver_status_t task_advertisement_stop (void);

/** @brief Send given message as a BLE advertisement.
 *
 *  This function configures the primary advertisement packet with the flags and manufacturer specific data.
 *  Payload of the msg will be sent as the manufacturer specific data payload.
 *  Manufacturer ID is defined by RUUVI_BOARD_BLE_MANUFACTURER_ID in ruuvi_board.h.
 *
 *  If the device is connectable, call @ref task_gatt_init to setup the scan response and flags to advertise
 *  connectability.
 *
 *  @param[in] msg message to be sent as manufacturer specific data payload
 *  @return    RUUVI_DRIVER_ERROR_NULL if msg is NULL
 *  @return    RUUVI_DRIVER_ERROR_INVALID_STATE if advertising isn't initialized or started.
 *  @return    error code from stack on other error.
 */
ruuvi_driver_status_t task_advertisement_send_data (
    ruuvi_interface_communication_message_t * const msg);

#endif