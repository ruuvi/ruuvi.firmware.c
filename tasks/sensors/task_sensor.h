#ifndef TASK_SENSOR_H
#define TASK_SENSOR_H
/**
 * @defgroup sensor_tasks Sensor tasks
 */
/*@{*/
/**
 * @file task_sensor.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-10-11
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 * 
 * Helper functions common to all sensors
 *
 */
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"

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
ruuvi_driver_status_t task_sensor_configure(ruuvi_driver_sensor_t* const sensor, 
                                            ruuvi_driver_sensor_configuration_t* const config, 
                                            const char* const unit);

/*@}*/
#endif