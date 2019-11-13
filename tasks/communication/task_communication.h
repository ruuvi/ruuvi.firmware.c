/**
 * @file task_communication.h
 * @author Otso Jousimaa
 * @date 2019-06-20
 * @brief Control application via 2-way communication
 * @copyright Copyright 2019 Ruuvi Innovations.
 *   This project is released under the BSD-3-Clause License.
 *
 */

#ifndef  TASK_COMMUNICATION_H
#define  TASK_COMMUNICATION_H

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_log.h"

#define TASK_COMMUNICATION_SENSOR_OFFSET_U16_INVALID 0xFFFF //!< Signal that sensor offsets were invalid / non-applicable

/** @brief set up or read offsets, data from the task. */
typedef void (*task_api_data_fp_t)(uint8_t* const bytes);

/** @brief Function pointer to send data to once available, for example send to RAM buffer */
typedef ruuvi_driver_status_t (*task_api_data_target_t)(const ruuvi_driver_sensor_data_t*
    const data, const uint8_t source);

/** @brief Function to configure logging. */
typedef ruuvi_driver_status_t (*task_api_log_cfg_t)(const task_api_data_target_t target,
    const uint8_t interval);

/** @brief Function to read logs. */
typedef ruuvi_driver_status_t (*task_api_log_read_t)(const
    ruuvi_interface_communication_xfer_fp_t reply_fp,
    const ruuvi_interface_communication_message_t* const query);

/** @brief API to control sensors. */
typedef struct task_communication_api_t
{
  ruuvi_driver_sensor_t** sensor;      //!< Sensor control functions, NULL if not applicable
  task_api_data_fp_t      offset_set;  //!< Function to setup offset
  task_api_data_fp_t      offset_get;  //!< Function to read offset
  task_api_data_fp_t      data_get;    //!< Function to value from sensor
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
 * @return RUUVI_DRIVER_SUCCESS on success
 */
ruuvi_driver_status_t task_communication_on_data(const
    ruuvi_interface_communication_message_t* const incoming,
    ruuvi_interface_communication_xfer_fp_t reply_fp);

/** @brief convert uint8_t array into ruuvi_driver_sensor_data_t
 *  Offsets are encoded as a timestamp offset uint16_t in ms
 *  and I16 fixed-point offset. 0xFFFF is considered as INVALID/NAN
 *  I16 is interpreted as -126 ... 126 (MSB) + 1/128 * (LSB),
 *  e.g. -1.5 = -1 - 0.5 = 0xFFC0.
 *
 * @param[in]  offsets uint8_t array of 8 bytes
 * @param[out] converted @ref ruuvi_driver_sensor_data_t with offsets.
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return RUUVI_DRIVER_ERROR_NULL if given a NULL parameter
 */
ruuvi_driver_status_t task_communication_offsets_u8_to_float(const uint8_t* const offsets,
    ruuvi_driver_sensor_data_t* const converted);

/** @brief convert ruuvi_driver_sensor_data_t array into  uint8_t
 *  Offsets are encoded as a timestamp offset uint16_t in ms
 *  and I16 fixed-point offset. 0xFFFF is considered as INVALID/NAN
 *  I16 is interpreted as -126 ... 126 (MSB) + 1/128 * (LSB),
 *  e.g. -0.5 = -1 + 0.5 = 0xFFC0.
 *  If given offsets are larger than maximum u8 array can represent, the offsets are clipped to maximum.
 *
 * @param[in]  offsets @ref ruuvi_driver_sensor_data_t with offsets.
 * @param[out] converted uint8_t array of 8 bytes
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return RUUVI_DRIVER_ERROR_NULL if given a NULL parameter
 */
ruuvi_driver_status_t task_communication_offsets_float_to_u8(const
    ruuvi_driver_sensor_data_t* const offsets, uint8_t* const converted);

/** @brief convert fixed point 32.32 into a float
 *  0xFFFFFFFFFFFFFFFF is considered as INVALID/NAN
 *  I32 is interpreted as -2147483648 ... 2147483648 (MSB) + 1/2147483648 * (LSB),
 *  e.g. -1.5 = -1 - 0.5 = 0xFF FF FF FF C0 00 00 00.
 *
 * @param[in]  offsets uint8_t array of 8 bytes
 * @param[out] converted @ref ruuvi_driver_sensor_data_t with offsets.
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return RUUVI_DRIVER_ERROR_NULL if given a NULL parameter
 */
ruuvi_driver_status_t task_communication_offsets_i32f32_to_float(
  const uint8_t* const offset, float* const converted);

/** @brief convert a float into fixed a point 32.32.
 *  INVALID/NAN is considered as 0xFFFFFFFFFFFFFFFF
 *  I32 is interpreted as -2147483648 ... 2147483648 (MSB) + 1/2147483648 * (LSB),
 *  e.g. -1.5 = -1 - 0.5 = 0xFF FF FF FF C0 00 00 00.
 *  Value is clipped to max/min if the float is larger than presentable with I32.32
 *
 * @param[in]  offset float with offset
 * @param[out] converted uint8_t array with offset.
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return RUUVI_DRIVER_ERROR_NULL if given a NULL parameter
 */
ruuvi_driver_status_t task_communication_offsets_float_to_i32f32(
  const float* const offset, uint8_t* const converted);

/**
 * @brief Apply offsets to the data by summing the offset.
 * Only finite values are summed, INFINITE, NAN etc are ignored.
 *
 * @param[in,out] data sensor readings
 * @param[in]     offsets offsets to data. 0, NAN will be ignored.
 */
void task_communication_offsets_apply(ruuvi_driver_sensor_data_t* const data,
                                      const ruuvi_driver_sensor_data_t* const offsets);

/**
 * @brief Start sending a "hearbeat" signal over given channel to a connected device.
 *
 * Heartbeat is data format 5 encoded status. If the data format 5 payload doesn't fit into given max length,
 * data is cropped to maximum transmittable size. New heartbeat can be configured over old one without stopping.
 *
 * @param[in] interval_ms interval to send the data, in milliseconds. Set to 0 to stop the heartbeat
 * @param[in] max_len Maximum length of data to send.
 * @param[in] send function pointer to send the data through. May be NULL if interval is 0.
 *
 * @return RUUVI_DRIVER_SUCCESS if heartbeat was initialized (or stopped)
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if timer cannot be initialized.
 * @return RUUVI_DRIVER_ERROR_NULL if interval wasn't 0 and send is NULL
 * @return error code from stack on other error.
 *
 */
ruuvi_driver_status_t task_communication_heartbeat_configure(const uint32_t interval_ms,
    const size_t max_len, const ruuvi_interface_communication_xfer_fp_t send);

#endif