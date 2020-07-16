#include "unity.h"

#include "app_sensor.h"

#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_driver_sensor.h"
#include "mock_ruuvi_interface_gpio.h"
#include "mock_ruuvi_interface_gpio_interrupt.h"
#include "mock_ruuvi_interface_i2c.h"
#include "mock_ruuvi_interface_log.h"
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

extern rt_sensor_ctx_t * m_sensors[];

void setUp (void)
{
    rd_error_check_Ignore();
    ri_log_Ignore();
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

    ri_radio_activity_callback_set_Expect (&on_radio_isr);
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

    ri_radio_activity_callback_set_Expect (&on_radio_isr);
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

    ri_radio_activity_callback_set_Expect (&on_radio_isr);
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

    ri_radio_activity_callback_set_Expect (&on_radio_isr);
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
    on_radio_isr (RI_RADIO_BEFORE);
}

void test_app_sensor_on_radio_after_ok (void)
{
    static uint64_t time = 1ULL;
    ri_rtc_millis_ExpectAndReturn (time);
    rt_adc_is_init_ExpectAndReturn (true);
    ri_rtc_millis_ExpectAndReturn (time);
    rt_adc_vdd_sample_ExpectAndReturn (RD_SUCCESS);
    on_radio_isr (RI_RADIO_AFTER);
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

/**
 * @brief Find and return a sensor which can provide requested data.
 *
 * Loops through sensors in order of priority, if board has SHTC temperature and
 * humidity sensor and LIS2DH12 acceleration and temperature sensor, searching
 * for the sensor will return the one which is first in m_sensors list.
 *
 * Works only witjh initialized sensors, will not return a sensor which is supported
 * in firmawre but not initialized due to self-test error etc.
 *
 * @param[in] data fields which sensor must provide.
 * @return Pointer to SENSOR, NULL if suitable sensor is not found.
 */
void test_app_sensor_find_provider_ok (void)
{
    if (SENSOR_COUNT > 3)
    {
        for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), (ii < 3));
        }

        // Mock provided data
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_lis;
        m_sensors[2]->sensor.provides = fields_photo;
        const rd_sensor_t * const  p_sensor = app_sensor_find_provider (fields_photo);
        TEST_ASSERT (p_sensor == & (m_sensors[2]->sensor));
    }
}

void test_app_sensor_find_provider_overlap (void)
{
    if (SENSOR_COUNT > 3)
    {
        for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), (ii < 3));
        }

        // Mock provided data
        rd_sensor_data_fields_t fields_wanted =
        {
            .datas.temperature_c = 1
        };
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_lis;
        m_sensors[2]->sensor.provides = fields_photo;
        const rd_sensor_t * const  p_sensor = app_sensor_find_provider (fields_wanted);
        TEST_ASSERT (p_sensor == & (m_sensors[0]->sensor));
    }
}

void test_app_sensor_find_provider_empty (void)
{
    if (SENSOR_COUNT > 3)
    {
        for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), (ii));
        }

        // Mock provided data
        rd_sensor_data_fields_t fields_wanted =
        {
            0
        };
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_lis;
        m_sensors[2]->sensor.provides = fields_photo;
        const rd_sensor_t * const  p_sensor = app_sensor_find_provider (fields_wanted);
        TEST_ASSERT (p_sensor == & (m_sensors[1]->sensor));
    }
}

void test_app_sensor_find_provider_no_valid (void)
{
    if (SENSOR_COUNT > 3)
    {
        for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), (ii < 3));
        }

        // Mock provided data
        rd_sensor_data_fields_t fields_wanted =
        {
            .datas.temperature_c = 1,
            .datas.luminosity = 1
        };
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_lis;
        m_sensors[2]->sensor.provides = fields_photo;
        const rd_sensor_t * const  p_sensor = app_sensor_find_provider (fields_wanted);
        TEST_ASSERT (p_sensor == NULL);
    }
}

/**
 * @brief Set threshold for accelerometer interrupts.
 *
 * Accelerometers are high-passed so gravity won't affect given threshold.
 * Acceleration event is triggered when the threshold is exceeded on any axis.
 * Acceleration event ceases when acceleration falls below the threshold, and
 * can then be triggered again. Maximum rate for acceleration events is then
 * accelerometer sample rate divided by two.
 *
 * On acceleration event @ref app_sensor_event_increment is called.
 *
 * @param[in, out] threshold_g In: Thershold of acceleration, > 0. Interpreted as
 *                                 "at least this much". NULL to disable interrupts.
 *                             Out: Configured threshold.
 * @retval RD_SUCCESS if threshold was configured.
 * @retval RD_ERROR_NOT_IMPLEMENTED if threshold is lower than 0 (negative).
 * @retval RD_ERROR_NOT_SUPPORTED if no suitable accelerometer is initialized.
 *
 *
 */
static uint32_t level_interrupt_set_enabled = 0;
static uint32_t level_interrupt_set_disabled = 0;
static rd_status_t mock_level_interrupt_set (const bool enable,
        float * limit_g)
{
    if (enable)
    {
        level_interrupt_set_enabled++;
    }
    else
    {
        level_interrupt_set_disabled++;
    }

    return RD_SUCCESS;
}

void test_app_sensor_acceleration_threshold_set_ok (void)
{
    if (SENSOR_COUNT > 2U && (RI_GPIO_ID_UNUSED != RB_INT_LEVEL_PIN))
    {
        for (size_t ii = 0U; ii < 2U; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), true);
        }

        // Mock provided data
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_lis;
        m_sensors[1]->sensor.level_interrupt_set = &mock_level_interrupt_set;
        level_interrupt_set_enabled = 0U;
        float ths = 0.1F;
        ri_gpio_interrupt_enable_ExpectAndReturn (RB_INT_LEVEL_PIN,
                RI_GPIO_SLOPE_TOGGLE,
                RI_GPIO_MODE_INPUT_NOPULL,
                &on_accelerometer_isr,
                RD_SUCCESS);
        rd_status_t err_code = app_sensor_acceleration_threshold_set (&ths);
        TEST_ASSERT (RD_SUCCESS == err_code);
        TEST_ASSERT (1U == level_interrupt_set_enabled);
    }
}

void test_app_sensor_acceleration_threshold_set_disable (void)
{
    if (SENSOR_COUNT > 2U && (RI_GPIO_ID_UNUSED != RB_INT_LEVEL_PIN))
    {
        for (size_t ii = 0U; ii < 2U; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), true);
        }

        // Mock provided data
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_lis;
        m_sensors[1]->sensor.level_interrupt_set = &mock_level_interrupt_set;
        level_interrupt_set_disabled = 0U;
        ri_gpio_interrupt_disable_ExpectAndReturn (RB_INT_LEVEL_PIN,
                RD_SUCCESS);
        rd_status_t err_code = app_sensor_acceleration_threshold_set (NULL);
        TEST_ASSERT (RD_SUCCESS == err_code);
        TEST_ASSERT (1U == level_interrupt_set_disabled);
    }
}

void test_app_sensor_acceleration_threshold_set_no_provider (void)
{
    if (SENSOR_COUNT > 3U && (RI_GPIO_ID_UNUSED != RB_INT_LEVEL_PIN))
    {
        for (size_t ii = 0U; ii < 3U; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), true);
        }

        // Mock provided data
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_photo;
        m_sensors[2]->sensor.provides = fields_photo;
        m_sensors[1]->sensor.level_interrupt_set = &mock_level_interrupt_set;
        level_interrupt_set_enabled = 0U;
        float ths = 0.1F;
        rd_status_t err_code = app_sensor_acceleration_threshold_set (&ths);
        TEST_ASSERT (RD_ERROR_NOT_SUPPORTED == err_code);
        TEST_ASSERT (0U == level_interrupt_set_enabled);
    }
}

void test_app_sensor_acceleration_threshold_set_null_interrupt_set (void)
{
    if (SENSOR_COUNT > 3U && (RI_GPIO_ID_UNUSED != RB_INT_LEVEL_PIN))
    {
        for (size_t ii = 0U; ii < 3U; ii++)
        {
            rd_sensor_is_init_ExpectAndReturn (& (m_sensors[ii]->sensor), true);
        }

        // Mock provided data
        m_sensors[0]->sensor.provides = fields_bme;
        m_sensors[1]->sensor.provides = fields_lis;
        m_sensors[2]->sensor.provides = fields_photo;
        m_sensors[1]->sensor.level_interrupt_set = NULL;
        level_interrupt_set_enabled = 0U;
        float ths = 0.1F;
        rd_status_t err_code = app_sensor_acceleration_threshold_set (&ths);
        TEST_ASSERT (RD_ERROR_NOT_SUPPORTED == err_code);
        TEST_ASSERT (0U == level_interrupt_set_enabled);
    }
}

/**
 * @brief Increment event counter of application. Rolls over at 2^32.
 *
 *  void app_sensor_event_increment (void);
 */

/**
 * @brief Get current event count.
 *
 * @return Number of events accumulated, rolls over at int32_t.
 */
void test_app_sensor_event_count_get (void)
{
    uint32_t orig_cnt = app_sensor_event_count_get ();
    app_sensor_event_increment();
    uint32_t incremented_cnt = app_sensor_event_count_get ();
    TEST_ASSERT ( (orig_cnt + 1) == incremented_cnt);
}

void test_app_sensor_accelerometer_isr (void)
{
    ri_gpio_evt_t evt;
    evt.slope = RI_GPIO_SLOPE_LOTOHI;
    uint32_t orig_cnt = app_sensor_event_count_get ();
    on_accelerometer_isr (evt);
    uint32_t incremented_cnt = app_sensor_event_count_get ();
    TEST_ASSERT ( (orig_cnt + 1) == incremented_cnt);
    evt.slope = RI_GPIO_SLOPE_HITOLO;
    on_accelerometer_isr (evt);
    incremented_cnt = app_sensor_event_count_get ();
    TEST_ASSERT ( (orig_cnt + 1) == incremented_cnt);
}