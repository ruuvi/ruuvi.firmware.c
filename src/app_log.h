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

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"

#define STORAGE_RECORD_HEADER_SIZE (96U) //!< bytes allocated for header.
/** @brief bytes of compressed data.  */
#define STORAGE_BLOCK_SIZE (RB_FLASH_PAGE_SIZE - STORAGE_RECORD_HEADER_SIZE)

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

// TODO: Generalize to N elements.
typedef struct
{
    uint32_t timestamp_s;
    float temperature_c;
    float humidity_rh;
    float pressure_pa;
} app_log_element_t;

typedef struct
{
    uint8_t page_idx; //!< Index of page being read.
    uint16_t element_idx; //!< Index of element being read.
    const uint64_t oldest_element_ms; //!< Age of oldest element to return in system time.
} app_log_read_state_t; //!< Log read state.

#define APP_LOG_MAX_SAMPLES (STORAGE_BLOCK_SIZE/sizeof(app_log_element_t))

typedef struct
{
    uint32_t start_timestamp_s;           //!< Timestamp of first sample.
    uint32_t end_timestamp_s;             //!< Timestamp of last sample.
    size_t num_samples;                   //!< Number of samples in block.
    app_log_config_t block_configuration; //!< Configuration of this data block.
    app_log_element_t storage[APP_LOG_MAX_SAMPLES];  //!< Raw storage.
} app_log_record_t; //!< Record for application sensor logs.

/**
 * @brief Initialize logging.
 *
 * After initialization flash driver is ready to store data.
 * If there is a logging configuration stored to flash, stored configuration is used.
 * If not, default configuration is used and stored to flash.
 *
 * @retval RD_SUCCESS if logging was initialized or if logging is disabled in config.
 */
rd_status_t app_log_init();

/**
 * @brief Process data into log.
 *
 * If time elapsed since last logged element is larger than logging interval, data is
 * stored to RAM buffer. The RAM buffer may be compressed. When the buffer fills
 * @ref STORAGE_BLOCK_SIZE bytes it will be written to flash.
 * If there is no more room for new blocks in flash, oldest flash block is erased and
 * replaced with new data.
 *
 * @retval RD_SUCCESS Data was logged.
 * @retval RD_ERROR_NO_MEMORY Data cannot be stored to flash and overflow is configured
 *                            as false.
 * @retval RD_ERROR_BUSY Previous operation is in process, e.g. writing to flash.
 */
rd_status_t app_log_process (const rd_sensor_data_t * const sample);

/**
 * @brief Get data from log.
 *
 * Searches for first logged samples after given timestamp and returns them
 * without guarantees about sample order. Loop over this function to get all
 * logged data.
 *
 * @param[out] sample Sensor sample.
 * @param[in,out] p_read_state State of reads.
 *
 * @retval RD_SUCCESS if a sample was retrieved.
 * @retval RD_ERROR_NULL if either parameter is NULL.
 * @
 * @retval RD_ERROR_NOT_FOUND if no newer data than requested timestamp was found.
 *
 */
rd_status_t app_log_read (rd_sensor_data_t * const sample,
                          app_log_read_state_t * const p_read_state);

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

/**
 * @brief Purge everything stored to flash.
 */
void app_log_purge_flash (void);

/** @} */
#endif // APP_LOG_H
