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

#define LOG(msg) ruuvi_interface_log(TASK_GATT_LOG_LEVEL, msg)
#define LOGD(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_DEBUG, msg)
#define LOGE(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_ERROR, msg)
#define LOGHEX(msg, len) ruuvi_interface_log_hex(TASK_GATT_LOG_LEVEL, msg, len)
#define LOGDHEX(msg, len) ruuvi_interface_log_hex(RUUVI_INTERFACE_LOG_DEBUG, msg, len)

static ruuvi_interface_communication_t m_channel;   //!< API for sending data.
static bool m_is_init;
static bool m_nus_is_connected;
static char m_name[SCAN_RSP_NAME_MAX_LEN + 1] = {0};

static task_gatt_cb_t m_on_connected;    //!< Callback for connection established
static task_gatt_cb_t m_on_disconnected; //!< Callback for connection lost
static task_gatt_cb_t m_on_received;     //!< Callback for data received
static task_gatt_cb_t m_on_sent;         //!< Callback for data sent

// https://github.com/arm-embedded/gcc-arm-none-eabi.debian/blob/master/src/libiberty/strnlen.c
// Not included when compiled with std=c99.
static size_t safe_strlen (const char * s, size_t maxlen)
{
    size_t i;

    for (i = 0; i < maxlen; ++i)
        if (s[i] == '\0')
        { break; }

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
 * @param p_data pointer to event data, if event is @c RUUVI_INTERFACE_COMMUNICATION_RECEIVED received data, NULL otherwise.
 * @param data_len number of bytes in received data, 0 if p_data is NULL.
 *
 */
#ifndef CEEDLING
static
#endif
ruuvi_driver_status_t task_gatt_on_nus_isr (ruuvi_interface_communication_evt_t evt,
        void * p_data, size_t data_len)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    switch (evt)
    {
        // Note: This gets called only after the NUS notifications have been registered.
        case RUUVI_INTERFACE_COMMUNICATION_CONNECTED:
            break;

        case RUUVI_INTERFACE_COMMUNICATION_DISCONNECTED:
            break;

        case RUUVI_INTERFACE_COMMUNICATION_SENT:
            break;

        case RUUVI_INTERFACE_COMMUNICATION_RECEIVED:
            break;

        default:
            break;
    }

    return err_code;
}

ruuvi_driver_status_t task_gatt_dis_init (const
        ruuvi_interface_communication_ble4_gatt_dis_init_t * const dis)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (task_gatt_is_init())
    {
        err_code |= ruuvi_interface_communication_ble4_gatt_dis_init (&dis);
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

    if (task_gatt_is_init())
    {
        err_code |= ruuvi_interface_communication_ble4_gatt_nus_init (&m_channel);
        m_channel.on_evt = task_gatt_on_nus_isr;
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

    if (task_gatt_is_init())
    {
        err_code |= ruuvi_interface_communication_ble4_gatt_dfu_init();
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
    else if (task_advertisement_is_init() && !task_gatt_is_init())
    {
        size_t name_length = safe_strlen (name, sizeof (m_name));

        if (sizeof (m_name) >= name_length)
        {
            err_code |= ruuvi_interface_communication_ble4_gatt_init();
            err_code |= ruuvi_interface_communication_ble4_advertising_scan_response_setup (name,
                        false);
            memcpy (m_name, name, sizeof (m_name) - 1);
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

ruuvi_driver_status_t task_gatt_enable()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (task_gatt_is_init())
    {
        // Note that this doesn't update sofdevice buffer, you have to call
        // advertising functions to encode data and start the advertisements.
        err_code |= ruuvi_interface_communication_ble4_advertising_type_set (
                        CONNECTABLE_SCANNABLE);
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    return err_code;
}

ruuvi_driver_status_t task_gatt_disable()
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

bool task_gatt_is_init()
{
    return m_is_init;
}

/**
 * @brief check if NUS is connected, i.e. central has registered to TX notifications.
 *
 * @return true if NUS is connected is initialized, false otherwise.
 */
bool task_gatt_nus_is_connected()
{
    return m_nus_is_connected;
}



/**
 * @brief Function for sending data out via Nordic UART Service
 *
 * This function must not be called from interrupt context, as it may add data  to circular
 * buffer which gets consumed in interrupt context. The function will return immediately,
 * with not guarantee or acknowledgement that the message will be received.
 * In general if this function returns successfully the message will be sent and
 * delivery verified by link layer, but if connection is lost before data is sent
 * the data is lost.
 *
 * @param[in] msg Message to be sent out.
 * @return RUUVI_DRIVER_SUCCESS if message was queued to softdevice or application ringbuffer
 * @return RUUVI_DRIVER_ERROR_NULL if message is NULL
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE of GATT is not initialized
 * @return RUUVI_DRIVER_ERROR_NO_MEM if message cannot be queued due to buffers being full.
 */
ruuvi_driver_status_t task_gatt_send_asynchronous (ruuvi_interface_communication_message_t
        * const p_msg)
{
    // State, input check
    if (NULL == p_msg)
    {
        return RUUVI_DRIVER_ERROR_NULL;
    }

    if (NULL == m_channel.send)
    {
        return RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    // Try to put data to SD
    err_code |= m_channel.send (p_msg);

    // If success, return. Else put data to ringbuffer
    if (RUUVI_DRIVER_SUCCESS == err_code)
    {
        LOGD (">>>;");
        LOGDHEX (p_msg->data, p_msg->data_length);
        LOGD ("\r\n");
        return err_code;
    }
    // If the error code is something else than buffer full, return error.
    else if (err_code != RUUVI_DRIVER_ERROR_RESOURCES)
    {
        RUUVI_DRIVER_ERROR_CHECK (err_code, ~RUUVI_DRIVER_ERROR_FATAL);
        return err_code;
    }

    return err_code;
}

/** @brief Setup connection event handler
 *
 *  The event handler has signature of void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size)
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on connection in interrupt context.
 */
void task_gatt_set_on_connected_isr (const task_gatt_cb_t cb)
{
    m_on_connected = cb;
}

/** @brief Setup disconnection event handler
 *
 *  The event handler has signature of void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size)
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on disconnection in interrupt context.
 */
void task_gatt_set_on_disconnected_isr (const task_gatt_cb_t cb)
{
    m_on_disconnected = cb;
}

/** @brief Setup data received event handler
 *
 *  The event handler has signature of void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size)
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on data received in interrupt context.
 */
void task_gatt_set_on_received_isr (const task_gatt_cb_t cb)
{
    m_on_received = cb;
}

/** @brief Setup data sent event handler
 *
 *  The event handler has signature of void(*task_gatt_cb_t)(void* p_event_data, uint16_t event_size)
 *  where event data is NULL and event_size is 0.
 *  The event handler is called in interrupt context.
 *
 * @param[in] cb Callback which gets called on data sent in interrupt context.
 */
void task_gatt_set_on_sent_isr (const task_gatt_cb_t cb)
{
    m_on_sent = cb;
}

#endif
