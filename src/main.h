#ifndef MAIN_H
#define MAIN_H

#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_gpio_interrupt.h"

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

#ifdef CEEDLING
void button_on_event_isr(const ruuvi_interface_gpio_evt_t event);
void on_radio (const ruuvi_interface_communication_radio_activity_evt_t evt);
void on_gatt_connected_isr (void * data, size_t data_len);
void on_gatt_disconnected_isr (void * data, size_t data_len);
void process_gatt_command (void * p_event_data, uint16_t event_size);
void on_gatt_received_isr (void * data, size_t data_len);
void on_gatt_sent_isr (void * data, size_t data_len);

#endif
/*@}*/


#endif // MAIN_H
