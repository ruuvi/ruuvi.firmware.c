#ifndef TASK_SENSOR_H
#define TASK_SENSOR_H
/**
 * @defgroup sensor_tasks Sensor tasks
 */
/*@{*/
/**
 * @file task_sensor.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-12-25
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Helper functions common to all sensors
 *
 */
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"

/** @brief Configure a sensor with given settings.
 *
 * @param[in] sensor Sensor to configure
 * @param[in,out] config Input: Desired configuration. Output: Actual configuration.
 * @param[in] unit Physical unit the sensor measures, e.g. "g". May be NULL.
 *
 * @return RUUVI_DRIVER_SUCCESS on success.
 * @return RUUVI_DRIVER_ERROR_NULL if sensor or config is NULL.
 * @return error code from sensor on other error.
 */
ruuvi_driver_status_t task_sensor_configure (ruuvi_driver_sensor_t * const sensor,
        ruuvi_driver_sensor_configuration_t * const config,
        const char * const unit);

/**
 * @brief Read sensors and encode to given buffer in Ruuvi DF5.
 *
 * @param[in] buffer uint8_t array with length of 24 bytes.
 * @return RUUVI_DRIVER_SUCCESS if data was encoded
 */
ruuvi_endpoint_status_t task_sensor_encode_to_5 (uint8_t * const buffer);

/**
 * @brief Search for requested sensor backend in given list of sensors.
 *
 * @param[in] sensor_list array of sensor to search the backend from.
 * @param[in] count number of sensor backends in the list
 * @param[in] name NULL-terminated, max 9-byte (including trailing NULL) string representation of sensor.
 * @return pointer to requested sensor if found
 * @return NULL if requested sensor was not found
 */
ruuvi_driver_sensor_t * task_sensor_find_backend (ruuvi_driver_sensor_t * const
        sensor_list,
        const size_t count, const char * const name);
/*@}*/
#endif
