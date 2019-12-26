#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoint_5.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_communication_ble4_advertising.h"
#include "ruuvi_interface_log.h"
#include "task_acceleration.h"
#include "task_adc.h"
#include "task_environmental.h"
#include "task_sensor.h"

#include <string.h>

#ifndef TASK_SENSOR_LOG_LEVEL
#define TASK_SENSOR_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

#define LOG(msg) ruuvi_interface_log(TASK_SENSOR_LOG_LEVEL, msg)
#define LOGD(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_DEBUG, msg)
#define LOGHEX(msg, len) ruuvi_interface_log_hex(TASK_SENSOR_LOG_LEVEL, msg, len)

ruuvi_driver_status_t task_sensor_configure (ruuvi_driver_sensor_t * const sensor,
        ruuvi_driver_sensor_configuration_t * const config,
        const char * const unit)
{
    if (NULL == sensor || NULL == config)
    {
        return RUUVI_DRIVER_ERROR_NULL;
    }

    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    LOGD ("\r\nAttempting to configure ");
    LOGD (sensor->name);
    LOGD (" with:\r\n");
    ruuvi_interface_log_sensor_configuration (RUUVI_INTERFACE_LOG_DEBUG, config, unit);
    err_code |= sensor->configuration_set (sensor, config);
    RUUVI_DRIVER_ERROR_CHECK (err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    LOGD ("Actual configuration:\r\n");
    ruuvi_interface_log_sensor_configuration (RUUVI_INTERFACE_LOG_DEBUG, config, unit);
    return err_code;
}

ruuvi_endpoint_status_t task_sensor_encode_to_5 (uint8_t * const buffer)
{
    if (NULL == buffer)
    {
        return RUUVI_ENDPOINT_ERROR_NULL;
    }

    ruuvi_driver_status_t driver_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_endpoint_status_t err_code = RUUVI_ENDPOINT_SUCCESS;
    static uint16_t measurement_counter = 0;
    uint8_t movement_counter = 0;
    ruuvi_driver_sensor_data_t acceleration = {0};
    float acceleration_values[3];
    acceleration.data = acceleration_values;
    acceleration.fields.datas.acceleration_x_g = 1;
    acceleration.fields.datas.acceleration_y_g = 1;
    acceleration.fields.datas.acceleration_z_g = 1;
    float vdd;
    ruuvi_driver_sensor_data_t environmental = {0};
    float environmental_values[3];
    environmental.data = environmental_values;
    environmental.fields.datas.humidity_rh = 1;
    environmental.fields.datas.temperature_c = 1;
    environmental.fields.datas.pressure_pa = 1;
    driver_code |= task_acceleration_data_get (&acceleration);
    driver_code |= task_acceleration_movement_count_get (&movement_counter);
    driver_code |= task_environmental_data_get (&environmental);
    driver_code |= task_adc_vdd_get (&vdd);
    ruuvi_endpoint_5_data_t ep5_data = {0};
    ep5_data.accelerationx_g = ruuvi_driver_sensor_data_parse (&acceleration,
                               (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.acceleration_x_g = 1
    });
    ep5_data.accelerationy_g = ruuvi_driver_sensor_data_parse (&acceleration,
                               (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.acceleration_y_g = 1
    });
    ep5_data.accelerationz_g = ruuvi_driver_sensor_data_parse (&acceleration,
                               (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.acceleration_z_g = 1
    });
    ep5_data.battery_v       = vdd;
    ep5_data.humidity_rh     = ruuvi_driver_sensor_data_parse (&environmental,
                               (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.humidity_rh = 1
    });
    ep5_data.temperature_c   = ruuvi_driver_sensor_data_parse (&environmental,
                               (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.temperature_c = 1
    });
    ep5_data.pressure_pa     = ruuvi_driver_sensor_data_parse (&environmental,
                               (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.pressure_pa = 1
    });
    ep5_data.movement_count  = movement_counter;
    driver_code |= ruuvi_interface_communication_radio_address_get (& (ep5_data.address));
    driver_code |= ruuvi_interface_communication_ble4_advertising_tx_power_get (
                       &ep5_data.tx_power);
    ep5_data.measurement_count = measurement_counter++;

    // Wrap measurement counter off the invalid.
    if (measurement_counter == UINT16_MAX)
    {
        measurement_counter = 0;
    }

    err_code = ruuvi_endpoint_5_encode (buffer, &ep5_data);
    return ( (RUUVI_DRIVER_SUCCESS | RUUVI_ENDPOINT_SUCCESS) == (err_code | driver_code)) ?
           RUUVI_ENDPOINT_SUCCESS : RUUVI_ENDPOINT_ERROR_ENCODING;
}

ruuvi_driver_sensor_t * task_sensor_find_backend (ruuvi_driver_sensor_t * const
        sensor_list,
        const size_t count, const char * const name)
{
    ruuvi_driver_sensor_t * p_sensor = NULL;

    for (size_t ii = 0; ii < count; ii++)
    {
        if (0 == strcmp (sensor_list[ii].name, name))
        {
            p_sensor = & (sensor_list[ii]);
            break;
        }
    }

    return p_sensor;
}