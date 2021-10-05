#include "app_dataformats.h"
#include "app_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_3.h"
#include "ruuvi_endpoint_5.h"
#include "ruuvi_endpoint_8.h"
#include "ruuvi_endpoint_fa.h"
#include "ruuvi_task_adc.h"

#include <math.h>

#ifdef CEEDLING
#   define TESTABLE_STATIC
#else
#   define TESTABLE_STATIC static
#endif

/**
 * @brief Return next dataformat to send
 *
 * @param[in] formats Enabled formats
 * @param[in] state Current state of dataformat picker.
 * @return    Next dataformat to use in app.
 */
app_dataformat_t app_dataformat_next (const app_dataformats_t formats,
                                      const app_dataformat_t state)
{
    return DF_INVALID;
}

TESTABLE_STATIC rd_status_t
encode_to_3 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    rd_status_t err_code = RD_SUCCESS;
    re_3_data_t ep_data = {0};
    ep_data.accelerationx_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_X_FIELD);
    ep_data.accelerationy_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_Y_FIELD);
    ep_data.accelerationz_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_Z_FIELD);
    ep_data.humidity_rh       = rd_sensor_data_parse (data, RD_SENSOR_HUMI_FIELD);
    ep_data.pressure_pa       = rd_sensor_data_parse (data, RD_SENSOR_PRES_FIELD);
    ep_data.temperature_c     = rd_sensor_data_parse (data, RD_SENSOR_TEMP_FIELD);
    err_code |= rt_adc_vdd_get (&ep_data.battery_v);
    re_3_encode (output, &ep_data, RD_FLOAT_INVALID);
    *output_length = RE_3_DATA_LENGTH;
    return err_code;
}

TESTABLE_STATIC rd_status_t
encode_to_5 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}

TESTABLE_STATIC rd_status_t
encode_to_8 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}

TESTABLE_STATIC rd_status_t
encode_to_fa (uint8_t * const output,
              size_t * const output_length,
              const rd_sensor_data_t * const data)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}

/**
 * @brief Encode data into given buffer with given format.
 *
 * A call to this function will increment measurement sequence counter
 * where applicable. Sensors are read to get latest data from board.
 *
 * @param[out] output Buffer to which data is encoded.
 * @param[in,out] output_length Input: Size of output buffer.
 *                              Output: Size of encoded data.
 * @param[in] format Format to encode data into.
 *
 */
rd_status_t app_dataformat_encode (uint8_t * const output,
                                   size_t * const output_length,
                                   const app_dataformat_t format)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    data.fields = app_sensor_available_data();
    float data_values[rd_sensor_data_fieldcount (&data)];
    data.data = data_values;
    app_sensor_get (&data);

    switch (format)
    {
        case DF_3:
            encode_to_3 (output, output_length, &data);
            break;

        default:
            err_code |= RD_ERROR_NOT_IMPLEMENTED;
    }

    return err_code;
}