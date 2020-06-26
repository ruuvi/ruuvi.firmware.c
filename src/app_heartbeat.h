#ifndef APP_HEARTBEAT_H
#define APP_HEARTBEAT_H

/**
 * @file app_heartbeat.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @data 2020-06-12
 * @copyright Ruuvi Innovations Ltd, License BSD-3-Clause.
 *
 * Read and transmit latest sensor data via any means possible,
 * such as BLE advertisements, BLE Gatt, NFC.
 *
 */

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"

/**
 * @brief Initializes timers for reading and sending heartbeat transmissions.
 *
 * The heartbeat interval should be at most as logging rate to make sure
 * that application will log fresh data.
 *
 * @retval RD_SUCCESS on success
 * @retval RD_ERROR_INVALID_STATE if timers or scheduler is not initialized.
 * @retval RD_ERROR_RESOURCES if a timer cannot be allocated.
 */
rd_status_t app_heartbeat_init (void);

#ifdef CEEDLING
#include "ruuvi_interface_timer.h"
ri_timer_id_t * get_heart_timer (void);
void schedule_heartbeat_isr (void * const p_context);
void heartbeat (void * p_event, uint16_t event_size);
#endif

#endif // APP_HEARTBEAT_H
