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

/**
 * Delay to let user see actions on board, such as led sequence.
 * Must be at least 1000 ms to avoid RTC hangup in test, shorter is generally better.
 */
#define BOOT_DELAY_MS (1000U)
#define LOG_BUF_SIZE  (128U)      //!< Maximum line length to print. 
#define GATT_HEARTBEAT_SIZE (18U) //!< 20 would be max, 18 cuts the data to while fields.

#ifndef MAIN_LOG_LEVEL
#define MAIN_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

#ifdef CEEDLING
void button_on_event_isr (const ruuvi_interface_gpio_evt_t event);
void on_radio (const ruuvi_interface_communication_radio_activity_evt_t evt);
void on_gatt_connected_isr (void * data, size_t data_len);
void on_gatt_disconnected_isr (void * data, size_t data_len);
void process_gatt_command (void * p_event_data, uint16_t event_size);
void on_gatt_received_isr (void * data, size_t data_len);
void on_gatt_sent_isr (void * data, size_t data_len);
#define LOOP_FOREVER 0 //!< Ceedling run exits
#else
#define LOOP_FOREVER 1 //!< Normal run does not exit.
#endif
/*@}*/


#endif // MAIN_H
