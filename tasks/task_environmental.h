/**
 * Ruuvi Firmware 3.x Environmental tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_ENVIRONMENTAL_H
#define  TASK_ENVIRONMENTAL_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_environmental.h"
#include "ruuvi_interface_environmental_mcu.h"
#include "ruuvi_interface_log.h"
#include "task_communication.h"

typedef struct {
  uint32_t timestamp_s;   //seconds since device boot
  float temperature_c;    //celcius   
  float humidity_rh;      //RH-%
  float pressure_pa;      //Pascals
}environmental_log_t;

/**
 * Auto-detects and initializes environmental sensor in low-power state
 * Reads supported sensors from board.h and tries to initialize them.
 * Configures the sensor according to defaults in application_config.h
 */
ruuvi_driver_status_t task_environmental_init(void);

/**
 * Prints environmental data to log at given severity
 */
ruuvi_driver_status_t task_environmental_data_log(const ruuvi_interface_log_severity_t
    level);

/**
 * Get latest sample from the sensor. Remember to trigger sampling in case you're in single-shot mode
 */
ruuvi_driver_status_t task_environmental_data_get(ruuvi_interface_environmental_data_t*
    const data);

/**
 * Blink RED led for the duration of task. Get environmental data and print it on console.
 */
ruuvi_driver_status_t task_environmental_on_button(void);

/**
 *  Command environmental sensor to take a single sample. Leaves sensor in sleep
 **/
ruuvi_driver_status_t task_environmental_sample(void);

/*
 * @brief log data - dummy, todo
 */
ruuvi_driver_status_t task_environmental_log(void);

/*
 * @brief send logged data - dummy, todo
 */

ruuvi_driver_status_t task_environmental_log_read(const ruuvi_interface_communication_xfer_fp_t reply_fp,
                                                  const ruuvi_interface_communication_message_t* const query);

ruuvi_driver_status_t task_environmental_api_get(task_communication_api_t** api);


#endif