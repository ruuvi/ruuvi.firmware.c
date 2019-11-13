/**
 * @file task_acceleration.h
 * @author Otso Jousimaa
 * @date 2019-06-17
 * @brief Acceleration and accelerometer related program tasks.
 * @copyright Copyright 2019 Ruuvi Innovations.
 *   This project is released under the BSD-3-Clause License.
 */

#ifndef  TASK_ACCELERATION_H
#define  TASK_ACCELERATION_H

#include "ruuvi_boards.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_log.h"
#include "task_communication.h"


/**
 * @brief Auto-detects and initializes acceleration sensor in low-power state.
 * Reads supported sensors from board.h and tries to initialize them.
 * Configures the sensor according to defaults in application_config.h.
 * Does not enable FiFo or activity interrupts, these must be enabled separately.
 * Initializes SPI bus if accelerometer requires SPI and SPI is not initialized.
 * Initializes I2C bus if accelerometer requires I2C and I2C is not initialized.
 *
 * @return @c RUUVI_DRIVER_SUCCESS on success.
 * @return @c RUUVI_DRIVER_ERROR_NOT_FOUND if not suitable accelerometer is found.
 * @return error code from driver if configuration fails
 */
ruuvi_driver_status_t task_acceleration_init(void);

/**
 * @brief loads current configuration of sensor from non-volatile storage.
 * Requires that flash has been initialized.
 *
 * @param[out] config configuration loaded from flash, or application defaults were not found.
 * @return RUUVI_DRIVER_SUCCESS if configuration was loaded from flash.
 * @return RUUVI_DRIVER_ERROR_NULL if config is NULL
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if flash is not initialized, and populate config with application defaults.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if flash is not initialized, and populate config with application defaults.
 */
ruuvi_driver_status_t task_acceleration_configuration_load(
  ruuvi_driver_sensor_configuration_t* const config);

/**
 * @brief stores current configuration of sensor into non-volatile storage.
 * Requires that flash has been initialized.
 *
 * @param[in] config configuration to be stored.
 * @return RUUVI_DRIVER_SUCCESS if configuration was stored to flash.
 * @return RUUVI_DRIVER_ERROR_NULL if config is NULL
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if flash is not initialized.
 */
ruuvi_driver_status_t task_acceleration_configuration_store(const
    ruuvi_driver_sensor_configuration_t* const config);

/**
 * @brief Get latest sample from the sensor.
 * Call to this function fetches data from sensor, however it does not trigger sampling.
 * Remember to trigger sampling in case you're in single-shot mode.
 * The data will be offset with values given in @ref task_acceleration_offset_apply.
 *
 * @param [in] data structure which will be filled with acceleration data.
 * @return @c RUUVI_DRIVER_SUCCESS on success
 * @return @c RUUVI_DRIVER_ERROR_NULL if @c data is @c NULL
 * @return @c RUUVI_DRIVER_ERROR_INVALID_STATE if accelerometer is not initialize.
 */
ruuvi_driver_status_t task_acceleration_data_get(ruuvi_driver_sensor_data_t*
    const data);

/**
 * @brief Get API function pointers from sensor.
 *
 * @param[out] api pointer to a pointer to a struct of sensor api functions.
 * @return @c RUUVI_DRIVER_SUCCESS if sensor API functions could be set up
 * @return @c RUUVI_DRIVER_ERROR_NULL if given pointer to api was null
 * @return @c RUUVI_DRIVER_ERROR_INVALID_STATE if sensor was not initialized
 */
ruuvi_driver_status_t task_acceleration_api_get(task_communication_api_t** api);

/**
 * @brief return number of movement counts
 *
 * @param count[out]
 */
ruuvi_driver_status_t task_acceleration_movement_count_get(uint8_t* const count);

#endif