#ifndef  TASK_NFC_H
#define  TASK_NFC_H
/**
 * @addtogroup peripheral_tasks
 */
/*@{*/
/**
 * @defgroup nfc_tasks NFC tasks
 * @brief NFC functions
 *
 */
/*@}*/
/**
 * @addtogroup nfc_tasks
 */
/*@{*/
/**
 * @file task_nfc.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-10-11
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * @brief NFC control.
 *
 * Typical usage:
 *
 * @code{.c}
 *  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
 *  err_code = task_nfc_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 * @endcode
 */
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_nfc.h"
#include "ruuvi_interface_log.h"

/**
 * @brief Initializes NFC and configures FW, ADDR and ID records according to application_config.h constants.
 *
 * @return RUUVI_DRIVER_SUCCESS on success
 * 2return error code from stack on error
 */
ruuvi_driver_status_t task_nfc_init(void);

/**
 * @brief Sets given message to NFC RAM buffer. Clears previous message.
 *
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return error code from stack on error
 */
ruuvi_driver_status_t task_nfc_send(ruuvi_interface_communication_message_t* message);

/**
 * @brief Handle Ruuvi communication events from NFC driver
 *
 * @param[in] evt Type of event.
 * @param[in] p_data pointer to data received. NULL if data was not received.
 * @param[in] data_len length of data received. 0 if data was NULL.
 *
 * @return RUUVI_DRIVER_SUCCESS if no error occurred
 * @return error code from stack on error.
 */
ruuvi_driver_status_t task_nfc_on_nfc(ruuvi_interface_communication_evt_t evt,
                                      void* p_data, size_t data_len);


#endif