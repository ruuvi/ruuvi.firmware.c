/**
 * @addtogroup advertisement_tasks
 */
/*@{*/
/**
 * @file task_advertisement.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-11-19
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Advertising data and GATT connection if available
 *
 * @warning In middle of refactoring, doesn't do what you'd expect and does what you don't expect.
 *
 * Typical usage:
 */

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication_ble4_advertising.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "task_advertisement.h"
#include "task_communication.h"
#include "task_environmental.h"

#include <string.h>

static ruuvi_interface_communication_t m_channel;
static bool m_is_init;

ruuvi_driver_status_t task_advertisement_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (m_is_init)
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        err_code |= ruuvi_interface_communication_ble4_advertising_init (&m_channel);
        err_code |= ruuvi_interface_communication_ble4_advertising_tx_interval_set (
                        APPLICATION_ADVERTISEMENT_UPDATE_INTERVAL_MS);
        int8_t power = APPLICATION_ADVERTISING_POWER_DBM;
        err_code |= ruuvi_interface_communication_ble4_advertising_tx_power_set (&power);
        err_code |= ruuvi_interface_communication_ble4_advertising_type_set (
                        NONCONNECTABLE_NONSCANNABLE);
        err_code |= ruuvi_interface_communication_ble4_advertising_manufacturer_id_set (
                        RUUVI_BOARD_BLE_MANUFACTURER_ID);

        if (RUUVI_DRIVER_SUCCESS == err_code)
        {
            m_is_init = true;
        }
    }

    return err_code;
}

ruuvi_driver_status_t task_advertisement_uninit (void)
{
    m_is_init = false;
    return ruuvi_interface_communication_ble4_advertising_uninit (&m_channel);
}

ruuvi_driver_status_t task_advertisement_start (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!m_is_init)
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        err_code |= ruuvi_interface_communication_ble4_advertising_start();
    }

    return err_code;
}

ruuvi_driver_status_t task_advertisement_stop (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!m_is_init)
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        err_code |= ruuvi_interface_communication_ble4_advertising_stop();
    }

    return err_code;
}

ruuvi_driver_status_t task_advertisement_send_data (
    ruuvi_interface_communication_message_t * const msg)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (NULL == msg)
    {
        err_code |= RUUVI_DRIVER_ERROR_NULL;
    }
    else if (!task_advertisement_is_init())
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else if (24 < msg->data_length)
    {
        err_code |= RUUVI_DRIVER_ERROR_DATA_SIZE;
    }
    else
    {
        err_code |= m_channel.send (msg);
    }

    return err_code;
}

/** @brief Start advertising BLE GATT connection
 *
 *  This function configures the primary advertisement to be SCANNABLE_CONNECTABLE and
 *  sets up a scan response which has given device name (max 10 characters + NULL)
 *  and UUID of Nordic UART Service.
 *
 *  Be sure to configure the GATT
 *
 *  @param[in] enable true to enable connectability, false to disable.
 *  @param[in] name NULL-terminated string representing device name, max 10 Chars + NULL.
 *  @retval    RUUVI_DRIVER_ERROR_INVALID_STATE if
 *  @retval    RUUVI_DRIVER_ERROR_NULL if name is NULL
 *  @retval    RUUVI_DRIVER_ERROR_INVALID_STATE if advertising isn't initialized or started.
 *  @retval    error code from stack on other error.
 */
ruuvi_driver_status_t task_advertisement_connectability_set (const bool enable,
        const char * const device_name)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!task_advertisement_is_init())
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else if (!enable)
    {
        err_code |= ruuvi_interface_communication_ble4_advertising_type_set (
                        NONCONNECTABLE_NONSCANNABLE);
    }
    else if (NULL == device_name)
    {
        err_code |= RUUVI_DRIVER_ERROR_NULL;
    }
    else if (10 < strnlen (device_name, 11))
    {
        // TODO - #define 10
        err_code |= RUUVI_DRIVER_ERROR_INVALID_LENGTH;
    }
    else
    {
        err_code |= ruuvi_interface_communication_ble4_advertising_type_set (
                        CONNECTABLE_SCANNABLE);
        err_code |= ruuvi_interface_communication_ble4_advertising_scan_response_setup (
                        device_name, true);
    }

    return err_code;
}

inline bool task_advertisement_is_init (void)
{
    return ( (NULL != m_channel.send) && (true == m_is_init));
}
/*@}*/
