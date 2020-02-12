#include "unity.h"

#include "app_button.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "mock_ruuvi_interface_gpio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_task_button.h"
#include "mock_ruuvi_task_gpio.h"

#include <stddef.h>

void setUp (void)
{
    ri_log_Ignore();
    ri_error_to_string_IgnoreAndReturn (0);
}

void tearDown (void)
{
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

    // It would be difficult to mock the callback addresses, allow any argument
    rt_button_init_ExpectAnyArgsAndReturn (RD_SUCCESS);
    err_code = app_button_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_button_activated (void)
{
    // TODO: Change mode, start reboot timer
    TEST_IGNORE_MESSAGE ("Implement");
    ri_gpio_state_t activation[] = RB_BUTTONS_ACTIVE_STATE;
    ri_gpio_slope_t e_slope = (RI_GPIO_HIGH == activation[0]) ? RI_GPIO_SLOPE_LOTOHI :
                              RI_GPIO_SLOPE_HITOLO;
    const ri_gpio_evt_t evt =
    {
        .pin = RB_BUTTON_1,
        .slope = e_slope
    };
    on_button_press (evt);
}

void test_app_button_released (void)
{
    // TODO: Stop reboot timer
    TEST_IGNORE_MESSAGE ("Implement");
    ri_gpio_state_t activation[] = RB_BUTTONS_ACTIVE_STATE;
    ri_gpio_slope_t e_slope = (RI_GPIO_LOW == activation[0]) ? RI_GPIO_SLOPE_LOTOHI :
                              RI_GPIO_SLOPE_HITOLO;
    const ri_gpio_evt_t evt =
    {
        .pin = RB_BUTTON_1,
        .slope = e_slope
    };
    on_button_press (evt);
}

void test_app_button_invalid (void)
{
    // Nothing should happen
    const ri_gpio_evt_t evt = {.pin = 0xFFFF, .slope = 54};
    on_button_press (evt);
}