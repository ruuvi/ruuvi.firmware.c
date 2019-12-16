/**
 * Ruuvi Firmware 3.x GATT tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_atomic.h"
#include "ruuvi_interface_communication_ble4_advertising.h"
#include "ruuvi_interface_communication_ble4_gatt.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_library.h"
#include "ruuvi_library_ringbuffer.h"
#include "task_advertisement.h"
#include "task_communication.h"
#include "task_gatt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if APPLICATION_COMMUNICATION_GATT_ENABLED

#ifndef TASK_GATT_LOG_LEVEL
#define TASK_GATT_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

#if 0
static inline void LOG (const char * const msg)
{
    ruuvi_interface_log (TASK_GATT_LOG_LEVEL, msg);
}

static inline void LOGE (const char * const msg)
{
    ruuvi_interface_log (RUUVI_INTERFACE_LOG_ERROR, msg);
}

static inline void LOGHEX (const uint8_t * const msg, const size_t len)
{
    ruuvi_interface_log_hex (TASK_GATT_LOG_LEVEL, msg, len);
}
#endif

static inline void LOGD (const char * const msg)
{
    ruuvi_interface_log (RUUVI_INTERFACE_LOG_DEBUG, msg);
}

static inline void LOGDHEX (const uint8_t * const msg, const size_t len)
{
    ruuvi_interface_log_hex (RUUVI_INTERFACE_LOG_DEBUG, msg, len);
}

static ruuvi_interface_communication_t m_channel;   //!< API for sending data.
static bool m_is_init;
static bool m_nus_is_init;
static bool m_dis_is_init;
static bool m_dfu_is_init;
static bool m_nus_is_connected;
static char m_name[SCAN_RSP_NAME_MAX_LEN + 1] = { 0 };

static task_gatt_cb_t m_on_connected;    //!< Callback for connection established
static task_gatt_cb_t m_on_disconnected; //!< Callback for connection lost
static task_gatt_cb_t m_on_received;     //!< Callback for data received
static task_gatt_cb_t m_on_sent;         //!< Callback for data sent

// https://github.com/arm-embedded/gcc-arm-none-eabi.debian/blob/master/src/libiberty/strnlen.c
// Not included when compiled with std=c99.
static inline size_t safe_strlen (const char * s, size_t maxlen)
{
    size_t i;

    for (i = 0; (i < maxlen) && ('\0' != s[i]); ++i);

    return i;
}

#ifdef CEEDLING
void task_gatt_mock_state_reset()
{
    m_on_connected = NULL;
    m_on_disconnected = NULL;
    m_on_received = NULL;
    m_on_sent = NULL;
    m_is_init = false;
    m_nus_is_init = false;
    m_dfu_is_init = false;
    m_dis_is_init = false;
    m_nus_is_connected = false;
    memset (&m_channel, 0, sizeof (ruuvi_interface_communication_t));
    memset (m_name, 0, sizeof (m_name));
}
#endif

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
 * @param p_data pointer to event data, if event is
 *               @c RUUVI_INTERFACE_COMMUNICATION_RECEIVED received data, NULL otherwise.
 * @param data_len number of bytes in received data, 0 if p_data is NULL.
 *
 */
#ifndef CEEDLING
static
#endif
ruuvi_driver_status_t task_gatt_on_nus_isr (ruuvi_interface_communication_evt_t evt,
        void * p_data, size_t data_len)
{
    switch (evt)
    {
        // Note: This gets called only after the NUS notifications have been registered.
        case RUUVI_INTERFACE_COMMUNICATION_CONNECTED:
            m_nus_is_connected = true;
            (NULL != m_on_connected) ? m_on_connected (p_data, data_len) : false;
            break;

        case RUUVI_INTERFACE_COMMUNICATION_DISCONNECTED:
            m_nus_is_connected = false;
            (NULL != m_on_disconnected) ? m_on_disconnected (p_data, data_len) : false;
            break;

        case RUUVI_INTERFACE_COMMUNICATION_SENT:
            (NULL != m_on_sent) ? m_on_sent (p_data, data_len) : false;
            break;

        case RUUVI_INTERFACE_COMMUNICATION_RECEIVED:
            (NULL != m_on_received) ? m_on_received (p_data, data_len) : false;
            break;

        default:
            break;
    }

    return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_gatt_dis_init (const
        ruuvi_interface_communication_ble4_gatt_dis_init_t * const p_dis)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (NULL == p_dis)
    {
        err_code |= RUUVI_DRIVER_ERROR_NULL;
    }
    else if (task_gatt_is_init() && (!m_dis_is_init))
    {
        err_code |= ruuvi_interface_communication_ble4_gatt_dis_init (p_dis);
        m_dis_is_init = (RUUVI_DRIVER_SUCCESS == err_code);
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    return err_code;
}

ruuvi_driver_status_t task_gatt_nus_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (task_gatt_is_init() && (!m_nus_is_init))
    {
        err_code |= ruuvi_interface_communication_ble4_gatt_nus_init (&m_channel);

        if (RUUVI_DRIVER_SUCCESS == err_code)
        {
            m_channel.on_evt = task_gatt_on_nus_isr;
            m_nus_is_init = true;
        }
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    return err_code;
}

ruuvi_driver_status_t task_gatt_dfu_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (task_gatt_is_init() && (!m_dfu_is_init))
    {
        err_code |= ruuvi_interface_communication_ble4_gatt_dfu_init();
        m_dfu_is_init = (RUUVI_DRIVER_SUCCESS == err_code);
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    return err_code;
}

ruuvi_driver_status_t task_gatt_init (const char * const name)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (NULL == name)
    {
        err_code |= RUUVI_DRIVER_ERROR_NULL;
    }
    else if (task_advertisement_is_init() && (!task_gatt_is_init()))
    {
        const size_t name_length = safe_strlen (name, sizeof (m_name));

        if (sizeof (m_name) > name_length)
        {
            err_code |= ruuvi_interface_communication_ble4_gatt_init();
            memcpy (m_name, name, name_length);
            m_name[name_length] = '\0';
        }
        else
        {
            err_code |= RUUVI_DRIVER_ERROR_INVALID_LENGTH;
        }
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    if (RUUVI_DRIVER_SUCCESS == err_code)
    {
        m_is_init = true;
    }

    return err_code;
}

ruuvi_driver_status_t task_gatt_enable (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (task_gatt_is_init())
    {
        // Note that this doesn't update sofdevice buffer, you have to call
        // advertising functions to encode data and start the advertisements.
        err_code |= ruuvi_interface_communication_ble4_advertising_scan_response_setup (
                        m_name, m_nus_is_init);
        err_code |= ruuvi_interface_communication_ble4_advertising_type_set (
                        CONNECTABLE_SCANNABLE);
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    return err_code;
}

ruuvi_driver_status_t task_gatt_disable (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (task_gatt_is_init())
    {
        err_code |= ruuvi_interface_communication_ble4_advertising_type_set (
                        NONCONNECTABLE_NONSCANNABLE);
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    return err_code;
}

bool task_gatt_is_init (void)
{
    return m_is_init;
}

/**
 * @brief check if NUS is connected, i.e. central has registered to TX notifications.
 *
 * @return true if NUS is connected is initialized, false otherwise.
 */
bool task_gatt_nus_is_connected (void)
{
    return m_nus_is_connected && (NULL != m_channel.send);
}

ruuvi_driver_status_t task_gatt_send_asynchronous (ruuvi_interface_communication_message_t
        * const p_msg)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    // State, input check
    if (NULL == p_msg)
    {
        err_code |= RUUVI_DRIVER_ERROR_NULL;
    }
    else if (!task_gatt_nus_is_connected())
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        // Try to put data to SD
        err_code |= m_channel.send (p_msg);

        // If success, return. Else put data to ringbuffer
        if (RUUVI_DRIVER_SUCCESS == err_code)
        {
            LOGD (">>>;");
            LOGDHEX (p_msg->data, p_msg->data_length);
            LOGD ("\r\n");
        }
        else if (RUUVI_DRIVER_ERROR_RESOURCES == err_code)
        {
            err_code = RUUVI_DRIVER_ERROR_NO_MEM;
        }
        // If the error code is something else than buffer full, return error.
        else
        {
            RUUVI_DRIVER_ERROR_CHECK (err_code, ~RUUVI_DRIVER_ERROR_FATAL);
        }
    }

    return err_code;
}

void task_gatt_set_on_connected_isr (const task_gatt_cb_t cb)
{
    m_on_connected = cb;
}


void task_gatt_set_on_disconn_isr (const task_gatt_cb_t cb)
{
    m_on_disconnected = cb;
}

void task_gatt_set_on_received_isr (const task_gatt_cb_t cb)
{
    m_on_received = cb;
}

void task_gatt_set_on_sent_isr (const task_gatt_cb_t cb)
{
    m_on_sent = cb;
}

#endif
