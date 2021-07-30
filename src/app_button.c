#include "app_button.h"
#include "app_comms.h"
#include "app_heartbeat.h"
#include "app_led.h"
#include "app_log.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_flash.h"
#include "ruuvi_interface_power.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_task_button.h"

/**
 * @addtogroup app_button
 */
/** @{ */
/**
 * @file app_button.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-02-12
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#if RB_BUTTONS_NUMBER

#ifndef CEEDLING
static
#endif
ri_timer_id_t m_button_timer;

#ifndef CEEDLING
typedef struct
{
    unsigned int factory_reset : 1; //!< Button should do a factory reset.
} button_action_t;
#endif
#ifndef CEEDLING
static
#endif
button_action_t m_button_action;

static inline void LOG (const char * const msg)
{
    ri_log (RI_LOG_LEVEL_INFO, msg);
}

#ifndef CEEDLING
static
#endif
void factory_reset (void * p_event_data, uint16_t event_size)
{
    app_heartbeat_stop();
    app_log_purge_flash();
    // Execution stops here normally
    ri_power_enter_bootloader();
    // Reset on fail to enter BL
    ri_power_reset();
}

#ifndef CEEDLING
static
#endif
void button_timer_handler_isr (void * p_context)
{
    button_action_t * p_action = (button_action_t *) p_context;
    rd_status_t err_code = RD_SUCCESS;

    if (p_action->factory_reset)
    {
        err_code |= ri_scheduler_event_put (NULL, 0, &factory_reset);
    }

    RD_ERROR_CHECK (err_code, RD_SUCCESS);
}

// Find which button was pressed
#ifndef CEEDLING
static
#endif
ri_gpio_slope_t get_activation (const ri_gpio_evt_t * const evt)
{
    size_t ii;
    const ri_gpio_id_t buttons[] = RB_BUTTONS_LIST;
    const ri_gpio_state_t states[] = RB_BUTTONS_ACTIVE_STATE;
    ri_gpio_slope_t activation;

    for (ii = 0; (ii < RB_BUTTONS_NUMBER) && (buttons[ii] != evt->pin); ii++) {};

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

static void handle_enable_config_button (const bool activated)
{
    if (activated)
    {
        LOG ("Config button pressed\r\n");
        m_button_action.factory_reset = 1;
        // Disable activity led to not turn off the button indication led.
        app_led_interaction_signal (true);
        ri_timer_stop (m_button_timer);
        ri_timer_start (m_button_timer, APP_BUTTON_LONG_PRESS_TIME_MS, &m_button_action);
    }
    else
    {
        LOG ("Config button released\r\n");
        m_button_action.factory_reset = 0;
        app_led_interaction_signal (false);
        (void) ri_timer_stop (m_button_timer);
        (void) app_comms_configure_next_enable();
    }
}

#ifndef CEEDLING
static
#endif
void button_handler (void * p_event_data, uint16_t event_size)
{
    if ( (NULL != p_event_data) && (sizeof (ri_gpio_evt_t) == event_size))
    {
        const ri_gpio_evt_t * const p_evt = (ri_gpio_evt_t *) p_event_data;
        const ri_gpio_slope_t activation = get_activation (p_evt);

        if (RB_BUTTON_ENABLE_CONFIG == p_evt->pin)
        {
            handle_enable_config_button (p_evt->slope == activation);
        }
    }
}

#if RB_BUTTONS_NUMBER > 0
#ifndef CEEDLING
static
#endif
void on_button_1_press_isr (const ri_gpio_evt_t evt)
{
    rd_status_t err_code = ri_scheduler_event_put (&evt, sizeof (ri_gpio_evt_t),
                           &button_handler);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
}
#endif

#if RB_BUTTONS_NUMBER > 1
#ifndef CEEDLING
static
#endif
void on_button_2_press_isr (const ri_gpio_evt_t evt)
{
    rd_status_t err_code = ri_scheduler_event_put (&evt, sizeof (ri_gpio_evt_t),
                           &button_handler);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
}
#endif

#if RB_BUTTONS_NUMBER > 2
#ifndef CEEDLING
static
#endif
void on_button_3_press_isr (const ri_gpio_evt_t evt)
{
    rd_status_t err_code = ri_scheduler_event_put (&evt, sizeof (ri_gpio_evt_t),
                           &button_handler);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
}
#endif

#if RB_BUTTONS_NUMBER > 3
#ifndef CEEDLING
static
#endif
void on_button_4_press_isr (const ri_gpio_evt_t evt)
{
    rd_status_t err_code = ri_scheduler_event_put (&evt, sizeof (ri_gpio_evt_t),
                           &button_handler);
    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
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
    & on_button_1_press_isr,
#endif
#if RB_BUTTONS_NUMBER > 1
    & on_button_2_press_isr,
#endif
#if RB_BUTTONS_NUMBER > 2
    & on_button_3_press_isr,
#endif
#if RB_BUTTONS_NUMBER > 3
    & on_button_4_press_isr,
#endif
};

#ifndef CEEDLING
static
#endif
rt_button_init_t m_init_data =
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
    err_code |= ri_timer_create (&m_button_timer, RI_TIMER_MODE_SINGLE_SHOT,
                                 &button_timer_handler_isr);
    err_code |= rt_button_init (&m_init_data);
    return err_code;
}

#else
rd_status_t app_button_init (void)
{
    return RD_SUCCESS;
}
#endif

/** @} */
