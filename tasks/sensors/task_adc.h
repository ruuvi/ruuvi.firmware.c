/**
 * Ruuvi Firmware 3.x ADC tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_ADC_H
#define  TASK_ADC_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_log.h"

/**
 * Initializes adc in low-power state
 * Configures the sensor according to defaults in application_config.h
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error.
 */
ruuvi_driver_status_t task_adc_init(void);

/**
 * Prints adc data to log at given severity
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error.
 */
ruuvi_driver_status_t task_adc_data_log(const ruuvi_interface_log_severity_t level);

/**
 * Populates data with latest sample. Remember to trigger sampling with task_adc_sample if you're in single shot mode
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error.
 */
ruuvi_driver_status_t task_adc_data_get(ruuvi_interface_adc_data_t* const data);

/**
 * Populates data with battery voltage sampled after radio activity.
 *
 * returns RUUVI_DRIVER_SUCCESS on success
 * returns error code from stack on error.
 */
ruuvi_driver_status_t task_adc_battery_get(ruuvi_driver_sensor_data_t* const data);


#endif