#include "unity.h"

#include "app_sensor.h"

#include "mock_ruuvi_driver_sensor.h"
#include "mock_ruuvi_task_sensor.h"
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


void setUp (void)
{
    m_sensors_init();
    ri_spi_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_i2c_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    ri_rtc_init_ExpectAndReturn (RD_SUCCESS);
    rd_sensor_timestamp_function_set_ExpectAnyArgsAndReturn (RD_SUCCESS);
    // Uninits are called through function pointers, setting up expects would be
    // difficult.
#if APP_SENSOR_TMP117_ENABLED
    ri_tmp117_uninit_IgnoreAndReturn (RD_SUCCESS);
#endif
#if APP_SENSOR_SHTCX_ENABLED
    ri_shtcx_uninit_IgnoreAndReturn (RD_SUCCESS);
#endif
#if APP_SENSOR_BME280_ENABLED
    ri_bme280_uninit_IgnoreAndReturn (RD_SUCCESS);
#endif
#if APP_SENSOR_NTC_ENABLED
    ri_adc_ntc_uninit_IgnoreAndReturn (RD_SUCCESS);
#endif
#if APP_SENSOR_PHOTO_ENABLED
    ri_adc_photo_uninit_IgnoreAndReturn (RD_SUCCESS);
#endif
#if APP_SENSOR_MCU_ENABLED
    ruuvi_interface_environmental_mcu_uninit_IgnoreAndReturn (RD_SUCCESS);
#endif
#if APP_SENSOR_LIS2DH12_ENABLED
    ri_lis2dh12_uninit_IgnoreAndReturn (RD_SUCCESS);
#endif
#if APP_SENSOR_LIS2DW12_ENABLED
    ri_lis2dw12_uninit_IgnoreAndReturn (RD_SUCCESS);
#endif
}

void tearDown (void)
{
}

void test_app_sensor_init_ok (void)
{
    rd_status_t err_code;

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        rt_sensor_initialize_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
        rt_sensor_load_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
        rt_sensor_configure_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
    }

    err_code = app_sensor_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_sensor_init_first_time (void)
{
    rd_status_t err_code;

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        rt_sensor_initialize_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
        rt_sensor_load_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_ERROR_NOT_FOUND);
        rt_sensor_configure_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
        rt_sensor_store_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_SUCCESS);
    }

    err_code = app_sensor_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_sensor_init_not_found (void)
{
    rd_status_t err_code;

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        rt_sensor_initialize_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_ERROR_NOT_FOUND);
    }

    err_code = app_sensor_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_sensor_init_selftest_fail (void)
{
    rd_status_t err_code;

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        size_t retries = 0;

        do
        {
            rt_sensor_initialize_ExpectWithArrayAndReturn (m_sensors[ii], 1, RD_ERROR_SELFTEST);
        } while (retries++ < APP_SENSOR_SELFTEST_RETRIES);
    }

    err_code = app_sensor_init();
    TEST_ASSERT (RD_ERROR_SELFTEST == err_code);
}

void test_app_sensor_uninit_ok (void)
{
    rd_status_t err_code;

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        // Uninits ignored as they're called through function pointers set up in
        // init.
        if (m_sensors[ii]->pwr_pin != RI_GPIO_ID_UNUSED)
        {
            ri_gpio_write_ExpectAndReturn (m_sensors[ii]->pwr_pin,
                                           !m_sensors[ii]->pwr_on,
                                           RD_SUCCESS);
        }
    }

    ri_spi_uninit_ExpectAndReturn (RD_SUCCESS);
    ri_i2c_uninit_ExpectAndReturn (RD_SUCCESS);
    err_code = app_sensor_uninit();
    TEST_ASSERT (RD_SUCCESS == err_code);
}