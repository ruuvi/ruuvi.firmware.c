#ifndef TASK_GPIO_H
#define TASK_GPIO_H
#include <stdbool.h>
/**
 * @addtogroup peripheral_tasks
 */
/*@{*/
/**
 * @defgroup gpio_tasks GPIO tasks
 * @brief General purpose input-output functions.
 *
 */
/*@}*/
/**
 * @addtogroup gpio_tasks
 */
/*@{*/
/**
 * @file task_gpio.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-10-11
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * GPIO control.
 */
#include "ruuvi_driver_error.h"

/**
 * @brief initialise GPIO. Pins are in high-Z state by default.
 *
 * @return RUUVI_DRIVER_SUCCESS
 */
ruuvi_driver_status_t task_gpio_init(void);

/**
 * @brief check that GPIO is initialized.
 *
 * @return True if GPIO is initialized, false otherwise.
 */
bool task_gpio_is_init(void);
/*@}*/

#endif