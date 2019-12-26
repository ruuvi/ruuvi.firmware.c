#include "unity.h"

#include "main.h"

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_endpoints.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_rtc.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_watchdog.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_task_acceleration.h"
#include "mock_task_adc.h"
#include "mock_task_advertisement.h"
#include "mock_task_button.h"
#include "mock_task_communication.h"
#include "mock_task_environmental.h"
#include "mock_task_flash.h"
#include "mock_task_gatt.h"
#include "mock_task_gpio.h"
#include "mock_task_led.h"
#include "mock_task_i2c.h"
#include "mock_task_nfc.h"
#include "mock_task_power.h"
#include "mock_task_rtc.h"
#include "mock_task_scheduler.h"
#include "mock_task_sensor.h"
#include "mock_task_spi.h"
#include "mock_task_timer.h"
#include "mock_test_sensor.h"
#include "mock_test_acceleration.h"
#include "mock_test_adc.h"
#include "mock_test_environmental.h"
#include "mock_test_library.h"

void setUp (void)
{
    ruuvi_driver_error_check_Ignore();
}

void tearDown (void)
{
    task_adc_vdd_sample_IgnoreAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_rtc_millis_IgnoreAndReturn (0);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}

void test_main_NeedToImplement (void)
{
    TEST_IGNORE_MESSAGE ("Need to Implement main");
}

/**
 *  @brief Synchronize ADC measurement to radio.
 *  This is common to all radio modules, i.e.
 *  the callback gets called for every radio action.
 *
 *  @param[in] evt Type of radio event
 */
void test_main_on_radio_prepare_update (void)
{
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS * 2U);
    task_adc_vdd_prepare_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_BEFORE);
}

void test_main_on_radio_no_need_to_update (void)
{
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS / 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_BEFORE);
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS / 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}

void test_main_on_radio_do_update (void)
{
    test_main_on_radio_prepare_update();
    task_adc_vdd_sample_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_rtc_millis_ExpectAndReturn (APPLICATION_ADC_SAMPLE_INTERVAL_MS * 2U);
    on_radio (RUUVI_INTERFACE_COMMUNICATION_RADIO_AFTER);
}