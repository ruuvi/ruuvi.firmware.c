#include "app_dataformats.h"
#include "app_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_3.h"
#include "ruuvi_endpoint_5.h"
#include "ruuvi_endpoint_8.h"
#include "ruuvi_endpoint_fa.h"
#include "ruuvi_interface_aes.h"
#include "ruuvi_interface_communication_ble_advertising.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_task_adc.h"

#include <math.h>
#include <string.h>

#ifdef CEEDLING
#   define TESTABLE_STATIC
#else
#   define TESTABLE_STATIC static
#endif

#if (RE_8_ENABLED || RE_FA_ENABLED)
uint32_t app_data_encrypt (const uint8_t * const cleartext,
                           uint8_t * const ciphertext,
                           const size_t data_size,
                           const uint8_t * const key,
                           const size_t key_size)
{
    rd_status_t err_code = RD_SUCCESS;
    uint32_t ret_code = 0;

    if (16U != key_size)
    {
        err_code |= RD_ERROR_INVALID_LENGTH;
    }
    else
    {
        err_code |= ri_aes_ecb_128_encrypt (cleartext,
                                            ciphertext,
                                            key,
                                            data_size);
    }

    if (RD_SUCCESS != err_code)
    {
        ret_code = 1;
    }

    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
    return ret_code;
}
#endif

app_dataformat_t app_dataformat_next (const app_dataformats_t formats,
                                      const app_dataformat_t state)
{
    // TODO: Return enabled value instead of hardcoded one
    return DF_5;
}

#if RE_3_ENABLED
TESTABLE_STATIC rd_status_t
encode_to_3 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    rd_status_t err_code = RD_SUCCESS;
    re_status_t enc_code = RE_SUCCESS;
    re_3_data_t ep_data = {0};
    ep_data.accelerationx_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_X_FIELD);
    ep_data.accelerationy_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_Y_FIELD);
    ep_data.accelerationz_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_Z_FIELD);
    ep_data.humidity_rh       = rd_sensor_data_parse (data, RD_SENSOR_HUMI_FIELD);
    ep_data.pressure_pa       = rd_sensor_data_parse (data, RD_SENSOR_PRES_FIELD);
    ep_data.temperature_c     = rd_sensor_data_parse (data, RD_SENSOR_TEMP_FIELD);
    err_code |= rt_adc_vdd_get (&ep_data.battery_v);
    enc_code |= re_3_encode (output, &ep_data, RD_FLOAT_INVALID);

    if (RE_SUCCESS != enc_code)
    {
        err_code |= RD_ERROR_INTERNAL;
    }

    *output_length = RE_3_DATA_LENGTH;
    return err_code;
}
#endif

#if RE_5_ENABLED
TESTABLE_STATIC rd_status_t
encode_to_5 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    static uint16_t ep_5_measurement_count = 0;
    rd_status_t err_code = RD_SUCCESS;
    re_status_t enc_code = RE_SUCCESS;
    re_5_data_t ep_data = {0};
    ep_5_measurement_count++;
    ep_5_measurement_count %= (RE_5_SEQCTR_MAX + 1);
    ep_data.accelerationx_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_X_FIELD);
    ep_data.accelerationy_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_Y_FIELD);
    ep_data.accelerationz_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_Z_FIELD);
    ep_data.humidity_rh       = rd_sensor_data_parse (data, RD_SENSOR_HUMI_FIELD);
    ep_data.pressure_pa       = rd_sensor_data_parse (data, RD_SENSOR_PRES_FIELD);
    ep_data.temperature_c     = rd_sensor_data_parse (data, RD_SENSOR_TEMP_FIELD);
    ep_data.measurement_count = ep_5_measurement_count;
    uint8_t mvtctr = (uint8_t) (app_sensor_event_count_get() % (RE_5_MVTCTR_MAX + 1));
    ep_data.movement_count    = mvtctr;
    err_code |= ri_radio_address_get (&ep_data.address);
    err_code |= ri_adv_tx_power_get (&ep_data.tx_power);
    err_code |= rt_adc_vdd_get (&ep_data.battery_v);
    enc_code |= re_5_encode (output, &ep_data);

    if (RE_SUCCESS != enc_code)
    {
        err_code |= RD_ERROR_INTERNAL;
    }

    *output_length = RE_5_DATA_LENGTH;
    return err_code;
}
#endif

#if RE_8_ENABLED
#ifndef APP_8_KEY
// "RuuviComRuuviTag"
#define APP_8_KEY { 0x52, 0x75, 0x75, 0x76, 0x69, 0x43, 0x6F, 0x6D, 0x52, 0x75, 0x75, 0x76, 0x69, 0x54, 0x61, 0x67}
#endif
static const uint8_t ep_8_key[RE_8_CIPHERTEXT_LENGTH] = APP_8_KEY;

TESTABLE_STATIC rd_status_t
ep_8_key_generate (uint8_t * const key)
{
    rd_status_t err_code = RD_SUCCESS;
    memcpy (key, ep_8_key, RE_8_CIPHERTEXT_LENGTH);
    uint64_t device_id = 0;
    err_code |= ri_comm_id_get (&device_id);

    for (uint8_t ii = 0U; ii < 8; ii++)
    {
        key[ii] = key[ii] ^ ( (device_id >> (ii * 8U)) & 0xFFU);
    }

    return err_code;
}

TESTABLE_STATIC rd_status_t
encode_to_8 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    static uint16_t ep_8_measurement_count = 0;
    uint8_t final_key[RE_8_CIPHERTEXT_LENGTH] = { 0 };
    rd_status_t err_code = RD_SUCCESS;
    re_status_t enc_code = RE_SUCCESS;
    re_8_data_t ep_data = {0};
    ep_8_measurement_count++;
    ep_8_measurement_count %= (RE_8_SEQCTR_MAX + 1);
    ep_data.humidity_rh       = rd_sensor_data_parse (data, RD_SENSOR_HUMI_FIELD);
    ep_data.pressure_pa       = rd_sensor_data_parse (data, RD_SENSOR_PRES_FIELD);
    ep_data.temperature_c     = rd_sensor_data_parse (data, RD_SENSOR_TEMP_FIELD);
    ep_data.message_counter = ep_8_measurement_count;
    uint8_t mvtctr = (uint8_t) (app_sensor_event_count_get() % (RE_8_MVTCTR_MAX + 1));
    ep_data.movement_count    = mvtctr;
    err_code |= ep_8_key_generate (final_key);
    err_code |= ri_radio_address_get (&ep_data.address);
    err_code |= ri_adv_tx_power_get (&ep_data.tx_power);
    err_code |= rt_adc_vdd_get (&ep_data.battery_v);
    enc_code |= re_8_encode (output,
                             &ep_data,
                             &app_data_encrypt,
                             final_key,
                             RE_8_CIPHERTEXT_LENGTH);

    if (RE_SUCCESS != enc_code)
    {
        err_code |= RD_ERROR_INTERNAL;
    }

    *output_length = RE_5_DATA_LENGTH;
    return err_code;
}
#endif

#if RE_FA_ENABLED
#ifndef APP_FA_KEY
#define APP_FA_KEY {00, 11, 22, 33, 44, 55, 66, 77, 88, 99, 11, 12, 13, 14, 15, 16}
#endif
static const uint8_t ep_fa_key[RE_FA_CIPHERTEXT_LENGTH] = APP_FA_KEY;

TESTABLE_STATIC rd_status_t
encode_to_fa (uint8_t * const output,
              size_t * const output_length,
              const rd_sensor_data_t * const data)
{
    static uint8_t ep_fa_measurement_count = 0;
    rd_status_t err_code = RD_SUCCESS;
    re_status_t enc_code = RE_SUCCESS;
    re_fa_data_t ep_data = {0};
    ep_fa_measurement_count++;
    ep_fa_measurement_count %= 0xFFU;
    ep_data.accelerationx_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_X_FIELD);
    ep_data.accelerationy_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_Y_FIELD);
    ep_data.accelerationz_g   = rd_sensor_data_parse (data, RD_SENSOR_ACC_Z_FIELD);
    ep_data.humidity_rh       = rd_sensor_data_parse (data, RD_SENSOR_HUMI_FIELD);
    ep_data.pressure_pa       = rd_sensor_data_parse (data, RD_SENSOR_PRES_FIELD);
    ep_data.temperature_c     = rd_sensor_data_parse (data, RD_SENSOR_TEMP_FIELD);
    ep_data.message_counter   = ep_fa_measurement_count;
    err_code |= rt_adc_vdd_get (&ep_data.battery_v);
    err_code |= ri_radio_address_get (&ep_data.address);
    enc_code |= re_fa_encode (output,
                              &ep_data,
                              &app_data_encrypt,
                              ep_fa_key,
                              RE_FA_CIPHERTEXT_LENGTH); //!< Cipher length == key lenght

    if (RE_SUCCESS != enc_code)
    {
        err_code |= RD_ERROR_INTERNAL;
    }

    *output_length = RE_FA_DATA_LENGTH;
    return err_code;
}
#endif

rd_status_t app_dataformat_encode (uint8_t * const output,
                                   size_t * const output_length,
                                   const rd_sensor_data_t * const p_data,
                                   const app_dataformat_t format)
{
    rd_status_t err_code = RD_SUCCESS;

    switch (format)
    {
#       if RE_3_ENABLED

        case DF_3:
            err_code |= encode_to_3 (output, output_length, p_data);
            break;
#       endif
#       if RE_5_ENABLED

        case DF_5:
            err_code |= encode_to_5 (output, output_length, p_data);
            break;
#       endif
#       if RE_8_ENABLED

        case DF_8:
            err_code |= encode_to_8 (output, output_length, p_data);
            break;
#       endif
#       if RE_FA_ENABLED

        case DF_FA:
            err_code |= encode_to_fa (output, output_length, p_data);
            break;
#       endif

        default:
            err_code |= RD_ERROR_NOT_ENABLED;
    }

    return err_code;
}
