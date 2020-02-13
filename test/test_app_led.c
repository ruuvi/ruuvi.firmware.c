#include "unity.h"

#include "app_led.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_ruuvi_task_led.h"

static const ri_gpio_id_t    m_led_pins[]    = RB_LEDS_LIST;
static const ri_gpio_state_t m_leds_active[] = RB_LEDS_ACTIVE_STATE;

void setUp (void)
{
}

void tearDown (void)
{
}

void test_app_led_init_ok (void)
{
    rt_led_init_ExpectWithArrayAndReturn (m_led_pins, RB_LEDS_NUMBER,
                                          m_leds_active, RB_LEDS_NUMBER,
                                          RB_LEDS_NUMBER, RD_SUCCESS);

    for (size_t ii = 0; ii < RB_LEDS_NUMBER; ii++)
    {
        rt_led_write_ExpectAndReturn (m_led_pins[ii], RT_LED_ON, RD_SUCCESS);
        ri_delay_ms_ExpectAndReturn (APP_LED_INIT_DELAY_MS, RD_SUCCESS);
        rt_led_write_ExpectAndReturn (m_led_pins[ii], RT_LED_OFF, RD_SUCCESS);
    }

    rt_led_activity_led_set_ExpectAndReturn (RB_LED_ACTIVITY, RD_SUCCESS);
    ri_yield_indication_set_Expect (&rt_led_activity_indicate);
    rd_status_t err_code = app_led_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_led_init_twice (void)
{
    rt_led_init_ExpectWithArrayAndReturn (m_led_pins, RB_LEDS_NUMBER,
                                          m_leds_active, RB_LEDS_NUMBER,
                                          RB_LEDS_NUMBER, RD_ERROR_INVALID_STATE);
    rd_status_t err_code = app_led_init();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}

void test_app_led_init_invalid_led (void)
{
    rt_led_init_ExpectWithArrayAndReturn (m_led_pins, RB_LEDS_NUMBER,
                                          m_leds_active, RB_LEDS_NUMBER,
                                          RB_LEDS_NUMBER, RD_SUCCESS);

    for (size_t ii = 0; ii < RB_LEDS_NUMBER; ii++)
    {
        rt_led_write_ExpectAndReturn (m_led_pins[ii], RT_LED_ON, RD_SUCCESS);
        ri_delay_ms_ExpectAndReturn (APP_LED_INIT_DELAY_MS, RD_SUCCESS);
        rt_led_write_ExpectAndReturn (m_led_pins[ii], RT_LED_OFF, RD_SUCCESS);
    }

    rt_led_activity_led_set_ExpectAndReturn (RB_LED_ACTIVITY, RD_ERROR_INVALID_PARAM);
    rd_status_t err_code = app_led_init();
    TEST_ASSERT (RD_ERROR_INVALID_PARAM == err_code);
}