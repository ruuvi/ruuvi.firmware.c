#ifndef APP_SENSOR_H
#define APP_SENSOR_H
/**
 * @addtogroup app
 */
/** @{ */
/**
 * @defgroup app_sensor Application sensor control
 * @brief Initialize, configure and read sensors.
 */
/** @} */
/**
 * @addtogroup app_sensor
 */
/** @{ */
/**
 * @file app_sensor.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-04-16
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Typical usage:
 * @code{.c}
 * TODO
 * @endcode
 */

#include "app_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_task_sensor.h"

#define APP_SENSOR_SELFTEST_RETRIES (5U) //!< Number of times to retry init on self-test fail.

enum
{
#if APP_SENSOR_TMP117_ENABLED
    TMP117_INDEX,
#endif
#if APP_SENSOR_SHTCX_ENABLED
    SHTCX_INDEX,
#endif
#if APP_SENSOR_BME280_ENABLED
    BME280_INDEX,
#endif
#if APP_SENSOR_NTC_ENABLED
    NTC_INDEX,
#endif
#if APP_SENSOR_PHOTO_ENABLED
    PHOTO_INDEX,
#endif
#if APP_SENSOR_MCU_ENABLED
    ENV_MCU_INDEX,
#endif
#if APP_SENSOR_LIS2DH12_ENABLED
    LIS2DH12_INDEX,
#endif
#if APP_SENSOR_LIS2DW12_ENABLED
    LIS2DW12_INDEX,
#endif
    SENSOR_COUNT
};

#ifdef CEEDLING
extern rt_sensor_ctx_t * m_sensors[]; //!< Give Ceedling a handle to sensors in project
void m_sensors_init (void); //!< Give Ceedling a handle to initialize structs.
#endif

/**
 * @brief Initialize sensors into default mode or to a mode stored to flash.
 *
 * This function checks app_config.h for enabled sensors and initializes each of them
 * into mode stored into flash, or into default mode defined in app_config.h if the
 * mode is not stored in flash. Internal sampling rate of sensors does not affect
 * reading rate of sensors, reading rate must be configured separately.
 *
 * @retval RD_SUCCESS on success, NOT_FOUND sensors are allowed.
 * @retval RD_ERROR_SELFTEST if sensor is found on the bus and fails selftest.
 */
rd_status_t app_sensor_init (void);

/**
 * @brief Configure sampling of sensors.
 *
 * This function lets application know what data and how often should be read.
 * To use sampled data, call @ref app_sensor_get.
 *
 * @param[in] data Data fields to sample.
 * @param[in] interval_ms Interval to sample at. At minimum 1000 ms.
 *
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_PARAM if interval is less than 1000 ms.
 *
 * @note: Future minor version may allow using intervals down to 1 ms on
 *        sensors with built-in FIFOs and read the FIFO in batches.
 *
 */
rd_status_t app_sensor_sample_config (const rd_sensor_data_fields_t data,
                                      const uint32_t interval_ms);

/**
 * @brief Return available data types.
 *
 * @note This is refreshed from sensor structs RAM which makes
 * this a relatively expensive function call due to looping over
 * all sensor contexts. Cache this if microseconds count in your application.
 *
 * @return Listing of data the application can provide.
 */
rd_sensor_data_fields_t app_sensor_available_data (void);

/**
 * @brief Return last sampled data.
 *
 * This function checks loops through initialized sensors until all data in
 * data->fields is valid or all sensors are checked.
 *
 * @retval RD_SUCCESS on success, NOT_FOUND sensors are allowed.
 * @retval RD_ERROR_SELFTEST if sensor is found on the bus and fails selftest.
 */
rd_status_t app_sensor_get (rd_sensor_data_t * const data);

/**
 * @brief Uninitialize sensors into low-power mode.
 *
 * @retval RD_SUCCESS On success.
 * @return Error code from stack on error.
 */
rd_status_t app_sensor_uninit (void);

#ifdef RUUVI_RUN_TESTS
void app_sensor_ctx_get (rt_sensor_ctx_t *** m_sensors, size_t * num_sensors);
#endif

#ifdef CEEDLING
#include "ruuvi_interface_communication_radio.h"
void on_radio (const ri_radio_activity_evt_t evt);
#endif

#endif // APP_SENSOR_H
