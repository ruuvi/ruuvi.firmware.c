#ifndef MAIN_H
#define MAIN_H

#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_log.h"
#include <stdlib.h>

/**
 * @addtogroup main
 */
/*@{*/
/**
 * @file main.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-12-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * This file exists only to provide Ceedling with handles to functions to test.
 *
 */

// Submodule requirements
#define RUUVI_BOARDS_REQ "0.3.2"
#define RUUVI_DRIVERS_REQ "0.1.5"
#define RUUVI_LIBRARIES_REQ "0.1.3"

#ifdef CEEDLING
void on_button_isr (const ri_gpio_evt_t event);
void on_gatt_connected_isr (void * data, size_t data_len);
void on_gatt_disconnected_isr (void * data, size_t data_len);
void on_gatt_received_isr (void * data, size_t data_len);
void on_gatt_sent_isr (void * data, size_t data_len);
void on_radio_isr (const ri_radio_activity_evt_t evt);
void on_wdt (void);
void process_gatt_command (void * p_event_data, uint16_t event_size);
void setup (void);
int app_main (void);
#define LOOP_FOREVER 0 //!< Ceedling run exits
#else
#define LOOP_FOREVER 1 //!< Normal run does not exit.
#endif

/*@}*/
#endif // MAIN_H
