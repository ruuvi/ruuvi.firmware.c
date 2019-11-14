#ifndef  TASK_I2C_H
#define  TASK_I2C_H
/**
 * @addtogroup peripheral_tasks
 */
/*@{*/
/**
 * @defgroup i2c_tasks I2C tasks
 * @brief I2C functions
 *
 */
/*@}*/
/**
 * @addtogroup i2c_tasks
 */
/*@{*/
/**
 * @file task_i2c.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-10-11
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * I2C control.
 *
 * The Ruuvi drivers will take care of the I2C traffic, but the bus has to be initialized.
 * Typical usage:
 *
 * @code{.c}
 *  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
 *  err_code = task_gpio_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_i2c_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  do_stuff_with_sensors_on_i2c_bus();
 * @endcode
 */

#include "ruuvi_driver_error.h"

/**
 * @brief Initialize I2C bus.
 *
 * This function looks up the I2C definitions from ruuvi_boards.h and initializes the bus and relevant GPIO pins.
 *
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return RUUVI_DRIVER_ERROR_INTERNAL if I2C pins cannot be pulled high
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if I2C is already initialized
 *
 * @warning asserts (reboot / break on debugger) if SCL pin equals SDA pin
 */
ruuvi_driver_status_t task_i2c_init(void);


#endif