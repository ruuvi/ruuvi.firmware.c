#include "app_led.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_task_led.h"

/**
 * @addtogroup app_led
 */
/** @{ */
/**
 * @file app_led.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-09-10
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Depriciations added on 3.30.X, can be removed earliest at 4.0.0
 */

#ifndef CEEDLING
#define TESTABLE_STATIC static
#else
#define TESTABLE_STATIC
#endif

#define ACTIVITY_BIT     (1U << 0U)
#define CONFIGURABLE_BIT (1U << 1U)
#define INTERACTION_BIT  (1U << 2U)
#define ERROR_BIT        (1U << 3U)
#define PAUSE_BIT        (1U << 4U) // For backwards compatibility, do not use.

static const uint16_t        m_led_pins[]   = RB_LEDS_LIST;
static const ri_gpio_state_t m_led_active[] = RB_LEDS_ACTIVE_STATE;
TESTABLE_STATIC ri_gpio_id_t m_activity_led          = RI_GPIO_ID_UNUSED;
TESTABLE_STATIC uint32_t m_signals;

static void state_change_process (void)
{
    rd_status_t err_code = RD_SUCCESS;

    //State: Paused
    if (PAUSE_BIT & m_signals)
    {
        // No action needed
    }
    //State: Error on
    else if (ERROR_BIT & m_signals)
    {
        err_code |= rt_led_write (RB_LED_STATUS_ERROR, true);
    }
    //State: Interaction on
    else if (INTERACTION_BIT & m_signals)
    {
        err_code |= rt_led_write (RB_LED_BUTTON_PRESS, true);
    }
    // State: configurable, active
    else if ( (ACTIVITY_BIT | CONFIGURABLE_BIT) == m_signals)
    {
        err_code |= rt_led_write (RB_LED_CONFIG_ENABLED, true);
    }
    // State: active, not configurable
    else if (ACTIVITY_BIT == m_signals)
    {
        err_code |= rt_led_write (RB_LED_ACTIVITY, true);
    }
    //Default: leds off
    else
    {
        for (size_t ii = 0; ii < RB_LEDS_NUMBER; ii++)
        {
            err_code |= rt_led_write (m_led_pins[ii], false);
        }
    }

    RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
}


rd_status_t app_led_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_led_init (m_led_pins, m_led_active, RB_LEDS_NUMBER);
    m_signals = 0;
    return  err_code;
}

rd_status_t app_led_activate (const ri_gpio_id_t led)
{
    RD_ERROR_CHECK (RD_WARNING_DEPRECATED, ~RD_ERROR_FATAL);
    return rt_led_write (led, true);
}

rd_status_t app_led_deactivate (const ri_gpio_id_t led)
{
    RD_ERROR_CHECK (RD_WARNING_DEPRECATED, ~RD_ERROR_FATAL);
    return rt_led_write (led, false);
}

rd_status_t app_led_activity_set (const ri_gpio_id_t led)
{
    rd_status_t err_code = RD_SUCCESS;

    if (RI_GPIO_ID_UNUSED != m_activity_led)
    {
        err_code |= app_led_deactivate (m_activity_led);
    }

    err_code |= app_led_deactivate (led);

    if (RD_SUCCESS == err_code)
    {
        m_activity_led = led;
    }

    return err_code;
}

rd_status_t app_led_activity_indicate (const bool active)
{
    rd_status_t err_code = RD_SUCCESS;
    RD_ERROR_CHECK (RD_WARNING_DEPRECATED, ~RD_ERROR_FATAL);

    if (! (PAUSE_BIT & m_signals))
    {
        if (active)
        {
            err_code |= app_led_activate (m_activity_led);
        }
        else
        {
            err_code |= app_led_deactivate (m_activity_led);
        }
    }

    return err_code;
}

static void app_led_signal (const uint32_t signal, const bool active)
{
    if (active)
    {
        m_signals |= signal;
    }
    else
    {
        m_signals &= ~signal;
    }

    state_change_process();
}

void app_led_activity_pause (const bool pause)
{
    RD_ERROR_CHECK (RD_WARNING_DEPRECATED, ~RD_ERROR_FATAL);

    if (pause)
    {
        m_signals |= PAUSE_BIT;
    }
    else
    {
        m_signals &= ~PAUSE_BIT;
    }
}

/**
 * @brief Set/Clear activity indication
 *
 * Call this function to set / clear activity state of the leds. app_led decides
 * action based on other signals
 *
 * @param[in] active True to activate signal, false to deactivate.
 */
void app_led_activity_signal (const bool active)
{
    app_led_signal (ACTIVITY_BIT, active);
}

/**
 * @brief Set/Clear configuration indication
 *
 * Call this function to set / clear configurable state of the leds. app_led decides
 * action based on other signals
 *
* @param[in] active True to activate signal, false to deactivate.
 */
void app_led_configuration_signal (const bool active)
{
    app_led_signal (CONFIGURABLE_BIT, active);
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
void app_led_interaction_signal (const bool active)
{
    app_led_signal (INTERACTION_BIT, active);
}

/**
 * @brief Set/Clear error indication
 *
 * Call this function to set / clear error state of the leds.
 * App_led decides action based on other signals.
 *
 * @param[in] active True to activate signal, false to deactivate.
 */
void app_led_error_signal (const bool active)
{
    app_led_signal (ERROR_BIT, active);
}

/** @} */
