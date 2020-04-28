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
 * This function checks app_config.h for enabled sensors and initializes each of them into
 * mode stored into flash, or into default mode defined in app_config.h if the mode is not
 * stored in flash.
 *
 * @retval RD_SUCCESS on success, NOT_FOUND sensors are allowed.
 * @retval RD_ERROR_SELFTEST if sensor is found on the bus and fails selftest.
 */
rd_status_t app_sensor_init (void);

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

#endif // APP_SENSOR_H
