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
 * @date 2020-04-29
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Typical usage:
 * @code{.c}
 * TODO
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

/** @} */
#endif // APP_COMMS_H
