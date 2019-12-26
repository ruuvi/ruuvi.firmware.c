/**
 * @defgroup advertisement_tasks Advertisement tasks
 * @brief Bluetooth Low Energy advertising
 *
 */
/*@{*/
/**
 * @defgroup button_tasks  Button functionality
 */
/*@}*/
/*@{*/
/**
 * @file task_button.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-12-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 *
 */
#ifndef  TASK_BUTTON_H
#define  TASK_BUTTON_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio_interrupt.h"

/** @brief Called on button event with the slope of edge */
typedef void (*task_button_fp_t) (const ruuvi_interface_gpio_evt_t event);

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
ruuvi_driver_status_t task_button_init (task_button_fp_t action);

#ifdef CEEDLING
/** @brief Provide Ceedling a way to trigger interrupt */
void on_button_isr (ruuvi_interface_gpio_evt_t event);
#endif

/*@}*/
#endif