/**
 * @file app_heartbeat.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-06-17
 * @copyright Ruuvi Innovations Ltd, License BSD-3-Clause.
 */

#include "app_config.h"
#include "app_heartbeat.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"

static ri_timer_id_t heart_timer; //!< Timer for updating data.

/**
 * @brief When timer triggers, schedule reading sensors and sending data.
 *
 * @param[in] p_context Always NULL.
 */
#ifndef CEEDLING
static
#endif
void heartbeat (void * p_event, uint16_t event_size)
{
}

/**
 * @brief When timer triggers, schedule reading sensors and sending data.
 *
 * @param[in] p_context Always NULL.
 */
#ifndef CEEDLING
static
#endif
void schedule_heartbeat_isr (void * const p_context)
{
    ri_scheduler_event_put (NULL, 0, &heartbeat);
}

rd_status_t app_heartbeat_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_timer_create (&heart_timer, RI_TIMER_MODE_REPEATED,
                                 &schedule_heartbeat_isr);

    if (RD_SUCCESS == err_code)
    {
        err_code |= ri_timer_start (heart_timer, APP_HEARTBEAT_INTERVAL_MS, NULL);
    }

    return err_code;
}

// Give CEEDLING a handle to state of module.
#ifdef CEEDLING
ri_timer_id_t * get_heart_timer (void)
{
    return &heart_timer;
}
#endif
