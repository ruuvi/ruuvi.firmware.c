/**
 * @addtogroup TASKS
 * @brief Functions related to GATT connection
 *
 *
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

/**
 * @brief Initializes GATT with DFU, DIF and NUS.
 * Requires a new packet to be queued with advertising module
 * to update the advertising data with scannable name of device.
 *
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return error code from stack on error
 */
ruuvi_driver_status_t task_gatt_init(void);

/**
 * @brief Event handler for NUS events
 *
 * This function is called in interrupt context, which allows for real-time processing
 * such as feeding softdevice data buffers during connection event.
 * Care must be taken to not call any function which requires external peripherals,
 * such as sensors in this context.
 *
 * If sensors must be read / configured as a response to GATT event, schedule
 * the action and send the results back during next connection event by buffering
 * the response with task_gatt_send.
 *
 * @param evt Event type
 * @param p_data pointer to event data, if event is @c RUUVI_INTERFACE_COMMUNICATION_RECEIVED received data, NULL otherwise.
 * @param data_len number of bytes in received data, 0 if p_data is NULL.
 *
 */
ruuvi_driver_status_t task_gatt_on_nus(ruuvi_interface_communication_evt_t evt,
                                       void* p_data, size_t data_len);

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
ruuvi_driver_status_t task_gatt_send_asynchronous(ruuvi_interface_communication_message_t*
    const msg);


#endif
/*@}*/