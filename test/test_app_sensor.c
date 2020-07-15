#include "unity.h"

#include "app_sensor.h"

#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_driver_sensor.h"
#include "mock_ruuvi_interface_gpio.h"
#include "mock_ruuvi_interface_i2c.h"
#include "mock_ruuvi_interface_rtc.h"
#include "mock_ruuvi_interface_spi.h"
#include "mock_ruuvi_interface_adc_ntc.h"
#include "mock_ruuvi_interface_adc_photo.h"
#include "mock_ruuvi_interface_bme280.h"
#include "mock_ruuvi_interface_lis2dh12.h"
#include "mock_ruuvi_interface_shtcx.h"
#include "mock_ruuvi_interface_tmp117.h"
#include "mock_ruuvi_task_adc.h"
#include "mock_ruuvi_task_sensor.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include <string.h>


void setUp (void)
{
    rd_error_check_Ignore();
    m_sensors_init();
}

void tearDown (void)
{
}

void test_app_sensor_init_ok (void)
{
    rd_status_t err_code;
    ri_spi_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_i2c_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_rtc_init_ExpectAndReturn (RD_SUCCESS);
    rd_sensor_timestamp_function_set_ExpectAnyArgsAndReturn (RD_SUCCESS);
    // Emulate power pin for one sensor
    ri_gpio_id_t    pwr_0 = m_sensors[0]->pwr_pin;
    ri_gpio_state_t act_0 = m_sensors[0]->pwr_on;
    m_sensors[0]->pwr_pin = 1;
    m_sensors[0]->pwr_on = 1;
    ri_gpio_configure_ExpectAndReturn (1, RI_GPIO_MODE_OUTPUT_HIGHDRIVE, RD_SUCCESS);
    ri_gpio_write_ExpectAndReturn (1, 1, RD_SUCCESS);

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        rt_sensor_initialize_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
        rt_sensor_load_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
        rt_sensor_configure_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
    }

    ri_radio_activity_callback_set_Expect (&on_radio);
    err_code = app_sensor_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
    m_sensors[0]->pwr_pin = pwr_0;
    m_sensors[0]->pwr_on = act_0;
}

static const rd_sensor_data_fields_t fields_bme =
{
    .datas.temperature_c = 1,
    .datas.humidity_rh = 1,
    .datas.pressure_pa = 1
};
static const rd_sensor_data_fields_t fields_lis =
{
    .datas.temperature_c = 1,
    .datas.acceleration_x_g = 1,
    .datas.acceleration_y_g = 1,
    .datas.acceleration_z_g = 1
};
static const rd_sensor_data_fields_t fields_photo =
{
    .datas.luminosity = 1
};
static const rd_sensor_data_fields_t fields_expected =
{
    .datas.temperature_c = 1,
    .datas.humidity_rh = 1,
    .datas.pressure_pa = 1,
    .datas.acceleration_x_g = 1,
    .datas.acceleration_y_g = 1,
    .datas.acceleration_z_g = 1
};

void test_app_sensor_init_first_time (void)
{
    rd_status_t err_code;
    ri_spi_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_i2c_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_rtc_init_ExpectAndReturn (RD_SUCCESS);
    rd_sensor_timestamp_function_set_ExpectAnyArgsAndReturn (RD_SUCCESS);

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        rt_sensor_initialize_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
        rt_sensor_load_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_ERROR_NOT_FOUND);
        rt_sensor_configure_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
        rt_sensor_store_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
    }

    ri_radio_activity_callback_set_Expect (&on_radio);
    err_code = app_sensor_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_sensor_init_not_found (void)
{
    rd_status_t err_code;
    ri_spi_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_i2c_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_rtc_init_ExpectAndReturn (RD_SUCCESS);
    rd_sensor_timestamp_function_set_ExpectAnyArgsAndReturn (RD_SUCCESS);

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        rt_sensor_initialize_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_ERROR_NOT_FOUND);
    }

    ri_radio_activity_callback_set_Expect (&on_radio);
    err_code = app_sensor_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_sensor_init_selftest_fail (void)
{
    rd_status_t err_code;
    ri_spi_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_i2c_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_rtc_init_ExpectAndReturn (RD_SUCCESS);
    rd_sensor_timestamp_function_set_ExpectAnyArgsAndReturn (RD_SUCCESS);

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        size_t retries = 0;

        do
        {
            rt_sensor_initialize_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_ERROR_SELFTEST);
        } while (retries++ < APP_SENSOR_SELFTEST_RETRIES);
    }

    ri_radio_activity_callback_set_Expect (&on_radio);
    err_code = app_sensor_init();
    TEST_ASSERT (RD_ERROR_SELFTEST == err_code);
}

static rd_status_t mock_uninit (rd_sensor_t * sensor, rd_bus_t bus, uint8_t handle)
{
    return RD_SUCCESS;
}

void test_app_sensor_uninit_ok (void)
{
    rd_status_t err_code;
    // Emulate power pin for one sensor
    ri_gpio_id_t    pwr_1 = m_sensors[1]->pwr_pin;
    ri_gpio_state_t act_1 = m_sensors[1]->pwr_on;
    m_sensors[1]->pwr_pin = 1;
    m_sensors[1]->pwr_on = 1;
    rd_sensor_is_init_ExpectAnyArgsAndReturn (false);

    for (size_t ii = 1; ii < SENSOR_COUNT; ii++)
    {
        m_sensors[ii]->sensor.uninit = &mock_uninit;
        rd_sensor_is_init_ExpectAnyArgsAndReturn (true);

        if (m_sensors[ii]->pwr_pin != RI_GPIO_ID_UNUSED)
        {
            ri_gpio_write_ExpectAndReturn (m_sensors[ii]->pwr_pin,
                                           !m_sensors[ii]->pwr_on,
                                           RD_SUCCESS);
            ri_gpio_configure_ExpectAndReturn (1, RI_GPIO_MODE_HIGH_Z, RD_SUCCESS);
        }
    }

    ri_spi_uninit_ExpectAndReturn (RD_SUCCESS);
    ri_i2c_uninit_ExpectAndReturn (RD_SUCCESS);
    rd_sensor_timestamp_function_set_ExpectAndReturn (NULL, RD_SUCCESS);
    ri_rtc_uninit_ExpectAndReturn (RD_SUCCESS);
    ri_radio_activity_callback_set_Expect (NULL);
    err_code = app_sensor_uninit();
    TEST_ASSERT (RD_SUCCESS == err_code);
    m_sensors[1]->pwr_pin = pwr_1;
    m_sensors[1]->pwr_on = act_1;
}

void test_app_sensor_on_radio_before_ok (void)
{
    static rd_sensor_configuration_t configuration =
    {
        .dsp_function  = RD_SENSOR_CFG_DEFAULT,
        .dsp_parameter = RD_SENSOR_CFG_DEFAULT,
        .mode          = RD_SENSOR_CFG_SINGLE,
        .resolution    = RD_SENSOR_CFG_DEFAULT,
        .samplerate    = RD_SENSOR_CFG_DEFAULT,
        .scale         = RD_SENSOR_CFG_DEFAULT
    };
    static uint64_t time = 1ULL;
    ri_rtc_millis_ExpectAndReturn (time);
    rt_adc_vdd_prepare_ExpectWithArrayAndReturn (&configuration, 1, RD_SUCCESS);
    on_radio (RI_RADIO_BEFORE);
}

void test_app_sensor_on_radio_after_ok (void)
{
    static uint64_t time = 1ULL;
    static bool adc_init_value = true;
    ri_rtc_millis_ExpectAndReturn (time);
    rt_adc_is_init_ExpectAndReturn (true);
    ri_rtc_millis_ExpectAndReturn (time);
    rt_adc_vdd_sample_ExpectAndReturn (RD_SUCCESS);
    on_radio (RI_RADIO_AFTER);
}

/**
 * @brief Return available data types.
 *
 * @note This is refreshed from sensor structs RAM which makes
 * this a relatively expensive function call due to looping over
 * all sensor contexts. Cache this if microseconds count in your application.
 *
 * @return Listing of data the application can provide.
 */
void test_app_sensor_available_data (void)
{
    if (SENSOR_COUNT > 2)
    {
        for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), (ii < 2));
        }

        rd_sensor_data_fields_t fields_found = {0};
        // Mock provided data
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_lis;
        m_sensors[2]->sensor.provides = fields_photo;
        fields_found = app_sensor_available_data();
        TEST_ASSERT (!memcmp (&fields_found.bitfield, &fields_expected.bitfield,
                              sizeof (fields_expected.bitfield)));
    }
}

/**
 * @brief Return last sampled data.
 *
 * This function checks loops through initialized sensors until all data in
 * data->fields is valid or all sensors are checked.
 *
 * @retval RD_SUCCESS on success, NOT_FOUND sensors are allowed.
 * @retval RD_ERROR_SELFTEST if sensor is found on the bus and fails selftest.
 */
static size_t data_get_calls = 0;
static rd_status_t mock_data_get (rd_sensor_data_t * const data)
{
    switch (data_get_calls++)
    {
        case 0:
            data->valid.bitfield |= (data->fields.bitfield & fields_bme.bitfield);
            break;

        case 1:
            data->valid.bitfield |= (data->fields.bitfield & fields_lis.bitfield);
            break;

        case 2:
            data->valid.bitfield |= (data->fields.bitfield & fields_photo.bitfield);
            break;

        default:
            break;
    }

    return RD_SUCCESS;
}

void test_app_sensor_get (void)
{
    rd_sensor_data_t data = {0};
    data.fields.bitfield |= fields_expected.bitfield;
    rd_sensor_data_fp dg_0 = m_sensors[0]->sensor.data_get;
    m_sensors[0]->sensor.data_get = &mock_data_get;
    m_sensors[1]->sensor.data_get = &mock_data_get;
    m_sensors[2]->sensor.data_get = &mock_data_get;

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor),
                                           (NULL != & (m_sensors[ii]->sensor.data_get)));
    }

    app_sensor_get (&data);
    TEST_ASSERT (!memcmp (&data.valid.bitfield, &fields_expected.bitfield,
                          sizeof (fields_expected.bitfield)));
    m_sensors[0]->sensor.data_get = dg_0;
}
