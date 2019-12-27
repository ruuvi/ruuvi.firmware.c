#include "unity.h"

#include "ruuvi_boards.h"
#include "application_config.h"

#include "task_button.h"
#include "mock_ruuvi_interface_gpio.h"
#include "mock_ruuvi_interface_gpio_interrupt.h"
#include "mock_ruuvi_interface_log.h"

#include <stddef.h>

static bool pressed;

static void ceedling_cb (const ruuvi_interface_gpio_evt_t evt)
{
    pressed = true;
}

void setUp (void)
{
    ruuvi_interface_log_Ignore();
}

void tearDown (void)
{
    pressed = false;
}

/**
 * @brief Button initialization function.
 *
 * Requires GPIO and interrupts to be initialized.
 * Configures GPIO as pullup/-down according to button active state in ruuvi_boards.h
 *
 * @param[in] action: Function to be called when button interrupt occurs
 *
 * @return Status code from the stack. RUUVI_DRIVER_SUCCESS if no errors occured.
 **/
void test_task_button_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_gpio_id_t bpin = {.pin = RUUVI_BOARD_BUTTON_1};
    ruuvi_interface_gpio_evt_t evt =
    {
        .pin = bpin,
        .slope = RUUVI_INTERFACE_GPIO_SLOPE_HITOLO
    };
    ruuvi_interface_gpio_slope_t slope = RUUVI_INTERFACE_GPIO_SLOPE_TOGGLE;
    ruuvi_interface_gpio_mode_t  mode  = (RUUVI_BOARD_BUTTONS_ACTIVE_STATE) ?
                                         RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLDOWN : RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP;
    // Does nothing, callback not set up
    on_button_isr (evt);
    TEST_ASSERT (!pressed);
    ruuvi_interface_gpio_interrupt_enable_ExpectAndReturn (bpin, slope, mode, &on_button_isr,
            RUUVI_DRIVER_SUCCESS);
    err_code = task_button_init (&ceedling_cb);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    on_button_isr (evt);
    TEST_ASSERT (pressed);
}