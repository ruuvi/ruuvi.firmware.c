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

/** @brief Repeat advertisement until explicitly stopped. */
#define APP_COMM_ADV_REPEAT_FOREVER (255U)
/** @brief Disable advertising. */
#define APP_COMM_ADV_DISABLE (0U)
/** @brief Initial period of fast advertising */
#define APP_FAST_ADV_TIME_MS (5U * 1000U)

/**
 * @brief Initialize communication methods supported by board
 *
 * After calling this function, NFC is ready to provide device information,
 * advertisement data can be sent and advertisements are connectable for enabling GATT
 * connection.
 *
 * Use ruuvi task functions, such as rt_gatt_send_asynchronous to send data out.
 *
 * @param[in] secure If true, GATT won't expose DFU service and device ID.
 *                   If false, DFU service is open and device ID can be read through GATT.
 * @retval RD_SUCCESS on success.
 * @return error code from stack on error.
 */
rd_status_t app_comms_init (const bool secure);

/**
 * @brief Get number of time BLE advertisement should be sent.
 *
 * @return Number of times to send same advertisement.
 *
 * @note Special value APP_COMM_ADV_REPEAT_FOREVER keeps sending
 *       advertisement until new value is set.
 * @note APP_COMM_ADV_DISABLE means advertisement should not be sent at all.
 */
uint8_t app_comms_bleadv_send_count_get (void);

/**
 * @brief Set number of time BLE advertisement should be sent.
 *
 * @param[in] count Number of times to send same advertisement.
 *
 * @note Special value APP_COMM_ADV_REPEAT_FOREVER keeps sending
 *       advertisement until new value is set.
 * @note APP_COMM_ADV_DISABLE means advertisement should not be sent at all.
 */
void app_comms_bleadv_send_count_set (const uint8_t count);

/**
 * @brief Enable security-sensitive configuration options.
 *
 * After calling this function:
 *   - DFU service is enabled which allows firmware updates.
 *   - Device ID is readable through DIS/Serial Number characteristic.
 *   - Configuration commands are allowed.
 *
 * The configuration is enabled for the next communication connection and is disabled
 * after disconnection or once timeout is triggered.
 *
 * This also calls app_led to indicate configuration is enabled.
 *
 * @retval RD_SUCCESS if configuration mode was entered.
 * @retval RD_ERROR_INVALID_STATE if communications are not initialized.
 */
rd_status_t app_comms_configure_next_enable (void);

/**
 * @brief Enable Bluetooth on device
 *
 * After calling this function GATT server is initialized and ready to accept connection
 * if GATT is enabled in application. Advertisements can be sent.
 *
 * If ble comms are initialized as secure, DFU service is disabled and serial number is not
 * readable over GATT.
 *
 * If ble comms are initialized as unsecure, DFU service is enabled and serial number is
 * readable over GATT.
 *
 * Ble must be uninitialized to re-enter secure mode or vice versa.
 *
 * @param[in] secure True to not enable DFU service and serial number read over GATT.
 *                   False to enable DFU service and serial number read over GATT.
 */
rd_status_t app_comms_ble_init (const bool secure);

/**
 * @brief Enable Bluetooth advertising on device
 *
 * Calling this function will re-enable BLE advertising during activated GATT connection.
 *
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_STATE if advertising is already initialized.
 * @retval RD_ERROR_INVALID_PARAM if rt_adv_init()'s constant parameter is invalid. Not initialized.
 */
rd_status_t app_comms_ble_adv_init (void);

/**
 * @brief Uninitialize GATT.
 *
 * After calling this function advertisements cannot be sent and GATT server is disabled.
 */
rd_status_t app_comms_ble_uninit (void);

/**
 * @brief Blocking send message function.
 *
 * Calls reply_fp with given data, and if reply_fp returns ERR_NO_MEM
 * yields and retries. Has optional timeout. Function will return once
 * message has been queued to driver buffer, not necessarily sent.
 *
 * @param[in] reply_fp Function pointer to use to send the data.
 * @param[in] msg Message to send.
 * @retval RD_SUCCESS Message was queued to TX buffer.
 * @retval RD_ERROR_TIMEOUT Message was queued to TX buffer. Error code from
 *                          reply_fp is also included,
 *                          e.g. RD_ERROR_TIMEOUT | RD_ERROR_INVALID_STATE
 *
 * @note Timeout requires RTC and some process which brings thread out
 * of yield.
 */
rd_status_t app_comms_blocking_send (const ri_comm_xfer_fp_t reply_fp,
                                     ri_comm_message_t * const msg);

#ifdef CEEDLING
/** Handles for unit test framework */
typedef struct
{
    unsigned int switch_to_normal : 1;
    unsigned int disable_config : 1;
} mode_changes_t;
void on_gatt_connected_isr (void * p_data, size_t data_len);
void on_gatt_disconnected_isr (void * p_data, size_t data_len);
void on_gatt_data_isr (void * p_data, size_t data_len);
void handle_config_disable (void * p_data, uint16_t data_len);
void handle_gatt_data (void * p_data, uint16_t data_len);
void handle_gatt_connected (void * p_data, uint16_t data_len);
void handle_gatt_disconnected (void * p_data, uint16_t data_len);
void handle_nfc_connected (void * p_data, uint16_t data_len);
void handle_nfc_disconnected (void * p_data, uint16_t data_len);
void on_nfc_connected_isr (void * p_data, size_t data_len);
void on_nfc_disconnected_isr (void * p_data, size_t data_len);
void comm_mode_change_isr (void * const p_context);
#endif


/** @} */
#endif // APP_COMMS_H
