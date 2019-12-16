/**
 * @file task_communication.h
 * @author Otso Jousimaa
 * @date 2019-12-16
 * @brief Control application via 2-way communication
 * @copyright Copyright 2019 Ruuvi Innovations.
 *   This project is released under the BSD-3-Clause License.
 *
 */
/**
 * @addtogroup communication_tasks
 */
/*@{*/

#ifndef  TASK_COMMUNICATION_H
#define  TASK_COMMUNICATION_H

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_log.h"

#define TASK_COMMUNICATION_SENSOR_OFFSET_U16_INVALID 0xFFFF //!< Signal that sensor offsets were invalid / non-applicable

/** @brief set up or read offsets, data from the task. */
typedef void (*task_api_data_fp_t) (uint8_t * const bytes);

/** @brief Function pointer to send data to once available, for example send to RAM buffer */
typedef ruuvi_driver_status_t (*task_api_data_target_t) (const ruuvi_driver_sensor_data_t *
        const data, const uint8_t source);

/** @brief Function to configure logging. */
typedef ruuvi_driver_status_t (*task_api_log_cfg_t) (const task_api_data_target_t target,
        const uint8_t interval);

/** @brief Function to read logs. */
typedef ruuvi_driver_status_t (*task_api_log_read_t) (const
        ruuvi_interface_communication_xfer_fp_t reply_fp,
        const ruuvi_interface_communication_message_t * const query);

/** @brief Function to get heartbeat data */
typedef ruuvi_driver_status_t
(*heartbeat_data_fp_t)
(uint8_t * const msg);

/** @brief API to control sensors. */
typedef struct task_communication_api_t
{
    ruuvi_driver_sensor_t ** sensor;     //!< Sensor control functions, NULL if not applicable
    task_api_data_fp_t      offset_set;  //!< Function to setup offset
    task_api_data_fp_t      offset_get;  //!< Function to read offset
    task_api_data_fp_t      data_get;    //!< Function to get value from sensor
    task_api_data_target_t  data_target; //!< Function to send data to.
    task_api_log_cfg_t      log_cfg;     //!< Function to configure logging
    task_api_log_read_t     log_read;    //!< Function to read logs
} task_communication_api_t;

/** @brief handle incoming data and prepare a reply
 *
 *  This function passes incoming data to relevant listener(s) and provides a function through which
 *  acknowledgment can be sent back.
 *
 * @param[in] incoming received data
 * @param[in] reply_fp function pointer to which acknowledgment should be sent, reply_fp(acknowledge)
 * @retval RUUVI_DRIVER_SUCCESS on success
 */
ruuvi_driver_status_t task_communication_on_data (const
        ruuvi_interface_communication_message_t * const incoming,
        ruuvi_interface_communication_xfer_fp_t reply_fp);

/**
 * @brief Start sending a "hearbeat" signal over given channel to a connected device.
 *
 * Heartbeat is data format 5 encoded status. If the data format 5 payload doesn't fit into given max length,
 * data is cropped to maximum transmittable size. New heartbeat can be configured over old one without stopping.
 *
 * @param[in] interval_ms interval to send the data, in milliseconds. Set to 0 to stop the heartbeat
 * @param[in] max_len Maximum length of data to send.
 * @param[in] data_src function to generate the heartbeat message
 * @param[in] send function pointer to send the data through. May be NULL if interval is 0.
 *
 * @retval RUUVI_DRIVER_SUCCESS if heartbeat was initialized (or stopped)
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if timer cannot be initialized.
 * @retval RUUVI_DRIVER_ERROR_NULL if interval wasn't 0 and send is NULL
 * @retval error code from stack on other error.
 *
 */
ruuvi_driver_status_t task_communication_heartbeat_configure (const uint32_t interval_ms,
        const size_t max_len,
        const heartbeat_data_fp_t data_src,
        const ruuvi_interface_communication_xfer_fp_t send);

/*@}*/

#endif