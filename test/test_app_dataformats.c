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
    static rd_sensor_data_fields_t fields = {0}; //!< Gets ignored in test.
    app_sensor_available_data_ExpectAndReturn (fields);
    rd_sensor_data_fieldcount_ExpectAnyArgsAndReturn (7);
    app_sensor_get_ExpectAnyArgsAndReturn (RD_SUCCESS);
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
    status = app_dataformat_encode (output, &output_length, format);
    TEST_ASSERT (RD_SUCCESS == status);
}

void test_app_dataformat_encode_5 (void)
{
}

void test_app_dataformat_encode_8 (void)
{
}

void test_app_dataformat_encode_fa (void)
{
}

#endif // TEST
