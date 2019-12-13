/**
 * Ruuvi Firmware 3.x RTC tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TASK_RTC_H
#define  TASK_RTC_H

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"
#include "task_communication.h"
/**
 * @brief Initializes the RTC and sets up timestamp function on sensor interface.
 *
 * @return RUUVI_DRIVER_SUCCESS on success
 */
ruuvi_driver_status_t task_rtc_init (void);

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
ruuvi_driver_status_t task_rtc_logging_configure (const task_api_data_target_t target,
        const uint8_t interval);

/**
 * @brief set offset to system clock
 *
 * Values returned by @code ruuvi_interface_rtc_millis @endcode are offset by this amount, which
 * can be positive or negative. Negative offset should be always smaller than current time,
 * otherwise error is returned.
 *
 * This function is useful for synchronizing the clock with EPOCH, for example.
 *
 * @param[in] offset number of offset
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return RUUVI_DRIVER_ERROR_INVALID_PARAM if offset is larger negative value than current system time
 */
ruuvi_driver_status_t task_rtc_millis_offset_set (const int64_t offset);

/**
 * @brief get offset to system clock
 *
 * Values returned by @code ruuvi_interface_rtc_millis @endcode  are offset by this amount, which
 * can be positive or negative.
 *
 * This function is useful for synchronizing the clock with EPOCH, for example.
 *
 * @return offset to system RTC.
 */
int64_t task_rtc_millis_offset_get (void);

/**
 * @brief Get milliseconds since init with summed offset.
 *
 * @return number of milliseconds since RTC init.
 * @return @c RUUVI_DRIVER_UINT64_INVALID if RTC is not running
  **/
uint64_t task_rtc_millis (void);

/**
 * @brief wrapper to take incoming big-endian 8-byte offset value and
 * set it to task_rtc.
 *
 * @param[in] bytes 8 bytes representing int64_t offset
 */
void task_rtc_comapi_offset_set (uint8_t * const bytes);

/**
 * @brief wrapper to take int64_t offset and set it to bytes as big-endian.
 *
 * @param[out] bytes 8 bytes representing int64_t offset
 */
void task_rtc_comapi_offset_get (uint8_t * const bytes);

/**
 * @brief wrapper to write current RTC value to given pointer.
 *
 * @param[in] bytes 8 bytes representing milliseconds since rtc_init.
 */
void task_rtc_comapi_data_get (uint8_t * const bytes);

/**
 * @brief Get API function pointers from sensor.
 *
 * @param[out] api pointer to a pointer to a struct of sensor api functions.
 * @return @c RUUVI_DRIVER_SUCCESS if sensor API functions could be set up
 * @return @c RUUVI_DRIVER_ERROR_NULL if given pointer to api was null
 * @return @c RUUVI_DRIVER_ERROR_INVALID_STATE if sensor was not initialized
 */
ruuvi_driver_status_t task_rtc_api_get (task_communication_api_t ** api);

ruuvi_driver_status_t task_rtc_logging_configure (const task_api_data_target_t target,
        const uint8_t interval);


#endif