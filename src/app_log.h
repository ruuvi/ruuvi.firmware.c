#ifndef APP_LOG_H
#define APP_LOG_H
/**
 * @addtogroup app
 */
/** @{ */
/**
 * @defgroup app_log Storing sensor data to memory and reading logs.
 *
 */
/** @} */
/**
 * @addtogroup app_log
 */
/** @{ */
/**
 * @file app_log.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-07-17
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"

#define STORAGE_BLOCK_SIZE (4000U) //!< bytes of compressed data. 

typedef struct
{
    uint16_t interval_s;            //!< Interval to log at, in seconds.
    /**
     * True -> erase old elements automatically.
     * False -> return RD_ERROR_NO_MEMORY when full.
     */
    bool overflow;
    rd_sensor_data_fields_t fields; //!< Fields to log.
} app_log_config_t;                 //!< Logging configuration.

typedef struct
{
    uint32_t start_timestamp_s;           //!< Timestamp of first sample.
    uint32_t end_timestamp_s;             //!< Timestamp of last sample.
    size_t num_samples;                   //!< Number of samples in block.
    app_log_config_t block_configuration; //!< Configuration of this data block.
    uint8_t storage[STORAGE_BLOCK_SIZE];  //!< Raw storage.
} app_log_record_t; //!< Record for application sensor logs.

/**
 * @brief Initialize logging.
 *
 * After initialization flash driver is ready to store data.
 * If there is a logging configuration stored to flash, stored configuration is used.
 * If not, default configuration is used and stored to flash.
 *
 * @retval RD_SUCCESS if logging was initialized.
 */
rd_status_t app_log_init();

/**
 * @brief Process data into log.
 *
 * If time elapsed since last logged element is larger than logging interval, data is
 * stored to RAM buffer. While data is being stored to RAM buffer, it is also stored to
 * compressed buffer. When compressed buffer fills 4000 bytes it will be written to flash.
 * If there is no more room for new blocks in flash, oldest flash block is erased and
 * replaced with new data.
 *
 * @retval RD_SUCCESS if data was logged.
 * @retval RD_ERROR_NO_MEMORY if data cannot be stored to flash and overflow is false.
 * @retval RD_ERROR_BUSY previous operation is in process, e.g. writing to flash.
 */
rd_status_t app_log_process (const rd_sensor_data_t * const sample);

/**
 * @brief Get data from log.
 *
 * Searches for first logged sample after given timestamp and returns it. Loop over
 * this function to get all logged data.
 *
 * @param[in,out] sample Input: Requested data fields with timestamp set.
 *                       Output: Filled fields with valid data.
 *
 * @retval RD_SUCCESS if a sample was retrieved.
 * @retval RD_ERROR_NOT_FOUND if no newer data than requested timestamp was found.
 *
 */
rd_status_t app_log_read (rd_sensor_data_t * const sample);

/**
 * @brief Configure logging.
 *
 * Calling this function will flush current log buffer into flash, possibly leading
 * to NULL entries.
 */
rd_status_t app_log_config_set (const app_log_config_t * const configuration);

/**
 * @brief Read current logging configuration.
 */
rd_status_t app_log_config_get (app_log_config_t * const configuration);

/** @} */
#endif // APP_LOG_H
