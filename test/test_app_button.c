#include "unity.h"

#include "app_button.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "mock_app_comms.h"
#include "mock_app_heartbeat.h"
#include "mock_app_led.h"
#include "mock_app_log.h"
#include "mock_ruuvi_interface_gpio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_power.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_timer.h"
#include "mock_ruuvi_task_button.h"
#include "mock_ruuvi_task_gpio.h"

#include <stddef.h>

extern ri_timer_id_t m_button_timer;
extern button_action_t m_button_action;
extern rt_button_init_t m_init_data;

void setUp (void)
{
    ri_log_Ignore();
    ri_error_to_string_IgnoreAndReturn (0);
}

void tearDown (void)
{
}

void test_factory_reset (void)
{
    app_heartbeat_stop_ExpectAndReturn (RD_SUCCESS);
    app_log_purge_flash_Expect();
    ri_power_enter_bootloader_Expect();
    ri_power_reset_Expect();
    factory_reset (NULL, 0);
}

void test_button_handler_factory_reset (void)
{
    button_action_t action = {0};
    action.factory_reset = 1;
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &factory_reset, RD_SUCCESS);
    button_timer_handler_isr (&action);
}

void test_button_handler_no_action (void)
{
    button_action_t action = {0};
    button_timer_handler_isr (&action);
}

void test_app_button_ok (void)
{
    rd_status_t err_code;
    ri_gpio_id_t button_pwr_pins[] = RB_BUTTON_PWR_PINS;

    for (size_t ii = 0; ii < RB_BUTTON_PWR_PIN_NUMBER; ii++)
    {
        ri_gpio_configure_ExpectAndReturn (button_pwr_pins[ii],
                                           RI_GPIO_MODE_OUTPUT_HIGHDRIVE, RD_SUCCESS);
        ri_gpio_write_ExpectAndReturn (button_pwr_pins[ii], RI_GPIO_HIGH, RD_SUCCESS);
    }

    ri_timer_create_ExpectAndReturn (&m_button_timer, RI_TIMER_MODE_SINGLE_SHOT,
                                     &button_timer_handler_isr, RD_SUCCESS);
    rt_button_init_ExpectAndReturn (&m_init_data, RD_SUCCESS);
    err_code = app_button_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_button1_activated (void)
{
    ri_gpio_state_t activation[] = RB_BUTTONS_ACTIVE_STATE;
    ri_gpio_slope_t e_slope = (RI_GPIO_HIGH == activation[0]) ? RI_GPIO_SLOPE_LOTOHI :
                              RI_GPIO_SLOPE_HITOLO;
    const ri_gpio_evt_t evt =
    {
        .pin = RB_BUTTON_1,
        .slope = e_slope
    };
    ri_scheduler_event_put_ExpectWithArrayAndReturn (&evt, 1, sizeof (evt), &button_handler,
            RD_SUCCESS);
    on_button_1_press_isr (evt);
}

void test_app_button1_released (void)
{
    ri_gpio_state_t activation[] = RB_BUTTONS_ACTIVE_STATE;
    ri_gpio_slope_t e_slope = (RI_GPIO_LOW == activation[0]) ? RI_GPIO_SLOPE_LOTOHI :
                              RI_GPIO_SLOPE_HITOLO;
    const ri_gpio_evt_t evt =
    {
        .pin = RB_BUTTON_1,
        .slope = e_slope
    };
    ri_scheduler_event_put_ExpectWithArrayAndReturn (&evt, 1, sizeof (evt), &button_handler,
            RD_SUCCESS);
    on_button_1_press_isr (evt);
}

void test_app_button_invalid (void)
{
    // Nothing should happen
    const ri_gpio_evt_t evt = {.pin = 0xFFFF, .slope = 54};
    ri_scheduler_event_put_ExpectWithArrayAndReturn (&evt, 1, sizeof (evt), &button_handler,
            RD_SUCCESS);
    on_button_1_press_isr (evt);
}

void test_button_handler_enable_config_press()
{
    bool active_states[] = RB_BUTTONS_ACTIVE_STATE;
    ri_gpio_evt_t evt =
    {
        .pin = RB_BUTTON_ENABLE_CONFIG,
        .slope = active_states[0]
    };
    app_led_interaction_signal_Expect (true);
    ri_timer_stop_ExpectAndReturn (m_button_timer, RD_SUCCESS);
    ri_timer_start_ExpectAndReturn (m_button_timer, APP_BUTTON_LONG_PRESS_TIME_MS,
                                    &m_button_action, RD_SUCCESS);
    button_handler (&evt, sizeof (evt));
    TEST_ASSERT (1 == m_button_action.factory_reset);
}

void test_button_handler_enable_config_release()
{
    bool active_states[] = RB_BUTTONS_ACTIVE_STATE;
    ri_gpio_evt_t evt =
    {
        .pin = RB_BUTTON_ENABLE_CONFIG,
        .slope = !active_states[0]
    };
    app_led_interaction_signal_Expect (false);
    ri_timer_stop_ExpectAndReturn (m_button_timer, RD_SUCCESS);
    app_comms_configure_next_enable_ExpectAndReturn (RD_SUCCESS);
    button_handler (&evt, sizeof (evt));
    TEST_ASSERT (0 == m_button_action.factory_reset);
}