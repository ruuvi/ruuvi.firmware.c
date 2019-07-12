/**
 * @file task_acceleration.h
 * @author Otso Jousimaa
 * @date 2019-06-17
 * @brief Acceleration and accelerometer related program tasks. 
 * @copyright Copyright 2019 Ruuvi Innovations.
 *   This project is released under the BSD-3-Clause License.
 *
 */

#ifndef  TASK_ACCELERATION_H
#define  TASK_ACCELERATION_H

#include "ruuvi_boards.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"
#include "task_communication.h"

#define TASK_ACCELERATION_X_INDEX 0
#define TASK_ACCELERATION_Y_INDEX 1
#define TASK_ACCELERATION_Z_INDEX 2

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
ruuvi_driver_status_t task_acceleration_configuration_load(ruuvi_driver_sensor_configuration_t* const config);

/**
 * @brief stores current configuration of sensor into non-volatile storage.
 * Requires that flash has been initialized.
 *
 * @param[in] config configuration to be stored.
 * @return RUUVI_DRIVER_SUCCESS if configuration was stored to flash.
 * @return RUUVI_DRIVER_ERROR_NULL if config is NULL
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if flash is not initialized.
 */
ruuvi_driver_status_t task_acceleration_configuration_store(const ruuvi_driver_sensor_configuration_t* const config);

/**
 * @brief Prints Acceleration data to log at given severity
 *
 * Fetches latest data from accelerometer, does not trigger sampling. 
 * Prints in format X: %.3f g, Y: %.3f g, Z: %.3f g\r\n. 
 * 
 * @return @c RUUVI_DRIVER_SUCCESS on success
 * @return @c RUUVI_DIVER_ERROR_INVALID_STATE if accelerometer is not initialized
 */
ruuvi_driver_status_t task_acceleration_data_log(const ruuvi_interface_log_severity_t
    level);

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
ruuvi_driver_status_t task_acceleration_data_get(ruuvi_interface_acceleration_data_t*
    const data);



/**
 * @brief Add offset to X sensor reading. 
 * Values given here will be summed to values read with _data_get. 
 * Set to @c NULL or all zeros to clear the offsets
 *
 * @param[in] data offsets
 */
void task_acceleration_offset_x_set(uint8_t* const data);

/**
 * @brief Read offsets configured by user
 *
 * @param[out] data offsets
 */
void task_acceleration_offset_x_get(uint8_t* const data);

/**
 * @brief Add offset to Y sensor reading. 
 * Values given here will be summed to values read with _data_get. 
 * Set to @c NULL or all zeros to clear the offsets
 *
 * @param[in] data offsets
 */
void task_acceleration_offset_y_set(uint8_t* const data);

/**
 * @brief Read offsets configured by user
 *
 * @param[out] data offsets
 */
void task_acceleration_offset_y_get(uint8_t* const data);

/**
 * @brief Add offset to Z sensor reading. 
 * Values given here will be summed to values read with _data_get. 
 * Set to @c NULL or all zeros to clear the offsets
 *
 * @param[in] data offsets
 */
void task_acceleration_offset_z_set(uint8_t* const data);

/**
 * @brief Read offsets configured by user
 *
 * @param[out] data offsets
 */
void task_acceleration_offset_z_get(uint8_t* const data);


/**
 * @brief Callback function for activity detected by accelerometer
 * By default increments activity counter.
 */
ruuvi_driver_status_t task_acceleration_on_activity(void* p_event_data, uint16_t event_size);


/**
 * @brief Callback function for button press. 
 * By default does nothing, but signature is included for convenience of the user.
 * 
 */
ruuvi_driver_status_t task_acceleration_on_button(void);

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
 * @brief Get API function pointer to configure X-axis
 *
 * @param[out] api pointer to a pointer to a struct of sensor api functions. 
 * @return @c RUUVI_DRIVER_SUCCESS if sensor API functions could be set up
 * @return @c RUUVI_DRIVER_ERROR_NULL if given pointer to api was null
 * @return @c RUUVI_DRIVER_ERROR_INVALID_STATE if sensor was not initialized
 */
ruuvi_driver_status_t task_acceleration_api_x_get(task_communication_api_t** api);

/**
 * @brief Get API function pointer to configure Y-axis
 *
 * @param[out] api pointer to a pointer to a struct of sensor api functions. 
 * @return @c RUUVI_DRIVER_SUCCESS if sensor API functions could be set up
 * @return @c RUUVI_DRIVER_ERROR_NULL if given pointer to api was null
 * @return @c RUUVI_DRIVER_ERROR_INVALID_STATE if sensor was not initialized
 */
ruuvi_driver_status_t task_acceleration_api_y_get(task_communication_api_t** api);

/**
 * @brief Get API function pointer to configure Z-axis
 *
 * @param[out] api pointer to a pointer to a struct of sensor api functions. 
 * @return @c RUUVI_DRIVER_SUCCESS if sensor API functions could be set up
 * @return @c RUUVI_DRIVER_ERROR_NULL if given pointer to api was null
 * @return @c RUUVI_DRIVER_ERROR_INVALID_STATE if sensor was not initialized
 */
ruuvi_driver_status_t task_acceleration_api_z_get(task_communication_api_t** api);

/**
 * @brief return number of movement counts
 *
 * @param count[out] 
 */
ruuvi_driver_status_t task_acceleration_movement_count_get(uint8_t* const count);

/**
 * @brief Configure logging of sensor data
 *
 * This function configures how sensor data should be logged. The logging target can be
 * real-time, such as GATT or B LE advertisement or something more persistent, such as 
 * RAM or FLASH buffer.
 *
 * @param[in] target Target logging backend(s) 
 * @param[in] interval Logging interval, format TBD
 * @return RUUVI_DRIVER_ERROR_NOT_IMPLEMENTED
 */
ruuvi_driver_status_t task_acceleration_logging_configure(const task_api_data_target_t const target, const uint8_t interval);

#endif