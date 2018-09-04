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
#include "ruuvi_interface_log.h"

/**
 * Initializes adc in low-power state
 * Configures the sensor according to defaults in application_config.h
 */
ruuvi_driver_status_t task_adc_init(void);

/**
 * Prints adc data to log at given severity
 */
ruuvi_driver_status_t task_adc_data_log(const ruuvi_interface_log_severity_t level);

/**
 * Get adc data and print it on console.
 */
ruuvi_driver_status_t task_adc_on_button(void);

/**
 *  Command adc to take a single sample.
 **/
ruuvi_driver_status_t task_adc_sample(void);

#endif