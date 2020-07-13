#ifndef APP_COMMS_H
#define APP_COMMS_H
#include "ruuvi_driver_error.h"
/**
 * @addtogroup app
 */
/** @{ */
/**
 * @defgroup app_comms Application communication
 * @brief Initialize and configure communication withoutside world.
 */
/** @} */
/**
 * @addtogroup app_comms
 */
/** @{ */
/**
 * @file app_comms.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-07-13
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Typical usage:
 * @code{.c}
 * rd_status_t err_code = app_comms_init();
 * RD_ERROR_CHECK(err_code, RD_SUCCESS);
 * @endcode
 */

#include "ruuvi_interface_communication.h"


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
rd_status_t app_comms_init (void);

#ifdef CEEDLING
/** Handles for unit test framework */
void on_gatt_connected_isr (void * p_data, size_t data_len);
void on_gatt_disconnected_isr (void * p_data, size_t data_len);
#endif


/** @} */
#endif // APP_COMMS_H
