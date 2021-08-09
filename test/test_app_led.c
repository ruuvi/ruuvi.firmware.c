#include "unity.h"

#include "app_led.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_ruuvi_task_led.h"

static const ri_gpio_id_t    m_led_pins[]    = RB_LEDS_LIST;
static const ri_gpio_state_t m_leds_active[] = RB_LEDS_ACTIVE_STATE;
extern ri_gpio_id_t m_activity_led;
extern uint32_t m_signals;

void setUp (void)
{
    m_activity_led = RI_GPIO_ID_UNUSED;
    m_signals = 0;
    ri_log_Ignore();
    ri_error_to_string_IgnoreAndReturn (0);
}

void tearDown (void)
{
}

void test_app_led_init_ok (void)
{
    rt_led_init_ExpectWithArrayAndReturn (m_led_pins, RB_LEDS_NUMBER,
                                          m_leds_active, RB_LEDS_NUMBER,
                                          RB_LEDS_NUMBER, RD_SUCCESS);
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

void test_app_led_activate_ok (void)
{
    rt_led_write_ExpectAndReturn (m_led_pins[0], true, RD_SUCCESS);
    rd_status_t err_code = app_led_activate (m_led_pins[0]);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_led_deactivate_ok (void)
{
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    rd_status_t err_code = app_led_deactivate (m_led_pins[0]);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_led_activity_set (void)
{
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    rd_status_t err_code = app_led_activity_set (m_led_pins[0]);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_led_activity_set_reset_previous (void)
{
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    rd_status_t err_code = app_led_activity_set (m_led_pins[0]);
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    err_code |= app_led_activity_set (m_led_pins[0]);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_led_activity_indicate (void)
{
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    rd_status_t err_code = app_led_activity_set (m_led_pins[0]);
    rt_led_write_ExpectAndReturn (m_led_pins[0], true, RD_SUCCESS);
    err_code |= app_led_activity_indicate (true);
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    err_code |= app_led_activity_indicate (false);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_led_activity_pause (void)
{
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    rd_status_t err_code = app_led_activity_set (m_led_pins[0]);
    app_led_activity_pause (true);
    err_code |= app_led_activity_indicate (true);
    err_code |= app_led_activity_indicate (false);
    app_led_activity_pause (false);
    rt_led_write_ExpectAndReturn (m_led_pins[0], true, RD_SUCCESS);
    err_code |= app_led_activity_indicate (true);
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    err_code |= app_led_activity_indicate (false);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

/**
 * @brief Set/Clear activity indication
 *
 * Call this function to set / clear activity state of the leds. app_led decides
 * action based on other signals
 *
 * @param[in] active True to activate signal, false to deactivate.
 */
void test_app_led_activity_signal (void)
{
    rt_led_write_ExpectAndReturn (RB_LED_ACTIVITY, true, RD_SUCCESS);
    app_led_activity_signal (true);

    for (uint32_t ii = 0; ii < RB_LEDS_NUMBER; ii++)
    {
        rt_led_write_ExpectAndReturn (m_led_pins[ii], false, RD_SUCCESS);
    }

    app_led_activity_signal (false);
}

/**
 * @brief Set/Clear configuration indication
 *
 * Call this function to set / clear configurable state of the leds. app_led decides
 * action based on other signals
 *
 * @param[in] active True to activate signal, false to deactivate.
 */

void test_app_led_configuration_signal (void)
{
    for (uint32_t ii = 0; ii < RB_LEDS_NUMBER; ii++)
    {
        rt_led_write_ExpectAndReturn (m_led_pins[ii], false, RD_SUCCESS);
    }

    app_led_configuration_signal (true);
    rt_led_write_ExpectAndReturn (RB_LED_CONFIG_ENABLED, true, RD_SUCCESS);
    app_led_activity_signal (true);

    for (uint32_t ii = 0; ii < RB_LEDS_NUMBER; ii++)
    {
        rt_led_write_ExpectAndReturn (m_led_pins[ii], false, RD_SUCCESS);
    }

    app_led_activity_signal (false);
}

/**
 * @brief Set/Clear interaction indication
 *
 * Call this function to set / clear interaction state of the leds.
 * Typically this is called on button press and release.
 * App_led decides action based on other signals.
 *
 * @param[in] active True to activate signal, false to deactivate.
 */
void test_app_led_interaction_signal (void)
{
    rt_led_write_ExpectAndReturn (RB_LED_ACTIVITY, true, RD_SUCCESS);
    app_led_interaction_signal (true);
    rt_led_write_ExpectAndReturn (RB_LED_ACTIVITY, true, RD_SUCCESS);
    app_led_activity_signal (true);
    rt_led_write_ExpectAndReturn (RB_LED_ACTIVITY, true, RD_SUCCESS);
    app_led_activity_signal (false);

    for (uint32_t ii = 0; ii < RB_LEDS_NUMBER; ii++)
    {
        rt_led_write_ExpectAndReturn (m_led_pins[ii], false, RD_SUCCESS);
    }

    app_led_interaction_signal (false);
}

/**
 * @brief Set/Clear error indication
 *
 * Call this function to set / clear error state of the leds.
 * App_led decides action based on other signals.
 *
 * @param[in] active True to activate signal, false to deactivate.
 */
void test_app_led_error_signal (void)
{
    rt_led_write_ExpectAndReturn (RB_LED_STATUS_ERROR, true, RD_SUCCESS);
    app_led_error_signal (true);
}