#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_log.h"
#include "task_communication.h"
#include "task_rtc.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

static task_communication_api_t rtc_api =
{
    .sensor      = NULL,
    .offset_set  = task_rtc_comapi_offset_get,
    .offset_get  = task_rtc_comapi_offset_get,
    .data_get    = task_rtc_comapi_data_get,
    .data_target = NULL,
    .log_cfg     = task_rtc_logging_configure
};

static uint64_t offset;

uint64_t task_rtc_millis()
{
    return ruuvi_interface_rtc_millis() + offset;
}

ruuvi_driver_status_t task_rtc_init (void)
{
    // Use task_rtc function to apply offset configured by user to sensor values.
    ruuvi_driver_sensor_timestamp_function_set (task_rtc_millis);
    return ruuvi_interface_rtc_init();
}

ruuvi_driver_status_t task_rtc_logging_configure (const task_api_data_target_t target,
        const uint8_t interval)
{
    return RUUVI_DRIVER_ERROR_NOT_IMPLEMENTED;
}

ruuvi_driver_status_t task_rtc_millis_offset_set (const int64_t _offset)
{
    uint64_t time = ruuvi_interface_rtc_millis();

    if (0 > time + offset)
    {
        return RUUVI_DRIVER_ERROR_INVALID_PARAM;
    }

    offset = _offset;
    return RUUVI_DRIVER_SUCCESS;
}

int64_t task_rtc_millis_offset_get()
{
    return offset;
}

void task_rtc_comapi_offset_get (uint8_t * const bytes)
{
    if (NULL == bytes)
    {
        return;
    }

    bytes[0]  = offset >> 56;
    bytes[1]  = offset >> 48;
    bytes[2]  = offset >> 40;
    bytes[3]  = offset >> 32;
    bytes[4]  = offset >> 24;
    bytes[5]  = offset >> 16;
    bytes[6]  = offset >> 8;
    bytes[7]  = offset >> 0;
}

void task_rtc_comapi_offset_set (uint8_t * const bytes)
{
    if (NULL == bytes)
    {
        return;
    }

    offset = ( (uint64_t) bytes[0]  << 56) +
             ( (uint64_t) bytes[1]  << 48) +
             ( (uint64_t) bytes[2]  << 40) +
             ( (uint64_t) bytes[3]  << 32) +
             ( (uint64_t) bytes[4]  << 24) +
             ( (uint64_t) bytes[5]  << 16) +
             ( (uint64_t) bytes[6]  << 8)  +
             ( (uint64_t) bytes[7]  << 0);
}

void task_rtc_comapi_data_get (uint8_t * const bytes)
{
    if (NULL == bytes)
    {
        return;
    }

    uint64_t millis = task_rtc_millis();
    bytes[0]  = millis >> 56;
    bytes[1]  = millis >> 48;
    bytes[2]  = millis >> 40;
    bytes[3]  = millis >> 32;
    bytes[4]  = millis >> 24;
    bytes[5]  = millis >> 16;
    bytes[6]  = millis >> 8;
    bytes[7]  = millis >> 0;
}

ruuvi_driver_status_t task_rtc_api_get (task_communication_api_t ** api)
{
    if (NULL == api)
    {
        return RUUVI_DRIVER_ERROR_NULL;
    }

    *api = &rtc_api;
    return RUUVI_DRIVER_SUCCESS;
}