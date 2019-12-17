#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"

#include "task_acceleration.h"
#include "task_adc.h"
#include "task_environmental.h"
#include "task_rtc.h"
#include "task_sensor.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>


static ruuvi_interface_timer_id_t
heartbeat_timer;   //!< Timer for heartbeat action
static size_t
m_heartbeat_data_max_len;  //!< Maximum data length for heartbeat data
static ruuvi_interface_communication_xfer_fp_t
heartbeat_target; //!< Function to which send the hearbeat data
static heartbeat_data_fp_t heartbeat_encoder;

static ruuvi_driver_status_t task_communication_target_api_get (
    task_communication_api_t ** api, uint8_t target)
{
    if (NULL == api)
    {
        return RUUVI_DRIVER_ERROR_NULL;
    }

    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    switch (target)
    {
        // All environmental values are controlled through the same API
        case RUUVI_ENDPOINT_STANDARD_DESTINATION_ENVIRONMENTAL:
        case RUUVI_ENDPOINT_STANDARD_DESTINATION_TEMPERATURE:
        case RUUVI_ENDPOINT_STANDARD_DESTINATION_HUMIDITY:
        case RUUVI_ENDPOINT_STANDARD_DESTINATION_PRESSURE:
            task_environmental_api_get (api);
            break;

        default:
            err_code = RUUVI_DRIVER_ERROR_INVALID_PARAM;
    }

    return err_code;
}


ruuvi_driver_status_t task_communication_on_data (const
        ruuvi_interface_communication_message_t * const incoming,
        ruuvi_interface_communication_xfer_fp_t reply_fp)
{
    // return error if data is not understood.
    ruuvi_interface_communication_message_t reply = {0};
    // Get target API
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    task_communication_api_t * api;
    err_code |= task_communication_target_api_get (&api,
                incoming->data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX]);
    ruuvi_driver_sensor_configuration_t config;
    uint8_t payload[RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH];
    // Unless something was done with the data, assume error
    reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;

    if (RUUVI_DRIVER_SUCCESS == err_code)
    {
        switch (incoming->data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX])
        {
            case RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_WRITE:
                if (NULL == api->sensor)
                {
                    reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
                    break;
                }

                memcpy (&config, & (incoming->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]),
                        RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
                (* (api->sensor))->configuration_set (* (api->sensor), &config);

            // Intentional fallthrough to configuration read

            case RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_READ:
                if (NULL == api->sensor)
                {
                    reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
                    break;
                }

                (* (api->sensor))->configuration_get (* (api->sensor), &config);
                memcpy (& (reply.data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), &config,
                        RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
                // Write state of sensor back to application
                reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] =
                    RUUVI_ENDPOINT_STANDARD_SENSOR_CONFIGURATION_WRITE;
                break;

            case RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_WRITE:
                if (NULL == api->offset_set)
                {
                    reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
                    break;
                }

                memcpy (payload, & (incoming->data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]),
                        RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
                api->offset_set (payload);

            // Intentional fallthrough to offset read

            case RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_READ:
                if (NULL == api->offset_get)
                {
                    reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
                    break;
                }

                api->offset_get (payload);
                memcpy (& (reply.data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), payload,
                        RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
                // Write state of sensor back to application
                reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] =
                    RUUVI_ENDPOINT_STANDARD_SENSOR_OFFSET_WRITE;
                break;

            case RUUVI_ENDPOINT_STANDARD_VALUE_READ:
                if (NULL == api->data_get)
                {
                    reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
                    break;
                }

                api->data_get (payload);
                memcpy (& (reply.data[RUUVI_ENDPOINT_STANDARD_PAYLOAD_START_INDEX]), payload,
                        RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
                reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_VALUE_WRITE;
                break;

            case RUUVI_ENDPOINT_STANDARD_LOG_VALUE_READ:
                if (NULL == api->log_read)
                {
                    reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
                    break;
                }

                // This call blocks until error occurs or all the requested data has been sent.
                api->log_read (reply_fp, incoming);
                // Send end of data element
                reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_LOG_VALUE_WRITE;
                memset (& (reply.data[3]), 0xFF, RUUVI_ENDPOINT_STANDARD_PAYLOAD_LENGTH);
                break;

            default:
                reply.data[RUUVI_ENDPOINT_STANDARD_TYPE_INDEX] = RUUVI_ENDPOINT_STANDARD_TYPE_ERROR;
                err_code |= RUUVI_DRIVER_ERROR_NOT_SUPPORTED;
        }
    }

    reply.data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX] =
        incoming->data[RUUVI_ENDPOINT_STANDARD_SOURCE_INDEX];
    reply.data[RUUVI_ENDPOINT_STANDARD_SOURCE_INDEX] =
        incoming->data[RUUVI_ENDPOINT_STANDARD_DESTINATION_INDEX];
    reply.data_length = RUUVI_ENDPOINT_STANDARD_MESSAGE_LENGTH;

    while (RUUVI_DRIVER_ERROR_NO_MEM == reply_fp (&reply))
    {
        ruuvi_interface_yield();
    }

    return err_code;
}

static void heartbeat_send (void * p_event_data, uint16_t event_size)
{
    ruuvi_interface_communication_message_t msg = {0};
    msg.data_length = m_heartbeat_data_max_len;
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if ( (NULL != heartbeat_target) && (NULL != heartbeat_encoder))
    {
        // get message to send
        err_code |= heartbeat_encoder (msg.data);
        // send sensor data
        err_code |= heartbeat_target (&msg);
    }

    if (RUUVI_DRIVER_SUCCESS == err_code)
    {
        ruuvi_interface_watchdog_feed();
    }

    RUUVI_DRIVER_ERROR_CHECK (err_code, ~RUUVI_DRIVER_ERROR_FATAL);
}

static void heartbeat_schedule_isr (void * p_context)
{
    ruuvi_interface_scheduler_event_put (NULL, 0, heartbeat_send);
}

ruuvi_driver_status_t task_communication_heartbeat_configure (const uint32_t interval_ms,
        const size_t max_len,
        const heartbeat_data_fp_t data_src,
        const ruuvi_interface_communication_xfer_fp_t send)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (NULL == heartbeat_timer)
    {
        err_code |= ruuvi_interface_timer_create (&heartbeat_timer,
                    RUUVI_INTERFACE_TIMER_MODE_REPEATED, heartbeat_schedule_isr);

        if (RUUVI_DRIVER_SUCCESS != err_code)
        {
            return err_code;
        }
    }

    ruuvi_interface_timer_stop (heartbeat_timer);

    if (NULL == send || NULL == data_src)
    {
        return RUUVI_DRIVER_ERROR_NULL;
    }

    m_heartbeat_data_max_len = max_len;
    heartbeat_target = send;
    heartbeat_encoder = data_src;

    if (0 != interval_ms)
    {
        err_code |= ruuvi_interface_timer_start (heartbeat_timer, interval_ms);
    }

    return err_code;
}
