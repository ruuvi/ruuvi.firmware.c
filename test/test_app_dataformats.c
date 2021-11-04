#ifdef TEST

#include "unity.h"

#include "app_dataformats.h"
#include <math.h>


#include "mock_app_sensor.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_driver_sensor.h"
#include "mock_ruuvi_endpoints.h"
#include "mock_ruuvi_endpoint_3.h"
#include "mock_ruuvi_endpoint_5.h"
#include "mock_ruuvi_endpoint_8.h"
#include "mock_ruuvi_endpoint_fa.h"
#include "mock_ruuvi_interface_aes.h"
#include "mock_ruuvi_interface_communication_ble_advertising.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_communication.h"
#include "mock_ruuvi_task_adc.h"

void setUp (void)
{
    rd_error_check_Ignore();
}

void tearDown (void)
{
}

/**
 * @brief Return next dataformat to send
 *
 * @param[in] formats Enabled formats
 * @param[in] state Current state of dataformat picker.
 * @return    Next dataformat to use in app.
 *
app_dataformat_t app_dataformat_next(const app_dataformats_t formats,
                                     const app_dataformat_t state);

**
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
 *
rd_status_t app_dataformat_encode(uint8_t * const output,
                                  size_t * const output_length,
                                  const app_dataformat_t format);
 */

void test_app_dataformat_encode_3_ok (void)
{
    uint8_t output[24] = {0};
    size_t output_length = sizeof (output);
    app_dataformat_t format = DF_3;
    float voltage = 2.5F;
    rd_status_t status = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rt_adc_vdd_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_adc_vdd_get_ReturnThruPtr_vdd (&voltage);
    re_3_encode_ExpectAndReturn (output, NULL, NAN, RE_SUCCESS);
    re_3_encode_IgnoreArg_data();
    status = app_dataformat_encode (output, &output_length, &data, format);
    TEST_ASSERT (RD_SUCCESS == status);
    TEST_ASSERT (RE_3_DATA_LENGTH == output_length);
}

void test_app_dataformat_encode_3_error (void)
{
    uint8_t output[24] = {0};
    size_t output_length = sizeof (output);
    app_dataformat_t format = DF_3;
    float voltage = 2.5F;
    rd_status_t status = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rt_adc_vdd_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_adc_vdd_get_ReturnThruPtr_vdd (&voltage);
    re_3_encode_ExpectAndReturn (output, NULL, NAN, RE_ERROR_ENCODING);
    re_3_encode_IgnoreArg_data();
    status = app_dataformat_encode (output, &output_length, &data, format);
    TEST_ASSERT (RD_ERROR_INTERNAL == status);
}


void test_app_dataformat_encode_5_ok (void)
{
    uint8_t output[24] = {0};
    size_t output_length = sizeof (output);
    app_dataformat_t format = DF_5;
    float voltage = 2.5F;
    uint64_t address = 0x0000AABBCCDDEEFF;
    int8_t power = 4;
    rd_status_t status = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    app_sensor_event_count_get_ExpectAndReturn (1);
    ri_radio_address_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_radio_address_get_ReturnThruPtr_address (&address);
    ri_adv_tx_power_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_adv_tx_power_get_ReturnThruPtr_dbm (&power);
    rt_adc_vdd_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_adc_vdd_get_ReturnThruPtr_vdd (&voltage);
    re_5_encode_ExpectAndReturn (NULL, NULL, RE_SUCCESS);
    re_5_encode_IgnoreArg_buffer();
    re_5_encode_IgnoreArg_data();
    status = app_dataformat_encode (output, &output_length, &data, format);
    TEST_ASSERT (RD_SUCCESS == status);
    TEST_ASSERT (RE_5_DATA_LENGTH == output_length);
}

void test_app_dataformat_encode_5_error (void)
{
    uint8_t output[24] = {0};
    size_t output_length = sizeof (output);
    app_dataformat_t format = DF_5;
    float voltage = 2.5F;
    uint64_t address = 0x0000AABBCCDDEEFF;
    int8_t power = 4;
    rd_status_t status = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    app_sensor_event_count_get_ExpectAndReturn (1);
    ri_radio_address_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_radio_address_get_ReturnThruPtr_address (&address);
    ri_adv_tx_power_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_adv_tx_power_get_ReturnThruPtr_dbm (&power);
    rt_adc_vdd_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_adc_vdd_get_ReturnThruPtr_vdd (&voltage);
    re_5_encode_ExpectAndReturn (NULL, NULL, RE_ERROR_ENCODING);
    re_5_encode_IgnoreArg_buffer();
    re_5_encode_IgnoreArg_data();
    status = app_dataformat_encode (output, &output_length, &data, format);
    TEST_ASSERT (RD_ERROR_INTERNAL == status);
}

void test_app_dataformat_encode_8_ok (void)
{
    uint8_t output[24] = {0};
    size_t output_length = sizeof (output);
    app_dataformat_t format = DF_8;
    float voltage = 2.5F;
    uint64_t address = 0x0000AABBCCDDEEFF;
    int8_t power = 4;
    rd_status_t status = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    app_sensor_event_count_get_ExpectAndReturn (1);
    ri_comm_id_get_ExpectAnyArgsAndReturn (RE_SUCCESS);
    ri_radio_address_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_radio_address_get_ReturnThruPtr_address (&address);
    ri_adv_tx_power_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_adv_tx_power_get_ReturnThruPtr_dbm (&power);
    rt_adc_vdd_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_adc_vdd_get_ReturnThruPtr_vdd (&voltage);
    re_8_encode_ExpectAndReturn (output,
                                 NULL, NULL, NULL,
                                 RE_8_CIPHERTEXT_LENGTH,
                                 RE_SUCCESS);
    re_8_encode_IgnoreArg_cipher();
    re_8_encode_IgnoreArg_data();
    re_8_encode_IgnoreArg_key();
    status = app_dataformat_encode (output, &output_length, &data, format);
    TEST_ASSERT (RD_SUCCESS == status);
    TEST_ASSERT (RE_8_DATA_LENGTH == output_length);
}

void test_app_dataformat_encode_8_error (void)
{
    uint8_t output[24] = {0};
    size_t output_length = sizeof (output);
    app_dataformat_t format = DF_8;
    float voltage = 2.5F;
    uint64_t address = 0x0000AABBCCDDEEFF;
    int8_t power = 4;
    rd_status_t status = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    app_sensor_event_count_get_ExpectAndReturn (1);
    ri_comm_id_get_ExpectAnyArgsAndReturn (RE_SUCCESS);
    ri_radio_address_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_radio_address_get_ReturnThruPtr_address (&address);
    ri_adv_tx_power_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_adv_tx_power_get_ReturnThruPtr_dbm (&power);
    rt_adc_vdd_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_adc_vdd_get_ReturnThruPtr_vdd (&voltage);
    re_8_encode_ExpectAndReturn (output,
                                 NULL, NULL, NULL,
                                 RE_8_CIPHERTEXT_LENGTH,
                                 RE_SUCCESS);
    re_8_encode_IgnoreArg_cipher();
    re_8_encode_IgnoreArg_data();
    re_8_encode_IgnoreArg_key();
    status = app_dataformat_encode (output, &output_length, &data, format);
    TEST_ASSERT (RD_SUCCESS == status);
    TEST_ASSERT (RE_8_DATA_LENGTH == output_length);
}

void test_app_dataformat_encode_fa_ok (void)
{
    uint8_t output[24] = {0};
    size_t output_length = sizeof (output);
    app_dataformat_t format = DF_FA;
    float voltage = 2.5F;
    uint64_t address = 0x0000AABBCCDDEEFF;
    rd_status_t status = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rt_adc_vdd_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_adc_vdd_get_ReturnThruPtr_vdd (&voltage);
    ri_radio_address_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_radio_address_get_ReturnThruPtr_address (&address);
    re_fa_encode_ExpectAndReturn (output,
                                  NULL, NULL, NULL,
                                  RE_FA_CIPHERTEXT_LENGTH,
                                  RE_ERROR_ENCODING);
    re_fa_encode_IgnoreArg_data();
    re_fa_encode_IgnoreArg_cipher();
    re_fa_encode_IgnoreArg_key();
    status = app_dataformat_encode (output, &output_length, &data, format);
    TEST_ASSERT (RD_ERROR_INTERNAL == status);
}

void test_app_dataformat_encode_fa_error (void)
{
    uint8_t output[24] = {0};
    size_t output_length = sizeof (output);
    app_dataformat_t format = DF_FA;
    float voltage = 2.5F;
    uint64_t address = 0x0000AABBCCDDEEFF;
    rd_status_t status = RD_SUCCESS;
    rd_sensor_data_t data = {0};
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    rt_adc_vdd_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    rt_adc_vdd_get_ReturnThruPtr_vdd (&voltage);
    ri_radio_address_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_radio_address_get_ReturnThruPtr_address (&address);
    re_fa_encode_ExpectAndReturn (output,
                                  NULL, NULL, NULL,
                                  RE_FA_CIPHERTEXT_LENGTH,
                                  RE_ERROR_ENCODING);
    re_fa_encode_IgnoreArg_data();
    re_fa_encode_IgnoreArg_cipher();
    re_fa_encode_IgnoreArg_key();
    status = app_dataformat_encode (output, &output_length, &data, format);
    TEST_ASSERT (RD_ERROR_INTERNAL == status);
}

#endif // TEST
