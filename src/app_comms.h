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
 * @retval RD_SUCCESS on success.
 * @return error code from stack on error.
 */
rd_status_t app_comms_init (void);

/**
 * @brief Send BLE advertisement.
 *
 * This function queues given message to BLE stack and returns. If the message
 * has to be repeated, use msg->repeat_count.
 * The advertisements are required for establishing a GATT connection, it's a good
 * idea to send at least at 1285 ms interval if you want centrals to establish
 * connections.
 *
 * If GATT is already connected, message type will automatically be non-connectable.
 *
 * @retval RD_SUCCESS On success and if ADV is not enabled in application.
 * @retval RD_ERROR_NULL if msg is NULL.
 * @retval RD_ERROR_INVALID_STATE if communication is not initialized.
 * @retval RD_ERROR_DATA_SIZE if msg->data_length is greater than maximum adv length.
 *
 * @note If message payload is larger than 24 bytes, the message may automatically
 *       be converted to extended advertisement on secondary channel with 2 MBit / s
 *       PHY. Most consumer devices won't see these advertisements as of 2020.
 */
rd_status_t app_comms_send_advertisement (const ri_comm_message_t * const msg);

/**
 * @brief Send BLE advertisement.
 *
 * This function queues given message to BLE stack and returns. If the message
 * has to be repeated, use msg->repeat_count.
 * The advertisements are required for establishing a GATT connection, it's a good
 * idea to send at least at 1285 ms interval if you want centrals to establish
 * connections.
 *
 * If GATT is already connected, message type will automatically be non-connectable.
 *
 * @retval RD_SUCCESS On success and if ADV is not enabled in application.
 * @retval RD_ERROR_NULL if msg is NULL.
 * @retval RD_ERROR_INVALID_STATE if communication is not initialized.
 * @retval RD_ERROR_DATA_SIZE if msg->data_length is greater than maximum adv length.
 *
 * @note If message payload is larger than 24 bytes, the message may automatically
 *       be converted to extended advertisement on secondary channel with 2 MBit / s
 *       PHY. Most consumer devices won't see these advertisements as of 2020.
 */
rd_status_t app_comms_send_advertisement (const ri_comm_message_t * const msg);

/**
 * @brief Send BLE GATT data via NUS TX.
 *
 * This function queues given message to BLE stack and returns.
 *
 * @retval RD_SUCCESS On success and if GATT is not enabled in application.
 * @retval RD_ERROR_NULL If msg is NULL.
 * @retval RD_ERROR_INVALID_STATE If communication is not initialized or GATT is not
 *                                connected.
 * @retval RD_ERROR_DATA_SIZE If msg->data_length is greater than maximum gatt length.
 *
 * @note GATT data length is at maximum 20 bytes as of 3.29.0, but it may be extended
 *       for larger MTUs in the future.
 */
rd_status_t app_comms_send_gatt (const ri_comm_message_t * const msg);

/**
 * @brief Sets up NFC data for a reader.
 *
 * This function prepares the given data as hex-encoded string into NFC data field.
 *       for larger MTUs in the future.
 */
rd_status_t app_comms_send_gatt (const ri_comm_message_t * const msg);

/**
 * @brief Sets up NFC data for a reader.
 *
 * This function prepares the given data as hex-encoded string into NFC data field.
 *
 * @retval RD_SUCCESS On success and if NFC is not enabled in application.
 * @retval RD_ERROR_NULL If msg is NULL.
 * @retval RD_ERROR_INVALID_STATE If communication is not initialized or NFC is connected.
 * @retval RD_ERROR_DATA_SIZE If msg->data_length is greater than maximum adv length.
 *
 * @note If message payload is larger than 24 bytes, the message may automatically
 *       be converted to extended advertisement on secondary channel with 2 MBit / s
 *       PHY. Most consumer devices won't see these advertisements as of 2020.
 */
rd_status_t app_comms_send_nfc (const ri_comm_message_t * const msg);
/** @} */
#endif // APP_COMMS_H
