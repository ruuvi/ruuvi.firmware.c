#include "app_button.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_task_button.h"

/**
 * @addtogroup app_button
 */
/*@{*/
/**
 * @file app_button.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-02-12
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#if RB_BUTTONS_NUMBER

static inline void LOG (const char * const msg)
{
    ri_log (RI_LOG_LEVEL_INFO, msg);
}

// Find which button was pressed
#ifndef CEEDLING
static
#endif
ri_gpio_slope_t get_activation (ri_gpio_evt_t evt)
{
    size_t ii;
    const ri_gpio_id_t buttons[] = RB_BUTTONS_LIST;
    const ri_gpio_state_t states[] = RB_BUTTONS_ACTIVE_STATE;
    ri_gpio_slope_t activation;

    for (ii = 0; (ii < RB_BUTTONS_NUMBER) && (buttons[ii] != evt.pin); ii++) {};

    if (ii < RB_BUTTONS_NUMBER)
    {
        activation = (states[ii] == RI_GPIO_HIGH) ? RI_GPIO_SLOPE_LOTOHI :
                     RI_GPIO_SLOPE_HITOLO;
    }
    else
    {
        activation = RI_GPIO_SLOPE_UNKNOWN;
    }

    return activation;
}

#if RB_BUTTONS_NUMBER > 0
#ifndef CEEDLING
static
#endif
void on_button_1_press (const ri_gpio_evt_t evt)
{
    ri_gpio_slope_t activation = get_activation (evt);

    if (activation == evt.slope)
    {
        LOG ("Button 1 pressed\r\n");
    }
    else
    {
        LOG ("Button 1 released\r\n");
    }
}
#endif

#if RB_BUTTONS_NUMBER > 1
#ifndef CEEDLING
static
#endif
void on_button_2_press (const ri_gpio_evt_t evt)
{
    ri_gpio_slope_t activation = get_activation (evt);

    if (activation == evt.slope)
    {
        LOG ("Button 2 pressed\r\n");
    }
    else
    {
        LOG ("Button 2 released\r\n");
    }
}
#endif

#if RB_BUTTONS_NUMBER > 2
#ifndef CEEDLING
static
#endif
void on_button_3_press (const ri_gpio_evt_t evt)
{
    ri_gpio_slope_t activation = get_activation (evt);

    if (activation == evt.slope)
    {
        LOG ("Button 3 pressed\r\n");
    }
    else
    {
        LOG ("Button 3 released\r\n");
    }
}
#endif

#if RB_BUTTONS_NUMBER > 3
#ifndef CEEDLING
static
#endif
void on_button_4_press (const ri_gpio_evt_t evt)
{
    ri_gpio_slope_t activation = get_activation (evt);

    if (activation == evt.slope)
    {
        LOG ("Button 4 pressed\r\n");
    }
    else
    {
        LOG ("Button 4 released\r\n");
    }
}
#endif

/** @brief List of buttons to initialize. */
static const ri_gpio_id_t button_pins[] = RB_BUTTONS_LIST;
/** @brief GPIO states when button is considered active */
static const ri_gpio_state_t button_active[] = RB_BUTTONS_ACTIVE_STATE;
/** @brief Function callbacks on button presses. - TODO: Generalise for multiple buttons */
static const rt_button_fp_t app_button_handlers[RB_BUTTONS_NUMBER] =
{
#if RB_BUTTONS_NUMBER > 0
    & on_button_1_press,
#endif
#if RB_BUTTONS_NUMBER > 1
    & on_button_2_press,
#endif
#if RB_BUTTONS_NUMBER > 2
    & on_button_3_press,
#endif
#if RB_BUTTONS_NUMBER > 3
    & on_button_4_press,
#endif
};

static rt_button_init_t m_init_data =
{
    .p_button_pins = button_pins,
    .p_button_active = button_active,
    .p_button_handlers = app_button_handlers,
    .num_buttons = RB_BUTTONS_NUMBER
};

/**
 * @brief Initialize buttons.
 *
 * After initialization buttons are powered if needed and
 * the callback defined at app_button.c is called on activation.
 *
 * @retval RD_SUCCESS if buttons were initialized
 * @retval RD_ERROR_INVALID_STATE if RI_GPIO or RI_GPIO_INTERRUPT are not initialized.
 */
rd_status_t app_button_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
#   if RB_BUTTON_PWR_PIN_NUMBER
    ri_gpio_id_t button_pwr_pins[RB_BUTTON_PWR_PIN_NUMBER] = RB_BUTTON_PWR_PINS;

    for (size_t ii = 0; ii < RB_BUTTON_PWR_PIN_NUMBER; ii++)
    {
        err_code |= ri_gpio_configure (button_pwr_pins[ii],
                                       RI_GPIO_MODE_OUTPUT_HIGHDRIVE);
        err_code |= ri_gpio_write (button_pwr_pins[ii], RI_GPIO_HIGH);
    }

#   endif
    err_code |= rt_button_init (&m_init_data);
    return err_code;
}

#else
rd_status_t app_button_init (void)
{
    return RD_SUCCESS;
}
#endif

/*@}*/
